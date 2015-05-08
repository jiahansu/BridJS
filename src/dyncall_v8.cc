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
#include <node_buffer.h>

#include "dyncall_v8.h"
#include "dyncall_v8_utils.h"
#include "native_function_v8.h"
extern "C" {
#include "dyncall.h"
}
#include <iostream>


using namespace v8;
using namespace bridjs;

void Dyncall::newCallVM(const v8::FunctionCallbackInfo<v8::Value>& args) {
    Isolate* isolate = Isolate::GetCurrent();
    HandleScope scope(isolate);
    GET_INT64_ARG(size, args, 0);
    DCCallVM *vm = dcNewCallVM(static_cast<DCsize> (size));

    //std::cout<<(void*)vm<<std::endl;

    args.GetReturnValue().Set(bridjs::Utils::wrapPointer(isolate,vm));
}

void Dyncall::free(const v8::FunctionCallbackInfo<v8::Value>& args) {
    Isolate* isolate = Isolate::GetCurrent();
    HandleScope scope(isolate);

    //CHECK_ARGUMENT(args,1);
    GET_POINTER_ARG(DCCallVM, vm, args, 0);

    //vm = (DCCallVM*)bridjs::Utils::unwrapPointer(args[0]->ToObject());

    dcFree((DCCallVM*) vm);

    args.GetReturnValue().Set(v8::Undefined(isolate));
}

void Dyncall::reset(const v8::FunctionCallbackInfo<v8::Value>& args) {
    Isolate* isolate = Isolate::GetCurrent();
    HandleScope scope(isolate);

    //CHECK_ARGUMENT(args,1);
    GET_POINTER_ARG(DCCallVM, vm, args, 0);

    dcReset((DCCallVM*) vm);

    //std::cout<<(void*)vm<<std::endl;

    args.GetReturnValue().Set(v8::Undefined(isolate));
}

void Dyncall::mode(const v8::FunctionCallbackInfo<v8::Value>& args) {
    Isolate* isolate = Isolate::GetCurrent();
    HandleScope scope(isolate);

    //CHECK_ARGUMENT(args,2);
    GET_POINTER_ARG(DCCallVM, vm, args, 0);
    GET_INT32_ARG(mode, args, 1);

    dcMode((DCCallVM*) vm, mode);

    args.GetReturnValue().Set(v8::Undefined(isolate));
}

void Dyncall::argBool(const v8::FunctionCallbackInfo<v8::Value>& args) {
    Isolate* isolate = Isolate::GetCurrent();
    HandleScope scope(isolate);

    //CHECK_ARGUMENT(args,2);
    GET_POINTER_ARG(DCCallVM, vm, args, 0);
    GET_BOOL_ARG(value, args, 1);

    dcArgBool(vm, value);

    args.GetReturnValue().Set(v8::Undefined(isolate));
}

void Dyncall::argChar(const v8::FunctionCallbackInfo<v8::Value>& args) {
    Isolate* isolate = Isolate::GetCurrent();
    HandleScope scope(isolate);

    //CHECK_ARGUMENT(args,2);
    GET_POINTER_ARG(DCCallVM, vm, args, 0);
    GET_CHAR_ARG(value, args, 1);

    dcArgChar(vm, value);

    args.GetReturnValue().Set(v8::Undefined(isolate));
}

void Dyncall::argShort(const v8::FunctionCallbackInfo<v8::Value>& args) {
    Isolate* isolate = Isolate::GetCurrent();
    HandleScope scope(isolate);

    //CHECK_ARGUMENT(args,2);
    GET_POINTER_ARG(DCCallVM, vm, args, 0);
    GET_INT32_ARG(value, args, 1);

    dcArgShort(vm, value);

    args.GetReturnValue().Set(v8::Undefined(isolate));
}

void Dyncall::argInt(const v8::FunctionCallbackInfo<v8::Value>& args) {
    Isolate* isolate = Isolate::GetCurrent();
    HandleScope scope(isolate);

    //CHECK_ARGUMENT(args,2);
    GET_POINTER_ARG(DCCallVM, vm, args, 0);
    GET_INT32_ARG(value, args, 1);

    dcArgInt(vm, value);

    args.GetReturnValue().Set(v8::Undefined(isolate));
}

