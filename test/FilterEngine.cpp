#include "BaseJsTest.h"

namespace
{
  typedef std::tr1::shared_ptr<AdblockPlus::FilterEngine> FilterEnginePtr;

  class FilterEngineTest : public BaseJsTest
  {
  protected:
    FilterEnginePtr filterEngine;

    void SetUp()
    {
      BaseJsTest::SetUp();
      // TODO: Don't use the default ErrorCallback/WebRequest
      jsEngine->SetErrorCallback(AdblockPlus::ErrorCallbackPtr(new AdblockPlus::DefaultErrorCallback));
      jsEngine->SetWebRequest(AdblockPlus::WebRequestPtr(new AdblockPlus::DefaultWebRequest));
      filterEngine = FilterEnginePtr(new AdblockPlus::FilterEngine(jsEngine));
    }
  };
}

TEST_F(FilterEngineTest, FilterCreation)
{
  AdblockPlus::FilterPtr filter1 = filterEngine->GetFilter("foo");
  ASSERT_EQ(AdblockPlus::Filter::TYPE_BLOCKING, filter1->GetProperty("type", -1));
  AdblockPlus::FilterPtr filter2 = filterEngine->GetFilter("@@foo");
  ASSERT_EQ(AdblockPlus::Filter::TYPE_EXCEPTION, filter2->GetProperty("type", -1));
  AdblockPlus::FilterPtr filter3 = filterEngine->GetFilter("example.com##foo");
  ASSERT_EQ(AdblockPlus::Filter::TYPE_ELEMHIDE, filter3->GetProperty("type", -1));
  AdblockPlus::FilterPtr filter4 = filterEngine->GetFilter("example.com#@#foo");
  ASSERT_EQ(AdblockPlus::Filter::TYPE_ELEMHIDE_EXCEPTION, filter4->GetProperty("type", -1));
  AdblockPlus::FilterPtr filter5 = filterEngine->GetFilter("  foo  ");
  ASSERT_EQ(*filter1, *filter5);
}

TEST_F(FilterEngineTest, FilterProperties)
{
  AdblockPlus::FilterPtr filter = filterEngine->GetFilter("foo");

  ASSERT_EQ("x", filter->GetProperty("stringFoo", "x"));
  ASSERT_EQ(42, filter->GetProperty("intFoo", 42));
  ASSERT_FALSE(filter->GetProperty("boolFoo", false));

  filter->SetProperty("stringFoo", "y");
  filter->SetProperty("intFoo", 24);
  filter->SetProperty("boolFoo", true);
  ASSERT_EQ("y", filter->GetProperty("stringFoo", "x"));
  ASSERT_EQ(24, filter->GetProperty("intFoo", 42));
  ASSERT_TRUE(filter->GetProperty("boolFoo", false));
}

TEST_F(FilterEngineTest, AddRemoveFilters)
{
  ASSERT_EQ(0u, filterEngine->GetListedFilters().size());
  AdblockPlus::FilterPtr filter = filterEngine->GetFilter("foo");
  ASSERT_EQ(0u, filterEngine->GetListedFilters().size());
  filter->AddToList();
  ASSERT_EQ(1u, filterEngine->GetListedFilters().size());
  ASSERT_EQ(*filter, *filterEngine->GetListedFilters()[0]);
  filter->AddToList();
  ASSERT_EQ(1u, filterEngine->GetListedFilters().size());
  ASSERT_EQ(*filter, *filterEngine->GetListedFilters()[0]);
  filter->RemoveFromList();
  ASSERT_EQ(0u, filterEngine->GetListedFilters().size());
  filter->RemoveFromList();
  ASSERT_EQ(0u, filterEngine->GetListedFilters().size());
}

TEST_F(FilterEngineTest, SubscriptionProperties)
{
  AdblockPlus::SubscriptionPtr subscription = filterEngine->GetSubscription("foo");

  ASSERT_EQ("x", subscription->GetProperty("stringFoo", "x"));
  ASSERT_EQ(42, subscription->GetProperty("intFoo", 42));
  ASSERT_FALSE(subscription->GetProperty("boolFoo", false));

  subscription->SetProperty("stringFoo", "y");
  subscription->SetProperty("intFoo", 24);
  subscription->SetProperty("boolFoo", true);
  ASSERT_EQ("y", subscription->GetProperty("stringFoo", "x"));
  ASSERT_EQ(24, subscription->GetProperty("intFoo", 42));
  ASSERT_TRUE(subscription->GetProperty("boolFoo", false));
}

TEST_F(FilterEngineTest, AddRemoveSubscriptions)
{
  ASSERT_EQ(0u, filterEngine->GetListedSubscriptions().size());
  AdblockPlus::SubscriptionPtr subscription = filterEngine->GetSubscription("foo");
  ASSERT_EQ(0u, filterEngine->GetListedSubscriptions().size());
  subscription->AddToList();
  ASSERT_EQ(1u, filterEngine->GetListedSubscriptions().size());
  ASSERT_EQ(*subscription, *filterEngine->GetListedSubscriptions()[0]);
  subscription->AddToList();
  ASSERT_EQ(1u, filterEngine->GetListedSubscriptions().size());
  ASSERT_EQ(*subscription, *filterEngine->GetListedSubscriptions()[0]);
  subscription->RemoveFromList();
  ASSERT_EQ(0u, filterEngine->GetListedSubscriptions().size());
  subscription->RemoveFromList();
  ASSERT_EQ(0u, filterEngine->GetListedSubscriptions().size());
}

TEST_F(FilterEngineTest, SubscriptionUpdates)
{
  AdblockPlus::SubscriptionPtr subscription = filterEngine->GetSubscription("foo");
  ASSERT_FALSE(subscription->IsUpdating());
  subscription->UpdateFilters();
}

TEST_F(FilterEngineTest, Matches)
{
  filterEngine->GetFilter("adbanner.gif")->AddToList();
  filterEngine->GetFilter("@@notbanner.gif")->AddToList();

  AdblockPlus::FilterPtr match1 = filterEngine->Matches("http://example.org/foobar.gif", "IMAGE", "");
  ASSERT_FALSE(match1);

  AdblockPlus::FilterPtr match2 = filterEngine->Matches("http://example.org/adbanner.gif", "IMAGE", "");
  ASSERT_TRUE(match2);
  ASSERT_EQ(AdblockPlus::Filter::TYPE_BLOCKING, match2->GetProperty("type", -1));

  AdblockPlus::FilterPtr match3 = filterEngine->Matches("http://example.org/notbanner.gif", "IMAGE", "");
  ASSERT_TRUE(match3);
  ASSERT_EQ(AdblockPlus::Filter::TYPE_EXCEPTION, match3->GetProperty("type", -1));

  AdblockPlus::FilterPtr match4 = filterEngine->Matches("http://example.org/notbanner.gif", "IMAGE", "");
  ASSERT_TRUE(match4);
  ASSERT_EQ(AdblockPlus::Filter::TYPE_EXCEPTION, match4->GetProperty("type", -1));
}
