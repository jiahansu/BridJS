BridJS
======
BridJS is a [BridJ](http://bridj.googlecode.com)-like API for runtime binding C function and struct without writing any extra native code.

###Key features
* BridJS binds native function at runtime, you never need to compile any extra native code 
* Uncompromized speed and power by [dyncall](http://www.dyncall.org/)
* Support implicit type wrapping/unwrapping (struct<->pointer and string<->number etc... )
* Support complex struct type (sub-struct and array field), and access theme straightforwadly.
* Execute any native function either by synchronous or by asynchronous way
* Whatever the native callbacks are invoked by any other thread, BridJS always forward callback to V8's default thread 

###Limitation
Like [BridJ](http://bridj.googlecode.com), BridJS also has some limitations:
* Pass structs by value not supported yet (neither as function arguments nor as function return values)
* BridJS does **not support** C++, COM, Objective-C...

###Requirement
* [nodejs](http://nodejs.org/) v0.10.8 or higher
* Windows x64, Linux x86/x64 & Mac OSX

###Installation
If node.js version is higher or equal than v0.12.0:
``` bash
npm install bridjs
```
If node.js version is lower or equal than v0.10.38:
``` bash
npm install bridjs@0.1.9-3
```

###Tutorial
####1. C function
-------------
If C code is something like this:
``` bash
double testMultiplyFunction(const int16_t w, const int32_t x,const long y, const LONGLONG z, const double e);
``` 

You can define JavaScript prototype like this:
``` bash
var bridjs = require('bridjs');

var NativeModule = bridjs.defineModule({
    testMultiplyFunction:  bridjs.defineFunction("double testMultiplyFunction(int16_t,int32_t ,long ,longlong , double)")
    }, libraryPath);
    
var nativeModule = new NativeModule();

var result = nativeModule.testMultiplyFunction(2,2,2,2,2.5);
``` 
Bind C function API
``` bash
bridjs.defineModule({
  functionName1: bridjs.DefineFunction(returnType, arg0Type, arg2Type...),
  //Or
  functionName2: bridjs.DefineFunction("function declaration in C"),
  ...
},libraryFile);
``` 
####2. C struct
---------------
If C code is something like this:
``` bash
typedef struct{
  double x;
  double y;
  double z;
} Point3d;

tydef struct{
  char x;
  Point3d y;
  char str[10];
} ComplexStruct

double testComplexStructFunction(const ComplexStruct* pTestStruct)
``` 	
You can define JavaScript prototype like this:
``` bash
var Point3d = bridjs.defineStruct({
    x : {type: "double", order: 0},
    y : {type: "double", order: 1},
    z : {type: "double", order: 2}
});

var ComplexStruct = bridjs.defineStruct({
    x:{type: "char", order: 0},
    y:{type: Point3d, order: 1},
    z:{type: "char[10]", order: 2}
});

var NativeModule = bridjs.defineModule({
    testComplexStructFunction : bridjs.defineFunction("double testComplexStructFunction(ComplexStruct*)"}, libraryPath);

var complexStruct = new ComplexStruct();
var nativeModule = new NativeModule();

complexStruct.x = 's';
complexStruct.y.x = 2;
complexStruct.str.set(3) = 's';

var result = nativeModule.testComplexStructFunction(bridjs.byPointer(complexStruct));

``` 
Bind C struct API
``` bash
bridjs.defineStruct({
    element1 : bridjs.structField(elementType,order),
    //Or
    element2 : {type: "<elementType>", order: <order>},
    element3 : bridjs.structArrayField(arrayType,arrayLength,order)
    ...
});
``` 
####3. Invoke native function asynchronously
--------------------------------------------
``` bash
/*You can execute any native function asynchronously (not in default thread), and get return value from callback*/
bridjs.aysnc(nativeModule).testMultiplyFunction(2,2,2,2,2.5, function(returnValue){
    console.log("Return value = "+returnValue)
});
```
Async execute native function API
``` bash
bridjs.async(moduleInstance).function(param1, param2,....callbackFunction);
```
####4. C function pointer
-------------------------
If C code is something like this:
``` bash
typedef double (*MultiplyCallbackFunction)(const int16_t w, const int32_t x,const long y, const LONGLONG z, const double e);
void testCallbackFunction(MultiplyCallbackFunction callbackFunction);
```
You can define JavaScript prototype like this:
``` bash
var callbackFunctionDefine = bridjs.defineFunction("double (int16_t, int32_t, long, longlong, double)");

var callback = bridjs.newCallback(callbackFunctionDefine, function(w, x, y, z, e) {
        console.log("Callback function was invoked");
    
        return w*x*y*z*e;
});

var NativeModule = bridjs.defineModule({
    testCallbackFunction : bridjs.defineFunction("void testCallbackFunction(MultiplyCallbackFunction callbackFunction)", {MultiplyCallbackFunction:callbackFunctionDefine})
    }, libraryPath);

var nativeModule = new NativeModule();

nativeModule.testAsyncCallbackFunction(callback);    
```
Create function pointer API
``` bash
bridjs.newCallback(functionSignature,callbackFunction);
```
####5. Pass primitive type by pointer
-------------------------------------
If C code is something like this:
``` bash
const double* testValuePassByPointerFunction(const double *returnValue);
```
You can define JavaScript prototype like this:
``` bash
var NativeModule = bridjs.defineModule({
    testValuePassByPointerFunction:  bridjs.defineFunction("double* testValuePassByPointerFunction(double*)")
    }, libraryPath);

var nativeDouble = new bridjs.NativeValue.double(2.5);  

var nativeModule = new NativeModule();

var returnNativeDouble = nativeModule.testValuePassByPointerFunction(bridjs.byPointer(nativeDouble));    

var result = returnNativeDouble.get();
```

###License

BSD License. See the `LICENSE` file.
