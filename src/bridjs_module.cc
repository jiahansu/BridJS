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
#include "union_struct_v8.h"

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
    Isolate* isolate = Isolate::GetCurrent();
    HandleScope scope(isolate);

    Local<Object> dynloadObj = Object::New(isolate);
    Local<Object> dyncallObj = Object::New(isolate);
    Local<Object> dyncallBackObj = Object::New(isolate);
    Local<Object> signatureObj = Object::New(isolate);
    Local<Object> utilsObj = Object::New(isolate);
    Local<Object> testObj = Object::New(isolate);
    char signedType = DC_SIGCHAR_VOID, unsignedType = DC_SIGCHAR_VOID;

#ifdef _DEBUG
    std::cerr << "bridjs::init() - Warning, it is debug version" << std::endl;
#endif

    /*dynload*/
    target->ForceSet(v8::String::NewFromUtf8(isolate,"dl"), dynloadObj);
    bridjs::Dynload::Init(dynloadObj);

    /*dyncall*/
    target->ForceSet(v8::String::NewFromUtf8(isolate,"dc", v8::String::kInternalizedString), dyncallObj);

    dyncallObj->ForceSet(v8::String::NewFromUtf8(isolate,"CALL_C_DEFAULT", v8::String::kInternalizedString),
            v8::Int32::New(isolate,DC_CALL_C_DEFAULT), ReadOnly);
    dyncallObj->ForceSet(v8::String::NewFromUtf8(isolate,"CALL_C_ELLIPSIS", v8::String::kInternalizedString),
            v8::Int32::New(isolate,DC_CALL_C_ELLIPSIS), ReadOnly);
    dyncallObj->ForceSet(v8::String::NewFromUtf8(isolate,"CALL_C_ELLIPSIS_VARARGS", v8::String::kInternalizedString),
            v8::Int32::New(isolate,DC_CALL_C_ELLIPSIS_VARARGS), ReadOnly);
    dyncallObj->ForceSet(v8::String::NewFromUtf8(isolate,"CALL_C_CDECL", v8::String::kInternalizedString),
            v8::Int32::New(isolate,DC_CALL_C_X86_CDECL), ReadOnly);
    dyncallObj->ForceSet(v8::String::NewFromUtf8(isolate,"CALL_C_X86_WIN32_STD", v8::String::kInternalizedString),
            v8::Int32::New(isolate,DC_CALL_C_X86_WIN32_STD), ReadOnly);
    dyncallObj->ForceSet(v8::String::NewFromUtf8(isolate,"CALL_C_X86_WIN32_FAST_MS", v8::String::kInternalizedString),
            v8::Int32::New(isolate,DC_CALL_C_X86_WIN32_FAST_MS), ReadOnly);
    dyncallObj->ForceSet(v8::String::NewFromUtf8(isolate,"CALL_C_X86_WIN32_FAST_GNU", v8::String::kInternalizedString),
            v8::Int32::New(isolate,DC_CALL_C_X86_WIN32_FAST_GNU), ReadOnly);
    dyncallObj->ForceSet(v8::String::NewFromUtf8(isolate,"CALL_C_X86_WIN32_THIS_MS", v8::String::kInternalizedString),
            v8::Int32::New(isolate,DC_CALL_C_X86_WIN32_THIS_MS), ReadOnly);
    dyncallObj->ForceSet(v8::String::NewFromUtf8(isolate,"CALL_C_X86_WIN32_THIS_GNU", v8::String::kInternalizedString),
            v8::Int32::New(isolate,DC_CALL_C_X86_WIN32_THIS_GNU), ReadOnly);
    dyncallObj->ForceSet(v8::String::NewFromUtf8(isolate,"CALL_C_X64_WIN64", v8::String::kInternalizedString),
            v8::Int32::New(isolate,DC_CALL_C_X64_WIN64), ReadOnly);
    dyncallObj->ForceSet(v8::String::NewFromUtf8(isolate,"CALL_C_X64_SYSV", v8::String::kInternalizedString),
            v8::Int32::New(isolate,DC_CALL_C_X64_SYSV), ReadOnly);
    dyncallObj->ForceSet(v8::String::NewFromUtf8(isolate,"CALL_C_PPC32_DARWIN", v8::String::kInternalizedString),
            v8::Int32::New(isolate,DC_CALL_C_PPC32_DARWIN), ReadOnly);
    dyncallObj->ForceSet(v8::String::NewFromUtf8(isolate,"CALL_C_PPC32_OSX", v8::String::kInternalizedString),
            v8::Int32::New(isolate,DC_CALL_C_PPC32_OSX), ReadOnly);
    dyncallObj->ForceSet(v8::String::NewFromUtf8(isolate,"CALL_C_ARM_ARM_EABI", v8::String::kInternalizedString),
            v8::Int32::New(isolate,DC_CALL_C_ARM_ARM_EABI), ReadOnly);
    dyncallObj->ForceSet(v8::String::NewFromUtf8(isolate,"CALL_C_ARM_THUMB_EABI", v8::String::kInternalizedString),
            v8::Int32::New(isolate,DC_CALL_C_ARM_THUMB_EABI), ReadOnly);
    dyncallObj->ForceSet(v8::String::NewFromUtf8(isolate,"CALL_C_MIPS32_EABI", v8::String::kInternalizedString),
            v8::Int32::New(isolate,DC_CALL_C_MIPS32_EABI), ReadOnly);
    dyncallObj->ForceSet(v8::String::NewFromUtf8(isolate,"CALL_C_PPC32_SYSV", v8::String::kInternalizedString),
            v8::Int32::New(isolate,DC_CALL_C_PPC32_SYSV), ReadOnly);
    dyncallObj->ForceSet(v8::String::NewFromUtf8(isolate,"CALL_C_PPC32_LINUX", v8::String::kInternalizedString),
            v8::Int32::New(isolate,DC_CALL_C_PPC32_LINUX), ReadOnly);
    dyncallObj->ForceSet(v8::String::NewFromUtf8(isolate,"CALL_C_ARM_ARM", v8::String::kInternalizedString),
            v8::Int32::New(isolate,DC_CALL_C_ARM_ARM), ReadOnly);
    dyncallObj->ForceSet(v8::String::NewFromUtf8(isolate,"CALL_C_ARM_THUMB", v8::String::kInternalizedString),
            v8::Int32::New(isolate,DC_CALL_C_ARM_THUMB), ReadOnly);
    dyncallObj->ForceSet(v8::String::NewFromUtf8(isolate,"CALL_C_MIPS32_O32", v8::String::kInternalizedString),
            v8::Int32::New(isolate,DC_CALL_C_MIPS32_O32), ReadOnly);
    dyncallObj->ForceSet(v8::String::NewFromUtf8(isolate,"CALL_C_MIPS64_N32", v8::String::kInternalizedString),
            v8::Int32::New(isolate,DC_CALL_C_MIPS64_N32), ReadOnly);
    dyncallObj->ForceSet(v8::String::NewFromUtf8(isolate,"CALL_C_MIPS64_N64", v8::String::kInternalizedString),
            v8::Int32::New(isolate,DC_CALL_C_MIPS64_N64), ReadOnly);
    dyncallObj->ForceSet(v8::String::NewFromUtf8(isolate,"CALL_C_X86_PLAN9", v8::String::kInternalizedString),
            v8::Int32::New(isolate,DC_CALL_C_X86_PLAN9), ReadOnly);
    dyncallObj->ForceSet(v8::String::NewFromUtf8(isolate,"CALL_C_SPARC32", v8::String::kInternalizedString),
            v8::Int32::New(isolate,DC_CALL_C_SPARC32), ReadOnly);
    dyncallObj->ForceSet(v8::String::NewFromUtf8(isolate,"CALL_C_SPARC64", v8::String::kInternalizedString),
            v8::Int32::New(isolate,DC_CALL_C_SPARC64), ReadOnly);
    dyncallObj->ForceSet(v8::String::NewFromUtf8(isolate,"CALL_SYS_DEFAULT", v8::String::kInternalizedString),
            v8::Int32::New(isolate,DC_CALL_SYS_DEFAULT), ReadOnly);
    dyncallObj->ForceSet(v8::String::NewFromUtf8(isolate,"CALL_SYS_X86_INT80H_LINUX", v8::String::kInternalizedString),
            v8::Int32::New(isolate,DC_CALL_SYS_X86_INT80H_LINUX), ReadOnly);
    dyncallObj->ForceSet(v8::String::NewFromUtf8(isolate,"CALL_SYS_X86_INT80H_BSD", v8::String::kInternalizedString),
            v8::Int32::New(isolate,DC_CALL_SYS_X86_INT80H_BSD), ReadOnly);

    dyncallObj->ForceSet(v8::String::NewFromUtf8(isolate,"DEFAULT_ALIGNMENT", v8::String::kInternalizedString),
            v8::Uint32::New(isolate,DEFAULT_ALIGNMENT), ReadOnly);

    dyncallObj->ForceSet(v8::String::NewFromUtf8(isolate,"Signature"),
            signatureObj, ReadOnly);

    /*Signature*/
    signatureObj->ForceSet(v8::String::NewFromUtf8(isolate,"VOID_TYPE", v8::String::kInternalizedString),
            bridjs::Utils::toV8String(isolate,DC_SIGCHAR_VOID), ReadOnly);
    signatureObj->ForceSet(v8::String::NewFromUtf8(isolate,"BOOL_TYPE", v8::String::kInternalizedString),
            bridjs::Utils::toV8String(isolate,DC_SIGCHAR_BOOL), ReadOnly);
    signatureObj->ForceSet(v8::String::NewFromUtf8(isolate,"UCHAR_TYPE", v8::String::kInternalizedString),
            bridjs::Utils::toV8String(isolate,DC_SIGCHAR_UCHAR), ReadOnly);
    signatureObj->ForceSet(v8::String::NewFromUtf8(isolate,"CHAR_TYPE", v8::String::kInternalizedString),
            bridjs::Utils::toV8String(isolate,DC_SIGCHAR_CHAR), ReadOnly);
    signatureObj->ForceSet(v8::String::NewFromUtf8(isolate,"SHORT_TYPE", v8::String::kInternalizedString),
            bridjs::Utils::toV8String(isolate,DC_SIGCHAR_SHORT), ReadOnly);
    signatureObj->ForceSet(v8::String::NewFromUtf8(isolate,"UCHAR_TYPE", v8::String::kInternalizedString),
            bridjs::Utils::toV8String(isolate,DC_SIGCHAR_USHORT), ReadOnly);
    signatureObj->ForceSet(v8::String::NewFromUtf8(isolate,"INT_TYPE", v8::String::kInternalizedString),
            bridjs::Utils::toV8String(isolate,DC_SIGCHAR_INT), ReadOnly);
    signatureObj->ForceSet(v8::String::NewFromUtf8(isolate,"UINT_TYPE", v8::String::kInternalizedString),
            bridjs::Utils::toV8String(isolate,DC_SIGCHAR_UINT), ReadOnly);
    signatureObj->ForceSet(v8::String::NewFromUtf8(isolate,"LONG_TYPE", v8::String::kInternalizedString),
            bridjs::Utils::toV8String(isolate,DC_SIGCHAR_LONG), ReadOnly);
    signatureObj->ForceSet(v8::String::NewFromUtf8(isolate,"ULONG_TYPE", v8::String::kInternalizedString),
            bridjs::Utils::toV8String(isolate,DC_SIGCHAR_ULONG), ReadOnly);
    signatureObj->ForceSet(v8::String::NewFromUtf8(isolate,"LONGLONG_TYPE", v8::String::kInternalizedString),
            bridjs::Utils::toV8String(isolate,DC_SIGCHAR_LONGLONG), ReadOnly);
    signatureObj->ForceSet(v8::String::NewFromUtf8(isolate,"ULONG_TYPE", v8::String::kInternalizedString),
            bridjs::Utils::toV8String(isolate,DC_SIGCHAR_ULONG), ReadOnly);
    signatureObj->ForceSet(v8::String::NewFromUtf8(isolate,"DOUBLE_TYPE", v8::String::kInternalizedString),
            bridjs::Utils::toV8String(isolate,DC_SIGCHAR_DOUBLE), ReadOnly);
    signatureObj->ForceSet(v8::String::NewFromUtf8(isolate,"FLOAT_TYPE", v8::String::kInternalizedString),
            bridjs::Utils::toV8String(isolate,DC_SIGCHAR_FLOAT), ReadOnly);
    signatureObj->ForceSet(v8::String::NewFromUtf8(isolate,"POINTER_TYPE", v8::String::kInternalizedString),
            bridjs::Utils::toV8String(isolate,DC_SIGCHAR_POINTER), ReadOnly);
    signatureObj->ForceSet(v8::String::NewFromUtf8(isolate,"STRING_TYPE", v8::String::kInternalizedString),
            bridjs::Utils::toV8String(isolate,DC_SIGCHAR_STRING), ReadOnly);
    signatureObj->ForceSet(v8::String::NewFromUtf8(isolate,"STRUCT_TYPE", v8::String::kInternalizedString),
            bridjs::Utils::toV8String(isolate,DC_SIGCHAR_STRUCT), ReadOnly);

    signatureObj->ForceSet(v8::String::NewFromUtf8(isolate,"CALLBACK_TYPE", v8::String::kInternalizedString),
            bridjs::Utils::toV8String(isolate,DC_SIGCHAR_POINTER), ReadOnly);

    signatureObj->ForceSet(v8::String::NewFromUtf8(isolate,"ENDARG", v8::String::kInternalizedString),
            bridjs::Utils::toV8String(isolate,DC_SIGCHAR_ENDARG), ReadOnly);
    /*INT8*/
    getSignedAndUnsinedTypeFromTypeSize(sizeof (int8_t), signedType, unsignedType);

    signatureObj->ForceSet(v8::String::NewFromUtf8(isolate,"INT8_TYPE", v8::String::kInternalizedString),
            bridjs::Utils::toV8String(isolate,signedType), ReadOnly);
    signatureObj->ForceSet(v8::String::NewFromUtf8(isolate,"UINT8_TYPE", v8::String::kInternalizedString),
            bridjs::Utils::toV8String(isolate,unsignedType), ReadOnly);

    /*INT16*/
    getSignedAndUnsinedTypeFromTypeSize(sizeof (int16_t), signedType, unsignedType);
    signatureObj->ForceSet(v8::String::NewFromUtf8(isolate,"INT16_TYPE", v8::String::kInternalizedString),
            bridjs::Utils::toV8String(isolate,signedType), ReadOnly);
    signatureObj->ForceSet(v8::String::NewFromUtf8(isolate,"UINT16_TYPE", v8::String::kInternalizedString),
            bridjs::Utils::toV8String(isolate,unsignedType), ReadOnly);

    /*INT32*/
    getSignedAndUnsinedTypeFromTypeSize(sizeof (int32_t), signedType, unsignedType);
    //char type[] = {signedType,'\0'};
    //std::cout<<"fefe "<<*(v8::String::Utf8Value(bridjs::Utils::toV8String(signedType)))<<std::endl;
    signatureObj->ForceSet(v8::String::NewFromUtf8(isolate,"INT32_TYPE", v8::String::kInternalizedString),
            bridjs::Utils::toV8String(isolate,signedType), ReadOnly);
    signatureObj->ForceSet(v8::String::NewFromUtf8(isolate,"UINT32_TYPE", v8::String::kInternalizedString),
            bridjs::Utils::toV8String(isolate,unsignedType), ReadOnly);

    /*INT64*/
    getSignedAndUnsinedTypeFromTypeSize(sizeof (int64_t), signedType, unsignedType);
    signatureObj->ForceSet(v8::String::NewFromUtf8(isolate,"INT64_TYPE", v8::String::kInternalizedString),
            bridjs::Utils::toV8String(isolate,signedType), ReadOnly);
    signatureObj->ForceSet(v8::String::NewFromUtf8(isolate,"UINT64_TYPE", v8::String::kInternalizedString),
            bridjs::Utils::toV8String(isolate,unsignedType), ReadOnly);

	/*size_t*/
	getSignedAndUnsinedTypeFromTypeSize(sizeof (size_t), signedType, unsignedType);
	signatureObj->ForceSet(v8::String::NewFromUtf8(isolate,"SIZE_TYPE", v8::String::kInternalizedString),
		bridjs::Utils::toV8String(isolate,signedType), ReadOnly);
	signatureObj->ForceSet(v8::String::NewFromUtf8(isolate,"SIZE_TYPE", v8::String::kInternalizedString),
		bridjs::Utils::toV8String(isolate,unsignedType), ReadOnly);

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
    bridjs::UnionStruct::Init(dyncallObj);
    /*dyncallback*/
    target->ForceSet(v8::String::NewFromUtf8(isolate,"dcb", v8::String::kInternalizedString), dyncallBackObj, ReadOnly);
    bridjs::Dyncallback::Init(dyncallBackObj);

    /*Utils module*/
    target->ForceSet(v8::String::NewFromUtf8(isolate,"utils", v8::String::kInternalizedString), utilsObj, ReadOnly);
    bridjs::Utils::Init(utilsObj);

    /*Test module*/
    target->ForceSet(v8::String::NewFromUtf8(isolate,"test", v8::String::kInternalizedString), testObj);
    NODE_SET_METHOD(testObj, "testMultiplyFunction", bridjs::Test::TestMultiplyFunction);

    std::locale::global(std::locale(""));
    std::wcout.imbue(std::locale(""));

    //std::cout<<sizeof(TestStruct)<<", "<<sizeof(DClong)<<", "<<sizeof(long)<<std::endl;
    //TempStruct tempStruct;

    //std::cout<<sizeof(TestArrayStruct)<<", "<<sizeof(TempStruct)<<", "<<(uint64_t*)&tempStruct.point-(uint64_t*)&tempStruct.w<<", "<<sizeof(TestComplexStruct)<<std::endl;
}


NODE_MODULE(bridjs, init);
