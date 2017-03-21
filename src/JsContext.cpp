/*
 * This file is part of Adblock Plus <https://adblockplus.org/>,
 * Copyright (C) 2006-2017 eyeo GmbH
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

#include "JsContext.h"

AdblockPlus::JsContext::JsContext(const JsEnginePtr jsEngine)
    : locker(jsEngine->GetIsolate()), isolateScope(jsEngine->GetIsolate()),
      handleScope(jsEngine->GetIsolate()),
      context(v8::Local<v8::Context>::New(jsEngine->GetIsolate(), *jsEngine->context)),
      contextScope(context)
{
}
