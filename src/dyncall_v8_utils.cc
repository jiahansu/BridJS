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
#include "dyncall_v8_utils.h"
#include "pointer_v8.h"
#include <iostream>
#include <cstring>
#include <node_version.h>

extern "C"
{
    #include "dyncall.h"    
    #include "dyncall_signature.h"
}

using namespace v8;
/*
void* bridjs::Utils::string2ptr(Local<Value> value) {
  String::AsciiValue str(value);
  void* ptr;
  switch (sizeof(void*)) {
  case 8:
    unsigned long long ll;
    sscanf(*str, "%llx", &ll);
    ptr = (void*)ll;
    break;
  case 4:
    unsigned long i;
    sscanf(*str, "%lx", &i);
    ptr = (void*)ll;
    break;
  default:
    ptr = 0;
  }
  return ptr;
}

Handle<Value> bridjs::Utils::ptr2string(void* ptr) {
  char str[20];
  switch (sizeof(void*)) {
  case 8:
    sprintf(str, "%#16llx", (unsigned long long)(size_t)ptr);
    break;
  case 4:
    sprintf(str, "%#8lx", (unsigned long)(size_t)ptr);
    break;
  default:
    *str = 0;
  }
  return String::New(str);
}
*/
void WriteInt64(const v8::FunctionCallbackInfo<v8::Value>& args);

size_t bridjs::Utils::getTypeSize(const char type){
	size_t size;

		switch(type){
		/*
		case DC_SIGCHAR_VOID:{
			size = sizeof(DCbool);
			}
			break;*/
		case DC_SIGCHAR_BOOL:{
			size = sizeof(DCbool);
			}
			break;
		case DC_SIGCHAR_UCHAR:{
			size = sizeof(DCuchar);
			}
			break;
		case DC_SIGCHAR_CHAR:{
			size = sizeof(DCchar);
			}
			break;
        case DC_SIGCHAR_SHORT:{
			size = sizeof(DCshort);
			}
			break;
		case DC_SIGCHAR_USHORT:{
			size = sizeof(DCushort);
			}
			break;
        case DC_SIGCHAR_INT:{
			size = sizeof(DCint);
			}
			break;
		case DC_SIGCHAR_UINT:{
			size = sizeof(DCuint);
			}
			break; 
        case DC_SIGCHAR_LONG:{
			size = sizeof(DClong);
			}
			break;
		case DC_SIGCHAR_ULONG:{
			size = sizeof(DCulong);
			}
			break;
		case DC_SIGCHAR_LONGLONG:{
			size = sizeof(DClonglong);
			}
			break;
		case DC_SIGCHAR_ULONGLONG:{
			size = sizeof(DCulonglong);
			}
			break;
        case DC_SIGCHAR_FLOAT:{
			size = sizeof(DCfloat);
			}
			break;
        case DC_SIGCHAR_DOUBLE:{
			size = sizeof(DCdouble);
			}
			break;
		case DC_SIGCHAR_STRING:
        case DC_SIGCHAR_POINTER:{
			size = sizeof(DCpointer);
			}
			break;
			/*
		case DC_SIGCHAR_STRUCT:{
			std::cerr<<"Not implement"<<std::endl;
			GET_POINTER_VALUE(void, rValue,returnValue,0);
			value->p = (rValue);
		    }
			break;*/

		default:
			std::stringstream message;
			message<<"Unknown type: "<<type<<std::endl;
			//throw std::runtime_error(message.str());
			throw std::runtime_error(message.str().c_str());
		}
	
		return size;
}

void GetTypeSize(const v8::FunctionCallbackInfo<v8::Value>& args){
    Isolate* isolate = Isolate::GetCurrent(); 
    HandleScope scope(isolate);
    
    try {
        size_t size;

        GET_CHAR_ARG(type, args, 0);

        size = bridjs::Utils::getTypeSize(type);

        args.GetReturnValue().Set(WRAP_ULONGLONG(size));
    } catch (std::exception &e) {
        THROW_EXCEPTION(e.what());
    }

}

void WriteInt64(const v8::FunctionCallbackInfo<v8::Value>& args){
	Isolate* isolate = Isolate::GetCurrent(); HandleScope scope(isolate);

	GET_POINTER_ARG(const char, ptr,args,0);
	GET_INT64_ARG(offset,args,1);
	GET_INT64_ARG(value,args,2);

	ptr += offset;

	(*(int64_t*)ptr) = value;

	args.GetReturnValue().Set(v8::Undefined(isolate));
}

