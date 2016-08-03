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
#include "pointer_v8.h"
#include "dyncall_v8_utils.h"
#include "native_function_v8.h"

using namespace bridjs;
using namespace v8;
using namespace node;

Persistent<Function> Pointer::constructor;

void ToString(const v8::FunctionCallbackInfo<v8::Value>& args);



void Pointer::Init(v8::Handle<v8::Object> exports){
    Isolate* isolate = Isolate::GetCurrent();
	  Local<FunctionTemplate> tpl = Nan::New<v8::FunctionTemplate>(New);//.FunctionTemplate::New(isolate,New);
	  tpl->SetClassName(v8::String::NewFromUtf8(isolate,"Pointer"));
	  tpl->InstanceTemplate()->SetInternalFieldCount(4);
	  // Prototype
          Nan::SetPrototypeMethod(tpl, "getAddress", Pointer::GetAddress);
          Nan::SetPrototypeMethod(tpl, "isNull", Pointer::IsNull);
          Nan::SetPrototypeMethod(tpl, "toString", Pointer::ToString);
          Nan::SetPrototypeMethod(tpl, "slice", Pointer::Slice);
          /*
	  tpl->PrototypeTemplate()->Set(v8::String::NewFromUtf8(isolate,"getAddress"),
		  FunctionTemplate::New(isolate,GetAddress)->GetFunction());
	  tpl->PrototypeTemplate()->Set(v8::String::NewFromUtf8(isolate,"isNull"),
		  FunctionTemplate::New(isolate,IsNull)->GetFunction());
	  tpl->PrototypeTemplate()->Set(v8::String::NewFromUtf8(isolate,"toString"),
		  FunctionTemplate::New(isolate,Pointer::ToString)->GetFunction());
	  tpl->PrototypeTemplate()->Set(v8::String::NewFromUtf8(isolate,"slice"),
		  FunctionTemplate::New(isolate,Pointer::Slice)->GetFunction());
          */
	  constructor.Reset(isolate,tpl->GetFunction());
          //exports->DefineOwnProperty(isolate->GetCurrentContext(),v8::String::NewFromUtf8(isolate,"Pointer"),tpl->GetFunction()).FromJust();
	  exports->Set(v8::String::NewFromUtf8(isolate,"Pointer"), tpl->GetFunction());
}

NAN_METHOD(Pointer::ToString){
    Isolate* isolate = Isolate::GetCurrent();
    HandleScope scope(isolate);
    Pointer* obj = ObjectWrap::Unwrap<Pointer>(info.This());
    std::stringstream ptrStream;

    ptrStream << "< Pointer: " << std::hex << obj->getAddress() << " >" << std::endl;

    info.GetReturnValue().Set(v8::String::NewFromUtf8(isolate,(ptrStream.str().c_str())));
}

NAN_METHOD(Pointer::Slice){
    Isolate* isolate = Isolate::GetCurrent(); HandleScope scope(isolate);
    Pointer* obj = ObjectWrap::Unwrap<Pointer>(info.This());

    GET_INT64_ARG(start, info, 0);

    info.GetReturnValue().Set(Pointer::NewInstance(isolate,static_cast<const char*>(obj->getAddress())+start));
}

Pointer::Pointer(const void* ptr){
	this->mPtr = ptr;
}

void* Pointer::getAddress(){
	return const_cast<void*>(this->mPtr);
}

const void* Pointer::Data(Isolate* isolate,v8::Handle<v8::Object> val){
	HandleScope scope(isolate);
	Pointer* obj = Pointer::Unwrap<Pointer>(val);


	return obj->getAddress();
}

Pointer* Pointer::New(const void* ptr){
	return new Pointer(ptr);
}
NAN_METHOD(Pointer::New) {
    Isolate* isolate = Isolate::GetCurrent();
    HandleScope scope(isolate);

    if (info.IsConstructCall()) {
        void* ptr;
        Local<Value> arg0 = info[0];

        if (arg0->IsObject() && !arg0->IsString()) {
            ptr = Utils::unwrapBufferToPointer(arg0);
        } else {
            GET_POINTER_ARG(void, value, info, 0);
            ptr = value;
        }


        Pointer* obj = new Pointer(ptr);
        obj->Wrap(info.This());
        info.GetReturnValue().Set(info.This());
    } else {
        const int argc = 1;
        Local<Value> argv[argc] = {info[0]};
        Local<Function> cons = Local<Function>::New(isolate, constructor);

        info.GetReturnValue().Set(cons->NewInstance(isolate->GetCurrentContext(), 
                argc, argv).ToLocalChecked());
    }
}
v8::Local<v8::Object>  Pointer::NewInstance(v8::Isolate* isolate,const void* ptr){
    v8::EscapableHandleScope scope(isolate);
    Local<v8::Object> returnObject;
    Local<Function> cons = Local<Function>::New(isolate, constructor);    
    Local<Value> argv[1] = {
		Local<Value>::New(isolate, bridjs::Utils::wrapPointerToBuffer(isolate,ptr))
    };
    
    returnObject = cons->NewInstance(isolate->GetCurrentContext(),1, argv).ToLocalChecked();

    
    return scope.Escape(returnObject);
}

NAN_METHOD(Pointer::GetAddress) {
    Isolate* isolate = Isolate::GetCurrent();
    HandleScope scope(isolate);
    Pointer* obj = ObjectWrap::Unwrap<Pointer>(info.This());
    std::stringstream ptrStream;

    ptrStream << obj->mPtr;

    info.GetReturnValue().Set(v8::String::NewFromUtf8(isolate,(ptrStream.str().c_str())));
}

NAN_METHOD(Pointer::IsNull){
    Isolate* isolate = Isolate::GetCurrent();
    HandleScope scope(isolate);
    Pointer* obj = ObjectWrap::Unwrap<Pointer>(info.This());

    info.GetReturnValue().Set(Boolean::New(isolate,obj->mPtr == NULL));
}

Pointer::~Pointer(){
	
}