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
#include <uv.h>
#include "native_function_v8.h"
#include "dyncall_v8_utils.h"
#include <iostream>

extern "C" {
#include "dyncall.h"
#include "dyncall_signature.h"
}

using namespace bridjs;
using namespace v8;
using namespace node;

Persistent<v8::Function> bridjs::NativeFunction::constructor;

Persistent<v8::String> ARGUMENTS_NAME;

ArgumentCollection::ArgumentCollection(v8::Isolate* pIsolate,
        const v8::FunctionCallbackInfo<v8::Value>* pArgs) :ValueCollection(pIsolate), 
        mpArgs(pArgs) {

};

v8::Local<v8::Value> ArgumentCollection::get(const uint32_t i) const {
    return (*this->mpArgs)[i];
}

uint32_t ArgumentCollection::length() const {
    return this->mpArgs->Length();
}

ArgumentCollection::~ArgumentCollection() {

}

ArrayCollection::ArrayCollection(v8::Isolate* pIsolate,const v8::Handle<v8::Array> arr) :
    ValueCollection(pIsolate),  mArray(arr) {

}

v8::Local<v8::Value> ArrayCollection::get(const uint32_t i) const {
    return this->mArray->Get(i);
}

uint32_t ArrayCollection::length() const {
    return this->mArray->Length();
}

ArrayCollection::~ArrayCollection() {

}

ObjectCollection::ObjectCollection(v8::Isolate* pIsolate, const v8::Handle<v8::Object> arr) :
    ValueCollection(pIsolate), mObject(arr) {

}

v8::Local<v8::Value> ObjectCollection::get(const uint32_t i) const {
    if (this->mObject->Has(i)) {
        return this->mObject->Get(i);
    } else {
        std::stringstream message;
        message << "Invalid number of arguments, fail to get argument by index " << i 
                << " from " << *v8::String::Utf8Value(this->mObject) << std::endl;
        throw std::runtime_error(message.str());
    }

}

uint32_t ObjectCollection::length() const {
    return this->mObject->GetRealNamedProperty(v8::String::NewFromUtf8(mpIsolate,"length"))->ToUint32()->Value();
}

ObjectCollection::~ObjectCollection() {

}