void Dyncall::argLong(const v8::FunctionCallbackInfo<v8::Value>& args) {
    Isolate* isolate = Isolate::GetCurrent();
    HandleScope scope(isolate);
    DClong value;

    //CHECK_ARGUMENT(args,2);
    GET_POINTER_ARG(DCCallVM, vm, args, 0);

    switch (sizeof (DClong)) {
        case sizeof (int32_t):
        {
            GET_INT32_ARG(int32Value, args, 1);
            value = int32Value;
        }
            break;
        case sizeof (int64_t):
        {
            GET_INT64_ARG(int64V, args, 1);
            value = static_cast<DClong> (int64V);
        }
            break;
        default:
        {
            std::stringstream message;
            message << "Unknown bytes length for DClong type: " << sizeof (DClong);
            THROW_EXCEPTION(message.str().c_str());
        }
    }

    dcArgLong(vm, value);

    args.GetReturnValue().Set(v8::Undefined(isolate));
}

void Dyncall::argLongLong(const v8::FunctionCallbackInfo<v8::Value>& args) {
    Isolate* isolate = Isolate::GetCurrent();
    HandleScope scope(isolate);
    DClonglong value;

    //CHECK_ARGUMENT(args,2);
    GET_POINTER_ARG(DCCallVM, vm, args, 0);

    switch (sizeof (DClonglong)) {
        case sizeof (int32_t):
        {
            GET_INT32_ARG(int32Value, args, 1);
            value = int32Value;
        }
            break;
        case sizeof (int64_t):
        {
            GET_INT64_ARG(int64V, args, 1);
            value = static_cast<DClonglong> (int64V);
        }
            break;
        default:
        {
            std::stringstream message;
            message << "Unknown bytes length for DClonglong type: " << sizeof (DClonglong);
            THROW_EXCEPTION(message.str().c_str());
        }
    }

    dcArgLongLong(vm, value);


    args.GetReturnValue().Set(v8::Undefined(isolate));
}

void Dyncall::argFloat(const v8::FunctionCallbackInfo<v8::Value>& args) {
    Isolate* isolate = Isolate::GetCurrent();
    HandleScope scope(isolate);

    //CHECK_ARGUMENT(args,2);
    GET_POINTER_ARG(DCCallVM, vm, args, 0);
    GET_FLOAT_ARG(value, args, 1);

    dcArgFloat(vm, value);

    args.GetReturnValue().Set(v8::Undefined(isolate));
}

void Dyncall::argDouble(const v8::FunctionCallbackInfo<v8::Value>& args) {
    Isolate* isolate = Isolate::GetCurrent();
    HandleScope scope(isolate);

    //CHECK_ARGUMENT(args,2);
    GET_POINTER_ARG(DCCallVM, vm, args, 0);
    GET_DOUBLE_ARG(value, args, 1);

    dcArgDouble(vm, value);

    args.GetReturnValue().Set(v8::Undefined(isolate));
}

void Dyncall::argPointer(const v8::FunctionCallbackInfo<v8::Value>& args) {
    Isolate* isolate = Isolate::GetCurrent();
    HandleScope scope(isolate);

    //CHECK_ARGUMENT(args,2);
    GET_POINTER_ARG(DCCallVM, vm, args, 0);
    GET_POINTER_ARG(char, value, args, 1);

    dcArgPointer(vm, value);

    args.GetReturnValue().Set(v8::Undefined(isolate));
}

void Dyncall::argStruct(const v8::FunctionCallbackInfo<v8::Value>& args) {
    Isolate* isolate = Isolate::GetCurrent();
    HandleScope scope(isolate);

    //CHECK_ARGUMENT(args,2);
    GET_POINTER_ARG(DCCallVM, vm, args, 0);
    GET_POINTER_ARG(DCstruct, structPtr, args, 1);
    GET_POINTER_ARG(char, value, args, 2);


    dcArgStruct(vm, structPtr, value);

    args.GetReturnValue().Set(v8::Undefined(isolate));
}

void Dyncall::callVoid(const v8::FunctionCallbackInfo<v8::Value>& args) {
    Isolate* isolate = Isolate::GetCurrent();
    HandleScope scope(isolate);

    GET_CALL_ARG(args)

    dcCallVoid(pVm, pFunction);

    args.GetReturnValue().Set(v8::Undefined(isolate));
}

