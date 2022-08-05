set(NAME market_data_subscription)
project(${NAME})
add_compile_definitions(CCAPI_ENABLE_SERVICE_MARKET_DATA)
add_compile_definitions(CCAPI_ENABLE_EXCHANGE_COINBASE)
add_compile_definitions(CCAPI_ENABLE_EXCHANGE_BINANCE)

find_library(LIBZMQ_PATH zmq /usr/local/lib)
find_package(ZeroMQ REQUIRED)
#find_package(OpenSSL REQUIRED)
link_libraries(libzmq.a libsodium.a)

if(NOT DEFINED CLI11_DIR)
    set (CLI11_DIR "${VENDOR_DIR}/CLI11" CACHE STRING "CLI11 git repository")
endif()
MESSAGE(STATUS "CLI11_DIR:" ${CLI11_DIR})
include_directories(${CLI11_DIR}/include)
include_directories(${VENDOR_DIR}/cpp_tools)

add_executable(${NAME} main.cpp)
