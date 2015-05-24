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
#include "dyncallback_v8.h"
#include "dyncall_v8_utils.h"

#include <cstring>
#include <map>
#include <iostream>
#include <memory>

using namespace v8;
using namespace bridjs;

std::map<DCCallback*, CallbackWrapper*> gValueWrapperMap;
uv_mutex_t gValueWrapperMutex;
uv_thread_t gDefaultThread;

uv_mutex_t gCallbackTaskQueueMutex;
uv_async_t gCallbackQueueAsync;
std::vector<std::shared_ptr<bridjs::CallbackTask >> gCallbackTaskQueue;

char callbackHandler(DCCallback* cb, DCArgs* args, DCValue* result, void* userdata);

void NewCallback(const v8::FunctionCallbackInfo<v8::Value>& args);
void InitCallback(const v8::FunctionCallbackInfo<v8::Value>& args);
void FreeCallback(const v8::FunctionCallbackInfo<v8::Value>& args);


void ArgBool(const v8::FunctionCallbackInfo<v8::Value>& args);
void ArgChar(const v8::FunctionCallbackInfo<v8::Value>& args);
void ArgShort(const v8::FunctionCallbackInfo<v8::Value>& args);
void ArgInt(const v8::FunctionCallbackInfo<v8::Value>& args);
void ArgLong(const v8::FunctionCallbackInfo<v8::Value>& args);
void ArgLongLong(const v8::FunctionCallbackInfo<v8::Value>& args);
void ArgUChar(const v8::FunctionCallbackInfo<v8::Value>& args);
void ArgUShort(const v8::FunctionCallbackInfo<v8::Value>& args);
void ArgUInt(const v8::FunctionCallbackInfo<v8::Value>& args);
void ArgULong(const v8::FunctionCallbackInfo<v8::Value>& args);
void ArgULongLong(const v8::FunctionCallbackInfo<v8::Value>& args);
void ArgFloat(const v8::FunctionCallbackInfo<v8::Value>& args);
void ArgDouble(const v8::FunctionCallbackInfo<v8::Value>& args);
void ArgPointer(const v8::FunctionCallbackInfo<v8::Value>& args);

void bridjs::Dyncallback::Init(v8::Handle<v8::Object> exports) {
    int32_t error;

    gDefaultThread = uv_thread_self();
    memset(&gValueWrapperMutex, 0, sizeof (uv_mutex_t));
    error = uv_mutex_init(&gValueWrapperMutex);

    if (error != 0) {
        std::string message("Fail to init gValueWrapperMutex");
        
        std::cerr <<message<< std::endl;

        throw std::runtime_error(message);
    }

    memset(&gCallbackTaskQueueMutex, 0, sizeof (uv_mutex_t));
    error = uv_mutex_init(&gCallbackTaskQueueMutex);

    if (error != 0) {
        std::string message("Fail to init gCallbackTaskQueueMutex");
        std::cerr <<message<< std::endl;

        throw std::runtime_error(message);
    }

    memset(&gCallbackQueueAsync, 0, sizeof (uv_async_t));

    uv_async_init(uv_default_loop(), &gCallbackQueueAsync, bridjs::CallbackTask::flushV8Callbacks);

    NODE_SET_METHOD(exports, "newCallback", NewCallback);
    NODE_SET_METHOD(exports, "initCallback", InitCallback);
    NODE_SET_METHOD(exports, "freeCallback", FreeCallback);

    NODE_SET_METHOD(exports, "argBool", ArgBool);
    NODE_SET_METHOD(exports, "argChar", ArgChar);
    NODE_SET_METHOD(exports, "argShort", ArgShort);
    NODE_SET_METHOD(exports, "argInt", ArgInt);
    NODE_SET_METHOD(exports, "argLong", ArgLong);
    NODE_SET_METHOD(exports, "argLongLong", ArgLongLong);
    NODE_SET_METHOD(exports, "argUChar", ArgUChar);
    NODE_SET_METHOD(exports, "argUShort", ArgUShort);
    NODE_SET_METHOD(exports, "argUint", ArgUInt);
    NODE_SET_METHOD(exports, "argULong", ArgULong);
    NODE_SET_METHOD(exports, "argULongLong", ArgULongLong);
    NODE_SET_METHOD(exports, "argFloat", ArgFloat);
    NODE_SET_METHOD(exports, "argPointer", ArgPointer);
}