void pushArgs(v8::Isolate* isolate,DCCallVM *vm, const bridjs::NativeFunction *nativeFunction,
        const ValueCollection* args, const uint32_t offset,
        std::shared_ptr<v8::StdPersistentValueMap<uint32_t,v8::Value>> pPersistArgs, 
        std::shared_ptr<std::vector<std::shared_ptr<std::string>>> pStringArgs,
        v8::Local<Value>& returnValue) {
    
    uint32_t i;
    const size_t length = nativeFunction->getArgumentLength();

    for (uint32_t k = 0; k < length; ++k) {
        i = k + offset;
        
        switch (nativeFunction->getArgumentType(k)) {
            case DC_SIGCHAR_BOOL:
            {
                GET_BOOL_VALUE(val, args->get(i), i);
                dcArgBool(vm, val);
            }
                break;
            case DC_SIGCHAR_UCHAR:
            case DC_SIGCHAR_CHAR:
            {
                GET_CHAR_VALUE(val, args->get(i), i);
                dcArgChar(vm, val);
            }
                break;
            case DC_SIGCHAR_SHORT:
            case DC_SIGCHAR_USHORT:
            {
                GET_INT32_VALUE(val, args->get(i), i);
                dcArgShort(vm, static_cast<DCshort> (val));
            }
                break;
            case DC_SIGCHAR_INT:
            case DC_SIGCHAR_UINT:
            {
                GET_INT32_VALUE(val, args->get(i), i);
                dcArgInt(vm, (val));
            }
                break;
            case DC_SIGCHAR_LONG:
            case DC_SIGCHAR_ULONG:
            {
                GET_INT64_VALUE(val, args->get(i), i);
                dcArgLong(vm, static_cast<DClong> (val));
            }
                break;
            case DC_SIGCHAR_LONGLONG:
            case DC_SIGCHAR_ULONGLONG:
            {
                GET_INT64_VALUE(val, args->get(i), i);
                dcArgLongLong(vm, static_cast<DClonglong> (val));
            }
                break;
            case DC_SIGCHAR_FLOAT:
            {
                GET_FLOAT_VALUE(val, args->get(i), i);
                dcArgFloat(vm, val);
            }
                break;
            case DC_SIGCHAR_DOUBLE:
            {
                GET_DOUBLE_VALUE(val, args->get(i), i);
                dcArgDouble(vm, (val));
            }
            break;
            case DC_SIGCHAR_STRING:
            {
                if(node::Buffer::HasInstance(args->get(i))){
                    void* ptr = node::Buffer::Data(args->get(i));
                    
                    pPersistArgs->Set(i,args->get(i));
                    dcArgPointer(vm, ptr);
                }else{    
                    GET_STRING_VALUE(val, args->get(i), i);
                    
                    if ((*val)==NULL || val->IsNull()) {
                        dcArgPointer(vm, NULL);
                    }else {
                        std::shared_ptr<std::string> pString = 
                                std::make_shared<std::string>(*v8::String::Utf8Value(val));
                        //std::string string(*v8::String::Utf8Value(args->get(i)));
                        pStringArgs->push_back(pString);
                        //std::cout << "String: :" <<(*pString)<< std::endl;

                        dcArgPointer(vm, (void*) pString->c_str());
                    }
                }

            }
                break;
            case DC_SIGCHAR_POINTER:
            {
                GET_POINTER_VALUE(void, val, args->get(i), i);
                pPersistArgs->Set(i,args->get(i));
                dcArgPointer(vm, val);
            }
                break;
            case DC_SIGCHAR_STRUCT:
            {

                returnValue =  v8::Exception::TypeError(v8::String::NewFromUtf8(isolate,"Not implement"));
            }
                break;
        }
    }

    returnValue =  v8::Null(isolate);
}

std::shared_ptr<void> callByType(DCCallVM *vm, const bridjs::NativeFunction *nativeFunction) {
    //DCCallVM *vm = nativeFunction->getVM();
    void *pSymbol = nativeFunction->getSymbol();
    std::shared_ptr<void> data = NULL;

    switch (nativeFunction->getReturnType()) {
        case DC_SIGCHAR_VOID:
        {
            dcCallVoid(vm, pSymbol);
            data = NULL;
        }
            break;
        case DC_SIGCHAR_BOOL:
        {
            data = std::shared_ptr<void>(new DCbool(dcCallBool(vm, pSymbol)));
        }
            break;
        case DC_SIGCHAR_UCHAR:
        {
            data = std::shared_ptr<void>(new DCuchar(dcCallChar(vm, pSymbol)));
        }
            break;
        case DC_SIGCHAR_CHAR:
        {
            data = std::shared_ptr<void>(new DCchar(dcCallChar(vm, pSymbol)));
        }
            break;
        case DC_SIGCHAR_SHORT:
        {
            data = std::shared_ptr<void>(new DCshort(dcCallShort(vm, pSymbol)));
        }
            break;
        case DC_SIGCHAR_USHORT:
        {
            data = std::shared_ptr<void>(new DCushort(dcCallShort(vm, pSymbol)));
        }
            break;
        case DC_SIGCHAR_INT:
        {
            data = std::shared_ptr<void>(new DCint(dcCallInt(vm, pSymbol)));
        }
            break;
        case DC_SIGCHAR_UINT:
        {
            data = std::shared_ptr<void>(new DCuint(dcCallInt(vm, pSymbol)));
        }
            break;
        case DC_SIGCHAR_LONG:
        {
            data = std::shared_ptr<void>(new DClong(dcCallLong(vm, pSymbol)));
        }
            break;
        case DC_SIGCHAR_ULONG:
        {
            data = std::shared_ptr<void>(new DCulong(dcCallLong(vm, pSymbol)));
        }
            break;
        case DC_SIGCHAR_LONGLONG:
        {
            data = std::shared_ptr<void>(new DClonglong(dcCallLongLong(vm, pSymbol)));
        }
            break;
        case DC_SIGCHAR_ULONGLONG:
        {
            data = std::shared_ptr<void>(new DCulonglong(dcCallLongLong(vm, pSymbol)));
        }
            break;
        case DC_SIGCHAR_FLOAT:
        {
            data = std::shared_ptr<void>(new DCfloat(dcCallFloat(vm, pSymbol)));
        }
            break;
        case DC_SIGCHAR_DOUBLE:
        {
            data = std::shared_ptr<void>(new DCdouble(dcCallDouble(vm, pSymbol)));
        }
            break;
        case DC_SIGCHAR_STRING:
        case DC_SIGCHAR_POINTER:
        {
            data = std::shared_ptr<void>(new DCpointer(dcCallPointer(vm, pSymbol)));
        }
            break;
        case DC_SIGCHAR_STRUCT:
        {
            std::cerr << "Not implement" << std::endl;
            data = std::shared_ptr<void*>(NULL);
        }
            break;

        default:
            std::stringstream message;
            message << "Unknown returnType: " << nativeFunction->getReturnType() << std::endl;
            throw std::runtime_error(message.str());
            //return v8::Exception::TypeError(v8::String::New(message.str().c_str()));
    }

    return data;
}

