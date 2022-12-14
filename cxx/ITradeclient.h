#pragma once

#include "rust/cxx.h"

#include <memory>

enum class FixMessageType : uint8_t;
enum class TradingClientType : uint8_t;

struct QuickFixMessage;
struct TradingClientContext;

class ITradeClient {
public:
  //    TradeClient(const std::string &filepath) {};
  //    ~TradeClient() ;

  virtual auto start() const -> void = 0;
  virtual auto stop() const -> void = 0;

  virtual auto put_order(const std::string &symbol, const char side,
                         const double quantity, const double price,
                         const char time_in_force) const
      -> std::unique_ptr<std::string> = 0;
  virtual auto cancel_order(const std::string &order_id) const -> void = 0;
};

auto create_client(
    const TradingClientType type, const std::string &filepath,
    rust::Box<TradingClientContext> ctx,
    rust::Fn<void(const QuickFixMessage, const rust::Box<TradingClientContext> &)>
        inbound_callback) -> std::unique_ptr<ITradeClient>;