void  NewCallback(const v8::FunctionCallbackInfo<v8::Value>& args) {
    Isolate* isolate = Isolate::GetCurrent(); 
    HandleScope scope(isolate);
    std::stringstream signature;
    std::vector<char> argumentTypes;
    GET_CHAR_ARG(returnType, args, 0);
    GET_ARRAY_ARG(argumentArray, args, 1);
    GET_OBJECT_ARG(callbackObj, args, 2);
    CallbackWrapper* valueWrapper;
    DCCallback* pCallback;
    v8::Persistent<v8::Object> persistentObject(isolate,callbackObj);
            
    for (uint32_t i = 0; i < argumentArray->Length(); ++i) {
        GET_CHAR_VALUE(type, argumentArray->Get(i), i);
        argumentTypes.push_back(type);
        signature << type;
    }
    signature << ')' << returnType;

    valueWrapper = new CallbackWrapper(isolate, returnType, argumentTypes, 
            persistentObject); //GET_POINTER_ARG(void,pUserData,args,2);
    pCallback = dcbNewCallback(signature.str().c_str(), callbackHandler, valueWrapper);

    if (pCallback != NULL) {
        uv_mutex_lock(&gValueWrapperMutex);
        gValueWrapperMap[pCallback] = valueWrapper;
        uv_mutex_unlock(&gValueWrapperMutex);
        args.GetReturnValue().Set(bridjs::Utils::wrapPointer(isolate,pCallback));
    } else {
        delete valueWrapper;
        valueWrapper = NULL;
        THROW_EXCEPTION("Fail to new Callback object");
    }
}

void  InitCallback(const v8::FunctionCallbackInfo<v8::Value>& args) {
    Isolate* isolate = Isolate::GetCurrent(); HandleScope scope(isolate);
    std::stringstream signature;
    std::vector<char> argumentTypes;
    GET_POINTER_ARG(DCCallback, pCallback, args, 0);
    GET_ARRAY_ARG(argumentArray, args, 1);
    GET_CHAR_ARG(returnType, args, 2);
    GET_OBJECT_ARG(callbackObj, args, 3);
    v8::Persistent<Object> persistentCallbackObj(isolate,callbackObj);
    
    for (uint32_t i = 0; i < argumentArray->Length(); ++i) {
        GET_CHAR_VALUE(type, argumentArray->Get(i), i);
        argumentTypes.push_back(type);
        signature << type;
    }
    signature << ')' << returnType;

    if (pCallback != NULL) {
        CallbackWrapper* valueWrapper = new CallbackWrapper(isolate, returnType, 
                argumentTypes, persistentCallbackObj);

        dcbInitCallback(pCallback, signature.str().c_str(), callbackHandler, valueWrapper);

        uv_mutex_lock(&gValueWrapperMutex);
        gValueWrapperMap[pCallback] = valueWrapper;
        uv_mutex_unlock(&gValueWrapperMutex);

        args.GetReturnValue().Set(v8::Undefined(isolate));
    } else {
        THROW_EXCEPTION("pCallback was NULL");
    }
}

void  FreeCallback(const v8::FunctionCallbackInfo<v8::Value>& args) {
    Isolate* isolate = Isolate::GetCurrent(); HandleScope scope(isolate);
    GET_POINTER_ARG(DCCallback, pCallback, args, 0);

    if (pCallback != NULL) {
        uv_mutex_lock(&gValueWrapperMutex);

        std::map<DCCallback*, CallbackWrapper*>::iterator iterator = gValueWrapperMap.find(pCallback);

        if (iterator != gValueWrapperMap.end()) {
            CallbackWrapper* valueWrapper = iterator->second;

            if (valueWrapper != NULL) {
                delete valueWrapper;
                valueWrapper = NULL;
            }
        }
        uv_mutex_unlock(&gValueWrapperMutex);

        dcbFreeCallback(pCallback);
        args.GetReturnValue().Set(v8::Undefined(isolate));
    } else {
        THROW_EXCEPTION("pCallback was NULL");
    }
}

