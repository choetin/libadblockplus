/*
 * This file is part of Adblock Plus <https://adblockplus.org/>,
 * Copyright (C) 2006-present eyeo GmbH
 *
 * Adblock Plus is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License version 3 as
 * published by the Free Software Foundation.
 *
 * Adblock Plus is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with Adblock Plus.  If not, see <http://www.gnu.org/licenses/>.
 */

#ifndef ADBLOCK_PLUS_PLATFORM_H
#define ADBLOCK_PLUS_PLATFORM_H

#include <functional>
#include <future>
#include <memory>
#include <mutex>
#include <set>
#include <string>

#include "AppInfo.h"
#include "FilterEngineFactory.h"
#include "IFileSystem.h"
#include "IFilterEngine.h"
#include "IResourceReader.h"
#include "ITimer.h"
#include "IWebRequest.h"
#include "LogSystem.h"
#include "Scheduler.h"

namespace AdblockPlus
{
  struct IV8IsolateProvider;
  class JsEngine;
  class OptionalAsyncExecutor;

  /**
   * AdblockPlus platform is the main component providing access to other
   * modules.
   *
   * It manages the functionality modules, e.g. JsEngine and IFilterEngine as
   * well as allows to correctly work with asynchronous functionality.
   */
  class Platform
  {
  public:
    /**
     * Platform creation parameters.
     *
     * @param logSystem Implementation of log system.
     * @param timer Implementation of timer.
     * @param webRequest Implementation of asynchronous web request.
     * @param fileSystem Implementation of filesystem.
     */
    struct CreationParameters
    {
      LogSystemPtr logSystem;
      TimerPtr timer;
      WebRequestPtr webRequest;
      FileSystemPtr fileSystem;
      std::unique_ptr<IResourceReader> resourceReader;
    };

    /**
     * Callback type invoked when IFilterEngine is created.
     */
    typedef std::function<void(const IFilterEngine&)> OnFilterEngineCreatedCallback;

    /**
     * Platform constructor.
     *
     * When a parameter value is nullptr the corresponding default
     * implementation is chosen.
     */
    explicit Platform(CreationParameters&& creationParameters = CreationParameters());
    virtual ~Platform();

    /**
     * Ensures that JsEngine is constructed. If JsEngine is already present
     * then the parameters are ignored.
     *
     * @param appInfo Information about the app,
     * @param isolate A provider of v8::Isolate, if the value is nullptr then
     *        a default implementation is used.
     */
    void SetUpJsEngine(const AppInfo& appInfo = AppInfo(),
                       std::unique_ptr<IV8IsolateProvider> isolate = nullptr);

    /**
     * Retrieves the `JsEngine` instance. It calls SetUpJsEngine if JsEngine is
     * not initialized yet.
     */
    JsEngine& GetJsEngine();

    /**
     * Ensures that IFilterEngine is constructed. Only the first call is effective.
     *
     * @param parameters optional creation parameters.
     * @param onCreated A callback which is called when IFilterEngine is ready
     *        for use.
     */
    void CreateFilterEngineAsync(
        const FilterEngineFactory::CreationParameters& parameters =
            FilterEngineFactory::CreationParameters(),
        const OnFilterEngineCreatedCallback& onCreated = OnFilterEngineCreatedCallback());

    /**
     * Synchronous equivalent of `CreateFilterEngineAsync`.
     * Internally it blocks and waits for finishing of certain asynchronous
     * operations, please ensure that provided implementation does not lead to
     * a dead lock.
     */
    IFilterEngine& GetFilterEngine();

    typedef std::function<void(ITimer&)> WithTimerCallback;
    virtual void WithTimer(const WithTimerCallback&);

    typedef std::function<void(IFileSystem&)> WithFileSystemCallback;
    virtual void WithFileSystem(const WithFileSystemCallback&);

    typedef std::function<void(IWebRequest&)> WithWebRequestCallback;
    virtual void WithWebRequest(const WithWebRequestCallback&);

    typedef std::function<void(LogSystem&)> WithLogSystemCallback;
    virtual void WithLogSystem(const WithLogSystemCallback&);

    typedef std::function<void(IResourceReader&)> WithResourceReaderCallback;
    virtual void WithResourceReader(const WithResourceReaderCallback&);

  private:
    std::unique_ptr<JsEngine> jsEngine;

  protected:
    LogSystemPtr logSystem;
    TimerPtr timer;
    FileSystemPtr fileSystem;
    WebRequestPtr webRequest;
    std::unique_ptr<IResourceReader> resourceReader;

  private:
    // used for creation and deletion of modules.
    std::mutex modulesMutex;
    std::shared_future<std::unique_ptr<IFilterEngine>> filterEngine;
    std::set<std::string> evaluatedJsSources;
    std::mutex evaluatedJsSourcesMutex;
    std::function<void(const std::string&)> GetEvaluateCallback();
  };

  /**
   * A helper class allowing to construct a default Platform and to obtain
   * the Scheduler used by Platform before the latter is constructed.
   */
  class DefaultPlatformBuilder : public Platform::CreationParameters
  {
  public:
    /**
     * Private
     */
    typedef std::shared_ptr<OptionalAsyncExecutor> AsyncExecutorPtr;

    DefaultPlatformBuilder();

    /**
     * Constructs a default executor for asynchronous tasks. When Platform
     * is being destroyed it starts to ignore new tasks and waits for finishing
     * of already running tasks.
     * @return Scheduler allowing to execute tasks asynchronously.
     */
    Scheduler GetDefaultAsyncExecutor();

    /**
     * Constructs default implementation of `ITimer`.
     */
    void CreateDefaultTimer();

    /**
     * Constructs default implementation of `IFileSystem`.
     * @param basePath A working directory for file system operations.
     */
    void CreateDefaultFileSystem(const std::string& basePath = std::string());

    /**
     * Constructs default implementation of `IWebRequest`.
     */
    void CreateDefaultWebRequest();

    /**
     * Constructs default implementation of `LogSystem`.
     */
    void CreateDefaultLogSystem();

    void CreateDefaultResourceReader();

    /**
     * Constructs Platform with default implementations of platform interfaces
     * when a corresponding field is nullptr and with a default Scheduler.
     */
    std::unique_ptr<Platform> CreatePlatform();

  private:
    AsyncExecutorPtr sharedAsyncExecutor;
    Scheduler defaultScheduler;
  };
}

#endif // ADBLOCK_PLUS_PLATFORM_H
