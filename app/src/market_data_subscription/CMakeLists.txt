set(NAME market_data_subscription)
project(${NAME})
add_compile_definitions(CCAPI_ENABLE_SERVICE_MARKET_DATA)
add_compile_definitions(CCAPI_ENABLE_EXCHANGE_COINBASE)
add_compile_definitions(CCAPI_ENABLE_EXCHANGE_BINANCE)

if(NOT DEFINED CLI11_DIR)
    set (CLI11_DIR "${VENDOR_DIR}/CLI11" CACHE STRING "CLI11 git repository")
endif()
MESSAGE(STATUS "CLI11_DIR:" ${CLI11_DIR})
include_directories(${CLI11_DIR}/include)

add_executable(${NAME} main.cpp)
