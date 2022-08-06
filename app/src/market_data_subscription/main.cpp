#include "ccapi_cpp/ccapi_session.h"

#include "CLI/App.hpp"
#include "CLI/Formatter.hpp"
#include "CLI/Config.hpp"

#include "zmq_publish.h"
zmq_pub_sub::Publish gPub("");
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

        for (const auto& element : message.getElementList()) {
            const std::map<std::string, std::string>& elementNameValueMap = element.getNameValueMap();
//            std::cout << "  " + toString(elementNameValueMap) << std::endl;
            gPub.send_message(toString(elementNameValueMap));
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
/*
./src/market_data_subscription/market_data_subscription -e coinbase -t market_depth -c [BTC-ETH,BTC-USDT]
./src/market_data_subscription/market_data_subscription -e coinbase -t trade -c [BTC-ETH,BTC-USDT]
./src/market_data_subscription/market_data_subscription -e coinbase -t candlestick -c [BTC-ETH,BTC-USDT] -i 60
 */
int main(int argc, char** argv) {
  std::vector<std::string> coinpairs;
  std::string exchangeName;
  std::string eventType;
  std::string ipcaddress = "ipc:///tmp/pubsub1.ipc";
  int interval = 60;
  CLI::App app{"app: market_data_subscription"};
  CLI::App* subcom = app.add_subcommand("test", "description");

  app.add_option("-e,--exchane", exchangeName, "exchane name");
  app.add_option("-c,--coin", coinpairs, "coinpairs, -c [BTC-ETH, BTC-USDT]");
  app.add_option("-t,--eventtype", eventType, "eventtype, such as, market_depth,trade, candlestick");
  app.add_option("-i,--interval", interval, "candlestick interval, only for candlestick");
  app.add_option("-p,--ipc", ipcaddress, "ipcaddress, such as ipc:///tmp/pubsub1.ipc");

  CLI11_PARSE(app, argc, argv);
//
//  if ((argc < 2) || ((0 == std::strcmp("-h", argv[1])) || (0 == std::strcmp("--help", argv[1])))) {
//    std::cout <<
//              "USAGE:\n\n"
//              "market_data_subscription binance linear_swap candlestick 60 -c BTCUSDT,ETHUSDT.\n\n"
//              "market_data_subscription coinbase linear_swap candlestick 60 -c BTCUSDT,ETHUSDT.\n\n";
//              "  -c --color     Color output.\n";
//    exit(0);
//  }


  if (exchangeName != "coinbase" && exchangeName != "binance" )
  {
      std::cout << "not support exchange ("<< exchangeName << ")"<< std::endl;
      exit(0);
  }
  std::signal(SIGINT, signal_handler);
  std::signal(SIGKILL, signal_handler);

  gPub = zmq_pub_sub::Publish(ipcaddress);
  printf("start to connect zmq ipc\n");
  int ret = gPub.connect();
  if (ret != 0)
  {
     std::cout << "gPub.connect() failed ("<< ret << ")"<< std::endl;
     exit(0);
  }
  printf("connected zmq ipc\n");

  SessionOptions sessionOptions;
  SessionConfigs sessionConfigs;
  MyEventHandler eventHandler;
  printf("start to create session\n");
  std::cout << "start to create session 2" << std::endl;
  Session session(sessionOptions, sessionConfigs, &eventHandler);
//  Subscription subscription("coinbase", "BTC-USD", "MARKET_DEPTH");
//  Subscription subscription("binance", "BTC-USD", "MARKET_DEPTH");
  std::vector<Subscription> subscriptionList;
// 1、orderbook
  if ("market_depth" == eventType)
  {
      for (std::string& c : coinpairs)
      {
           //"BTC-USD"
          Subscription subscriptionMarketDepth(exchangeName, c, "MARKET_DEPTH", "MARKET_DEPTH_MAX=20");
          subscriptionList.push_back(subscriptionMarketDepth);
          printf("subscription: exchange(%s) type(%s) instrument(%s)\n", exchangeName.c_str(), "MARKET_DEPTH", c.c_str());
      }

  }
  else if ("trade" == eventType)
  {
      // 2、trade
      for (std::string& c : coinpairs)
      {
          //"BTC-USD"
          Subscription subscriptionTrade(exchangeName, c, "TRADE");
          subscriptionList.push_back(subscriptionTrade);
          printf("subscription: exchange(%s) type(%s) instrument(%s)\n", exchangeName.c_str(), "TRADE", c.c_str());
      }

  }
  else if ("candlestick" == eventType)
  {
//      char* option = "CONFLATE_INTERVAL_MILLISECONDS=300&CONFLATE_GRACE_PERIOD_MILLISECONDS=0";
      char option[300];
      memset(option, 0, sizeof(option));
      sprintf(option, "CONFLATE_INTERVAL_MILLISECONDS=%d&CONFLATE_GRACE_PERIOD_MILLISECONDS=0", interval*1000);
// 3、kline
      for (std::string& c : coinpairs)
      {
          Subscription subscriptionKline(exchangeName, c, "TRADE", option);
          subscriptionList.push_back(subscriptionKline);
          printf("subscription: exchange(%s) type(%s) instrument(%s) interval(%d)\n", exchangeName.c_str(), "candlestick", c.c_str(), interval);
      }
  }

  session.subscribe(subscriptionList);
  printf("subscribe done\n");
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
//  gPub.disconnect();
  std::cout << "Bye" << std::endl;
  return EXIT_SUCCESS;
}