void executeCallAsync(uv_work_t *req) {
    AsyncCallTask *pAsyncCall = static_cast<AsyncCallTask*> (req->data);
    //std::cout<<"Got it"<<std::endl;
    if (pAsyncCall != NULL) {
        pAsyncCall->execute();
    } else {
        std::cerr << "Fail to cast data pointer to AsyncCall" << std::endl;
    }

    //std::cout<<"Got it"<<std::endl;
}

void afterCallAsync(uv_work_t *req) {
    AsyncCallTask *pAsyncCall = static_cast<AsyncCallTask*> (req->data);

    if (pAsyncCall != NULL) {
        pAsyncCall->done();
        /*Do not delete req->data, it wil not call destrcutor*/
        delete pAsyncCall;
        pAsyncCall = NULL;
        req->data = NULL;
    } else {
        std::cerr << "Fail to cast data pointer to AsyncCall" << std::endl;
    }

    delete req;

    //std::cout<<"Got it2"<<std::endl;
}

void bridjs::NativeFunction::Init(v8::Handle<v8::Object> exports) {
    Isolate* isolate = Isolate::GetCurrent();
    
    ARGUMENTS_NAME.Reset(isolate,v8::String::NewFromUtf8(isolate,"Arguments"));

    // Prepare constructor template
    Local<FunctionTemplate> tpl = FunctionTemplate::New(isolate,New);

    tpl->SetClassName(v8::String::NewFromUtf8(isolate,"NativeFunction"));
    tpl->InstanceTemplate()->SetInternalFieldCount(5);
    // Prototype
    /*
    tpl->PrototypeTemplate()->Set(v8::String::NewFromUtf8(isolate,"getVM"),
            FunctionTemplate::New(GetVM)->GetFunction(), ReadOnly);*/
    tpl->PrototypeTemplate()->Set(v8::String::NewFromUtf8(isolate,"getSymbolName"),
            FunctionTemplate::New(isolate,GetSymbol)->GetFunction(), ReadOnly);
    tpl->PrototypeTemplate()->Set(v8::String::NewFromUtf8(isolate,"getReturnType"),
            FunctionTemplate::New(isolate,GetReturnType)->GetFunction(), ReadOnly);
    tpl->PrototypeTemplate()->Set(v8::String::NewFromUtf8(isolate,"getArgumentType"),
            FunctionTemplate::New(isolate,GetArgumentType)->GetFunction(), ReadOnly);
    tpl->PrototypeTemplate()->Set(v8::String::NewFromUtf8(isolate,"getArgumentsLength"),
            FunctionTemplate::New(isolate,GetArgumentsLength)->GetFunction(), ReadOnly);
    tpl->PrototypeTemplate()->Set(v8::String::NewFromUtf8(isolate,"call"),
            FunctionTemplate::New(isolate,Call)->GetFunction(), ReadOnly),
            tpl->PrototypeTemplate()->Set(v8::String::NewFromUtf8(isolate,"callAsync"),
            FunctionTemplate::New(isolate,CallAsync)->GetFunction(), ReadOnly);

    constructor.Reset(isolate,tpl->GetFunction());

    exports->Set(v8::String::NewFromUtf8(isolate,"NativeFunction"), tpl->GetFunction());
}

