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
#include "dyncall_v8.h"
#include "dyncallback_v8.h"
#include "dynload_v8.h"
#include "pointer_v8.h"
#include "native_function_v8.h"
#include "dyncall_v8_utils.h"
#include "struct_v8.h"
#include "test.h"
#include "array_struct_v8.h"

#include <iostream>
#include <node.h>

using namespace v8;

void getSignedAndUnsinedTypeFromTypeSize(const uint32_t size, char &signedType, char &unsignedType) {
    switch (size) {
        case sizeof (DCchar):
            unsignedType = DC_SIGCHAR_UCHAR;
            signedType = DC_SIGCHAR_CHAR;
            break;
        case sizeof (DCshort):
            unsignedType = DC_SIGCHAR_USHORT;
            signedType = DC_SIGCHAR_SHORT;
            break;
        case sizeof (DCint):
            unsignedType = DC_SIGCHAR_UINT;
            signedType = DC_SIGCHAR_INT;
            break;
            break;
        case sizeof (DClonglong):
            unsignedType = DC_SIGCHAR_ULONGLONG;
            signedType = DC_SIGCHAR_LONGLONG;
            break;
        default:
            if (size == sizeof (DClong)) {
                unsignedType = DC_SIGCHAR_ULONG;
                signedType = DC_SIGCHAR_LONG;
            } else {
                unsignedType = signedType = DC_SIGCHAR_VOID;
            }
    }
}

