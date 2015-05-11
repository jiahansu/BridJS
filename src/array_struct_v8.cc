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
#include "array_struct_v8.h"
#include "dyncall_v8_utils.h"
#include <iostream>
#include <algorithm>

extern "C" {
#include "dyncall.h"
#include "dyncall_signature.h"
#include <uv.h>
}

using namespace bridjs;
using namespace v8;
using namespace node;

Persistent<v8::Function> bridjs::ArrayStruct::constructor;
std::vector<char> bridjs::ArrayStruct::mEmptyTypes;
std::map<uint32_t,v8::Local<v8::Object>> bridjs::ArrayStruct::mEmptySubStructMap;

void bridjs::ArrayStruct::Init(v8::Handle<v8::Object> exports) {
    // Prepare constructor template
    Local<FunctionTemplate> tpl = FunctionTemplate::New(bridjs::ArrayStruct::New);


    tpl->SetClassName(String::NewSymbol("ArrayStruct"));
    tpl->InstanceTemplate()->SetInternalFieldCount(8);

    // Prototype
    tpl->PrototypeTemplate()->Set(String::NewSymbol("getFieldType"),
            FunctionTemplate::New(bridjs::Struct::GetFieldType)->GetFunction(), ReadOnly);
    tpl->PrototypeTemplate()->Set(String::NewSymbol("getFieldCount"),
            FunctionTemplate::New(bridjs::Struct::GetFieldCount)->GetFunction(), ReadOnly);
    tpl->PrototypeTemplate()->Set(String::NewSymbol("getFieldOffset"),
            FunctionTemplate::New(bridjs::Struct::GetFieldOffset)->GetFunction(), ReadOnly);
    tpl->PrototypeTemplate()->Set(String::NewSymbol("getField"),
            FunctionTemplate::New(bridjs::Struct::GetField)->GetFunction(), ReadOnly);
    tpl->PrototypeTemplate()->Set(String::NewSymbol("setField"),
            FunctionTemplate::New(bridjs::Struct::SetField)->GetFunction(), ReadOnly);
    tpl->PrototypeTemplate()->Set(String::NewSymbol("getSize"),
            FunctionTemplate::New(bridjs::Struct::GetSize)->GetFunction(), ReadOnly);
    tpl->PrototypeTemplate()->Set(String::NewSymbol("getSignature"),
            FunctionTemplate::New(bridjs::Struct::GetSignature)->GetFunction(), ReadOnly);
    tpl->PrototypeTemplate()->Set(String::NewSymbol("toString"),
            FunctionTemplate::New(bridjs::Struct::ToString)->GetFunction(), ReadOnly);

    constructor = Persistent<Function>::New(tpl->GetFunction());

    exports->Set(String::NewSymbol("ArrayStruct"), constructor);
    
    bridjs::ArrayStruct::mEmptyTypes = std::vector<char>();
    bridjs::ArrayStruct::mEmptySubStructMap = std::map<uint32_t,v8::Local<v8::Object>>();
}

bridjs::ArrayStruct* bridjs::ArrayStruct::New(const char type, const size_t length, const size_t alignment) {
    return new bridjs::ArrayStruct(type, length, alignment);
}

v8::Handle<v8::Value> bridjs::ArrayStruct::New(const v8::Arguments& args) {
    HandleScope scope;

    if (args.IsConstructCall()) {
        try {
            GET_CHAR_ARG(type, args, 0);
            GET_INT64_ARG(length, args, 1);
            size_t alignment = DEFAULT_ALIGNMENT;
            ArrayStruct *obj = new ArrayStruct(type, static_cast<size_t> (length), alignment);

            obj->Wrap(args.This());

            return args.This();
        } catch (std::exception &e) {
            return v8::Exception::TypeError(v8::String::New(e.what()));
        }
    } else {
        const int argc = 1;
        Local<Value> argv[argc] = {args[0]};
        return scope.Close(constructor->NewInstance(argc, argv));
    }
}

bridjs::ArrayStruct::ArrayStruct(const char type, const size_t length, const size_t alignment) :
    bridjs::Struct(bridjs::ArrayStruct::mEmptyTypes, bridjs::ArrayStruct::mEmptySubStructMap, alignment), 
        mType(type), mLength(length){

    this->mSize = this->deriveArrayLayout(alignment);
}

const size_t bridjs::ArrayStruct::deriveArrayLayout(const size_t alignment) {
    size_t calculatedSize = 0;
    size_t typeSize, fieldAlignment, alignmentInfo = alignment;
    //Struct* pSubStruct = NULL;

    fieldAlignment = bridjs::Utils::getTypeSize(this->mType);
    typeSize = fieldAlignment * this->mLength;
    fieldAlignment = getAlignmentSize(this->mType, fieldAlignment, true);

    // Align fields as appropriate
    if (fieldAlignment == 0) {
        std::stringstream message;
        message << "Field alignment is zero for type '" << this->mType;
        throw std::runtime_error(message.str());
    }

    alignmentInfo = std::max(alignmentInfo, fieldAlignment);

    if ((calculatedSize % fieldAlignment) != 0) {
        calculatedSize += fieldAlignment - (calculatedSize % fieldAlignment);
    }
    calculatedSize += typeSize;
    this->mOffsets.shrink_to_fit();

    if (calculatedSize > 0) {
        calculatedSize = addPadding(calculatedSize, alignmentInfo);
    }

    this->mAligment = alignmentInfo;

    return calculatedSize;
}

const char bridjs::ArrayStruct::getFieldType(const uint32_t index) const {
    //std::cout<<this->mArgumentTypes<<std::endl;
    this->checkRange(index);

    return this->mType;
}

const size_t bridjs::ArrayStruct::getFieldCount() const {

    return this->mLength;
}

void bridjs::ArrayStruct::checkRange(const uint32_t index) const {
    if (index >= this->mLength) {
        std::stringstream message;

        message << "Index: " << index << " was out of boundary, size = " << this->mFieldTypes.size();

        throw std::out_of_range(message.str().c_str());
    }
}

size_t bridjs::ArrayStruct::getFieldOffset(uint32_t index) const {
    this->checkRange(index);

    return bridjs::Utils::getTypeSize(this->mType) * index;
}

std::string bridjs::ArrayStruct::getSignature() {
    std::stringstream sig;

    sig << this->mType << '(' << this->mLength << ')';

    return sig.str();

}

bridjs::ArrayStruct::~ArrayStruct() {

}