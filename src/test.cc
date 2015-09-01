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
#include "test.h"
#include "dyncall.h"
#include <string.h>

using namespace v8;

void bridjs::Test::asyncTestCallback(uv_work_t *req) {
    MultiplyCallbackFunction func = (MultiplyCallbackFunction) (req->data);
    //std::cout<<"Got it"<<std::endl;
    if (func != NULL) {
        std::cout << "testAsyncCallbackFunction: " << func(2, 2, 2, 2, 2.5) << std::endl;
    } else {
        std::cerr << "Fail to cast data pointer to MultiplyCallbackFunction" << std::endl;
    }

    //std::cout<<"Got it"<<std::endl;
}

void bridjs::Test::afterCallAsync(uv_work_t *req) {
    delete req;
    //std::cout<<"Got it2"<<std::endl;
}

void bridjs::Test::TestMultiplyFunction(const v8::FunctionCallbackInfo<v8::Value>& args) {
    Isolate* isolate = Isolate::GetCurrent(); HandleScope scope(isolate);

    GET_INT32_ARG(w, args, 0);
    GET_INT32_ARG(x, args, 1);
    GET_INT64_ARG(y, args, 2);
    GET_INT64_ARG(z, args, 3);
    GET_DOUBLE_ARG(e, args, 4);

    args.GetReturnValue().Set(v8::Number::New(isolate,testMultiplyFunction(w, x, static_cast<const long> (y), z, e)));
}

extern "C" {

    double testMultiplyFunction(const int16_t w, const int32_t x, const long y, const DClonglong z, const double e) {
        return w * x * y * z*e;
    }

    void testCallbackFunction(MultiplyCallbackFunction callbackFunction) {
        std::cout << "testCallbackFunction: " << callbackFunction(2, 2, 2, 2, 2.5) << std::endl;
    }

    void testAsyncCallbackFunction(MultiplyCallbackFunction callbackFunction) {
        uv_work_t *req = new uv_work_t();
        req->data = (void*)callbackFunction;

        uv_queue_work(uv_default_loop(), req, bridjs::Test::asyncTestCallback, (uv_after_work_cb) bridjs::Test::afterCallAsync);
    }

    double testStructFunction(const TestStruct *pTestStruct) {

        //std::cout<<"Test struct size: "<<sizeof(TestStruct)<<std::endl;

        return pTestStruct->w * pTestStruct->x * pTestStruct->y * pTestStruct->z * pTestStruct->e;
    }

    double testStructValueFunction(const TestStruct testStruct) {

        //std::cout<<pTestStruct->e<<std::endl;

        return testStruct.w * testStruct.x * testStruct.y * testStruct.z * testStruct.e;
    }

    double testComplexStructFunction(const TestComplexStruct* pTestStruct) {
        return pTestStruct->w * pTestStruct->x * pTestStruct->y * pTestStruct->z * 
               pTestStruct->point2d.x * pTestStruct->point3d.y * pTestStruct->subStruct.e 
               * pTestStruct->unionValue.x;
    }

    double testArrayStructFunction(const TestArrayStruct* pTestStruct) {
       // std::cout<<"Fwfewf"<<std::endl;
        return pTestStruct->w * pTestStruct->first[1] * pTestStruct->second[2];
    }

    const TestStruct* testStructPassByPointerFunction(const TestStruct* pTestStruct) {
        return pTestStruct;
    }

    void testStructCallbackFunction(const TestStruct* pTestStruct, TestStructCallbackFunction callbackFunction) {
        //std::cout<<(void*)callbackFunction<<std::endl;
        std::cout << "testStructCallbackFunction: " << callbackFunction(pTestStruct) << std::endl;
    }

    const double* testValuePassByPointerFunction(const double *returnValue) {
        return returnValue;
    }

    const char* testStringFunction(const char* pTestString){
        //std::cout<<"testStringFunction "<<pTestString<<std::endl;
        return pTestString;
    }
    
    double testUnionValueFunction(const UnionValue *pUnionValue){
        //std::cout <<"Pointer: "<<pUnionValue<< ", testUnionValueFunction: " << pUnionValue->x << std::endl;
        return pUnionValue->x;
    }
    
    void testFillStringFunction(char* strBuffer, const uint32_t length){
        const char* srcStr = "test fillStringFunction";
        
        strncpy(strBuffer,srcStr , length);
    }
}