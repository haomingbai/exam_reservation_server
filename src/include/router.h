#pragma once

#ifndef ROUTER_H
#define ROUTER_H

#include <workflow/WFTaskFactory.h>

#include <array>
#include <functional>
#include <iostream>
#include <string>
#include <unordered_map>

/**
 * @namespace brscomp
 * @brief The namespace for development of Haoming Bai's reservation server.
 * @details This namespace contains all necessary classes and functions, which
 * is always used in the development of the server program.
 */
namespace brscomp {
struct Context;

/**
 * @class Router
 * @brief A simple router for the server.
 * @details This class is a simple router that can be used to route requests to
 * the correct handler. If the handler is not found, it will create empty
 * routers until it finds the correct handler.
 * @details The router can handle GET, POST, PUT, and DELETE requests.
 */
class Router {
 private:
  // Route to the next level
  std::unordered_map<std::string, Router> routes;
  std::array<std::function<void(WFHttpTask *, Context *)>, 4> methods;

 public:
  /// @enum Method
  /// @brief The enum class for the HTTP methods.
  enum Method {
    GET,     ///< GET method
    POST,    ///< POST method
    PUT,     ///< PUT method
    DELETE,  ///< DELETE method
  };

  /**
   * @brief Constructs a new Router object.
   */
  Router();

  /**
   * @brief Default destructor for the Router class.
   *
   * This destructor is defined as default, meaning it will be automatically
   * generated by the compiler. It ensures that any resources managed by
   * the Router class are properly released when an instance of the class
   * is destroyed.
   */
  ~Router() = default;

  /**
   * @brief Overloads the subscript operator to access a Router object by a
   * given path.
   *
   * This operator allows you to use the subscript syntax to retrieve a Router
   * object associated with a specific path. The path is provided as a string.
   * If the path does not exist, a new Router object is created and returned.
   *
   * @param path The path as a string to access the corresponding Router object.
   * @return Router& A reference to the Router object associated with the given
   * path.
   */
  Router &operator[](const std::string &path);

  /**
   * Overloads the subscript operator to allow access to a Router object
   * using a given path.
   *
   * @param path A string representing the path to be used for accessing
   *             the Router object. The path is passed as an rvalue reference.
   * @return A reference to the Router object associated with the given path.
   */
  Router &operator[](std::string &&path);

  /**
   * @brief Retrieves a reference to the Router object associated with the
   * specified path.
   *
   * @param path The path for which the Router object is to be retrieved.
   * @return Router& A reference to the Router object associated with the
   * specified path.
   * @exception std::out_of_range Thrown if the specified path does not exist.
   */
  Router &at(const std::string &path);

  /**
   * @brief Associates a route with the given path.
   *
   * This function allows you to specify a route by providing a path string.
   * The path string is moved into the function, which means the function
   * takes ownership of the string.
   *
   * @param path The path string to associate with a route.
   * @return Router& A reference to the Router object, allowing for method
   * chaining.
   */
  Router &at(std::string &&path);

  /**
   * @brief Checks if the given path exists in the router.
   *
   * @param path The path to check for existence, passed as an rvalue reference.
   * @return true if the path exists, false otherwise.
   */
  void operator()(Method method, WFHttpTask *task, Context *ctx);

  /**
   * @brief Calls the specified method with the given HTTP task and context.
   *
   * @param method The method to be called.
   * @param task The HTTP task to be processed.
   * @param ctx The context in which the method is called.
   */
  void call(Method method, WFHttpTask *task, Context *ctx);

  /**
   * @brief Handles HTTP GET requests.
   *
   * This function processes an incoming HTTP GET request and performs the
   * necessary actions based on the provided task and context.
   *
   * @param task A pointer to the WFHttpTask object representing the HTTP task.
   * @param ctx A pointer to the Context object containing the request context.
   */
  void get(WFHttpTask *task, Context *ctx);

  /**
   * @brief Handles HTTP POST requests.
   *
   * This function processes HTTP POST requests received by the server.
   *
   * @param task Pointer to the WFHttpTask object representing the HTTP task.
   * @param ctx Pointer to the Context object containing the request context.
   */
  void post(WFHttpTask *task, Context *ctx);

  /**
   * @brief Handles HTTP PUT requests.
   *
   * @param task Pointer to the WFHttpTask object representing the HTTP task.
   * @param ctx Pointer to the Context object containing the request context.
   */
  void put(WFHttpTask *task, Context *ctx);

  /**
   * @brief Handles the HTTP DELETE request.
   *
   * This function processes the HTTP DELETE request received by the server.
   * It performs the necessary operations to handle the deletion of resources
   * as specified in the request.
   *
   * @param task Pointer to the WFHttpTask object representing the HTTP task.
   * @param ctx Pointer to the Context object containing the request context.
   */
  void del(WFHttpTask *task, Context *ctx);

  /**
   * @brief Routes the given URL to the appropriate handler.
   *
   * This function takes a URL as input and routes it to the corresponding
   * handler within the router. It modifies the router's state to reflect
   * the new route.
   *
   * @param url The URL to be routed.
   * @return Router& A reference to the router object.
   */
  Router &route(const std::string &url);

  /**
   * @brief Adds a route to the router.
   *
   * This function registers a new route with the specified URL to the router.
   *
   * @param url The URL pattern for the route. This is an rvalue reference to a
   *            std::string, allowing for efficient transfer of the URL string.
   * @return Router& A reference to the router object, allowing for method
   * chaining.
   */
  Router &route(std::string &&url);

  /**
   * @brief Handles the HTTP request by routing it to the appropriate handler
   * based on the URL and method.
   *
   * @param url The URL of the incoming HTTP request.
   * @param method The HTTP method (e.g., GET, POST, etc.) of the request.
   * @param task Pointer to the WFHttpTask object representing the HTTP task.
   * @param ctx Pointer to the Context object containing additional information
   * for processing the request.
   */
  void call(const std::string &url, Method method, WFHttpTask *task,
            Context *ctx);
  
  bool has(const std::string &path);

  bool has(std::string &&path);
};

}  // namespace brscomp

#endif  // ROUTER_H