void bridjs::Utils::Init(v8::Handle<v8::Object> utilsObj){
	NODE_SET_METHOD(utilsObj,"getTypeSize",GetTypeSize);
	NODE_SET_METHOD(utilsObj,"writeInt64",WriteInt64);
	NODE_SET_METHOD(utilsObj,"pointerToString",PointerToString);
	NODE_SET_METHOD(utilsObj, "memoryCopy", MemCpy);
}

void bridjs::Utils::PointerToString(const v8::FunctionCallbackInfo<v8::Value>& args){
	Isolate* isolate = Isolate::GetCurrent(); HandleScope scope(isolate);
	GET_POINTER_ARG(const char,ptr,args,0);
	v8::Local<v8::String> str;

	if(args.Length()>=2){
		GET_INT32_ARG(length,args,1);
                str = v8::String::NewFromUtf8(isolate,ptr,v8::String::kNormalString, length);
		//str = v8::String::New(ptr,length);
	}else{
		//str = v8::String::New(ptr);
                str = v8::String::NewFromUtf8(isolate,ptr);
	}


	args.GetReturnValue().Set(str);
}

#if NODE_MAJOR_VERSION<4

Local<Value> bridjs::Utils::wrapPointerToBuffer(Isolate* isolate, const void* ptr) {
    v8::EscapableHandleScope scope(isolate);
    Handle<Value> result;

    if (ptr != NULL) {
        v8::Local<v8::Object> buf = node::Buffer::New(isolate, (char*) (&ptr), sizeof (void*));
        result = scope.Escape(buf);
    } else {
        result = scope.Escape(v8::Local<Primitive>::New(isolate, v8::Null(isolate)));
    }

    return result;

	//memcpy(&pptr,node::Buffer::Data(buf), sizeof(void*));

	
}

void* bridjs::Utils::unwrapBufferToPointer(v8::Local<v8::Value> value){
	void* ptr;

	memcpy(&ptr, node::Buffer::Data(value->ToObject()), sizeof(void*));

	return ptr;
}

#else

Local<Value> bridjs::Utils::wrapPointerToBuffer(Isolate* isolate, const void* ptr) {
    v8::EscapableHandleScope scope(isolate);
    
    if (ptr != NULL) { 
        
        v8::Local<v8::ArrayBuffer> ab = ArrayBuffer::New(isolate, sizeof(void*));
        
        std::memcpy(ab->GetContents().Data(), &ptr, sizeof(void*)); 
    
        return scope.Escape(ab);
    } else {
        return scope.Escape(v8::Local<Primitive>::New(isolate, v8::Null(isolate)));
    }
}

void* bridjs::Utils::unwrapBufferToPointer(v8::Local<v8::Value> value){
	void* ptr;
        void* bufferPtr;
        
        if(node::Buffer::HasInstance(value)){
            bufferPtr = node::Buffer::Data(value);
        }else{
            v8::Local<v8::ArrayBuffer> ab = value.As<v8::ArrayBuffer>();
            
            bufferPtr = ab->GetContents().Data();
        }
         
	std::memcpy(&ptr, bufferPtr, sizeof(void*));

	return ptr;
}

#endif

Local<v8::Value> bridjs::Utils::wrapPointer(v8::Isolate* isolate,const void* ptr){
    v8::EscapableHandleScope scope(isolate);
    
    if (ptr != NULL) {
        return scope.Escape(bridjs::Pointer::NewInstance(isolate, ptr));
    } else {
        return scope.Escape(v8::Local<Primitive>::New(isolate, v8::Null(isolate)));
    }
}


const void* bridjs::Utils::unwrapPointer(v8::Isolate* isolate, v8::Local<v8::Value> value){
	const void* ptr;

	//memcpy(&ptr, node::Buffer::Data(value->ToObject()), sizeof(void*));
	if(value->IsNull()){
		ptr = NULL;
	}else if(node::Buffer::HasInstance(value)){
		ptr = node::Buffer::Data(value);
	}else if(value->IsObject()){
		v8::Local<v8::Object> object = value->ToObject();
		
		if(object->InternalFieldCount()>0){
			ptr = bridjs::Pointer::Data(isolate, object);
		}else{
			throw std::runtime_error("Invalid bridjs::Pointer object");
		}
	}else{
		throw std::runtime_error("Unknown JavaScript value for pointer type");
	}

	return ptr;
}

