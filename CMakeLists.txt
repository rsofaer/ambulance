cmake_minimum_required(VERSION 2.8)
file(GLOB HEADERS "*.h")

find_package(OpenMP REQUIRED)

# Library targets:
#   ambulance_core - shared objects for ambulance
project(ambulance_core)
set(SRCS
    "combination.cpp")
add_library(ambulance_core STATIC ${SRCS} ${HEADERS})

if(UNIX)
  set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fPIC")
  set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} ${OpenMP_CXX_FLAGS}")
endif(UNIX)

# Executable targets:
#   ambulance - the main solution
#   ambulance_gtest - all tests

project(ambulance)
set(SRCS
    "ambulance.cpp")
add_executable(ambulance ${SRCS} ${HEADERS})
target_link_libraries(ambulance ambulance_core)
if(WIN32)
  set_target_properties(ambulance PROPERTIES
                        COMPILE_FLAGS ${OpenMP_CXX_FLAGS})
endif(WIN32)

if(HPS_GTEST_ENABLED)
  project(ambulance_gtest)
  set(SRCS
      "ambulance_gtest.cpp")
  include_directories(${GTEST_INCLUDE_DIRS})
  add_executable(ambulance_gtest ${SRCS} ${HEADERS})
  target_link_libraries(ambulance_gtest ambulance_core gtest)
  if(WIN32)
    set_target_properties(ambulance_gtest PROPERTIES
                          COMPILE_FLAGS ${OpenMP_CXX_FLAGS})
  endif(WIN32)
  add_test(ambulance_gtest ambulance_gtest)
endif(HPS_GTEST_ENABLED)

project(ambulance)

