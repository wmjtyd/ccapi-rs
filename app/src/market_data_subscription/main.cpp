#include "ccapi_cpp/ccapi_session.h"

#include "CLI/App.hpp"
#include "CLI/Formatter.hpp"
#include "CLI/Config.hpp"

namespace ccapi {
Logger* Logger::logger = nullptr;  // This line is needed.
class MyEventHandler : public EventHandler {
 public:
  bool processEvent(const Event& event, Session* session) override {
    if (event.getType() == Event::Type::SUBSCRIPTION_DATA) {
      for (const auto& message : event.getMessageList()) {
        if (message.getType() == Message::Type::MARKET_DATA_EVENTS_MARKET_DEPTH)
        {
            std::cout << "msg type: " << Message::typeToString(message.getType()) << " " << std::string("Best bid and ask at ") + UtilTime::getISOTimestamp(message.getTime()) + " are:" << std::endl;
            for (const auto& element : message.getElementList()) {
                const std::map<std::string, std::string>& elementNameValueMap = element.getNameValueMap();
                std::cout << "  " + toString(elementNameValueMap) << std::endl;
            }
        }
        else if (message.getType() == Message::Type::MARKET_DATA_EVENTS_TRADE)
        {
            std::cout << "msg type: " << Message::typeToString(message.getType()) << " " << std::string("trade at ") + UtilTime::getISOTimestamp(message.getTime()) + " are:" << std::endl;
            for (const auto& element : message.getElementList()) {
                const std::map<std::string, std::string>& elementNameValueMap = element.getNameValueMap();
                std::cout << "  " + toString(elementNameValueMap) << std::endl;
            }
        }
//        else if (message.getType() == Message::Type::MARKET_DATA_EVENTS_TRADE)
//        {
//
//        }

      }
    }
    return true;
  }
};
} /* namespace ccapi */
using ::ccapi::MyEventHandler;
using ::ccapi::Session;
using ::ccapi::SessionConfigs;
using ::ccapi::SessionOptions;
using ::ccapi::Subscription;
using ::ccapi::toString;

bool stoped = false;
void signal_handler(int signal)
{
    std::cout << "signal_handler:" << signal << std::endl;
    if (signal == SIGINT || signal == SIGKILL)
    {
        stoped = true;
        std::cout << "stoped" << stoped << std::endl;
    }
}

int main(int argc, char** argv) {
  CLI::App app{"App description"};
  CLI::App* subcom = app.add_subcommand("name", "description");

  std::string filename = "default";
  app.add_option("-f,--file", filename, "A help string");

  subcom->add_option("-f,--file", filename, "A help string");

  CLI11_PARSE(app, argc, argv);

  if ((argc < 2) || ((0 == std::strcmp("-h", argv[1])) || (0 == std::strcmp("--help", argv[1])))) {
    std::cout <<
              "USAGE:\n\n"
              "market_data_subscription binance linear_swap candlestick 60 -c BTCUSDT,ETHUSDT.\n\n"
              "market_data_subscription coinbase linear_swap candlestick 60 -c BTCUSDT,ETHUSDT.\n\n";
//              "  -c --color     Color output.\n";
    exit(0);
  }

  std::string exchaneName = argv[1];

  if (exchaneName != "coinbase" && exchaneName != "binance" )
  {
      std::cout << "not support exchange ("<< exchaneName << ")"<< std::endl;
      exit(0);
  }
  std::signal(SIGINT, signal_handler);
  std::signal(SIGKILL, signal_handler);

  SessionOptions sessionOptions;
  SessionConfigs sessionConfigs;
  MyEventHandler eventHandler;
  Session session(sessionOptions, sessionConfigs, &eventHandler);
//  Subscription subscription("coinbase", "BTC-USD", "MARKET_DEPTH");
//  Subscription subscription("binance", "BTC-USD", "MARKET_DEPTH");
// 1、orderbook
  Subscription subscriptionMarketDepth(exchaneName, "BTC-USD", "MARKET_DEPTH", "MARKET_DEPTH_MAX=20");
// 2、trade
//  Subscription subscriptionTrade("coinbase", "BTC-USD", "TRADE");
// 3、kline
  Subscription subscriptionKline(exchaneName, "BTC-USD", "TRADE", "CONFLATE_INTERVAL_MILLISECONDS=300&CONFLATE_GRACE_PERIOD_MILLISECONDS=0");

  std::vector<Subscription> subscriptionList;
  subscriptionList.push_back(subscriptionMarketDepth);
//  subscriptionList.push_back(subscriptionTrade);
  subscriptionList.push_back(subscriptionKline);
  session.subscribe(subscriptionList);
//  std::this_thread::sleep_for(std::chrono::seconds(10));
  while(true)
  {
//      std::cout << (stoped == true) << std::endl;
      if (stoped == true)
      {
          break;
      }
      std::this_thread::sleep_for(std::chrono::seconds(1));
  }
  std::cout << "session.stop()" << std::endl;
  session.stop();
  std::cout << "Bye" << std::endl;
  return EXIT_SUCCESS;
}
