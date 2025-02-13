#pragma once

#ifndef ROUTER_H
#define ROUTER_H

#include <workflow/WFTaskFactory.h>

#include <array>
#include <functional>
#include <iostream>
#include <string>
#include <unordered_map>

namespace brscomp {
struct Context;

class Router {
 private:
  // Route to the next level
  std::unordered_map<std::string, Router> routes;
  std::array<std::function<void(WFHttpTask *, Context *)>, 4> methods;

 public:
  enum Method { GET, POST, PUT, DELETE };
  Router();
  ~Router() = default;
  Router &operator[](const std::string &path);
  Router &operator[](std::string &&path);
  Router &at(const std::string &path);
  Router &at(std::string &&path);
  bool has(const std::string &path);
  bool has(std::string &&path);
  void operator()(Method method, WFHttpTask *task, Context *ctx);
  void call(Method method, WFHttpTask *task, Context *ctx);
  void get(WFHttpTask *task, Context *ctx);
  void post(WFHttpTask *task, Context *ctx);
  void put(WFHttpTask *task, Context *ctx);
  void del(WFHttpTask *task, Context *ctx);
  Router &route(const std::string &url);
  Router &route(std::string &&url);
  void call(const std::string &url, Method method, WFHttpTask *task,
            Context *ctx);
};

}  // namespace brscomp

#endif  // ROUTER_H