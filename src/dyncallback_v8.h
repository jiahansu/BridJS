/*
 * BridJS - Dynamic and blazing-fast native interop for JavaScript.
 * https://github.com/jiahansu/BridJS
 *
 * Copyright (c) 2013-2013, Olivier Chafik (http://ochafik.com/) 
 * and Jia-Han Su (https://github.com/jiahansu/BridJS)
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 * 
 *     * Redistributions of source code must retain the above copyright
 *       notice, this list of conditions and the following disclaimer.
 *     * Redistributions in binary form must reproduce the above copyright
 *       notice, this list of conditions and the following disclaimer in the
 *       documentation and/or other materials provided with the distribution.
 *     * Neither the name of Olivier Chafik nor the
 *       names of its contributors may be used to endorse or promote products
 *       derived from this software without specific prior written permission.
 * 
 * THIS SOFTWARE IS PROVIDED BY OLIVIER CHAFIK, JIA-HAN SU AND CONTRIBUTORS 
 * ``AS IS'' AND ANY * EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED 
 * TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR 
 * PURPOSE ARE * DISCLAIMED. IN NO EVENT SHALL THE REGENTS AND CONTRIBUTORS BE 
 * LIABLE FOR ANY * DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR 
 * CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF 
 * SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS 
 * INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN 
 * CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) 
 * ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
 * POSSIBILITY OF SUCH DAMAGE.
 */
#pragma once
#include <uv.h>
#include "native_function_v8.h"

extern "C" {
#include "dyncall_callback.h"  
}
namespace bridjs {

    class Dyncallback {
    public:
        static void Init(v8::Handle<v8::Object> exports);
    };

    class CallbackWrapper : public bridjs::NativeFunction {
        friend class CallbackTask;
    private:
        v8::Isolate* mpIsolate;
        v8::Persistent<v8::Object> mpCallbackObject;
        uv_mutex_t mMutex;
    public:
        CallbackWrapper(v8::Isolate* isolate, const char returnType, 
                const std::vector<char> &argumentTypes, 
                v8::Persistent<v8::Object> &pCallbackObject);
        char onCallback(DCCallback* cb, DCArgs* args, DCValue* result);
        uv_mutex_t* getMutex();
        v8::Isolate* getIsolate();
        virtual ~CallbackWrapper();
    };

    class CallbackTask {
    private:
        bridjs::CallbackWrapper* mpCallbackWrapper;
        DCCallback* mpDCCallBack;
        DCArgs* mpDCArgs;
        DCValue* mpDCresult;
        uv_cond_t mCond;
        //uv_async_t mAsync;
    public:
        CallbackTask(CallbackWrapper *pCallbackWrapper, DCCallback *pDCCallBack, DCArgs *pDCArgs, DCValue* pDCresult);
        void wait();
        void notify();
        void done();
        //uv_async_t* getAsync();

        static void flushV8Callbacks(uv_async_t *handle);

        virtual ~CallbackTask();
    };

} // namespace dyncallback