const bridjs::NativeFunction* bridjs::NativeFunction::Data(v8::Isolate *pIsolate, v8::Handle<v8::Value> val) {
    HandleScope scope(pIsolate);
    bridjs::NativeFunction* obj;

    if (val->IsObject()) {
        obj = ObjectWrap::Unwrap<NativeFunction>(val->ToObject());
    } else {
        obj = NULL;
    }

    return obj;
}

bridjs::NativeFunction* bridjs::NativeFunction::New(void *pSymbol, const char returnType,
        const std::vector<char> &argumentTypes) {
    return new bridjs::NativeFunction(pSymbol, returnType, argumentTypes);
}

void bridjs::NativeFunction::New(const v8::FunctionCallbackInfo<v8::Value>& args) {
    Isolate* isolate = Isolate::GetCurrent(); HandleScope scope(isolate);

    if (args.IsConstructCall()) {
        NativeFunction* obj;
        std::vector<char> argumentTypes;

        if (args[0]->IsArray()) {
            v8::Local<v8::Array> array = v8::Local<v8::Array>::Cast(args[0]);

            GET_POINTER_VALUE(DLSyms, pSymbol, array->Get(0), 0);
            GET_CHAR_VALUE(returnType, array->Get(1), 1);

            for (uint32_t i = 2; i < array->Length(); ++i) {
                GET_CHAR_VALUE(type, array->Get(i), i);
 
                argumentTypes.push_back(type);
            }
            obj = new bridjs::NativeFunction(pSymbol, returnType, argumentTypes);
        } else {
            GET_POINTER_ARG(DLSyms, pSymbol, args, 0);
            GET_CHAR_ARG(returnType, args, 1);

            for (int32_t i = 2; i < args.Length(); ++i) {
                GET_CHAR_ARG(type, args, i);
                argumentTypes.push_back(type);
            }
            obj = new NativeFunction(pSymbol, returnType, argumentTypes);
        }
        obj->Wrap(args.This());

        args.GetReturnValue().Set(args.This());
    } else {
        const int argc = 1;
        Local<Value> argv[argc] = {args[0]};
        Local<Function> cons = Local<Function>::New(isolate, constructor);

        args.GetReturnValue().Set(cons->NewInstance(argc, argv));
    }
}

void bridjs::NativeFunction::GetReturnType(const v8::FunctionCallbackInfo<v8::Value>& args) {
    Isolate* isolate = Isolate::GetCurrent(); HandleScope scope(isolate);
    bridjs::NativeFunction* obj = ObjectWrap::Unwrap<NativeFunction>(args.This());

    args.GetReturnValue().Set(bridjs::Utils::toV8String(isolate,obj->getReturnType()));
}

void bridjs::NativeFunction::GetArgumentType(const v8::FunctionCallbackInfo<v8::Value>& args) {
    Isolate* isolate = Isolate::GetCurrent(); HandleScope scope(isolate);
    bridjs::NativeFunction* obj = ObjectWrap::Unwrap<bridjs::NativeFunction>(args.This());
    GET_INT32_ARG(index, args, 0);

    try {
        args.GetReturnValue().Set(bridjs::Utils::toV8String(isolate,obj->getArgumentType(index)));
    } catch (std::out_of_range& e) {
        THROW_EXCEPTION(e.what());
    }
}