void Dyncall::callBool(const v8::FunctionCallbackInfo<v8::Value>& args) {
    Isolate* isolate = Isolate::GetCurrent();
    HandleScope scope(isolate);

    GET_CALL_ARG(args)

    args.GetReturnValue().Set(v8::Boolean::New(isolate,static_cast<bool> (dcCallBool(pVm, pFunction))));
}

void Dyncall::callChar(const v8::FunctionCallbackInfo<v8::Value>& args) {
    Isolate* isolate = Isolate::GetCurrent();
    HandleScope scope(isolate);
    std::stringstream retString;

    GET_CALL_ARG(args);

    retString << dcCallChar(pVm, pFunction);

    args.GetReturnValue().Set(String::NewFromUtf8(isolate,retString.str().c_str()));
}

void Dyncall::callShort(const v8::FunctionCallbackInfo<v8::Value>& args) {
    Isolate* isolate = Isolate::GetCurrent();
    HandleScope scope(isolate);

    GET_CALL_ARG(args);

    args.GetReturnValue().Set(v8::Int32::New(isolate,dcCallShort(pVm, pFunction)));
}

void Dyncall::callInt(const v8::FunctionCallbackInfo<v8::Value>& args) {
    Isolate* isolate = Isolate::GetCurrent();
    HandleScope scope(isolate);

    GET_CALL_ARG(args);

    args.GetReturnValue().Set(v8::Int32::New(isolate,dcCallInt(pVm, pFunction)));
}

void Dyncall::callLong(const v8::FunctionCallbackInfo<v8::Value>& args) {
    Isolate* isolate = Isolate::GetCurrent();
    HandleScope scope(isolate);

    GET_CALL_ARG(args);

    if (sizeof (DClong) == 4) {
        args.GetReturnValue().Set(v8::Int32::New(isolate,dcCallLong(pVm, pFunction)));
    } else if (sizeof (DClong) == 8) {
        args.GetReturnValue().Set(v8::Integer::New(isolate,dcCallLong(pVm, pFunction)));
    } else {
        std::stringstream message;
        message << "Unknown bytes length for =long type: " << sizeof (long);

        THROW_EXCEPTION(message.str().c_str());
    }
}

void Dyncall::callLongLong(const v8::FunctionCallbackInfo<v8::Value>& args) {
    Isolate* isolate = Isolate::GetCurrent();
    HandleScope scope(isolate);

    GET_CALL_ARG(args);

    if (sizeof (DClonglong) == 4) {
        args.GetReturnValue().Set(v8::Int32::New(isolate,static_cast<int32_t> (dcCallLongLong(pVm, pFunction))));
    } else if (sizeof (DClonglong) == 8) {
        args.GetReturnValue().Set(v8::Integer::New(isolate,static_cast<int32_t> (dcCallLongLong(pVm, pFunction))));
    } else {
        std::stringstream message;
        message << "Unknown bytes length for =long type: " << sizeof (long);

        THROW_EXCEPTION(message.str().c_str());
    }
}

void Dyncall::callFloat(const v8::FunctionCallbackInfo<v8::Value>& args) {
    Isolate* isolate = Isolate::GetCurrent();
    HandleScope scope(isolate);

    GET_CALL_ARG(args);

    args.GetReturnValue().Set(v8::Number::New(isolate,dcCallFloat(pVm, pFunction)));
}

void Dyncall::callDouble(const v8::FunctionCallbackInfo<v8::Value>& args) {
    Isolate* isolate = Isolate::GetCurrent();
    HandleScope scope(isolate);

    GET_CALL_ARG(args);

    args.GetReturnValue().Set(v8::Number::New(isolate,dcCallDouble(pVm, pFunction)));
}

void Dyncall::callPointer(const v8::FunctionCallbackInfo<v8::Value>& args) {
    Isolate* isolate = Isolate::GetCurrent();
    HandleScope scope(isolate);
    GET_CALL_ARG(args);

    args.GetReturnValue().Set(bridjs::Utils::wrapPointer(isolate, dcCallPointer(pVm, pFunction)));
}

