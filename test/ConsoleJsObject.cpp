#include <AdblockPlus.h>
#include <gtest/gtest.h>

class MockErrorCallback : public AdblockPlus::ErrorCallback
{
public:
  std::string lastMessage;

  void operator()(const std::string& message)
  {
    lastMessage = message;
  }
};

TEST(ConsoleJsObjectTest, ErrorInvokesErrorCallback)
{
  MockErrorCallback errorCallback;
  AdblockPlus::JsEngine jsEngine(0, 0, &errorCallback);
  jsEngine.Evaluate("console.error('foo')");
  ASSERT_EQ("foo", errorCallback.lastMessage);
}

TEST(ConsoleJsObjectTest, TraceDoesNothing)
{
  AdblockPlus::JsEngine jsEngine(0, 0, 0);
  jsEngine.Evaluate("console.trace()");
}