void  ArgBool(const v8::FunctionCallbackInfo<v8::Value>& args) {
    Isolate* isolate = Isolate::GetCurrent(); HandleScope scope(isolate);
    GET_POINTER_ARG(DCArgs, pArgs, args, 0);

    args.GetReturnValue().Set(WRAP_BOOL(dcbArgBool(pArgs)));
}

void  ArgChar(const v8::FunctionCallbackInfo<v8::Value>& args) {
    Isolate* isolate = Isolate::GetCurrent(); HandleScope scope(isolate);
    GET_POINTER_ARG(DCArgs, pArgs, args, 0);

    args.GetReturnValue().Set(bridjs::Utils::toV8String(isolate,dcbArgChar(pArgs)));
}

void  ArgShort(const v8::FunctionCallbackInfo<v8::Value>& args) {
    Isolate* isolate = Isolate::GetCurrent(); HandleScope scope(isolate);
    GET_POINTER_ARG(DCArgs, pArgs, args, 0);

    args.GetReturnValue().Set(WRAP_SHORT(dcbArgShort(pArgs)));
}

void ArgInt(const v8::FunctionCallbackInfo<v8::Value>& args) {
    Isolate* isolate = Isolate::GetCurrent(); HandleScope scope(isolate);
    GET_POINTER_ARG(DCArgs, pArgs, args, 0);

    args.GetReturnValue().Set(WRAP_INT(dcbArgInt(pArgs)));
}

void  ArgLong(const v8::FunctionCallbackInfo<v8::Value>& args) {
    Isolate* isolate = Isolate::GetCurrent(); HandleScope scope(isolate);
    GET_POINTER_ARG(DCArgs, pArgs, args, 0);

    args.GetReturnValue().Set(WRAP_LONG(dcbArgLong(pArgs)));
}

void  ArgLongLong(const v8::FunctionCallbackInfo<v8::Value>& args) {
    Isolate* isolate = Isolate::GetCurrent(); HandleScope scope(isolate);
    GET_POINTER_ARG(DCArgs, pArgs, args, 0);

    args.GetReturnValue().Set(WRAP_LONGLONG(dcbArgLongLong(pArgs)));
}

void  ArgUChar(const v8::FunctionCallbackInfo<v8::Value>& args) {
    Isolate* isolate = Isolate::GetCurrent(); HandleScope scope(isolate);
    GET_POINTER_ARG(DCArgs, pArgs, args, 0);

    args.GetReturnValue().Set(bridjs::Utils::toV8String(isolate,dcbArgUChar(pArgs)));
}

void  ArgUShort(const v8::FunctionCallbackInfo<v8::Value>& args) {
    Isolate* isolate = Isolate::GetCurrent(); HandleScope scope(isolate);
    GET_POINTER_ARG(DCArgs, pArgs, args, 0);

    args.GetReturnValue().Set(WRAP_USHORT(dcbArgUShort(pArgs)));
}

void ArgUInt(const v8::FunctionCallbackInfo<v8::Value>& args) {
    Isolate* isolate = Isolate::GetCurrent(); HandleScope scope(isolate);
    GET_POINTER_ARG(DCArgs, pArgs, args, 0);

    args.GetReturnValue().Set(WRAP_UINT(dcbArgUInt(pArgs)));
}

void ArgULong(const v8::FunctionCallbackInfo<v8::Value>& args) {
    Isolate* isolate = Isolate::GetCurrent(); HandleScope scope(isolate);
    GET_POINTER_ARG(DCArgs, pArgs, args, 0);

    args.GetReturnValue().Set(WRAP_ULONG(dcbArgULong(pArgs)));
}

