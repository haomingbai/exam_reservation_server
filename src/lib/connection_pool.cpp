#include "connection_pool.h"

#include <thread>

namespace brscomp {
ConnectionPool::ConnectionPool(size_t size, std::string dataSource,
                               std::string username, std::string password)
    : dataSource(dataSource), username(username), password(password) {
  if (size == 0) {
    throw std::invalid_argument("Connection pool size must be greater than 0");
  }

  std::shared_ptr<nanodbc::connection> firstConnection =
      std::make_shared<nanodbc::connection>(dataSource, username, password);
  this->pool.push(firstConnection);

  for (size_t i = 1; i < size; i++) {
    auto conn =
        std::make_shared<nanodbc::connection>(dataSource, username, password);
    this->pool.push(conn);
  }
}

ConnectionPool::ConnectionPool(std::string dataSource, std::string username,
                               std::string password)
    : dataSource(dataSource), username(username), password(password) {
  size_t size = std::thread::hardware_concurrency();

  std::shared_ptr<nanodbc::connection> firstConnection =
      std::make_shared<nanodbc::connection>(dataSource, username, password);
  this->pool.push(firstConnection);

  for (size_t i = 1; i < size; i++) {
    auto conn =
        std::make_shared<nanodbc::connection>(dataSource, username, password);
    this->pool.push(conn);
  }
}

std::shared_ptr<nanodbc::connection> ConnectionPool::getConnection() {
  std::lock_guard<std::mutex> lock(mtx);
  std::shared_ptr<nanodbc::connection> conn = pool.front();

  // If the connection is not only held by the pool and the current shared ptr,
  // then pop it and then put it to the back of the queue

  size_t cnt = 0;
  while (conn.use_count() > 2) {
    pool.pop();
    pool.push(conn);
    conn = pool.front();
    cnt++;
    if (cnt > pool.size()) {
      try {
        std::shared_ptr<nanodbc::connection> newConn =
            std::make_shared<nanodbc::connection>(dataSource, username,
                                                  password);
        conn = newConn;
        pool.push(newConn);
        break;
      } catch (...) {
        std::clog << "Connection pool: failed to create new connection"
                  << std::endl;
      }
    }
  }

  while (!conn->connected()) {
    try {
      conn->connect(this->dataSource, this->username, this->password);
    } catch (...) {
      std::clog << "Connection pool: failed to reconnect" << std::endl;
    }
  }

  return conn;
}

void ConnectionPool::updateConnection() {
  std::lock_guard<std::mutex> lock(mtx);

  auto conn = pool.front();  // The first connection.

  for (size_t i = 0; i < pool.size(); i++) {
    pool.pop();
    if (!conn->connected()) {
      conn =
          std::make_shared<nanodbc::connection>(dataSource, username, password);
    }
    pool.push(conn);
  }
}
}  // namespace brscomp