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
#include "struct_v8.h"
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

Persistent<v8::Function> bridjs::Struct::constructor;

v8::Handle<v8::Value> bridjs::Struct::GetSize(const v8::Arguments& args) {
    HandleScope scope;
    bridjs::Struct* obj = ObjectWrap::Unwrap<bridjs::Struct>(args.This());
    v8::Handle<v8::Value> value;

    try {
        value = scope.Close(WRAP_LONGLONG(obj->getSize()));
    } catch (std::out_of_range& e) {
        value = THROW_EXCEPTION(e.what());
    }

    return value;
}

v8::Handle<v8::Value> bridjs::Struct::GetField(const v8::Arguments& args) {
    HandleScope scope;
    bridjs::Struct* obj = ObjectWrap::Unwrap<bridjs::Struct>(args.This());
    v8::Handle<v8::Value> value;
    GET_INT32_ARG(index, args, 0);
    GET_POINTER_ARG(void, pTarget, args, 1);

    try {
        value = scope.Close(bridjs::Utils::convertDataByType(obj->getField(index, pTarget), obj->getFieldType(index)));
    } catch (std::out_of_range& e) {
        value = THROW_EXCEPTION(e.what());
    }

    return value;
}

v8::Handle<v8::Value> bridjs::Struct::SetField(const v8::Arguments& args) {
    HandleScope scope;
    bridjs::Struct* obj = ObjectWrap::Unwrap<bridjs::Struct>(args.This());
    v8::Handle<v8::Value> value;
    GET_INT32_ARG(index, args, 0);
    GET_POINTER_ARG(void, pTarget, args, 2);
    std::shared_ptr<void> data;
    const char type = obj->getFieldType(index);

    try {
        switch (type) {
            case DC_SIGCHAR_BOOL:
            {
                GET_BOOL_ARG(value, args, 1);
                data = std::shared_ptr<void>(new DCbool(value));
            }
                break;
            case DC_SIGCHAR_UCHAR:
            {
                GET_CHAR_ARG(value, args, 1);
                data = std::shared_ptr<void>(new DCuchar(value));
            }
                break;
            case DC_SIGCHAR_CHAR:
            {
                GET_CHAR_ARG(value, args, 1);
                data = std::shared_ptr<void>(new DCchar(value));
            }
                break;
            case DC_SIGCHAR_SHORT:
            {
                GET_INT32_ARG(value, args, 1);
                data = std::shared_ptr<void>(new DCshort(value));
            }
                break;
            case DC_SIGCHAR_USHORT:
            {
                GET_INT32_ARG(value, args, 1);
                data = std::shared_ptr<void>(new DCushort(value));
            }
                break;
            case DC_SIGCHAR_INT:
            {
                GET_INT32_ARG(value, args, 1);
                data = std::shared_ptr<void>(new DCint(value));
            }
                break;
            case DC_SIGCHAR_UINT:
            {
                GET_INT32_ARG(value, args, 1);
                data = std::shared_ptr<void>(new DCuint(value));
            }
                break;
            case DC_SIGCHAR_LONG:
            {
                GET_INT64_ARG(value, args, 1);
                data = std::shared_ptr<void>(new DClong(static_cast<DClong> (value)));
            }
                break;
            case DC_SIGCHAR_ULONG:
            {
                GET_INT64_ARG(value, args, 1);
                data = std::shared_ptr<void>(new DCulong(static_cast<DCulong> (value)));
            }
                break;
            case DC_SIGCHAR_LONGLONG:
            {
                GET_INT64_ARG(value, args, 1);
                data = std::shared_ptr<void>(new DClonglong(value));
            }
                break;
            case DC_SIGCHAR_ULONGLONG:
            {
                GET_INT64_ARG(value, args, 1);
                data = std::shared_ptr<void>(new DCulonglong(value));
            }
                break;
            case DC_SIGCHAR_FLOAT:
            {
                GET_FLOAT_ARG(value, args, 1);
                data = std::shared_ptr<void>(new DCfloat(value));
            }
                break;
            case DC_SIGCHAR_DOUBLE:
            {
                GET_DOUBLE_ARG(value, args, 1);
                data = std::shared_ptr<void>(new DCdouble(value));
            }
                break;
            case DC_SIGCHAR_STRING:
            case DC_SIGCHAR_POINTER:
            {
                GET_POINTER_ARG(void, value, args, 1);
                data = std::shared_ptr<void>(new DCpointer(value));
            }
                break;
            case DC_SIGCHAR_STRUCT:
            {
                std::cerr << "Not implement" << std::endl;
                data = std::shared_ptr<void*>(NULL);
            }
                break;
            case DC_SIGCHAR_VOID:
            default:
                std::stringstream message;
                message << "Unknown returnType: " << type << std::endl;
                throw std::runtime_error(message.str());
                //return v8::Exception::TypeError(v8::String::New(message.str().c_str()));
        }

        obj->setField(index, data, pTarget);

        return scope.Close(v8::Undefined());
    } catch (std::out_of_range& e) {
        return THROW_EXCEPTION(e.what());
    }
}