void bridjs::NativeFunction::GetArgumentsLength(const v8::FunctionCallbackInfo<v8::Value>& args) {
    Isolate* isolate = Isolate::GetCurrent(); HandleScope scope(isolate);
    bridjs::NativeFunction* obj = ObjectWrap::Unwrap<NativeFunction>(args.This());

    args.GetReturnValue().Set(v8::Int32::New(isolate,static_cast<int32_t> (obj->getArgumentLength())));
}

void bridjs::NativeFunction::GetSymbol(const v8::FunctionCallbackInfo<v8::Value>& args) {
    Isolate* isolate = Isolate::GetCurrent(); HandleScope scope(isolate);
    bridjs::NativeFunction* obj = ObjectWrap::Unwrap<NativeFunction>(args.This());

    args.GetReturnValue().Set(bridjs::Utils::wrapPointer(isolate,obj->getSymbol()));
}

void bridjs::NativeFunction::Call(const v8::FunctionCallbackInfo<v8::Value>& args) {
    Isolate* isolate = Isolate::GetCurrent(); HandleScope scope(isolate);
    bridjs::NativeFunction* nativeFunction = ObjectWrap::Unwrap<NativeFunction>(args.This());
    GET_POINTER_ARG(DCCallVM, vm, args, 0);

    if (nativeFunction != NULL) {
        Local<Value> error;

        try {
            dcReset(vm);
            bool hasArgParameter = false;
            std::shared_ptr<v8::StdPersistentValueMap<uint32_t,v8::Value>> pPersistArgs = 
                    std::make_shared<v8::StdPersistentValueMap<uint32_t,v8::Value>>(isolate);
            std::shared_ptr<std::vector<std::shared_ptr<std::string>>> pStringArgs 
                    = std::make_shared<std::vector<std::shared_ptr<std::string>>>();
            if (/*args[1]->IsArray()*/args[1]->IsObject()) {
                v8::Local<Object> object = args[1]->ToObject();
                if (object->GetConstructorName()->Equals(Local<v8::String>::New(isolate,ARGUMENTS_NAME))) {

                    const ObjectCollection collection(isolate,object);

                    pushArgs(isolate,vm, nativeFunction, &collection, 0,pPersistArgs,pStringArgs,error);

                    hasArgParameter = true;
                } else {
                    hasArgParameter = false;
                }
            }

            if (!hasArgParameter) {
                const ArgumentCollection collection(isolate,&args);

                 pushArgs(isolate,vm, nativeFunction, &collection, 1,pPersistArgs,pStringArgs,error);
            }

            if ((*error)==NULL || error->IsNull()) {
                Local<Value> returnValue = bridjs::Utils::convertDataByType(isolate,callByType(vm, nativeFunction), nativeFunction->getReturnType());
                DCint errorCode = dcGetError(vm);

                if (errorCode != 0) {
                    std::stringstream message;
                    message << "Dyncall error, errorCode: " << errorCode << std::endl;

                    THROW_EXCEPTION(message.str().c_str());
                } else {
                    args.GetReturnValue().Set(returnValue);
                }
            } else {
                isolate->ThrowException(error);
                //args.GetReturnValue().Set(error);
            }
        } catch (std::runtime_error e) {
            THROW_EXCEPTION(e.what());
        } catch (std::out_of_range& e) {
            THROW_EXCEPTION(e.what());
        }
    } else {
        THROW_EXCEPTION("This must be NativeFunction's instance");
    }
}

