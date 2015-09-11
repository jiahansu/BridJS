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
var bridjs = require('../lib/bridjs.js'), Signature = bridjs.Signature, 
        libPath = __dirname + "/" + bridjs.LIBRARY_PATH + ".node", nativeModule, 
        ret;

/*
 typedef struct{
    int16_t w;
    int32_t x;
    long y;
    LONGLONG z;
    double e;
 } TestStruct;
 */
var TestStruct = bridjs.defineStruct({
    x: {type: "int16", order: 0},
    y: {type: "int32", order: 1},
    z: {type: "long", order: 2},
    w: {type: "longlong", order: 3},
    e: {type: "double", order: 4}
});
/*
typedef struct{
    double x;
    double y;
} Point2d;
*/
var Point2d = bridjs.defineStruct({
    x : {type: "double", order: 0},
    y : {type: "double", order: 1}
});
/*
typedef struct{
    double x;
    double y;
    double z;
} Point3d;
 */        
var Point3d = bridjs.defineStruct({
    x : {type: "double", order: 0},
    y : {type: "double", order: 1},
    z : {type: "double", order: 2}
});

var UnionValue = bridjs.defineUnion({
    x: {type: "double", order: 0},
    y: {type: "char", order: 1},
    z: {type: "int32_t", order: 2}
});

var TestComplexStruct = bridjs.defineStruct({
    w:{type: "char", order: 0},
    subStruct:{type: TestStruct, order: 1},
    unionValue: {type: UnionValue, order: 2},
    x:{type: "int16", order: 3},
    point2d:{type: Point2d, order: 4},
    y:{type: "int32", order: 5},
    point3d:{type: Point3d, order: 6},
    z:{type: "int64", order: 7}
});

var TestArrayStruct = bridjs.defineStruct( {
    w: {type: "char", order: 0},
    first: {type: "char[3]", order: 1},
    second: {type: "char[3]", order: 2}
});

var TestStructCallbackFunction = bridjs.defineFunction("double (TestStruct* pTestStruct)", {TestStruct: TestStruct});

var callbackFunctionDefine = bridjs.defineFunction("double (int16_t, int32_t, long, longlong, double)");

var callback = bridjs.newCallback(callbackFunctionDefine, function(w, x, y, z, e) {
        console.log("Callback function was invoked");
    
        return w*x*y*z*e;
});

var structCallback = bridjs.newCallback(TestStructCallbackFunction, function (testStructArg) {
    //log.info(testStructArg.e);
    assert(testStructArg.e === testStruct.e, "Fail to call testerInstance.testStructCallbackFunction");

    return testStructArg.w * testStructArg.x * testStructArg.y * testStructArg.z * testStructArg.e;
});

var NativeModule = bridjs.defineModule({
    /*double testMultiplyFunction(const int16_t w, const int32_t x,const long y, const LONGLONG z, const double e)*/
    testMultiply: bridjs.defineFunction("double testMultiplyFunction(int16_t,int32_t ,long ,longlong , double)"),
            
    /*double testStructFunction(const TestStruct* pTestStruct)*/
    testStructFunction: bridjs.defineFunction("double testStructFunction(TestStruct *pTestStruct)", {TestStruct:TestStruct}),
    
    /*double testComplexStructFunction(const TestComplexStruct* pTestStruct)*/
    testComplexStructFunction: bridjs.defineFunction("double testComplexStructFunction(TestComplexStruct*)",  {TestComplexStruct:TestComplexStruct}),
    
    /*double testArrayStructFunction(const TestArrayStruct* pTestStruct)*/
    testArrayStructFunction: bridjs.defineFunction("double testArrayStructFunction(TestArrayStruct* pTestStruct)", {TestArrayStruct:TestArrayStruct}),
    
    /*void testAsyncCallbackFunction(MultiplyCallbackFunction callbackFunction);*/
    testAsyncCallbackFunction: bridjs.defineFunction("void testAsyncCallbackFunction(MultiplyCallbackFunction callbackFunction)", {MultiplyCallbackFunction:callbackFunctionDefine}),
    
    /*const TestStruct* testStructPassByPointerFunction(const TestStruct* pTestStruct)*/
    testStructPassByPointerFunction: bridjs.defineFunction("TestStruct* testStructPassByPointerFunction(const TestStruct* pTestStruct)", {TestStruct:TestStruct}),
    
    /*void testStructCallbackFunction(const TestStruct* pTestStruct,TestStructCallbackFunction callbackFunction)*/
    testStructCallbackFunction: bridjs.defineFunction("void testStructCallbackFunction(TestStruct* pTestStruct, TestStructCallbackFunction callbackFunction)", {TestStruct:TestStruct, TestStructCallbackFunction:TestStructCallbackFunction}),
    
    /*const double* testValuePassByPointerFunction(const double *returnValue)*/
    testValuePassByPointerFunction: bridjs.defineFunction("double* testValuePassByPointerFunction(const double *returnValue)")
}, libPath);

nativeModule = new NativeModule();

console.log(nativeModule.testMultiply(2,2,2,2,2.5));

var testComplexStruct = new TestComplexStruct();
testComplexStruct.w = testComplexStruct.x = testComplexStruct.y = testComplexStruct.z = 2;
testComplexStruct.subStruct.e =  testComplexStruct.point2d.x = testComplexStruct.point3d.y = testComplexStruct.unionValue.x= 2.5;

console.log(testComplexStruct.point3d.y);
console.log(nativeModule.testComplexStructFunction(bridjs.byPointer(testComplexStruct)));

bridjs.async(nativeModule).testMultiply(2,2,2,2,2.5, function(returnValue){
    console.log("return value: "+returnValue);
});

nativeModule.testAsyncCallbackFunction(callback);

var nativeDouble = new bridjs.NativeValue.double(2.5);
var returnNativeDouble = nativeModule.testValuePassByPointerFunction(bridjs.byPointer(nativeDouble));

console.log(nativeDouble.get());