void ArgULongLong(const v8::FunctionCallbackInfo<v8::Value>& args) {
    Isolate* isolate = Isolate::GetCurrent(); HandleScope scope(isolate);
    GET_POINTER_ARG(DCArgs, pArgs, args, 0);

    args.GetReturnValue().Set(WRAP_ULONGLONG(dcbArgULongLong(pArgs)));
}

void ArgFloat(const v8::FunctionCallbackInfo<v8::Value>& args) {
    Isolate* isolate = Isolate::GetCurrent(); HandleScope scope(isolate);
    GET_POINTER_ARG(DCArgs, pArgs, args, 0);

    args.GetReturnValue().Set(WRAP_FLOAT(dcbArgFloat(pArgs)));
}

void ArgDouble(const v8::FunctionCallbackInfo<v8::Value>& args) {
    Isolate* isolate = Isolate::GetCurrent(); HandleScope scope(isolate);
    GET_POINTER_ARG(DCArgs, pArgs, args, 0);

    args.GetReturnValue().Set(WRAP_DOUBLE(dcbArgDouble(pArgs)));
}

void ArgPointer(const v8::FunctionCallbackInfo<v8::Value>& args) {
    Isolate* isolate = Isolate::GetCurrent(); HandleScope scope(isolate);
    GET_POINTER_ARG(DCArgs, pArgs, args, 0);

    args.GetReturnValue().Set(WRAP_POINTER(dcbArgPointer(pArgs)));
}

std::shared_ptr<v8::Local<Value >> pullArgs(Isolate* isolate, DCCallback* cb, DCArgs* args, CallbackWrapper* pCallbackWrapper) {
    //Isolate* isolate = Isolate::GetCurrent(); HandleScope scope(isolate);
    const size_t length = pCallbackWrapper->getArgumentLength();
    std::shared_ptr < v8::Local < Value >> argv(new v8::Local<Value>[length], ArrayDeleter < v8::Local < Value >> ());

    for (uint32_t i = 0; i < length; ++i) {

        switch (pCallbackWrapper->getArgumentType(i)) {
            case DC_SIGCHAR_BOOL:
            {
                argv.get()[i] = WRAP_BOOL(dcbArgBool(args));
            }
                break;
            case DC_SIGCHAR_UCHAR:
            {
                argv.get()[i] = WRAP_UCHAR(dcbArgUChar(args));
            }
                break;
            case DC_SIGCHAR_CHAR:
            {
                argv.get()[i] = WRAP_CHAR(dcbArgChar(args));
            }
                break;
            case DC_SIGCHAR_SHORT:
            {
                argv.get()[i] = WRAP_SHORT(dcbArgShort(args));
            }
                break;
            case DC_SIGCHAR_USHORT:
            {
                argv.get()[i] = WRAP_USHORT(dcbArgUShort(args));
            }
                break;
            case DC_SIGCHAR_INT:
            {
                argv.get()[i] = WRAP_INT(dcbArgInt(args));
            }
                break;
            case DC_SIGCHAR_UINT:
            {
                argv.get()[i] = WRAP_UINT(dcbArgUInt(args));
            }
                break;
            case DC_SIGCHAR_LONG:
            {
                argv.get()[i] = WRAP_LONG(dcbArgLong(args));
            }
                break;
            case DC_SIGCHAR_ULONG:
            {
                argv.get()[i] = WRAP_ULONG(dcbArgULong(args));
            }
                break;
            case DC_SIGCHAR_LONGLONG:
            {
                argv.get()[i] = WRAP_LONGLONG(dcbArgLongLong(args));
            }
                break;
            case DC_SIGCHAR_ULONGLONG:
            {
                argv.get()[i] = WRAP_ULONGLONG(dcbArgULongLong(args));
            }
                break;
            case DC_SIGCHAR_FLOAT:
            {
                argv.get()[i] = WRAP_FLOAT(dcbArgFloat(args));
            }
                break;
            case DC_SIGCHAR_DOUBLE:
            {
                argv.get()[i] = WRAP_DOUBLE(dcbArgDouble(args));
            }
                break;
            case DC_SIGCHAR_STRING:
            {
                argv.get()[i] = WRAP_STRING((const char*) dcbArgPointer(args));
            }
                break;
            case DC_SIGCHAR_POINTER:
            {
                argv.get()[i] = WRAP_POINTER(dcbArgPointer(args));
            }
                break;
            case DC_SIGCHAR_STRUCT:
            {
                std::cerr << "Not implement" << std::endl;
                argv.get()[i] = WRAP_POINTER(dcbArgPointer(args));
                //return v8::Exception::TypeError(v8::String::New("Not implement"));
            }
                break;
        }
    }

    return argv;
}

