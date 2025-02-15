#include "router.h"

#include <boost/json.hpp>
#include <cctype>
#include <list>
#include <ranges>
#include <string_view>
#include <utility>

namespace brscomp {

Router::Router() {
  for (auto &method : methods) {
    method = [](WFHttpTask *task, Context *ctx) {
      auto resp = task->get_resp();
      boost::json::object obj;
      obj["message"] = "API not found";
      std::string body = boost::json::serialize(obj);
      resp->append_output_body(body.c_str(), body.length());
      resp->set_status_code("404");
    };
  }
}

Router &Router::operator[](const std::string &path) { return routes[path]; }
Router &Router::operator[](std::string &&path) {
  return routes[std::move(path)];
}
Router &Router::at(const std::string &path) { return routes.at(path); }
Router &Router::at(std::string &&path) { return routes.at(std::move(path)); }
bool Router::has(const std::string &path) {
  auto it = this->routes.find(path);
  if (it != routes.end()) {
    return true;
  } else {
    return false;
  }
}

bool Router::has(std::string &&path) {
  auto it = this->routes.find(std::move(path));

  if (it != routes.end()) {
    return true;
  } else {
    return false;
  }
}

void Router::operator()(Method method, WFHttpTask *task, Context *ctx) {
  methods[method](task, ctx);
}

void Router::call(Method method, WFHttpTask *task, Context *ctx) {
  methods[method](task, ctx);
}

void Router::get(WFHttpTask *task, Context *ctx) {
  this->methods[GET](task, ctx);
}

void Router::post(WFHttpTask *task, Context *ctx) {
  this->methods[POST](task, ctx);
}

void Router::put(WFHttpTask *task, Context *ctx) {
  this->methods[PUT](task, ctx);
}

void Router::del(WFHttpTask *task, Context *ctx) {
  this->methods[DELETE](task, ctx);
}

Router &Router::route(const std::string &url) {
  // First we get the path from the url by removing the query string
  std::string_view u(url);
  std::string_view path = u.substr(0, u.find("?"));

  // Get the path segments with ranges
  auto segments =
      path | std::views::split('/') | std::views::transform([](auto &&range) {
        return std::string(range.begin(), range.end());
      });

  std::list<std::string> words(segments.begin(), segments.end());

  // Remove the empty words from the list, including those that contains only
  // empty letters
  words.remove_if([](const std::string &word) {
    return word.empty() ||
           std::all_of(word.begin(), word.end(),
                       [](unsigned char c) { return std::isspace(c); });
  });

  auto it = words.begin();

  Router *router = this;
  while (it != words.end()) {
    if (router->has(*it)) {
      router = &router->at(*it);
    } else {
      router = &router->operator[](*it);
    }
    it++;
  }

  return *router;
}

Router &Router::route(std::string &&url) {
  // First we get the path from the url by removing the query string
  std::string_view u(url);
  std::string_view path = u.substr(0, u.find("?"));

  // Get the path segments with ranges
  auto segments =
      path | std::views::split('/') | std::views::transform([](auto &&range) {
        return std::string(range.begin(), range.end());
      });

  std::list<std::string> words(segments.begin(), segments.end());

  // Remove the empty words from the list, including those that contains only
  // empty letters
  words.remove_if([](const std::string &word) {
    return word.empty() ||
           std::all_of(word.begin(), word.end(),
                       [](unsigned char c) { return std::isspace(c); });
  });

  auto it = words.begin();

  Router *router = this;
  while (it != words.end()) {
    if (router->has(*it)) {
      router = &router->at(*it);
    } else {
      router = &router->operator[](*it);
    }
    it++;
  }

  return *router;
}

void Router::call(const std::string &url, Method method, WFHttpTask *task,
                  Context *ctx) {
  std::string_view u(url);
  std::string_view path = u.substr(0, u.find("?"));

  auto segments =
      path | std::views::split('/') | std::views::transform([](auto &&range) {
        return std::string(range.begin(), range.end());
      });

  std::list<std::string> words(segments.begin(), segments.end());
  words.remove_if([](const std::string &word) {
    return word.empty() ||
           std::all_of(word.begin(), word.end(),
                       [](unsigned char c) { return std::isspace(c); });
  });

  auto it = words.begin();
  Router *router = this;
  while (it != words.end()) {
    if (router->has(*it)) {
      router = &router->at(*it);
    } else {
      // Avoid creating to many empty routers
      auto func = [](WFHttpTask *task, Context *ctx) {
        auto resp = task->get_resp();
        boost::json::object obj;
        obj["message"] = "API not found";
        std::string body = boost::json::serialize(obj);
        resp->append_output_body(body.c_str(), body.length());
        resp->set_status_code("404");
      };
      func(task, ctx);
      return;
    }
    it++;
  }

  router->call(method, task, ctx);
  return;
}

}  // namespace brscomp
