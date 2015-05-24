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
#pragma once

#include "pointer_v8.h"
#include "struct_v8.h"
#include <node.h>
#include <memory>
#include <node_buffer.h>
#include <vector>
#include <map>

extern "C" {
#include "dynload.h"
#include "dyncall.h"
}

namespace bridjs {

    class ArrayStruct : public bridjs::Struct {
    public:
        static void Init(v8::Handle<v8::Object> exports);
        static void New(const v8::FunctionCallbackInfo<v8::Value>& args);
        static void GetFieldType(const v8::FunctionCallbackInfo<v8::Value>& args);
        static void GetFieldOffset(const v8::FunctionCallbackInfo<v8::Value>& args);
        static void GetFieldCount(const v8::FunctionCallbackInfo<v8::Value>& args);
        static ArrayStruct* New(v8::Isolate *isolate,const char type, const size_t length, const size_t alignment = DEFAULT_ALIGNMENT);
        
        //DCCallVM* getVM() const;
        virtual char getFieldType(const uint32_t index) const;
        virtual size_t getFieldCount() const;
        virtual std::string getSignature();
    protected:
        char mType;
        size_t mLength;

        static v8::Persistent<v8::Function> constructor;
        static std::vector<char> mEmptyTypes;
        static std::map<uint32_t,v8::Local<v8::Object>> mEmptySubStructMap;
        
        ArrayStruct(v8::Isolate *isolate,const char type, const size_t length, const size_t aligment = DEFAULT_ALIGNMENT);
        virtual void checkRange(const uint32_t index) const;
        size_t deriveArrayLayout(const size_t alignment);
        virtual size_t getFieldOffset(uint32_t index) const;
        virtual ~ArrayStruct();


    }; // namespace dyncall

}//bridjs