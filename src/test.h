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
#include <uv.h>
#include "dyncall_v8_utils.h"

#include <node.h>
#include <iostream>

extern "C" {
#include "dyncall.h"	
#include "dyncall_signature.h"
}

using namespace v8;

namespace bridjs {

    class Test {
    public:
        static void test();
        static void asyncTestCallback(uv_work_t *req);
        static void afterCallAsync(uv_work_t *req);
        static void TestMultiplyFunction(const v8::FunctionCallbackInfo<v8::Value>& args);
    };
}

extern "C" {

    typedef struct {
        int16_t w;
        int32_t x;
        long y;
        DClonglong z;
        double e;
    } TestStruct;

    typedef struct {
        double x;
        double y;
    } Point2d;

    typedef struct {
        double x;
        double y;
        double z;
    } Point3d;

    typedef struct {
        int8_t x;
        int8_t y;
        int8_t z;
    } Point3c;

    typedef struct {
        int8_t x;
        int8_t y;
        int8_t x1;
        int8_t y1;
    } Point2c;

    typedef struct {
        char w;
        char first[3];
        char second[3];
    } TestArrayStruct;

    typedef struct {
        int8_t w;
        TestStruct subStruct;
        int16_t x;
        Point2d point2d;
        int32_t y;
    } TempStruct2;
    
    typedef union{
        double  x;
        char    y;
        int32_t z;
    }UnionValue;
    
    typedef struct {
        char w;
        TestStruct subStruct;
        UnionValue unionValue;
        int16_t x;
        Point2d point2d;
        int32_t y;
        Point3d point3d;
        int64_t z;
    } TestComplexStruct;
    
    typedef double (*MultiplyCallbackFunction)(const int16_t w, const int32_t x, const long y, const DClonglong z, const double e);
    typedef double (*TestStructCallbackFunction)(const TestStruct* pTestStruct);


    UV_EXTERN double testMultiplyFunction(const int16_t w, const int32_t x, const long y, const DClonglong z, const double e);
    UV_EXTERN double testStructFunction(const TestStruct* pTestStruct);
    UV_EXTERN const char* testStringFunction(const char* pTestStruct);
    UV_EXTERN void testCallbackFunction(MultiplyCallbackFunction callbackFunction);
    UV_EXTERN void testAsyncCallbackFunction(MultiplyCallbackFunction callbackFunction);
    UV_EXTERN double testStructValueFunction(const TestStruct testStruct);
    UV_EXTERN double testComplexStructFunction(const TestComplexStruct* pTestStruct);
    UV_EXTERN double testArrayStructFunction(const TestArrayStruct* pTestStruct);
    UV_EXTERN const TestStruct* testStructPassByPointerFunction(const TestStruct* pTestStruct);
    UV_EXTERN void testStructCallbackFunction(const TestStruct* pTestStruct, TestStructCallbackFunction callbackFunction);
    UV_EXTERN const double* testValuePassByPointerFunction(const double *returnValue);
    UV_EXTERN void testFillStringFunction(char* strBuffer, const uint32_t length);
    UV_EXTERN double testUnionValueFunction(const UnionValue *pUnionValue);
}