void setReturnValue(Isolate* isolate,DCValue* value, Local<Value> returnValue, CallbackWrapper* pCallbackWrapper) {
    //DCCallVM *vm = nativeFunction->getVM();
    v8::EscapableHandleScope scope(isolate);

    switch (pCallbackWrapper->getReturnType()) {
        case DC_SIGCHAR_VOID:
        {
            //dothing
        }
            break;
        case DC_SIGCHAR_BOOL:
        {
            GET_BOOL_VALUE(rValue, returnValue, 0);
            value->B = rValue;
        }
            break;
        case DC_SIGCHAR_UCHAR:
        {
            GET_CHAR_VALUE(rValue, returnValue, 0);
            value->C = rValue;
        }
            break;
        case DC_SIGCHAR_CHAR:
        {
            GET_CHAR_VALUE(rValue, returnValue, 0);
            value->c = rValue;
        }
            break;
        case DC_SIGCHAR_SHORT:
        {
            GET_INT32_VALUE(rValue, returnValue, 0);
            value->s = static_cast<DCushort> (rValue);
        }
            break;
        case DC_SIGCHAR_USHORT:
        {
            GET_UINT32_VALUE(rValue, returnValue, 0);
            value->S = static_cast<DCshort> (rValue);
        }
            break;
        case DC_SIGCHAR_INT:
        {
            GET_INT32_VALUE(rValue, returnValue, 0);
            value->i = (rValue);
        }
            break;
        case DC_SIGCHAR_UINT:
        {
            GET_UINT32_VALUE(rValue, returnValue, 0);
            value->I = (rValue);
        }
            break;
        case DC_SIGCHAR_LONG:
        {
            GET_INT64_VALUE(rValue, returnValue, 0);
            value->j = static_cast<DClong> (rValue);
        }
            break;
        case DC_SIGCHAR_ULONG:
        {
            GET_INT64_VALUE(rValue, returnValue, 0);
            value->J = static_cast<DCulong> (rValue);
        }
            break;
        case DC_SIGCHAR_LONGLONG:
        {
            GET_INT64_VALUE(rValue, returnValue, 0);
            value->l = static_cast<DClonglong> (rValue);
        }
            break;
        case DC_SIGCHAR_ULONGLONG:
        {
            GET_INT64_VALUE(rValue, returnValue, 0);
            value->L = static_cast<DClonglong> (rValue);
        }
            break;
        case DC_SIGCHAR_FLOAT:
        {
            GET_FLOAT_VALUE(rValue, returnValue, 0);
            value->f = (rValue);
        }
            break;
        case DC_SIGCHAR_DOUBLE:
        {
            GET_DOUBLE_VALUE(rValue, returnValue, 0);
            value->d = (rValue);
        }
            break;
        case DC_SIGCHAR_STRING:
        {
            GET_POINTER_VALUE(const char, rValue, returnValue, 0);
            value->Z = rValue;
        }
            break;
        case DC_SIGCHAR_POINTER:
        {
            GET_POINTER_VALUE(void, rValue, returnValue, 0);
            value->p = (rValue);
        }
            break;
        case DC_SIGCHAR_STRUCT:
        {
            std::cerr << "Not implement" << std::endl;
            GET_POINTER_VALUE(void, rValue, returnValue, 0);
            value->p = (rValue);
        }
            break;

        default:
            std::stringstream message;
            message << "Unknown returnType: " << pCallbackWrapper->getReturnType() << std::endl;
            //throw std::runtime_error(message.str());
            //return v8::Exception::TypeError(v8::String::New(message.str().c_str()));
    }

    //return scope.Escape(v8::Null());
}

