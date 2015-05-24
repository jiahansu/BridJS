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

#include <node.h>
#include <node_object_wrap.h>
#include <v8-util.h>
#include <vector>
#include <memory>
#include <exception>

extern "C" {
#include "dynload.h"
#include "dyncall.h"
}

namespace bridjs {

    class ValueCollection {
    protected:
        v8::Isolate* mpIsolate;
    public:
        ValueCollection(v8::Isolate* pIsolate):mpIsolate(pIsolate){};
        
        virtual v8::Local<v8::Value> get(const uint32_t i) const {
            throw std::runtime_error("Not implement");
        };

        virtual uint32_t length() const {
            throw std::runtime_error("Not implement");
        };

        virtual ~ValueCollection() {
        };
    };

    class ArgumentCollection : public ValueCollection {
    private:
        const v8::FunctionCallbackInfo<v8::Value>* mpArgs;
    public:
        ArgumentCollection(v8::Isolate* pIsolate,const v8::FunctionCallbackInfo<v8::Value>* pArg);
        v8::Local<v8::Value> get(const uint32_t i) const;
        uint32_t length() const;
        ~ArgumentCollection();
    };

    class ArrayCollection : public ValueCollection {
    private:
        const v8::Handle<v8::Array> mArray;
    public:
        ArrayCollection(v8::Isolate* pIsolate,const v8::Handle<v8::Array> arr);
        v8::Local<v8::Value> get(const uint32_t i) const;
        uint32_t length() const;
        ~ArrayCollection();
    };

    class ObjectCollection : public ValueCollection {
    private:
        const v8::Handle<v8::Object> mObject;
        
    public:
        ObjectCollection(v8::Isolate* pIsolate,const v8::Handle<v8::Object> arr);
        v8::Local<v8::Value> get(const uint32_t i) const;
        uint32_t length() const;
        ~ObjectCollection();
    };

    class NativeFunction : public node::ObjectWrap {
    public:
        static void Init(v8::Handle<v8::Object> exports);
        static void New(const v8::FunctionCallbackInfo<v8::Value>& args);
        //static v8::Handle<v8::Value> NewInstance(const void* ptr);
        static void GetReturnType(const v8::FunctionCallbackInfo<v8::Value>& args);
        static void GetArgumentType(const v8::FunctionCallbackInfo<v8::Value>& args);
        static void GetArgumentsLength(const v8::FunctionCallbackInfo<v8::Value>& args);
        static void GetVM(const v8::FunctionCallbackInfo<v8::Value>& args);
        static void GetSymbol(const v8::FunctionCallbackInfo<v8::Value>& args);
        static void Call(const v8::FunctionCallbackInfo<v8::Value>& args);
        static void CallAsync(const v8::FunctionCallbackInfo<v8::Value>& args);

        static const bridjs::NativeFunction* Data(v8::Isolate *pIsolate,v8::Handle<v8::Value> val);
        static NativeFunction* New(void *pSymbol, const char returnType,
                const std::vector<char> &argumentType);

        //DCCallVM* getVM() const;
        void* getSymbol() const;
        char getReturnType() const;
        char getArgumentType(const uint32_t index) const;
        size_t getArgumentLength() const;
    protected:
        static v8::Persistent<v8::Function> constructor;
        //DCCallVM *mpVm;
        void* mpSymbol;
        char mReturnType;
        std::vector<char> mArgumentTypes;


        NativeFunction(void *pSymbol, const char returnType, const std::vector<char> &argumentType);
        virtual ~NativeFunction();


    }; // namespace dyncall

    class AsyncCallTask {
    private:
        v8::Isolate *mpIsolate;
        DCCallVM* mpVM;
        const NativeFunction* mpNativeFunction;
        std::shared_ptr<v8::StdPersistentValueMap<uint32_t,v8::Value>> mpPersistArgs;
        std::shared_ptr<std::vector<std::shared_ptr<std::string>>> mpStringArgs;
        v8::Persistent<v8::Object> mpCallbackObject;
        std::shared_ptr<void> mpData;
    public:
        AsyncCallTask(v8::Isolate *pIsolate, DCCallVM* mpVM, 
                const NativeFunction* mpNativeFunction,
                std::shared_ptr<v8::StdPersistentValueMap<uint32_t,v8::Value>> pPersistArgs, 
                std::shared_ptr<std::vector<std::shared_ptr<std::string>>> pStringArgs,
                v8::Persistent<v8::Object>& pCallbackObject);
        void execute();
        void done();
        v8::Handle<v8::Value> getReturnValue();
        //DCCallVM* getVM() const;
        //const NativeFunction* getNativeFunction() const;
        void setReturnData(std::shared_ptr<void> data);
        virtual ~AsyncCallTask();

    };

}//bridjs