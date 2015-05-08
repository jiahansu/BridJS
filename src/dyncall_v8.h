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

extern "C"
{
	#include "dyncall.h"
}

namespace bridjs{
class Dyncall {
public:
	static void newCallVM(const v8::FunctionCallbackInfo<v8::Value>& args);
	static void free(const v8::FunctionCallbackInfo<v8::Value>& args);
	static void reset(const v8::FunctionCallbackInfo<v8::Value>& args);
	static void mode(const v8::FunctionCallbackInfo<v8::Value>& args);
	static void argBool(const v8::FunctionCallbackInfo<v8::Value>& args);
	static void argChar(const v8::FunctionCallbackInfo<v8::Value>& args);
	static void argShort(const v8::FunctionCallbackInfo<v8::Value>& args);
	static void argInt(const v8::FunctionCallbackInfo<v8::Value>& args);
	static void argLong(const v8::FunctionCallbackInfo<v8::Value>& args);
	static void argLongLong(const v8::FunctionCallbackInfo<v8::Value>& args);
	static void argFloat(const v8::FunctionCallbackInfo<v8::Value>& args);
	static void argDouble(const v8::FunctionCallbackInfo<v8::Value>& args);
	static void argPointer(const v8::FunctionCallbackInfo<v8::Value>& args);
	static void argStruct(const v8::FunctionCallbackInfo<v8::Value>& args);

	static void callVoid(const v8::FunctionCallbackInfo<v8::Value>& args);
	static void callBool(const v8::FunctionCallbackInfo<v8::Value>& args);
	static void callChar(const v8::FunctionCallbackInfo<v8::Value>& args);
	static void callShort(const v8::FunctionCallbackInfo<v8::Value>& args);
	static void callInt(const v8::FunctionCallbackInfo<v8::Value>& args);
	static void callLong(const v8::FunctionCallbackInfo<v8::Value>& args);
	static void callLongLong(const v8::FunctionCallbackInfo<v8::Value>& args);
	static void callFloat(const v8::FunctionCallbackInfo<v8::Value>& args);
	static void callDouble(const v8::FunctionCallbackInfo<v8::Value>& args);
	static void callPointer(const v8::FunctionCallbackInfo<v8::Value>& args);
	static void callStruct(const v8::FunctionCallbackInfo<v8::Value>& args);

	static void getError(const v8::FunctionCallbackInfo<v8::Value>& args);

	static void newStruct(const v8::FunctionCallbackInfo<v8::Value>& args);
	static void structField(const v8::FunctionCallbackInfo<v8::Value>& args);
	static void subStruct(const v8::FunctionCallbackInfo<v8::Value>& args);
	static void closeStruct(const v8::FunctionCallbackInfo<v8::Value>& args);
	//static void structAlignment(const v8::FunctionCallbackInfo<v8::Value>& args);
	static void freeStruct(const v8::FunctionCallbackInfo<v8::Value>& args);
	static void structSize(const v8::FunctionCallbackInfo<v8::Value>& args);
	static void defineStruct(const v8::FunctionCallbackInfo<v8::Value>& args);
}; // namespace dyncall
}//bridjs
