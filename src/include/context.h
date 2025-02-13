#pragma once

#ifndef CONTEXT_HPP
#define CONTEXT_HPP

#include <workflow/WFHttpServer.h>

#include <memory>

#include "connection_pool.h"
#include "router.h"

namespace brscomp {

struct ContextV1_t {
  std::shared_ptr<ConnectionPool> connectionPool;
  std::shared_ptr<Router> router;
  std::shared_ptr<WFHttpServer> server;
};

using Context = ContextV1_t;
using ContextV1 = ContextV1_t;

}  // namespace brscomp

#endif  // CONTEXT_HPP