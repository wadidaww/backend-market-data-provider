cmake_minimum_required(VERSION 3.20)
project(backend_market_data_provider VERSION 0.1.0 LANGUAGES CXX)

option(MDP_BUILD_TESTS "Build tests" ON)
option(MDP_BUILD_BENCHMARKS "Build benchmarks" ON)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

if(CMAKE_CXX_COMPILER_ID MATCHES "Clang|GNU")
  add_compile_options(-Wall -Wextra -Wpedantic -Werror)
endif()

add_library(market_data_provider
  src/config/config.cpp
  src/core/market_data_service.cpp
  src/feed_adapters/mock_feed_adapter.cpp
  src/feed_adapters/replay_feed_adapter.cpp
  src/normalization/upstream_parser.cpp
  src/orderbook/order_book.cpp
)

target_include_directories(market_data_provider PUBLIC include)

target_compile_features(market_data_provider PUBLIC cxx_std_20)

add_executable(mdp_example examples/main.cpp)
target_link_libraries(mdp_example PRIVATE market_data_provider)

if(MDP_BUILD_TESTS)
  include(CTest)
  enable_testing()

  add_executable(order_book_test tests/order_book_test.cpp)
  target_link_libraries(order_book_test PRIVATE market_data_provider)
  add_test(NAME order_book_test COMMAND order_book_test)

  add_executable(pipeline_integration_test tests/pipeline_integration_test.cpp)
  target_link_libraries(pipeline_integration_test PRIVATE market_data_provider)
  add_test(NAME pipeline_integration_test COMMAND pipeline_integration_test)

  add_executable(config_validation_test tests/config_validation_test.cpp)
  target_link_libraries(config_validation_test PRIVATE market_data_provider)
  add_test(NAME config_validation_test COMMAND config_validation_test)

  add_executable(parser_robustness_test tests/parser_robustness_test.cpp)
  target_link_libraries(parser_robustness_test PRIVATE market_data_provider)
  add_test(NAME parser_robustness_test COMMAND parser_robustness_test)
endif()

if(MDP_BUILD_BENCHMARKS)
  add_executable(order_book_benchmark benchmarks/order_book_benchmark.cpp)
  target_link_libraries(order_book_benchmark PRIVATE market_data_provider)
endif()