void Dyncall::callStruct(const v8::FunctionCallbackInfo<v8::Value>& args) {
    Isolate* isolate = Isolate::GetCurrent();
    HandleScope scope(isolate);

    GET_CALL_ARG(args);
    GET_POINTER_ARG(DCstruct, structPtr, args, 2);
    GET_POINTER_ARG(char, value, args, 3);

    dcCallStruct(pVm, pFunction, structPtr, value);

    args.GetReturnValue().Set(v8::Undefined(isolate));
}

void Dyncall::getError(const v8::FunctionCallbackInfo<v8::Value>& args) {
    Isolate* isolate = Isolate::GetCurrent();
    HandleScope scope(isolate);

    GET_POINTER_ARG(DCCallVM, pVm, args, 0);

    dcGetError(pVm);

    args.GetReturnValue().Set(v8::Int32::New(isolate,dcGetError(pVm)));
}

void Dyncall::newStruct(const v8::FunctionCallbackInfo<v8::Value>& args) {
    Isolate* isolate = Isolate::GetCurrent();
    HandleScope scope(isolate);

    GET_INT64_ARG(fieldCount, args, 0);
    GET_INT32_ARG(aligment, args, 1);

    dcNewStruct(fieldCount, aligment);

    args.GetReturnValue().Set(bridjs::Utils::wrapPointer(isolate,dcNewStruct(fieldCount, aligment)));
}

void Dyncall::structField(const v8::FunctionCallbackInfo<v8::Value>& args) {
    Isolate* isolate = Isolate::GetCurrent();
    HandleScope scope(isolate);

    GET_POINTER_ARG(DCstruct, structPtr, args, 0);
    GET_CHAR_ARG(type, args, 1);
    GET_INT32_ARG(aligment, args, 2);
    GET_INT64_ARG(arrayLength, args, 3);

    dcStructField(structPtr, type, aligment, arrayLength);

    args.GetReturnValue().Set(v8::Undefined(isolate));
}

void Dyncall::subStruct(const v8::FunctionCallbackInfo<v8::Value>& args) {
    Isolate* isolate = Isolate::GetCurrent();
    HandleScope scope(isolate);

    GET_POINTER_ARG(DCstruct, structPtr, args, 0);
    GET_INT64_ARG(fieldCount, args, 1);
    GET_INT32_ARG(aligment, args, 2);
    GET_INT64_ARG(arrayLength, args, 3);

    dcSubStruct(structPtr, fieldCount, aligment, arrayLength);

    args.GetReturnValue().Set(v8::Undefined(isolate));
}

void Dyncall::closeStruct(const v8::FunctionCallbackInfo<v8::Value>& args) {
    Isolate* isolate = Isolate::GetCurrent();
    HandleScope scope(isolate);

    GET_POINTER_ARG(DCstruct, structPtr, args, 0);

    dcCloseStruct(structPtr);

    args.GetReturnValue().Set(v8::Undefined(isolate));
}

/*
void Dyncall::structAlignment(const v8::FunctionCallbackInfo<v8::Value>& args){
        Isolate* isolate = Isolate::GetCurrent(); HandleScope scope(isolate);

        GET_POINTER_ARG(DCstruct,structPtr, args,0);

        args.GetReturnValue().Set(v8::Integer::New(dcStructAlignment(structPtr)));
}*/

void Dyncall::freeStruct(const v8::FunctionCallbackInfo<v8::Value>& args) {
    Isolate* isolate = Isolate::GetCurrent();
    HandleScope scope(isolate);

    GET_POINTER_ARG(DCstruct, structPtr, args, 0);

    dcFreeStruct(structPtr);

    args.GetReturnValue().Set(v8::Undefined(isolate));
}

void Dyncall::structSize(const v8::FunctionCallbackInfo<v8::Value>& args) {
    Isolate* isolate = Isolate::GetCurrent();
    HandleScope scope(isolate);

    GET_POINTER_ARG(DCstruct, structPtr, args, 0);

    args.GetReturnValue().Set(WRAP_UINT(static_cast<uint32_t> (dcStructSize(structPtr))));
}

void Dyncall::defineStruct(const v8::FunctionCallbackInfo<v8::Value>& args) {
    Isolate* isolate = Isolate::GetCurrent();
    HandleScope scope(isolate);

    GET_STRING_ARG(signature, args, 0);

    args.GetReturnValue().Set(WRAP_POINTER(dcDefineStruct(*v8::String::Utf8Value(signature))));
}