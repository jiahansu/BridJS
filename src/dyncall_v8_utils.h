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
#include <node_buffer.h>
#include <sstream>
#include <memory>

#define EXPORT_FUNCTION(obj, ns, name) NODE_SET_METHOD(obj, #name, ns::name);

#define GET_CALL_ARG(args) \
	GET_POINTER_ARG(DCCallVM,pVm,args,0)\
	GET_POINTER_ARG(char,pFunction,args,1);\
	if(pVm==NULL){ \
		isolate->ThrowException(Exception::TypeError(String::NewFromUtf8(isolate,"DCCallVM was NULL")));\
                return;\
	}

#define GET_INT32_ARG(name, args, index) \
 GET_INT32_VALUE(name, args[index], index)

#define GET_INT32_VALUE(name, value, index) \
  Local<Value> name ## Value = value; \
  int32_t name;\
  if (name ## Value->IsNumber()) { \
   Local<Number> name ## Number = Local<Number>::Cast(name ## Value); \
    name = (*name ## Number)->Int32Value();\
  }else if(name ## Value->IsString()){ \
    v8::String::Utf8Value str(name ## Value); \
    name = atol(*str); \
  }else{ \
	std::stringstream message; \
	v8::String::Utf8Value valueStr(value); \
	message<< "Invalid value for int32_t argument " #name " at index ="<<index<<", value = "<<(*valueStr); \
    isolate->ThrowException(Exception::TypeError(String::NewFromUtf8(isolate,message.str().c_str()))); \
    return; \
  }\

#define GET_UINT32_ARG(name, args, index) \
	GET_UINT32_VALUE(name, args[index], index)

#define GET_UINT32_VALUE(name, value, index) \
  Local<Value> name ## Value = value; \
  uint32_t name;\
  if (name ## Value->IsNumber()) { \
   Local<Number> name ## Number = Local<Number>::Cast(name ## Value); \
    name = (*name ## Number)->Uint32Value();\
  }else if(name ## Value->IsString()){ \
    v8::String::Utf8Value str(name ## Value); \
    name = atol(*str); \
  }else{ \
	std::stringstream message; \
	v8::String::Utf8Value valueStr(value); \
	message<< "Invalid value for int32_t argument " #name " at index ="<<index<<", value = "<<(*valueStr); \
    isolate->ThrowException(Exception::TypeError(        String::NewFromUtf8(isolate,message.str().c_str()))); \
    return; \
  }\

#define GET_FLOAT_ARG(name, args, index) \
	GET_FLOAT_VALUE(name,args[index],index)

#define GET_FLOAT_VALUE(name, value, index) \
  Local<Value> name ## Value = value; \
 float name; \
 if (name ## Value->IsNumber()) { \
    Local<Number> name ## Number = Local<Number>::Cast(name ## Value); \
	name = static_cast<float>(name ## Number->Value()); \
  }else if(name ## Value->IsString()){ \
    v8::String::Utf8Value str(name ## Value); \
    name = static_cast<float>(atof(*str)); \
  }else{ \
    std::stringstream message; \
    v8::String::Utf8Value valueStr(value); \
    message<< "Invalid value for float argument " #name " at index ="<<index<<", value = "<<(*valueStr); \
    isolate->ThrowException(Exception::TypeError(String::NewFromUtf8(isolate,message.str().c_str()))); \
    return; \
  } 

#define GET_INT64_ARG(name, args, index) \
	GET_INT64_VALUE(name,args[index],index);

#define GET_INT64_VALUE(name, value, index) \
 Local<Value> name ## Value = value; \
 int64_t name; \
 if (name ## Value->IsNumber()) { \
	name = name ## Value->IntegerValue(); \
  }else if(name ## Value->IsString()){ \
    v8::String::Utf8Value str(name ## Value); \
	name = atol(*str); \
  }else{ \
    std::stringstream message; \
    v8::String::Utf8Value valueStr(value); \
    message<< "Invalid value for int64_t argument " #name " at index ="<<index<<", value = "<<(*valueStr); \
    isolate->ThrowException(Exception::TypeError(String::NewFromUtf8(isolate,message.str().c_str()))); \
    return;\
  } 

#define GET_DOUBLE_ARG(name, args, index) \
	GET_DOUBLE_VALUE(name,args[index],index)

#define GET_DOUBLE_VALUE(name, value, index) \
 Local<Value> name ## Value = value; \
 double name; \
 if (name ## Value->IsNumber()) { \
    Local<Number> name ## Number = Local<Number>::Cast(name ## Value); \
	name = name ## Number->Value(); \
  }else if(name ## Value->IsString()){ \
    v8::String::Utf8Value str(name ## Value); \
    name = atof(*str); \
  }else{ \
    std::stringstream message; \
    v8::String::Utf8Value valueStr(value); \
    message<< "Invalid value for double argument " #name " at index ="<<index<<", value = "<<(*valueStr); \
    isolate->ThrowException(Exception::TypeError(String::NewFromUtf8(isolate,message.str().c_str()))); \
    return;\
  } 

#define GET_BOOL_ARG(name, args, index) \
   GET_BOOL_VALUE(name, args[index],index)

#define GET_BOOL_VALUE(name, value,index) \
  Local<Value> name ## Value = value; \
  bool name; \
  if (name ## Value->IsBoolean()) { \
	name = name ## Value->ToBoolean()->BooleanValue(); \
  }else if(name ## Value->IsBooleanObject()){ \
	name = Local<BooleanObject>::Cast(name ## Value)->BooleanValue(); \
  }else if (name ## Value->IsNumber()) { \
    name = Local<Number>::Cast(name ## Value)->BooleanValue(); \
  }else{ \
    std::stringstream message; \
    v8::String::Utf8Value valueStr(value); \
    message<< "Invalid value for bool argument " #name " at index ="<<index<<", value = "<<(*valueStr); \
    isolate->ThrowException(Exception::TypeError(String::NewFromUtf8(isolate,message.str().c_str()))); \
    return;\
  } 

#define GET_CHAR_ARG(name, args, index) \
  GET_CHAR_VALUE(name,args[index],index);

#define GET_CHAR_VALUE(name, value, index) \
  Local<Value> name ## Value = value; \
  char name; \
  if(name ## Value->IsString()){ \
    v8::String::Utf8Value str(name ## Value); \
    if(str.length()!=1){ \
        std::stringstream message; \
        message<<"Illegal cahr value: "<<(*str); \
        isolate->ThrowException(Exception::TypeError(String::NewFromUtf8(isolate,message.str().c_str()))); \
        return;\
    } \
    name = (*str)[0]; \
  }else if (name ## Value->IsNumber()) { \
	name = Local<Number>::Cast(name ## Value)->Int32Value(); \
  }else{ \
	std::stringstream message; \
	v8::String::Utf8Value valueStr(value); \
	message<< "Invalid value for char argument " #name " at index ="<<index<<", value = "<<(*valueStr); \
    isolate->ThrowException(Exception::TypeError(String::NewFromUtf8(isolate,message.str().c_str()))); \
    return;\
  } 
  
#define GET_STRING_ARG(name, args, index) \
	GET_STRING_VALUE(name, args[index], index)

#define GET_STRING_VALUE(name, value, index) \
  Local<Value> name = value; \
  if (!name->IsString()&&!name->IsNull()) { \
    std::stringstream message; \
	v8::String::Utf8Value valueStr(value); \
	message<< "Invalid value for string argument " #name " at index ="<<index<<", value = "<<(*valueStr); \
    isolate->ThrowException(Exception::TypeError(String::NewFromUtf8(isolate,message.str().c_str()))); \
    return;\
  }


#define GET_OBJECT_ARG(name, args, index) \
	Local<Value> name ## Value = args[index]; \
  if (!name ## Value->IsObject()) { \
    std::stringstream message; \
	v8::String::Utf8Value valueStr(args[index]); \
	message<< "Invalid value for Object argument " #name " at index ="<<index<<", value = "<<(*valueStr); \
    isolate->ThrowException(Exception::TypeError(String::NewFromUtf8(isolate,message.str().c_str()))); \
    return;\
  } \
  Local<Object> name = name ## Value->ToObject();


  #define GET_ARRAY_ARG(name, args, index) \
	Local<Value> name ## Value = args[index]; \
  if (!name ## Value->IsArray()) { \
    std::stringstream message; \
	v8::String::Utf8Value valueStr(name ## Value); \
	message<< "Invalid value for Array argument " #name " at index ="<<index<<", value = "<<(*valueStr); \
    isolate->ThrowException(Exception::TypeError(String::NewFromUtf8(isolate,message.str().c_str()))); \
    return;\
  } \
  Local<Array> name = Local<Array>::Cast(name ## Value);

#define GET_POINTER_ARG(type, name, args, index) \
	GET_POINTER_VALUE(type, name, args[index], index)

#define GET_POINTER_VALUE(type, name, value, index) \
  Local<Value> name ## Value = value; \
  type* name; \
  if (name ## Value->IsNumber()) {\
	name = (type*)name ## Value->IntegerValue(); \
  }else if(name ## Value->IsString()){ \
	  v8::String::Utf8Value str(name ## Value); \
	  name = (type*)(*name ## Value); \
  }else if (name ## Value->IsObject() || name ## Value->IsNull()) { \
	  try{ \
		name = (type*)bridjs::Utils::unwrapPointer(isolate, name ## Value); \
	  }catch(std::exception &e){ \
		std::stringstream message; \
		v8::String::Utf8Value valueStr(value); \
		message<< "Invalid object for pointer argument " #name " at index ="<<index<<", value = "<<(*valueStr)<<": "<<e.what(); \
		isolate->ThrowException(Exception::TypeError(String::NewFromUtf8(isolate,message.str().c_str()))); \
                return;\
	  } \
  }else{ \
	std::stringstream message; \
	v8::String::Utf8Value valueStr(value); \
	message<< "Invalid value for pointer argument " #name " at index ="<<index<<", value = "<<(*valueStr); \
    isolate->ThrowException(Exception::TypeError(String::NewFromUtf8(isolate,message.str().c_str()))); \
    return;\
  } 
  

#define THROW_EXCEPTION(message) isolate->ThrowException(Exception::Error(v8::String::NewFromUtf8(isolate,message))); return
        
        

#define CHECK_ARGUMENT(args, num) \
	if(args.Length()<num){ \
		std::stringstream message; \
		message<<"Illegal argument, it only had"<<args.Length()<<" arguments, but the number of arguments should be "<<num; \
		THROW_EXCEPTION(message.str().c_str()); \
	} 

#define WRAP_BOOL(val) v8::Boolean::New(isolate,val)
#define WRAP_CHAR(val) bridjs::Utils::toV8String(isolate,val)
#define WRAP_SHORT(val) v8::Int32::New(isolate,val)
#define WRAP_INT(val) v8::Int32::New(isolate,val)
#define WRAP_LONG(val) v8::Number::New(isolate,val)
#define WRAP_LONGLONG(val) v8::Number::New(isolate,static_cast<double>(val))
#define WRAP_UCHAR(val) bridjs::Utils::toV8String(isolate,val)
#define WRAP_USHORT(val) v8::Uint32::New(isolate,val)
#define WRAP_UINT(val) v8::Uint32::New(isolate,val)
#define WRAP_ULONG(val) v8::Number::New(isolate,val)
#define WRAP_ULONGLONG(val) v8::Number::New(isolate,static_cast<double>(val))
#define WRAP_FLOAT(val) v8::Number::New(isolate,val)
#define WRAP_DOUBLE(val) v8::Number::New(isolate,val)
#define WRAP_POINTER(val) bridjs::Utils::wrapPointer(isolate,val)
#define WRAP_STRING(val) String::NewFromUtf8(isolate,val)

template< typename T >
struct ArrayDeleter
{
  void operator ()( T const * p)
  { 
    delete[] p; 
  }
};

namespace bridjs {

	class Utils{
	public:
		static void Init(v8::Handle<v8::Object> utilsObj);
		static void PointerToString(const v8::FunctionCallbackInfo<v8::Value>& args);
		static void MemCpy(const v8::FunctionCallbackInfo<v8::Value>& args);
		static v8::Local<v8::Value> wrapPointerToBuffer(v8::Isolate* isolate, const void* ptr);
		static void* unwrapBufferToPointer(v8::Local<v8::Value> value);
		static v8::Local<v8::Value> wrapPointer(v8::Isolate* isolate,const void* ptr);
		static const void* unwrapPointer(v8::Isolate* isolate,v8::Local<v8::Value> buffer);
		static v8::Local<v8::String> toV8String(v8::Isolate* isolate,const char val);
		static size_t getTypeSize(const char type);
		static v8::Local<v8::Value> convertDataByType(v8::Isolate* isolate,std::shared_ptr<void> spData,const char type);
	};
	class ValueWrapper{
	private:
            v8::Isolate* mpIsolate;
            v8::Persistent<v8::Value> mValue;
	public:
            v8::Local<v8::Value> getValue();
            ValueWrapper(v8::Isolate* isolate,v8::Persistent<v8::Value> value);
            ~ValueWrapper();
	};


}
