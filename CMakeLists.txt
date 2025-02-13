cmake_minimum_required(VERSION 3.5.0)
project(reservation_server VERSION 0.1.0 LANGUAGES C CXX)

set(CMAKE_CXX_STANDARD 23)
if (NOT DEFINED CMAKE_BUILD_TYPE)
        set(CMAKE_BUILD_TYPE "Release")
endif()

if ("${CMAKE_BUILD_TYPE}" STREQUAL "Debug")
  add_definitions(-DDEBUG=1)
  set(CMAKE_CXX_FLAGS "-Wall")
else()
  add_definitions(-UDEBUG=1)
  set(CMAKE_CXX_FLAGS "-O3")
endif()

# Workflow library
find_package(workflow)
if (workflow_FOUND)
  message(STATUS "Found workflow library")
  include_directories(${workflow_INCLUDE_DIRS})
else(workflow_FOUND)
  message(FATAL_ERROR "workflow library not found")
endif(workflow_FOUND)

# nanodbc library
find_package(nanodbc)
if (nanodbc_FOUND)
  message(STATUS "Found nanodbc library")
  include_directories(${nanodbc_INCLUDE_DIRS})
else(nanodbc_FOUND)
  message(FATAL_ERROR "nanodbc library not found")
endif(nanodbc_FOUND)

aux_source_directory(${PROJECT_SOURCE_DIR}/src/lib COMP_SRC)
include_directories(src/include)

# Components
add_library(brscomp SHARED ${COMP_SRC})
