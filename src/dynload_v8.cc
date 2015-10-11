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
#include "dynload_v8.h"
#include "dyncall_v8_utils.h"

#include <iostream>
#include <string>
#include <map>
#include <list>

#ifdef _MSC_VER
#include <codecvt>
#endif

#ifndef DWORD
typedef unsigned long int DWORD;
#endif        

extern "C" {
#include "dynload.h"
#ifdef _MSC_VER
#include <Windows.h>
#endif
}
using namespace v8;
using namespace bridjs;

struct DLSyms_ {
    DLLib* pLib;
    const char* pBase;
    const DWORD* pNames;
    const DWORD* pFuncs;
    const unsigned short* pOrds;
    size_t count;
};

std::map<std::string, DLLib*> gLoadedLibraryMap;
uv_mutex_t gLoadLubraryMutex;


bool removeLibraryFromMap(DLLib* lib);

bool removeLibraryFromMap(DLLib* lib) {
    bool found = false;

    uv_mutex_lock(&gLoadLubraryMutex);
    //gLoadLubraryMutex.lock();

    try {
        std::list<std::map<std::string, DLLib*>::iterator> removeList;

        for (std::map<std::string, DLLib*>::iterator it = gLoadedLibraryMap.begin(); it != gLoadedLibraryMap.end(); ++it) {
            if (it->second == lib) {
                removeList.push_front(it);
                found = true;
            }
            //std::cout<<"Find library: "<<lib<<std::endl;
        }

        for (std::list<std::map<std::string, DLLib*>::iterator>::iterator it = removeList.begin(); it != removeList.end(); ++it) {
            gLoadedLibraryMap.erase(*it);
            //std::cout<<"Remove library: "<<lib<<std::endl;
        }

    } catch (...) {
        std::cerr << "Unknown exception for cleanup library: " << lib << std::endl;
    }

    uv_mutex_unlock(&gLoadLubraryMutex);

    return found;
}

void Dynload::Init(v8::Handle<v8::Object> dynloadObj) {
    int32_t error = uv_mutex_init(&gLoadLubraryMutex);
    if (error != 0) {
        std::string message("Fail to init gLoadLubraryMutex");
        std::cerr << message << std::endl;

        throw std::runtime_error(message);
    }

    EXPORT_FUNCTION(dynloadObj, bridjs::Dynload, loadLibrary);
    EXPORT_FUNCTION(dynloadObj, bridjs::Dynload, freeLibrary);
    EXPORT_FUNCTION(dynloadObj, bridjs::Dynload, findSymbol);
    EXPORT_FUNCTION(dynloadObj, bridjs::Dynload, symsInit);
    EXPORT_FUNCTION(dynloadObj, bridjs::Dynload, symsCleanup);
    EXPORT_FUNCTION(dynloadObj, bridjs::Dynload, symsCount);
    EXPORT_FUNCTION(dynloadObj, bridjs::Dynload, symsName);
    EXPORT_FUNCTION(dynloadObj, bridjs::Dynload, symsNameFromValue);
}

void Dynload::loadLibrary(const v8::FunctionCallbackInfo<v8::Value>& args) {
    Isolate* isolate = Isolate::GetCurrent(); HandleScope scope(isolate);
    v8::String::Utf8Value libpath(args[0]);
    DLLib *lib = NULL;

    try {
        uv_mutex_lock(&gLoadLubraryMutex);

        std::map<std::string, DLLib*>::iterator libIterator = gLoadedLibraryMap.find(*libpath);

        if (libIterator != gLoadedLibraryMap.end()) {
            lib = libIterator->second;
        } else {
#ifndef _MSC_VER
            lib = dlLoadLibrary((*libpath));
#else
            std::wstring_convert<std::codecvt_utf8_utf16<wchar_t>, wchar_t> cvt;
            std::wstring libPathStr = cvt.from_bytes(*libpath); //ConvertFromUtf8ToUtf16(*libpath);

            lib = (DLLib*) LoadLibraryW((LPCWSTR) libPathStr.c_str());
#endif
            gLoadedLibraryMap[*libpath] = lib;
        }
    } catch (...) {
        std::stringstream message;
        message << "Unknown exception for loading library: " << *libpath;
        
        std::cerr<<message.str()<<std::endl;
        
        THROW_EXCEPTION( message.str().c_str());
    }

    uv_mutex_unlock(&gLoadLubraryMutex);
    
    args.GetReturnValue().Set(bridjs::Utils::wrapPointer(isolate,lib));
}

void Dynload::freeLibrary(const v8::FunctionCallbackInfo<v8::Value>& args) {
    Isolate* isolate = Isolate::GetCurrent(); HandleScope scope(isolate);

    GET_POINTER_ARG(DLLib, lib, args, 0);
    dlFreeLibrary(lib);
    if (!removeLibraryFromMap(lib)) {
        std::stringstream message;

        message << "Illegal library pointer: " << lib << std::endl;

        THROW_EXCEPTION(message.str().c_str());
    }

    args.GetReturnValue().Set(Undefined(isolate));
}