char callbackHandler(DCCallback* cb, DCArgs* args, DCValue* result, void* userdata) {
    CallbackWrapper *pCallback = static_cast<CallbackWrapper*> (userdata);

    if (pCallback != NULL) {
        pCallback->onCallback(cb, args, result);

        return pCallback->getReturnType();
    } else {
        std::cerr << "Fail to cast data pointer to CallbackWrapper" << std::endl;

        return DC_SIGCHAR_VOID;
    }
}

void invokeV8Callback(uv_async_t *handle, int status /*UNUSED*/) {
    CallbackTask *pCallTask = static_cast<CallbackTask*> (handle->data);

    if (pCallTask != NULL) {
        pCallTask->done();
    } else {
        std::cerr << "Fail to cast data pointer to CallbackTask" << std::endl;
    }
}

void bridjs::CallbackTask::flushV8Callbacks(uv_async_t *handle) {
    std::shared_ptr<CallbackTask> pCallbackTask;
    std::vector < std::shared_ptr < CallbackTask >> ::iterator iterator;
    do {
        pCallbackTask = NULL;
        uv_mutex_lock(&gCallbackTaskQueueMutex);
        try {
            if (!gCallbackTaskQueue.empty()) {
                iterator = gCallbackTaskQueue.begin();
                pCallbackTask = *iterator;
                gCallbackTaskQueue.erase(iterator);
            }
        } catch (...) {
            std::cerr << "Unknown error to call gCallbackTaskQueue.pop_back()" << std::endl;
        }
        uv_mutex_unlock(&gCallbackTaskQueueMutex);

        if (pCallbackTask != NULL) {
            pCallbackTask->done();
        } else {
            break;
        }

    } while (true);
}

void closeAsyncCallback(uv_handle_t *handle) {
    CallbackTask *pCallTask = static_cast<CallbackTask*> (handle->data);

    if (pCallTask != NULL) {
        //std::cout<<pCallTask<<std::endl;
        delete pCallTask;
    } else {
        std::cerr << "Fail to cast data pointer to CallbackTask" << std::endl;
    }
}

bridjs::CallbackWrapper::CallbackWrapper(v8::Isolate* isolate, const char returnType, 
        const std::vector<char> &argumentTypes, v8::Persistent<v8::Object> &pCallbackObject) : 
NativeFunction(NULL, returnType, argumentTypes),mpIsolate(isolate), mpCallbackObject(isolate,pCallbackObject) {
    int32_t error;
    memset(&mMutex, 0, sizeof (uv_mutex_t));
    error = uv_mutex_init(&mMutex);

    //std::cout<<returnType<<std::endl<<argumentTypes.size()<<std::endl;

    if (error != 0) {
        std::ostringstream message;

        message << "bridjs::CallbackWrapper::CallbackWrapper() => Fail to init mMutex, error = " << error;
		std::cerr << message.str() << std::endl;

        throw std::runtime_error(message.str());
    }

}

