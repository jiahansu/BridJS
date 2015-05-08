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

#include <node.h>
#include <node_object_wrap.h>

namespace bridjs{
class Pointer :public node::ObjectWrap{
public:
	static void Init(v8::Handle<v8::Object> exports);
	static void New(const v8::FunctionCallbackInfo<v8::Value>& args);
	static v8::Local<v8::Object> NewInstance(v8::Isolate* isolate,const void* ptr);
	static Pointer* New(const void* ptr);
	static void GetAddress(const v8::FunctionCallbackInfo<v8::Value>& args);
	static void IsNull(const v8::FunctionCallbackInfo<v8::Value>& args);
	static const void* Data(v8::Isolate* isolate, v8::Handle<v8::Object> val);
	static void Slice(const v8::FunctionCallbackInfo<v8::Value>& args);
	static void ToString(const v8::FunctionCallbackInfo<v8::Value>& args);

	virtual void* getAddress();
	virtual ~Pointer();

protected:
	static v8::Persistent<v8::Function> constructor;
	const void* mPtr;

	explicit Pointer(const void* ptr);
	
	
	
}; // namespace dyncall
}//bridjs