void Dynload::findSymbol(const v8::FunctionCallbackInfo<v8::Value>& args) {
    Isolate* isolate = Isolate::GetCurrent(); HandleScope scope(isolate);
    GET_POINTER_ARG(DLLib, lib, args, 0);
    GET_STRING_ARG(name, args, 1);
    v8::String::Utf8Value valueStr(name);
    
    void* symbol = dlFindSymbol(lib, *valueStr);
    args.GetReturnValue().Set(bridjs::Utils::wrapPointer(isolate,symbol));
}

void Dynload::symsInit(const v8::FunctionCallbackInfo<v8::Value>& args) {
    Isolate* isolate = Isolate::GetCurrent(); HandleScope scope(isolate);
    v8::String::Utf8Value libpath(args[0]);
    DLSyms *pSyms = NULL;

#ifndef _MSC_VER
    pSyms = dlSymsInit(*libpath);
#else
    try {
        std::wstring_convert<std::codecvt_utf8_utf16<wchar_t>, wchar_t> cvt;
        std::wstring libPathStr = cvt.from_bytes(*libpath); //ConvertFromUtf8ToUtf16(*libpath);
        DLLib* pLib = (DLLib*) LoadLibraryW((LPCWSTR) libPathStr.c_str());
        pSyms = (DLSyms*) malloc(sizeof (DLSyms));

        if (pSyms != NULL) {
            const char* base = (const char*) pLib;
            IMAGE_DOS_HEADER* pDOSHeader = (IMAGE_DOS_HEADER*) base;
            if (pDOSHeader != NULL) {
                IMAGE_NT_HEADERS* pNTHeader = (IMAGE_NT_HEADERS*) (base + pDOSHeader->e_lfanew);
                IMAGE_DATA_DIRECTORY* pExportsDataDir = &pNTHeader->OptionalHeader.DataDirectory[IMAGE_DIRECTORY_ENTRY_EXPORT];
                IMAGE_EXPORT_DIRECTORY* pExports = (IMAGE_EXPORT_DIRECTORY*) (base + pExportsDataDir->VirtualAddress);

                pSyms->pBase = base;
                pSyms->pNames = (DWORD*) (base + pExports->AddressOfNames);
                pSyms->pFuncs = (DWORD*) (base + pExports->AddressOfFunctions);
                pSyms->pOrds = (unsigned short*) (base + pExports->AddressOfNameOrdinals);
                pSyms->count = (size_t) pExports->NumberOfNames;
                pSyms->pLib = pLib;
            } else {
                throw std::runtime_error("Fail to allocate DLSyms");
            }
        } else {
            throw std::runtime_error("Fail to load library symbols");
        }
    } catch (std::exception &e) {
        THROW_EXCEPTION(e.what());
    }
#endif

    args.GetReturnValue().Set(bridjs::Utils::wrapPointer(isolate,pSyms));
}

void Dynload::symsCleanup(const v8::FunctionCallbackInfo<v8::Value>& args) {
    Isolate* isolate = Isolate::GetCurrent(); HandleScope scope(isolate);
    GET_POINTER_ARG(DLSyms, pSyms, args, 0);
    //removeLibraryFromMap(pSyms->pLib);
    dlSymsCleanup(pSyms);
    args.GetReturnValue().Set(Undefined(isolate));
}

void Dynload::symsCount(const v8::FunctionCallbackInfo<v8::Value>& args) {
    Isolate* isolate = Isolate::GetCurrent(); HandleScope scope(isolate);
    GET_POINTER_ARG(DLSyms, pSyms, args, 0);

    int count = dlSymsCount(pSyms);
    args.GetReturnValue().Set(Number::New(isolate,count));
}

void Dynload::symsName(const v8::FunctionCallbackInfo<v8::Value>& args) {
    Isolate* isolate = Isolate::GetCurrent(); 
    HandleScope scope(isolate);
    GET_POINTER_ARG(DLSyms, pSyms, args, 0);
    GET_INT32_ARG(index, args, 1);
    const char* name = dlSymsName(pSyms, index);
    
    args.GetReturnValue().Set(name ? v8::String::NewFromUtf8(isolate,name) : String::Empty(isolate));
}

void Dynload::symsNameFromValue(const v8::FunctionCallbackInfo<v8::Value>& args) {
    Isolate* isolate = Isolate::GetCurrent(); 
    HandleScope scope(isolate);
    GET_POINTER_ARG(DLSyms, pSyms, args, 0);
    GET_POINTER_ARG(void, value, args, 1);
    const char* name = dlSymsNameFromValue(pSyms, value);
    
    args.GetReturnValue().Set(v8::String::NewFromUtf8(isolate,name));
}