void bridjs::NativeFunction::CallAsync(const v8::FunctionCallbackInfo<v8::Value>& args) {
    Isolate* isolate = Isolate::GetCurrent(); HandleScope scope(isolate);
    bridjs::NativeFunction* nativeFunction = ObjectWrap::Unwrap<NativeFunction>(args.This());
    //GET_POINTER_ARG(DCCallVM,vm,args,0);

    if (nativeFunction != NULL) {
        Local<v8::Value> callbackObject;
        Local<Value> error;
        bool hasArgParameter = false;
        GET_UINT32_ARG(stackSize, args, 0);
        DCCallVM *vm = dcNewCallVM(stackSize);

        try {
            std::shared_ptr<v8::StdPersistentValueMap<uint32_t,v8::Value>> pPersistArgs = 
                    std::make_shared<v8::StdPersistentValueMap<uint32_t,v8::Value>>(isolate);
            std::shared_ptr<std::vector<std::shared_ptr<std::string>>> pStringArgs 
                    = std::make_shared<std::vector<std::shared_ptr<std::string>>>();
            
            dcReset(vm);

            if (/*args[1]->IsArray()*/args[1]->IsObject()) {
                v8::Local<Object> object = args[1]->ToObject();
                if (object->GetConstructorName()->Equals(Local<v8::String>::New(isolate,ARGUMENTS_NAME))) {

                    const ObjectCollection collection(isolate,object);
                    callbackObject = collection.get(collection.length() - 1);

                    if (callbackObject->IsObject()) {
                        pushArgs(isolate,vm, nativeFunction, &collection, 0,pPersistArgs,pStringArgs, error);
                    } else {
                        std::stringstream message;
                        message << "Last argument must a CallbackObject's instance: " << (*v8::String::Utf8Value(callbackObject->ToString())) << std::endl;
                        THROW_EXCEPTION(message.str().c_str());
                    }

                    hasArgParameter = true;
                } else {
                    hasArgParameter = false;
                }
            }
            if (!hasArgParameter) {
                const ArgumentCollection collection(isolate,&args);
                callbackObject = args[args.Length() - 1];

                if (callbackObject->IsObject()) {
                    pushArgs(isolate,vm, nativeFunction, &collection, 1,pPersistArgs,pStringArgs,error);
                } else {
                    std::stringstream message;
                    message << "Last argument must a CallbackObject's instance: " << (*v8::String::Utf8Value(callbackObject->ToString())) << std::endl;
                    THROW_EXCEPTION(message.str().c_str());
                }
            }

            if (error->IsNull()) {
                uv_work_t *req = new uv_work_t;
                Persistent<Object> persistenObject(isolate,callbackObject->ToObject());
                
                req->data = new bridjs::AsyncCallTask(isolate,vm, nativeFunction,pPersistArgs,pStringArgs, persistenObject);
                uv_queue_work(uv_default_loop(), req, executeCallAsync, (uv_after_work_cb) afterCallAsync);

                args.GetReturnValue().Set(v8::Undefined(isolate));
                return;
            } else {
                args.GetReturnValue().Set(error);
                return;
            }
        } catch (std::out_of_range& e) {
            if (vm != NULL) {
                dcFree(vm);
            }

            THROW_EXCEPTION(e.what());
        }
    } else {
        THROW_EXCEPTION("This must be NativeFunction's instance");
    }
}

bridjs::NativeFunction::NativeFunction(void *pSymbol, const char returnType,
        const std::vector<char> &argumentTypes) {
    //this->mpVm = (DCCallVM*)pVm;
    this->mpSymbol = pSymbol;
    this->mReturnType = returnType;
    this->mArgumentTypes = argumentTypes;
    this->mArgumentTypes.shrink_to_fit();
}

char bridjs::NativeFunction::getReturnType() const {
    return this->mReturnType;
}

char bridjs::NativeFunction::getArgumentType(const uint32_t index) const {
    //std::cout<<this->mArgumentTypes<<std::endl;
    if (index<this->mArgumentTypes.size()) {
        return mArgumentTypes.at(index);
    } else {
        std::stringstream message;

        message << "Index: " << index << " was out of boundary, size = " << this->mArgumentTypes.size();

        throw std::out_of_range(message.str().c_str());
    }
}

size_t bridjs::NativeFunction::getArgumentLength() const {

    return this->mArgumentTypes.size();
}