char bridjs::CallbackWrapper::onCallback(DCCallback* cb, DCArgs* args, DCValue* result) {
    try {
        uv_mutex_lock(&mMutex);

        std::shared_ptr<CallbackTask> task(new CallbackTask(this, cb, args, result));
        //uv_work_t *req = new uv_work_t;

        //req->data = task;

        uv_mutex_lock(&gCallbackTaskQueueMutex);
        try {
            gCallbackTaskQueue.push_back(task);
        } catch (...) {
            std::cerr << "Unknown error to call gCallbackTaskQueue.push_back()" << std::endl;
        }
        uv_mutex_unlock(&gCallbackTaskQueueMutex);

        if (uv_thread_self() == gDefaultThread) {
            bridjs::CallbackTask::flushV8Callbacks(NULL);
            //invokeV8Callback(task->getAsync(),0);
        } else {
            //uv_queue_work(uv_default_loop(),req,NULL,(uv_after_work_cb)invokeV8Callback);

            uv_async_send(&gCallbackQueueAsync);
            //std::cout<<"1111111111111"<<std::endl;	
            task->wait();
            //std::cout<<"2222222222222"<<std::endl;	
        }


        //req->data = NULL;
        //delete req;
        /*Delete pTask object after close async object*/
        //uv_close((uv_handle_t*) task->getAsync(),closeAsyncCallback);
        //req = NULL;
        //task = NULL;
    } catch (...) {
        std::cerr << "bridjs::CallbackWrapper::onCallback=>Unknown exception" << std::endl;
    }

    uv_mutex_unlock(&mMutex);

    return this->mReturnType;
}

uv_mutex_t* CallbackWrapper::getMutex() {
    return &this->mMutex;
}

v8::Isolate* CallbackWrapper::getIsolate(){
    return this->mpIsolate;
}

bridjs::CallbackWrapper::~CallbackWrapper() {
    uv_mutex_destroy(&mMutex);
    this->mpCallbackObject.Reset();
}

CallbackTask::CallbackTask(CallbackWrapper *pCallbackWrapper, DCCallback *pDCCallBack, DCArgs *pDCArgs, DCValue* pDCresult) : mpCallbackWrapper(pCallbackWrapper),
mpDCCallBack(pDCCallBack), mpDCArgs(pDCArgs), mpDCresult(pDCresult) {
    int32_t error;

    memset(&mCond, 0, sizeof (uv_cond_t));
    //memset(&mAsync,0,sizeof(uv_async_t));

    //uv_async_init(uv_default_loop(),&mAsync,invokeV8Callback);

    //mAsync.data = this;

    error = uv_cond_init(&mCond);

    if (error != 0) {
        std::ostringstream message;

        message << "CallbackTask::CallbackTask() => Fail to init mMutex, error = " << error;
		std::cerr << message.str() << std::endl;

        throw std::runtime_error(message.str());
    }
}

/*
uv_async_t* CallbackTask::getAsync(){
        return &mAsync;
}*/

void CallbackTask::wait() {
    uv_cond_wait(&mCond, this->mpCallbackWrapper->getMutex());
}

void CallbackTask::notify() {
    uv_cond_signal(&mCond);
}

void CallbackTask::done() {
    Isolate* isolate = this->mpCallbackWrapper->getIsolate(); 
    v8::HandleScope scope(isolate);
    
    std::shared_ptr < v8::Local < Value >> argv = pullArgs(isolate,this->mpDCCallBack, this->mpDCArgs, this->mpCallbackWrapper);
    v8::Local<v8::Object> callbackObject = v8::Local<v8::Object>::New(isolate, this->mpCallbackWrapper->mpCallbackObject); 
    v8::Local<Value> onDoneValue = callbackObject->GetRealNamedProperty(v8::String::NewFromUtf8(isolate,"onDone"));

    if (onDoneValue->IsFunction()) {
        v8::Local<v8::Function> onDoneFunction = v8::Local<Function>::Cast(onDoneValue);
        v8::Local<v8::Value> returnValue = onDoneFunction->Call(callbackObject,
                static_cast<int32_t> (this->mpCallbackWrapper->getArgumentLength()),
                argv.get());

        setReturnValue(isolate,this->mpDCresult, returnValue, this->mpCallbackWrapper);
    } else {
        std::cerr << "Illegal callback object: " << (*v8::String::Utf8Value( callbackObject->ToString())) << std::endl;
    }
    //std::cout<<"43333333"<<std::endl;	
    this->notify();
}

CallbackTask::~CallbackTask() {
    uv_cond_destroy(&mCond);
    //std::cout<<"~CallbackTask()"<<std::endl;
}
