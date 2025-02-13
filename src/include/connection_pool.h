#pragma once

#ifndef CONNECTION_POOL_H
#define CONNECTION_POOL_H

#include <nanodbc/nanodbc.h>

#include <cstddef>
#include <iostream>
#include <memory>
#include <mutex>
#include <queue>

/*Haoming **Bai**'s reservation server component*/

namespace brscomp {

/**
 * @class ConnectionPool
 * @brief Manages a pool of database connections.
 *
 * This class provides a thread-safe way to manage a pool of database
 * connections using the nanodbc library. It allows for efficient reuse of
 * connections, reducing the overhead of establishing new connections.
 */
class ConnectionPool {
 private:
  std::queue<std::shared_ptr<nanodbc::connection>> pool;
  std::string dataSource;
  std::string username;
  std::string password;
  std::mutex mtx;

 public:
  /**
   * @brief Constructs a new ConnectionPool object.
   *
   * @param size The number of connections in the pool.
   * @param dataSource The data source name for the database connection.
   * @param username The username for the database connection.
   * @param password The password for the database connection.
   */
  ConnectionPool(size_t size, std::string dataSource, std::string username,
                 std::string password);

  ConnectionPool(std::string dataSource, std::string username,
                 std::string password);

  ~ConnectionPool() = default;

  ConnectionPool(const ConnectionPool&) = delete;

  ConnectionPool& operator=(const ConnectionPool&) = delete;

  ConnectionPool(ConnectionPool&&) = default;

  ConnectionPool& operator=(ConnectionPool&&) = default;

  /**
   * @brief Retrieves a shared pointer to a nanodbc::connection object from the
   * connection pool.
   *
   * @return std::shared_ptr<nanodbc::connection> A shared pointer to a
   * nanodbc::connection object.
   */
  std::shared_ptr<nanodbc::connection> getConnection();

  /// @brief Check and then update the connection status of the connections
  /// which are not in use
  void updateConnection();
};

}  // namespace brssrvcomp

#endif  // CONNECTION_POOL_H