v8::Handle<v8::Value> bridjs::Struct::GetSignature(const v8::Arguments& args) {
    HandleScope scope;
    bridjs::Struct* obj = ObjectWrap::Unwrap<bridjs::Struct>(args.This());


    return scope.Close(WRAP_STRING(obj->getSignature().c_str()));
}

v8::Handle<v8::Value> bridjs::Struct::ToString(const v8::Arguments& args) {
    HandleScope scope;
    bridjs::Struct* obj = ObjectWrap::Unwrap<bridjs::Struct>(args.This());


    return scope.Close(WRAP_STRING(obj->toString().c_str()));
}

const size_t bridjs::Struct::getAlignSize(size_t size, size_t alignment) {
    size_t mod = (size) % alignment;
    if (mod) {
        size_t rest = alignment - mod;
        return size + rest;
    } else {
        return size;
    }
}

const size_t bridjs::Struct::getFieldsSize(const std::vector<char> &fieldTypes, const size_t alignment) {
    size_t size = 0;

    for (uint32_t i = 0; i < fieldTypes.size(); ++i) {
        size += bridjs::Utils::getTypeSize(fieldTypes[i]);


    }
    size = getAlignSize(size, alignment);
    return size;
}

const size_t bridjs::Struct::getAlignmentSize(const char type, const size_t typeSize, const bool isFirst) {

    /*
    if (actualAlignType == ALIGN_NONE) {
    alignment = 1;
    }
    else if (actualAlignType == ALIGN_MSVC) {
    alignment = Math.min(8, alignment);
    }
    else if (actualAlignType == ALIGN_GNUC) {
    // NOTE this is published ABI for 32-bit gcc/linux/x86, osx/x86,
    // and osx/ppc.  osx/ppc special-cases the first element
    if (!isFirstElement || !(Platform.isMac() && Platform.isPPC())) {
    alignment = Math.min(Native.MAX_ALIGNMENT, alignment);
    }
    if (!isFirstElement && Platform.isAIX() && (type == double.class || type == Double.class)) {
    alignment = 4;
    }
    }
    return alignment;*/

#ifdef _MSC_VER
    return std::min(static_cast<const size_t> (8), typeSize);
#else
    size_t alignment = typeSize;
    
    if (!isFirst) {
        alignment = std::min(sizeof (void*), alignment);
    }
    
    return alignment;
    /*
    if (!isFirst && type==DCdouble) {
            alignment = 4);
    }*/
#endif

}

const size_t bridjs::Struct::addPadding(size_t calculatedSize, const size_t alignment) {
    // Structure size must be an integral multiple of its alignment,
    // add padding if necessary.
    //if (actualAlignType != ALIGN_NONE) {
    if ((calculatedSize % alignment) != 0) {
        calculatedSize += alignment - (calculatedSize % alignment);
    }
    //}
    return calculatedSize;
}