v8::Local<v8::String> bridjs::Utils::toV8String(v8::Isolate* isolate, const char val){
	//Isolate* isolate = Isolate::GetCurrent(); HandleScope scope(isolate);
	char str[] = {val, '\0'};

	return v8::String::NewFromUtf8(isolate,str);
}

void bridjs::Utils::MemCpy(const v8::FunctionCallbackInfo<v8::Value>& args){
	Isolate* isolate = Isolate::GetCurrent(); HandleScope scope(isolate);
	GET_POINTER_ARG(void, pDst, args, 0);
	GET_POINTER_ARG(void, pSrc, args, 1);
	GET_INT64_ARG(size, args, 2);

	memcpy(pDst, pSrc, size);

	args.GetReturnValue().Set(v8::Undefined(isolate));
}

bridjs::ValueWrapper::ValueWrapper(v8::Isolate* isolate,
        v8::Persistent<v8::Value> value):mpIsolate(isolate), mValue(isolate,value){

}

v8::Local<v8::Value> bridjs::ValueWrapper::getValue(){
	return v8::Local<v8::Value>::New(this->mpIsolate,this->mValue);
}

v8::Local<v8::Value> bridjs::Utils::convertDataByType(v8::Isolate* isolate,std::shared_ptr<void> spData,const char type){
	void* pData = spData.get();

    switch (type) {
        case DC_SIGCHAR_VOID:
        {
            return v8::Undefined(isolate);
        }
            break;
        case DC_SIGCHAR_BOOL:
        {
            return v8::Local<v8::Primitive>::New(isolate, v8::Boolean::New(isolate, *(static_cast<DCbool*> (pData))));
        }
            break;
        case DC_SIGCHAR_UCHAR:
        {
            return (bridjs::Utils::toV8String(isolate, *(static_cast<DCuchar*> (pData))));
        }
            break;
        case DC_SIGCHAR_CHAR:
        {
            return (bridjs::Utils::toV8String(isolate, *(static_cast<DCchar*> (pData))));
        }
            break;
        case DC_SIGCHAR_SHORT:
        {
            return v8::Int32::New(isolate, *(static_cast<DCshort*> (pData)));
        }
            break;
        case DC_SIGCHAR_USHORT:
        {
            return v8::Uint32::New(isolate,*(static_cast<DCushort*> (pData)));
        }
            break;
        case DC_SIGCHAR_INT:
        {
            return v8::Int32::New(isolate,*(static_cast<DCint*> (pData)));
        }
            break;
        case DC_SIGCHAR_UINT:
        {
            return v8::Uint32::New(isolate,*(static_cast<DCuint*> (pData)));
        }
            break;
        case DC_SIGCHAR_LONG:
        {
            return v8::Number::New(isolate,*(static_cast<DClong*> (pData)));
        }
            break;
        case DC_SIGCHAR_ULONG:
        {
            return v8::Number::New(isolate,*(static_cast<DCulong*> (pData)));
        }
            break;
        case DC_SIGCHAR_LONGLONG:
        {
            return v8::Number::New(isolate,static_cast<double> (*(static_cast<DClonglong*> (pData))));
        }
            break;
        case DC_SIGCHAR_ULONGLONG:
        {
            return v8::Number::New(isolate,static_cast<double> (*(static_cast<DCulonglong*> (pData))));
        }
            break;
        case DC_SIGCHAR_FLOAT:
        {
            return v8::Number::New(isolate,static_cast<double> (*(static_cast<DCfloat*> (pData))));
        }
            break;
        case DC_SIGCHAR_DOUBLE:
        {
            return v8::Number::New(isolate,static_cast<double> (*(static_cast<DCdouble*> (pData))));
        }
            break;
        case DC_SIGCHAR_STRING:
        {
            return WRAP_STRING(*(static_cast<const char**> (pData)));
        }
            break;
        case DC_SIGCHAR_POINTER:
        {
            return bridjs::Utils::wrapPointer(isolate,*(static_cast<DCpointer*> (pData)));
        }
            break;
        case DC_SIGCHAR_STRUCT:
        {
            return Exception::Error(String::NewFromUtf8(isolate,"Not implement"));
        }
            break;
        default:
            std::stringstream message;
            message << "Unknown returnType: " << type << std::endl;
            
            return Exception::Error(String::NewFromUtf8(isolate,message.str().c_str()));
    }
}


bridjs::ValueWrapper::~ValueWrapper(){
	this->mValue.Reset();
}