/*
DCCallVM* bridjs::NativeFunction::getVM() const{
        return this->mpVm;
}*/
void* bridjs::NativeFunction::getSymbol() const {
    return this->mpSymbol;
}

bridjs::NativeFunction::~NativeFunction() {

}

AsyncCallTask::AsyncCallTask(v8::Isolate* pIsolate,DCCallVM* pVM, 
        const NativeFunction* pNativeFunction, std::shared_ptr<v8::StdPersistentValueMap<uint32_t,v8::Value>> pPersistArgs,
        std::shared_ptr<std::vector<std::shared_ptr<std::string>>> pStringArgs,
        v8::Persistent<v8::Object>& pCallbackObject):mpIsolate(pIsolate),mpVM(pVM),
        mpNativeFunction(pNativeFunction),mpPersistArgs(pPersistArgs),mpStringArgs(pStringArgs), mpCallbackObject(pIsolate, pCallbackObject)   {
}

v8::Handle<Value> AsyncCallTask::getReturnValue() {
    return v8::Null(this->mpIsolate);
}

/*
DCCallVM* AsyncCallTask::getVM() const{
        return this->mpVM;
}
const NativeFunction* AsyncCallTask::getNativeFunction() const{
        return this->mpNativeFunction;
}*/

void AsyncCallTask::execute() {
    this->mpData = callByType(this->mpVM, this->mpNativeFunction);
}

void AsyncCallTask::done() {
    Isolate* isolate = this->mpIsolate; HandleScope scope(isolate);
    int32_t error = dcGetError(this->mpVM);
    Local<Object> callbackObject = Local<Object>::New(isolate,this->mpCallbackObject);
    std::string callbackErrorMessage("");
    
    if (error == 0) {
        v8::Local<Value> onDoneValue = callbackObject->GetRealNamedProperty(v8::String::NewFromUtf8(isolate,"onDone"));

        if (*onDoneValue != NULL && onDoneValue->IsFunction()) {
            try{
                v8::Local<v8::Function> onDoneFunction = v8::Local<Function>::Cast(onDoneValue);
                Handle<Value> argv[] = {bridjs::Utils::wrapPointer(isolate,this->mpVM), 
                bridjs::Utils::convertDataByType(isolate,this->mpData, this->mpNativeFunction->getReturnType())};

                onDoneFunction->Call(callbackObject, 2, argv);
            }catch(std::runtime_error& e){
                callbackErrorMessage.clear();
                callbackErrorMessage.append(e.what());
            }
        } else {
            std::cerr << "Illegal callback object: " << (*v8::String::Utf8Value(callbackObject->ToString())) << std::endl;
        }
    } 
    if(error!=0) {
        v8::Local<Value> onErrorValue = callbackObject->GetRealNamedProperty(v8::String::NewFromUtf8(isolate,"onError"));

        if (*onErrorValue != NULL && onErrorValue->IsFunction()) {
            v8::Local<v8::Function> onErrorFunction = v8::Local<Function>::Cast(onErrorValue);
            std::stringstream message;
            
            if(callbackErrorMessage.length()>0){
                message << callbackErrorMessage;
            }else{
              message << "Dyncall error, errorCode: " << error;  
            }
            

            Handle<Value> argv[] = {bridjs::Utils::wrapPointer(isolate,this->mpVM), 
            v8::Exception::Error(v8::String::NewFromUtf8(isolate,message.str().c_str()))};

            onErrorFunction->Call(callbackObject, 2, argv);
        } else {
            std::cerr << "Illegal callback object: " << (*v8::String::Utf8Value(callbackObject->ToString())) << std::endl;
        }
    }

    this->mpCallbackObject.Reset();
}

AsyncCallTask::~AsyncCallTask() {
    /*The VM was created by CallAsync*/
    if (this->mpVM != NULL) {
        dcFree(this->mpVM);
    }
    this->mpVM = NULL;
    this->mpPersistArgs->Clear();
    this->mpStringArgs->clear();
}