void bridjs::Struct::Init(v8::Handle<v8::Object> exports) {
    // Prepare constructor template
    Local<FunctionTemplate> tpl = FunctionTemplate::New(New);


    tpl->SetClassName(String::NewSymbol("Struct"));
    tpl->InstanceTemplate()->SetInternalFieldCount(8);
    // Prototype
    /*
    tpl->PrototypeTemplate()->Set(String::NewSymbol("getVM"),
            FunctionTemplate::New(GetVM)->GetFunction(), ReadOnly);*/
    tpl->PrototypeTemplate()->Set(String::NewSymbol("getFieldType"),
            FunctionTemplate::New(GetFieldType)->GetFunction(), ReadOnly);
    tpl->PrototypeTemplate()->Set(String::NewSymbol("getFieldCount"),
            FunctionTemplate::New(GetFieldCount)->GetFunction(), ReadOnly);
    tpl->PrototypeTemplate()->Set(String::NewSymbol("getFieldOffset"),
            FunctionTemplate::New(GetFieldOffset)->GetFunction(), ReadOnly);
    tpl->PrototypeTemplate()->Set(String::NewSymbol("getField"),
            FunctionTemplate::New(GetField)->GetFunction(), ReadOnly);
    tpl->PrototypeTemplate()->Set(String::NewSymbol("setField"),
            FunctionTemplate::New(SetField)->GetFunction(), ReadOnly);
    tpl->PrototypeTemplate()->Set(String::NewSymbol("getSize"),
            FunctionTemplate::New(GetSize)->GetFunction(), ReadOnly);
    tpl->PrototypeTemplate()->Set(String::NewSymbol("getSignature"),
            FunctionTemplate::New(GetSignature)->GetFunction(), ReadOnly);
    tpl->PrototypeTemplate()->Set(String::NewSymbol("toString"),
            FunctionTemplate::New(bridjs::Struct::ToString)->GetFunction(), ReadOnly);

    constructor = Persistent<Function>::New(tpl->GetFunction());

    exports->Set(String::NewSymbol("Struct"), constructor);
}

bridjs::Struct* bridjs::Struct::New(const std::vector<char> &fieldTypes, std::map<uint32_t, v8::Local<v8::Object >> &subStructMap) {
    return new bridjs::Struct(fieldTypes, subStructMap, DEFAULT_ALIGNMENT);
}

