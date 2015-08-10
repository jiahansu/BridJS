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
#include <v8-util.h>
#include <node.h>
#include <memory>
#include <node_buffer.h>
#include <vector>
#include <map>

extern "C"{
	#include "dynload.h"
	#include "dyncall.h"
}

namespace bridjs{
	class Struct :public node::ObjectWrap{
public:
	static void Init(v8::Handle<v8::Object> exports);
	static void New(const v8::FunctionCallbackInfo<v8::Value>& args);
	static void GetSize(const v8::FunctionCallbackInfo<v8::Value>& args);
	static void GetField(const v8::FunctionCallbackInfo<v8::Value>& args);
	static void SetField(const v8::FunctionCallbackInfo<v8::Value>& args);
	static void GetSignature(const v8::FunctionCallbackInfo<v8::Value>& args);
	static void GetFieldType(const v8::FunctionCallbackInfo<v8::Value>& args);
	static void GetFieldOffset(const v8::FunctionCallbackInfo<v8::Value>& args);
	static void GetFieldCount(const v8::FunctionCallbackInfo<v8::Value>& args);
	static void ToString(const v8::FunctionCallbackInfo<v8::Value>& args);
	static Struct* New(v8::Isolate* pIsolate,const std::vector<char> &fieldType,std::map<uint32_t,v8::Local<v8::Object> > &subStructMap);
	//DCCallVM* getVM() const;
	static size_t getAlignSize(size_t size, size_t alignment);
	static size_t getFieldsSize(const std::vector<char> &fieldTypes, const size_t alignment=DEFAULT_ALIGNMENT);
	static size_t getAlignmentSize(const char type,const size_t typeSize, const bool isFirst);
	static size_t addPadding(size_t calculatedSize, const size_t alignment);
        
	virtual char getFieldType(const uint32_t index) const ;
	virtual size_t getFieldCount() const;
	virtual std::shared_ptr<void> getField(const uint32_t index, const void* ptr) const;
	virtual void setField(const uint32_t index, std::shared_ptr<void> pValue, void* ptr);
	size_t getSize() const;
	virtual std::string getSignature();
	std::string toString();
protected:
	static v8::Persistent<v8::Function> constructor;
	std::vector<char> mFieldTypes;
	v8::StdPersistentValueMap<uint32_t,v8::Object> mSubStructMap;
	std::vector<size_t> mOffsets;
	size_t mSize;
	size_t mAligment;
        
        static void   parseJSArguments(v8::Isolate* pIsolate,const v8::FunctionCallbackInfo<v8::Value>& args, 
        std::vector<char> &fieldType, std::map<uint32_t,v8::Local<v8::Object>> &subStructMap);
        
	Struct(v8::Isolate* pIsolate,const std::vector<char> &fieldType, std::map<uint32_t,v8::Local<v8::Object>> &subStructMap,const size_t alignment);
	virtual void checkRange(const uint32_t index) const;
	size_t deriveLayout(const size_t alignment);
	Struct* getSubStruct(uint32_t index);
	virtual size_t getFieldOffset(uint32_t index) const;
	size_t getAlignment() const;
	virtual ~Struct();
	
	
}; // namespace dyncall

}//bridjs