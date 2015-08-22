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
#include "union_struct_v8.h"
#include "dyncall_v8_utils.h"
#include <v8.h>
#include <iostream>
#include <algorithm>

extern "C" {
#include "dyncall.h"
#include "dyncall_signature.h"
}

using namespace bridjs;
using namespace v8;
using namespace node;

Persistent<v8::Function> bridjs::UnionStruct::constructor;

void bridjs::UnionStruct::Init(v8::Handle<v8::Object> exports) {
    Isolate* isolate = Isolate::GetCurrent();
    // Prepare constructor template
    Local<FunctionTemplate> tpl = FunctionTemplate::New(isolate,bridjs::UnionStruct::New);


    tpl->SetClassName(v8::String::NewFromUtf8(isolate,"UnionStruct"));
    tpl->InstanceTemplate()->SetInternalFieldCount(8);

    // Prototype
    tpl->PrototypeTemplate()->Set(v8::String::NewFromUtf8(isolate,"getFieldType", v8::String::kInternalizedString),
            FunctionTemplate::New(isolate,bridjs::Struct::GetFieldType)->GetFunction(), ReadOnly);
    tpl->PrototypeTemplate()->Set(v8::String::NewFromUtf8(isolate,"getFieldCount", v8::String::kInternalizedString),
            FunctionTemplate::New(isolate,bridjs::Struct::GetFieldCount)->GetFunction(), ReadOnly);
    tpl->PrototypeTemplate()->Set(v8::String::NewFromUtf8(isolate,"getFieldOffset", v8::String::kInternalizedString),
            FunctionTemplate::New(isolate,bridjs::Struct::GetFieldOffset)->GetFunction(), ReadOnly);
    tpl->PrototypeTemplate()->Set(v8::String::NewFromUtf8(isolate,"getField", v8::String::kInternalizedString),
            FunctionTemplate::New(isolate,bridjs::Struct::GetField)->GetFunction(), ReadOnly);
    tpl->PrototypeTemplate()->Set(v8::String::NewFromUtf8(isolate,"setField", v8::String::kInternalizedString),
            FunctionTemplate::New(isolate,bridjs::Struct::SetField)->GetFunction(), ReadOnly);
    tpl->PrototypeTemplate()->Set(v8::String::NewFromUtf8(isolate,"getSize", v8::String::kInternalizedString),
            FunctionTemplate::New(isolate,bridjs::Struct::GetSize)->GetFunction(), ReadOnly);
    tpl->PrototypeTemplate()->Set(v8::String::NewFromUtf8(isolate,"getSignature", v8::String::kInternalizedString),
            FunctionTemplate::New(isolate,bridjs::Struct::GetSignature)->GetFunction(), ReadOnly);
    tpl->PrototypeTemplate()->Set(v8::String::NewFromUtf8(isolate,"toString", v8::String::kInternalizedString),
            FunctionTemplate::New(isolate,bridjs::Struct::ToString)->GetFunction(), ReadOnly);

    constructor.Reset(isolate,tpl->GetFunction());

    exports->Set(v8::String::NewFromUtf8(isolate,"UnionStruct"), tpl->GetFunction());
}

bridjs::UnionStruct* bridjs::UnionStruct::New(v8::Isolate* pIsolate, const std::vector<char> &fieldTypes, 
        std::map<uint32_t, v8::Local<v8::Object >> &subStructMap,const size_t alignment) {
    return new bridjs::UnionStruct(pIsolate, fieldTypes, subStructMap);
}

void bridjs::UnionStruct::New(const v8::FunctionCallbackInfo<v8::Value>& args) {
    Isolate* isolate = Isolate::GetCurrent();
    HandleScope scope(isolate);

    if (args.IsConstructCall()) {
        try {
            std::vector<char> argumentTypes;
            size_t alignment = DEFAULT_ALIGNMENT;
            std::map<uint32_t, v8::Local < v8::Object >> subStructMap;
            UnionStruct* obj;

            bridjs::Struct::parseJSArguments(isolate, args, argumentTypes,subStructMap);


            //buffer = std::shared_ptr<node::Buffer>(node::Buffer::New(getFieldsSize(argumentTypes,alignment)));
            obj = new UnionStruct(isolate,argumentTypes, subStructMap, alignment);
            obj->Wrap(args.This());
            args.GetReturnValue().Set(args.This());
        } catch (std::exception &e) {
            isolate->ThrowException(v8::Exception::TypeError(v8::String::NewFromUtf8(isolate,e.what())));
        }
    } else {
        const int argc = 1;
        Local<Value> argv[argc] = {args[0]};
        Local<Function> cons = Local<Function>::New(isolate, constructor);
        
        args.GetReturnValue().Set(cons->NewInstance(argc, argv));
        
        return;
    }
}

bridjs::UnionStruct::UnionStruct(v8::Isolate* pIsolate,const std::vector<char> &fieldType, 
        std::map<uint32_t,v8::Local<v8::Object>> &subStructMap,const size_t alignment) :
								 bridjs::Struct(pIsolate, fieldType, subStructMap, alignment)
         {
    uint32_t maxTypeSize = 0;
    char type;
    Struct* pSubStruct = NULL;
    uint32_t typeSize;
    
     for (uint32_t i = 0; i<this->mFieldTypes.size(); ++i) {
        type = this->mFieldTypes[i];
        if (type == DC_SIGCHAR_STRUCT) {
            pSubStruct = this->getSubStruct(i);
            typeSize = pSubStruct->getSize();
        } else {
            typeSize = bridjs::Utils::getTypeSize(type);
        }
        
        if(typeSize>=maxTypeSize){
            maxTypeSize = typeSize;
        }
     }
    
    this->mSize = maxTypeSize;
}

size_t bridjs::UnionStruct::getFieldOffset(uint32_t index) const {
    this->checkRange(index);
    
    return 0;
}

std::string bridjs::UnionStruct::getSignature() {
    char type;
    std::stringstream sig;
    
    sig<<"u(";
    for (uint32_t i = 0; i<this->mFieldTypes.size(); ++i) {
        type = this->mFieldTypes[i];

        if (type == DC_SIGCHAR_STRUCT) {
            sig << getSubStruct(i)->toString();
        } else {
            sig << type;
        }
    }
    sig<<')';
    
    return sig.str();

}

bridjs::UnionStruct::~UnionStruct() {

}