void init(Handle<Object> target) {
    HandleScope scope;

    Local<Object> dynloadObj = Object::New();
    Local<Object> dyncallObj = Object::New();
    Local<Object> dyncallBackObj = Object::New();
    Local<Object> signatureObj = Object::New();
    Local<Object> utilsObj = Object::New();
    Local<Object> testObj = Object::New();
    char signedType = DC_SIGCHAR_VOID, unsignedType = DC_SIGCHAR_VOID;

#ifdef _DEBUG
    std::cerr << "bridjs::init() - Warning, it is debug version" << std::endl;
#endif

    /*dynload*/
    target->Set(String::NewSymbol("dl"), dynloadObj);
    bridjs::Dynload::Init(dynloadObj);

    /*dyncall*/
    target->Set(String::NewSymbol("dc"), dyncallObj);

    dyncallObj->Set(String::NewSymbol("CALL_C_DEFAULT"),
            v8::Int32::New(DC_CALL_C_DEFAULT), ReadOnly);
    dyncallObj->Set(String::NewSymbol("CALL_C_ELLIPSIS"),
            v8::Int32::New(DC_CALL_C_ELLIPSIS), ReadOnly);
    dyncallObj->Set(String::NewSymbol("CALL_C_ELLIPSIS_VARARGS"),
            v8::Int32::New(DC_CALL_C_ELLIPSIS_VARARGS), ReadOnly);
    dyncallObj->Set(String::NewSymbol("CALL_C_CDECL"),
            v8::Int32::New(DC_CALL_C_X86_CDECL), ReadOnly);
    dyncallObj->Set(String::NewSymbol("CALL_C_X86_WIN32_STD"),
            v8::Int32::New(DC_CALL_C_X86_WIN32_STD), ReadOnly);
    dyncallObj->Set(String::NewSymbol("CALL_C_X86_WIN32_FAST_MS"),
            v8::Int32::New(DC_CALL_C_X86_WIN32_FAST_MS), ReadOnly);
    dyncallObj->Set(String::NewSymbol("CALL_C_X86_WIN32_FAST_GNU"),
            v8::Int32::New(DC_CALL_C_X86_WIN32_FAST_GNU), ReadOnly);
    dyncallObj->Set(String::NewSymbol("CALL_C_X86_WIN32_THIS_MS"),
            v8::Int32::New(DC_CALL_C_X86_WIN32_THIS_MS), ReadOnly);
    dyncallObj->Set(String::NewSymbol("CALL_C_X86_WIN32_THIS_GNU"),
            v8::Int32::New(DC_CALL_C_X86_WIN32_THIS_GNU), ReadOnly);
    dyncallObj->Set(String::NewSymbol("CALL_C_X64_WIN64"),
            v8::Int32::New(DC_CALL_C_X64_WIN64), ReadOnly);
    dyncallObj->Set(String::NewSymbol("CALL_C_X64_SYSV"),
            v8::Int32::New(DC_CALL_C_X64_SYSV), ReadOnly);
    dyncallObj->Set(String::NewSymbol("CALL_C_PPC32_DARWIN"),
            v8::Int32::New(DC_CALL_C_PPC32_DARWIN), ReadOnly);
    dyncallObj->Set(String::NewSymbol("CALL_C_PPC32_OSX"),
            v8::Int32::New(DC_CALL_C_PPC32_OSX), ReadOnly);
    dyncallObj->Set(String::NewSymbol("CALL_C_ARM_ARM_EABI"),
            v8::Int32::New(DC_CALL_C_ARM_ARM_EABI), ReadOnly);
    dyncallObj->Set(String::NewSymbol("CALL_C_ARM_THUMB_EABI"),
            v8::Int32::New(DC_CALL_C_ARM_THUMB_EABI), ReadOnly);
    dyncallObj->Set(String::NewSymbol("CALL_C_MIPS32_EABI"),
            v8::Int32::New(DC_CALL_C_MIPS32_EABI), ReadOnly);
    dyncallObj->Set(String::NewSymbol("CALL_C_PPC32_SYSV"),
            v8::Int32::New(DC_CALL_C_PPC32_SYSV), ReadOnly);
    dyncallObj->Set(String::NewSymbol("CALL_C_PPC32_LINUX"),
            v8::Int32::New(DC_CALL_C_PPC32_LINUX), ReadOnly);
    dyncallObj->Set(String::NewSymbol("CALL_C_ARM_ARM"),
            v8::Int32::New(DC_CALL_C_ARM_ARM), ReadOnly);
    dyncallObj->Set(String::NewSymbol("CALL_C_ARM_THUMB"),
            v8::Int32::New(DC_CALL_C_ARM_THUMB), ReadOnly);
    dyncallObj->Set(String::NewSymbol("CALL_C_MIPS32_O32"),
            v8::Int32::New(DC_CALL_C_MIPS32_O32), ReadOnly);
    dyncallObj->Set(String::NewSymbol("CALL_C_MIPS64_N32"),
            v8::Int32::New(DC_CALL_C_MIPS64_N32), ReadOnly);
    dyncallObj->Set(String::NewSymbol("CALL_C_MIPS64_N64"),
            v8::Int32::New(DC_CALL_C_MIPS64_N64), ReadOnly);
    dyncallObj->Set(String::NewSymbol("CALL_C_X86_PLAN9"),
            v8::Int32::New(DC_CALL_C_X86_PLAN9), ReadOnly);
    dyncallObj->Set(String::NewSymbol("CALL_C_SPARC32"),
            v8::Int32::New(DC_CALL_C_SPARC32), ReadOnly);
    dyncallObj->Set(String::NewSymbol("CALL_C_SPARC64"),
            v8::Int32::New(DC_CALL_C_SPARC64), ReadOnly);
    dyncallObj->Set(String::NewSymbol("CALL_SYS_DEFAULT"),
            v8::Int32::New(DC_CALL_SYS_DEFAULT), ReadOnly);
    dyncallObj->Set(String::NewSymbol("CALL_SYS_X86_INT80H_LINUX"),
            v8::Int32::New(DC_CALL_SYS_X86_INT80H_LINUX), ReadOnly);
    dyncallObj->Set(String::NewSymbol("CALL_SYS_X86_INT80H_BSD"),
            v8::Int32::New(DC_CALL_SYS_X86_INT80H_BSD), ReadOnly);

    dyncallObj->Set(String::NewSymbol("DEFAULT_ALIGNMENT"),
            v8::Uint32::New(DEFAULT_ALIGNMENT), ReadOnly);

    dyncallObj->Set(String::NewSymbol("Signature"),
            signatureObj, ReadOnly);

    /*Signature*/
    signatureObj->Set(String::NewSymbol("VOID_TYPE"),
            bridjs::Utils::toV8String(DC_SIGCHAR_VOID), ReadOnly);
    signatureObj->Set(String::NewSymbol("BOOL_TYPE"),
            bridjs::Utils::toV8String(DC_SIGCHAR_BOOL), ReadOnly);
    signatureObj->Set(String::NewSymbol("UCHAR_TYPE"),
            bridjs::Utils::toV8String(DC_SIGCHAR_UCHAR), ReadOnly);
    signatureObj->Set(String::NewSymbol("CHAR_TYPE"),
            bridjs::Utils::toV8String(DC_SIGCHAR_CHAR), ReadOnly);
    signatureObj->Set(String::NewSymbol("SHORT_TYPE"),
            bridjs::Utils::toV8String(DC_SIGCHAR_SHORT), ReadOnly);
    signatureObj->Set(String::NewSymbol("UCHAR_TYPE"),
            bridjs::Utils::toV8String(DC_SIGCHAR_USHORT), ReadOnly);
    signatureObj->Set(String::NewSymbol("INT_TYPE"),
            bridjs::Utils::toV8String(DC_SIGCHAR_INT), ReadOnly);
    signatureObj->Set(String::NewSymbol("UINT_TYPE"),
            bridjs::Utils::toV8String(DC_SIGCHAR_UINT), ReadOnly);
    signatureObj->Set(String::NewSymbol("LONG_TYPE"),
            bridjs::Utils::toV8String(DC_SIGCHAR_LONG), ReadOnly);
    signatureObj->Set(String::NewSymbol("ULONG_TYPE"),
            bridjs::Utils::toV8String(DC_SIGCHAR_ULONG), ReadOnly);
    signatureObj->Set(String::NewSymbol("LONGLONG_TYPE"),
            bridjs::Utils::toV8String(DC_SIGCHAR_LONGLONG), ReadOnly);
    signatureObj->Set(String::NewSymbol("ULONG_TYPE"),
            bridjs::Utils::toV8String(DC_SIGCHAR_ULONG), ReadOnly);
    signatureObj->Set(String::NewSymbol("DOUBLE_TYPE"),
            bridjs::Utils::toV8String(DC_SIGCHAR_DOUBLE), ReadOnly);
    signatureObj->Set(String::NewSymbol("FLOAT_TYPE"),
            bridjs::Utils::toV8String(DC_SIGCHAR_FLOAT), ReadOnly);
    signatureObj->Set(String::NewSymbol("POINTER_TYPE"),
            bridjs::Utils::toV8String(DC_SIGCHAR_POINTER), ReadOnly);
    signatureObj->Set(String::NewSymbol("STRING_TYPE"),
            bridjs::Utils::toV8String(DC_SIGCHAR_STRING), ReadOnly);
    signatureObj->Set(String::NewSymbol("STRUCT_TYPE"),
            bridjs::Utils::toV8String(DC_SIGCHAR_STRUCT), ReadOnly);

    signatureObj->Set(String::NewSymbol("CALLBACK_TYPE"),
            bridjs::Utils::toV8String(DC_SIGCHAR_POINTER), ReadOnly);

    signatureObj->Set(String::NewSymbol("ENDARG"),
            bridjs::Utils::toV8String(DC_SIGCHAR_ENDARG), ReadOnly);
    /*INT8*/
    getSignedAndUnsinedTypeFromTypeSize(sizeof (int8_t), signedType, unsignedType);

    signatureObj->Set(String::NewSymbol("INT8_TYPE"),
            bridjs::Utils::toV8String(signedType), ReadOnly);
    signatureObj->Set(String::NewSymbol("UINT8_TYPE"),
            bridjs::Utils::toV8String(unsignedType), ReadOnly);

    /*INT16*/
    getSignedAndUnsinedTypeFromTypeSize(sizeof (int16_t), signedType, unsignedType);
    signatureObj->Set(String::NewSymbol("INT16_TYPE"),
            bridjs::Utils::toV8String(signedType), ReadOnly);
    signatureObj->Set(String::NewSymbol("UINT16_TYPE"),
            bridjs::Utils::toV8String(unsignedType), ReadOnly);

    /*INT32*/
    getSignedAndUnsinedTypeFromTypeSize(sizeof (int32_t), signedType, unsignedType);
    //char type[] = {signedType,'\0'};
    //std::cout<<"fefe "<<*(v8::String::Utf8Value(bridjs::Utils::toV8String(signedType)))<<std::endl;
    signatureObj->Set(String::NewSymbol("INT32_TYPE"),
            bridjs::Utils::toV8String(signedType), ReadOnly);
    signatureObj->Set(String::NewSymbol("UINT32_TYPE"),
            bridjs::Utils::toV8String(unsignedType), ReadOnly);

    /*INT64*/
    getSignedAndUnsinedTypeFromTypeSize(sizeof (int64_t), signedType, unsignedType);
    signatureObj->Set(String::NewSymbol("INT64_TYPE"),
            bridjs::Utils::toV8String(signedType), ReadOnly);
    signatureObj->Set(String::NewSymbol("UINT64_TYPE"),
            bridjs::Utils::toV8String(unsignedType), ReadOnly);

	/*size_t*/
	getSignedAndUnsinedTypeFromTypeSize(sizeof (size_t), signedType, unsignedType);
	signatureObj->Set(String::NewSymbol("SIZE_TYPE"),
		bridjs::Utils::toV8String(signedType), ReadOnly);
	signatureObj->Set(String::NewSymbol("SIZE_TYPE"),
		bridjs::Utils::toV8String(unsignedType), ReadOnly);

    EXPORT_FUNCTION(dyncallObj, bridjs::Dyncall, newCallVM);
    EXPORT_FUNCTION(dyncallObj, bridjs::Dyncall, free);
    EXPORT_FUNCTION(dyncallObj, bridjs::Dyncall, reset);
    EXPORT_FUNCTION(dyncallObj, bridjs::Dyncall, mode);
    EXPORT_FUNCTION(dyncallObj, bridjs::Dyncall, argBool);
    EXPORT_FUNCTION(dyncallObj, bridjs::Dyncall, argShort);
    EXPORT_FUNCTION(dyncallObj, bridjs::Dyncall, argInt);
    EXPORT_FUNCTION(dyncallObj, bridjs::Dyncall, argLong);
    EXPORT_FUNCTION(dyncallObj, bridjs::Dyncall, argLongLong);
    EXPORT_FUNCTION(dyncallObj, bridjs::Dyncall, argFloat);
    EXPORT_FUNCTION(dyncallObj, bridjs::Dyncall, argDouble);
    EXPORT_FUNCTION(dyncallObj, bridjs::Dyncall, argStruct);
    EXPORT_FUNCTION(dyncallObj, bridjs::Dyncall, argPointer);

    EXPORT_FUNCTION(dyncallObj, bridjs::Dyncall, callVoid);
    EXPORT_FUNCTION(dyncallObj, bridjs::Dyncall, callBool);
    EXPORT_FUNCTION(dyncallObj, bridjs::Dyncall, callChar);
    EXPORT_FUNCTION(dyncallObj, bridjs::Dyncall, callShort);
    EXPORT_FUNCTION(dyncallObj, bridjs::Dyncall, callInt);
    EXPORT_FUNCTION(dyncallObj, bridjs::Dyncall, callLong);
    EXPORT_FUNCTION(dyncallObj, bridjs::Dyncall, callLongLong);
    EXPORT_FUNCTION(dyncallObj, bridjs::Dyncall, callFloat);
    EXPORT_FUNCTION(dyncallObj, bridjs::Dyncall, callDouble);
    EXPORT_FUNCTION(dyncallObj, bridjs::Dyncall, callStruct);
    EXPORT_FUNCTION(dyncallObj, bridjs::Dyncall, getError);
    EXPORT_FUNCTION(dyncallObj, bridjs::Dyncall, newStruct);
    EXPORT_FUNCTION(dyncallObj, bridjs::Dyncall, structField);
    EXPORT_FUNCTION(dyncallObj, bridjs::Dyncall, subStruct);
    EXPORT_FUNCTION(dyncallObj, bridjs::Dyncall, closeStruct);
    //EXPORT_FUNCTION(dyncallObj,bridjs::Dyncall, structAlignment);
    EXPORT_FUNCTION(dyncallObj, bridjs::Dyncall, freeStruct);
    EXPORT_FUNCTION(dyncallObj, bridjs::Dyncall, structSize);

    bridjs::Pointer::Init(dyncallObj);
    bridjs::NativeFunction::Init(dyncallObj);
    bridjs::Struct::Init(dyncallObj);
    bridjs::ArrayStruct::Init(dyncallObj);

    /*dyncallback*/
    target->Set(String::NewSymbol("dcb"), dyncallBackObj, ReadOnly);
    bridjs::Dyncallback::Init(dyncallBackObj);

    /*Utils module*/
    target->Set(String::NewSymbol("utils"), utilsObj, ReadOnly);
    bridjs::Utils::Init(utilsObj);

    /*Test module*/
    target->Set(String::NewSymbol("test"), testObj);
    NODE_SET_METHOD(testObj, "testMultiplyFunction", bridjs::Test::TestMultiplyFunction);

    std::locale::global(std::locale(""));
    std::wcout.imbue(std::locale(""));

    //std::cout<<sizeof(TestStruct)<<", "<<sizeof(DClong)<<", "<<sizeof(long)<<std::endl;
    //TempStruct tempStruct;

    //std::cout<<sizeof(TestArrayStruct)<<", "<<sizeof(TempStruct)<<", "<<(uint64_t*)&tempStruct.point-(uint64_t*)&tempStruct.w<<", "<<sizeof(TestComplexStruct)<<std::endl;
}


NODE_MODULE(bridjs, init);