v8::Handle<v8::Value> bridjs::Struct::New(const v8::Arguments& args) {
    HandleScope scope;

    if (args.IsConstructCall()) {
        try {
            std::vector<char> argumentTypes;
            size_t alignment = DEFAULT_ALIGNMENT;
            std::map<uint32_t, v8::Local < v8::Object >> subStructMap;
            v8::Local<v8::Value> value;
            char type;
            Struct* obj;

            if (args[0]->IsArray()) {
                v8::Local<v8::Array> array = v8::Local<v8::Array>::Cast(args[0]);

                for (uint32_t i = 0; i < array->Length(); ++i) {
                    value = array->Get(i);

                    if (value->IsObject() && !value->IsString()) {
                        type = DC_SIGCHAR_STRUCT;
                        subStructMap[i] = value->ToObject();
                    } else {
                        GET_CHAR_VALUE(tempType, value, i);
                        type = tempType;
                    }

                    argumentTypes.push_back(type);
                }
            } else {
                for (int32_t i = 0; i < args.Length(); ++i) {
                    value = args[i];

                    if (value->IsObject() && !value->IsString()) {
                        type = DC_SIGCHAR_STRUCT;
                        subStructMap[i] = value->ToObject();
                    } else {
                        GET_CHAR_VALUE(tempType, value, i);
                        type = tempType;
                    }
                    argumentTypes.push_back(type);
                }
            }


            //buffer = std::shared_ptr<node::Buffer>(node::Buffer::New(getFieldsSize(argumentTypes,alignment)));
            obj = new Struct(argumentTypes, subStructMap, alignment);
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

/*
v8::Handle<v8::Value> bridjs::Signature::NewInstance(const void* ptr){
        HandleScope scope;

    Local<Value> argv[1] = {
                Local<Value>::New(bridjs::Utils::wrapPointerToBuffer(ptr))
    };

    return scope.Close(constructor->NewInstance(1, argv));
}*/

v8::Handle<v8::Value> bridjs::Struct::GetFieldType(const v8::Arguments& args) {
    HandleScope scope;
    bridjs::Struct* obj = ObjectWrap::Unwrap<bridjs::Struct>(args.This());
    v8::Handle<v8::Value> value;
    GET_INT32_ARG(index, args, 0);

    try {
        value = scope.Close(bridjs::Utils::toV8String(obj->getFieldType(index)));
    } catch (std::out_of_range& e) {
        value = THROW_EXCEPTION(e.what());
    }

    return value;
}

v8::Handle<v8::Value> bridjs::Struct::GetFieldOffset(const v8::Arguments& args) {
    HandleScope scope;
    bridjs::Struct* obj = ObjectWrap::Unwrap<bridjs::Struct>(args.This());
    v8::Handle<v8::Value> value;
    GET_UINT32_ARG(index, args, 0);

    try {
        value = scope.Close(WRAP_UINT(static_cast<uint32_t>(obj->getFieldOffset(index))));
    } catch (std::out_of_range& e) {
        value = THROW_EXCEPTION(e.what());
    }

    return value;
}

v8::Handle<v8::Value> bridjs::Struct::GetFieldCount(const v8::Arguments& args) {
    HandleScope scope;
    bridjs::Struct* obj = ObjectWrap::Unwrap<Struct>(args.This());

    return scope.Close(v8::Int32::New(static_cast<int32_t> (obj->getFieldCount())));
}

bridjs::Struct::Struct(const std::vector<char> &fieldTypes,
        std::map<uint32_t, v8::Local<v8::Object >> &subStructMap,
        const size_t alignment) : mFieldTypes(fieldTypes) {

    for (std::map < uint32_t, v8::Local < v8::Object >> ::iterator it = subStructMap.begin();
            it != subStructMap.end(); ++it) {
        this->mSubStructMap[it->first] = v8::Persistent<v8::Object>::New(it->second);
    }

    mSize = this->deriveLayout(alignment);
}

const size_t bridjs::Struct::deriveLayout(const size_t alignment) {
    size_t calculatedSize = 0;
    char type;
    size_t typeSize, fieldAlignment, alignmentInfo = alignment;
    Struct* pSubStruct = NULL;

    for (uint32_t i = 0; i<this->mFieldTypes.size(); ++i) {
        type = this->mFieldTypes[i];
        if (type == DC_SIGCHAR_STRUCT) {
            pSubStruct = this->getSubStruct(i);
            typeSize = pSubStruct->getSize();
            /*Struct's aligment is computed from its max size element*/
            fieldAlignment = pSubStruct->getAlignment();
        } else {
            fieldAlignment = typeSize = bridjs::Utils::getTypeSize(type);
        }

        fieldAlignment = getAlignmentSize(type, fieldAlignment, i <= 0);

        // Align fields as appropriate
        if (fieldAlignment == 0) {
            std::stringstream message;

            message << "Field alignment is zero for field '" << i << "' within " << type;

            throw std::runtime_error(message.str());
        }


        alignmentInfo = std::max(alignmentInfo, fieldAlignment);

        if ((calculatedSize % fieldAlignment) != 0) {
            calculatedSize += fieldAlignment - (calculatedSize % fieldAlignment);
        }
        /*
        if (this instanceof Union) {
structField.offset = 0;
calculatedSize = Math.max(calculatedSize, structField.size);
        }
        else {
structField.offset = calculatedSize;
calculatedSize += structField.size;
        }*/
        this->mOffsets.push_back(calculatedSize); //structField.offset = calculatedSize;
        calculatedSize += typeSize;

        // Save the field in our list
        //info.fields.put(structField.name, structField);
    }
    this->mOffsets.shrink_to_fit();

    if (calculatedSize > 0) {
        calculatedSize = addPadding(calculatedSize, alignmentInfo);
    }

    this->mAligment = alignmentInfo;

    return calculatedSize;
}

const size_t bridjs::Struct::getSize() const {
    return this->mSize;
}

const char bridjs::Struct::getFieldType(const uint32_t index) const {
    //std::cout<<this->mArgumentTypes<<std::endl;
    this->checkRange(index);

    return mFieldTypes.at(index);
}

const size_t bridjs::Struct::getFieldCount() const {

    return this->mFieldTypes.size();
}

void bridjs::Struct::checkRange(const uint32_t index) const {
    if (index >= this->getFieldCount()) {
        std::stringstream message;

        message << "Index: " << index << " was out of boundary, size = " << this->mFieldTypes.size();

        throw std::out_of_range(message.str().c_str());
    }
}

Struct* bridjs::Struct::getSubStruct(uint32_t index) {
    HandleScope scope;
    //v8::Handle<v8::Object> structInstance =this->mSubStructMap[index];
    Struct* pSubStruct = Struct::Unwrap<Struct>(this->mSubStructMap[index]);

    if (pSubStruct != NULL) {
        return pSubStruct;
    } else {
        std::stringstream message;
        message << "Fail to cast sub Struct object, field index = " << index;
        throw std::runtime_error(message.str());
    }
}

std::shared_ptr<void> bridjs::Struct::getField(const uint32_t index, const void* mPtr) const {
    std::shared_ptr<void> data;
    const char type = this->getFieldType(index);
    const size_t offset = this->getFieldOffset(index);
    const char* ptr = static_cast<const char*> (mPtr) + offset;

    switch (type) {
        case DC_SIGCHAR_BOOL:
        {
            data = std::shared_ptr<void>(new DCbool(*(DCbool*) ptr));
        }
            break;
        case DC_SIGCHAR_UCHAR:
        {
            data = std::shared_ptr<void>(new DCuchar(*(DCuchar*) ptr));
        }
            break;
        case DC_SIGCHAR_CHAR:
        {
            data = std::shared_ptr<void>(new DCchar(*(DCchar*) ptr));
        }
            break;
        case DC_SIGCHAR_SHORT:
        {
            data = std::shared_ptr<void>(new DCshort(*(DCshort*) ptr));
        }
            break;
        case DC_SIGCHAR_USHORT:
        {
            data = std::shared_ptr<void>(new DCushort(*(DCushort*) ptr));
        }
            break;
        case DC_SIGCHAR_INT:
        {
            data = std::shared_ptr<void>(new DCint(*(DCint*) ptr));
        }
            break;
        case DC_SIGCHAR_UINT:
        {
            data = std::shared_ptr<void>(new DCuint(*(DCuint*) ptr));
        }
            break;
        case DC_SIGCHAR_LONG:
        {
            data = std::shared_ptr<void>(new DClong(*(DClong*) ptr));
        }
            break;
        case DC_SIGCHAR_ULONG:
        {
            data = std::shared_ptr<void>(new DCulong(*(DCulong*) ptr));
        }
            break;
        case DC_SIGCHAR_LONGLONG:
        {
            data = std::shared_ptr<void>(new DClonglong(*(DClonglong*) ptr));
        }
            break;
        case DC_SIGCHAR_ULONGLONG:
        {
            data = std::shared_ptr<void>(new DCulonglong(*(DCulonglong*) ptr));
        }
            break;
        case DC_SIGCHAR_FLOAT:
        {
            data = std::shared_ptr<void>(new DCfloat(*(DCfloat*) ptr));
        }
            break;
        case DC_SIGCHAR_DOUBLE:
        {
            //std::cout<<*(DCdouble*)ptr<<std::endl;
            data = std::shared_ptr<void>(new DCdouble(*(DCdouble*) ptr));
        }
            break;
        case DC_SIGCHAR_STRING:
        case DC_SIGCHAR_POINTER:
        {
            data = std::shared_ptr<void>(new DCpointer(*(DCpointer*) ptr));
        }
            break;
        case DC_SIGCHAR_STRUCT:
        {
            std::stringstream message;
            message << "bridjs should handle Struct type at JavaScript layer: " << index;
            throw std::runtime_error(message.str());
        }
            break;
        case DC_SIGCHAR_VOID:
        default:
            std::stringstream message;
            message << "Unknown returnType: " << type << std::endl;
            throw std::runtime_error(message.str());
            //return v8::Exception::TypeError(v8::String::New(message.str().c_str()));
    }

    return data;
}

void bridjs::Struct::setField(const uint32_t index, std::shared_ptr<void> pValue, void* mPtr) {
    const char type = this->getFieldType(index);
    const size_t offset = this->getFieldOffset(index);
    const char* ptr = static_cast<const char*> (mPtr) + offset;

    switch (type) {
        case DC_SIGCHAR_BOOL:
        {
            *(DCbool*) ptr = *(DCbool*) pValue.get();
        }
            break;
        case DC_SIGCHAR_UCHAR:
        {
            *(DCuchar*) ptr = *(DCuchar*) pValue.get();
        }
            break;
        case DC_SIGCHAR_CHAR:
        {
            *(DCchar*) ptr = *(DCchar*) pValue.get();
        }
            break;
        case DC_SIGCHAR_SHORT:
        {
            *(DCshort*) ptr = *(DCshort*) pValue.get();
        }
            break;
        case DC_SIGCHAR_USHORT:
        {
            *(DCushort*) ptr = *(DCushort*) pValue.get();
        }
            break;
        case DC_SIGCHAR_INT:
        {
            *(DCint*) ptr = *(DCint*) pValue.get();
        }
            break;
        case DC_SIGCHAR_UINT:
        {
            *(DCuint*) ptr = *(DCuint*) pValue.get();
        }
            break;
        case DC_SIGCHAR_LONG:
        {
            *(DClong*) ptr = *(DClong*) pValue.get();
        }
            break;
        case DC_SIGCHAR_ULONG:
        {
            *(DCulong*) ptr = *(DCulong*) pValue.get();
        }
            break;
        case DC_SIGCHAR_LONGLONG:
        {
            *(DClonglong*) ptr = *(DClonglong*) pValue.get();
        }
            break;
        case DC_SIGCHAR_ULONGLONG:
        {
            *(DCulonglong*) ptr = *(DCulonglong*) pValue.get();
        }
            break;
        case DC_SIGCHAR_FLOAT:
        {
            *(DCfloat*) ptr = *(DCfloat*) pValue.get();
        }
            break;
        case DC_SIGCHAR_DOUBLE:
        {
            *(DCdouble*) ptr = *(DCdouble*) pValue.get();
        }
            break;
        case DC_SIGCHAR_STRING:
        case DC_SIGCHAR_POINTER:
        {
            *(DCpointer*) ptr = *(DCpointer*) pValue.get();
        }
            break;
        case DC_SIGCHAR_STRUCT:
        {
            std::stringstream message;
            message << "bridjs should handle Struct type at JavaScript layer: " << index;
            throw std::runtime_error(message.str());
        }
            break;
        case DC_SIGCHAR_VOID:
        default:
            std::stringstream message;
            message << "Unknown returnType: " << type << std::endl;
            throw std::runtime_error(message.str());
            //return v8::Exception::TypeError(v8::String::New(message.str().c_str()));
    }
}

std::string bridjs::Struct::getSignature() {
    char type;
    std::stringstream sig;

    for (uint32_t i = 0; i<this->mFieldTypes.size(); ++i) {
        type = this->mFieldTypes[i];

        if (type == DC_SIGCHAR_STRUCT) {
            sig << getSubStruct(i)->toString();
        } else {
            sig << type;
        }
    }

    return sig.str();
}

size_t bridjs::Struct::getFieldOffset(uint32_t index) const {
    this->checkRange(index);

    return this->mOffsets[index];
}

std::string bridjs::Struct::toString() {
    std::stringstream sig;

    sig << DC_SIGCHAR_STRUCT << '(' << this->getSignature() << ')';

    return sig.str();
};

size_t bridjs::Struct::getAlignment() const {
    return this->mAligment;
}

bridjs::Struct::~Struct() {

    for (std::map < uint32_t, v8::Persistent < v8::Object >> ::iterator it = this->mSubStructMap.begin();
            it != this->mSubStructMap.end(); ++it) {

        it->second.Dispose();
        it->second.Clear();
    }
}