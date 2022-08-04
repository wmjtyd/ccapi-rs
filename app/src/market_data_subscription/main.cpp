#include "ccapi_cpp/ccapi_session.h"
namespace ccapi {
Logger* Logger::logger = nullptr;  // This line is needed.
class MyEventHandler : public EventHandler {
 public:
  bool processEvent(const Event& event, Session* session) override {
    if (event.getType() == Event::Type::SUBSCRIPTION_DATA) {
      for (const auto& message : event.getMessageList()) {
        std::cout << std::string("Best bid and ask at ") + UtilTime::getISOTimestamp(message.getTime()) + " are:" << std::endl;
        for (const auto& element : message.getElementList()) {
          const std::map<std::string, std::string>& elementNameValueMap = element.getNameValueMap();
          std::cout << "  " + toString(elementNameValueMap) << std::endl;
        }
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
    }
}

int main(int argc, char** argv) {
  std::signal(SIGINT, signal_handler);
  std::signal(SIGKILL, signal_handler);

  SessionOptions sessionOptions;
  SessionConfigs sessionConfigs;
  MyEventHandler eventHandler;
  Session session(sessionOptions, sessionConfigs, &eventHandler);
  Subscription subscription("coinbase", "BTC-USD", "MARKET_DEPTH");
//  Subscription subscription("binance", "BTC-USD", "MARKET_DEPTH");

  session.subscribe(subscription);
//  std::this_thread::sleep_for(std::chrono::seconds(10));

  while(true)
  {
      if (stoped)
      {
          break;
      }
  }

  session.stop();
  std::cout << "Bye" << std::endl;
  return EXIT_SUCCESS;
}
