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
 * THIS SOFTWARE IS PROVIDED BY OLIVIER CHAFIK, JIA-HAN SU AND CONTRIBUTORS ``AS IS'' AND ANY
 * EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
 * WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
 * DISCLAIMED. IN NO EVENT SHALL THE REGENTS AND CONTRIBUTORS BE LIABLE FOR ANY
 * DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
 * (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
 * LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND
 * ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
 * SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */
var my = require('myclass'), BridjsException = require("./bridjs_exception"),
        CallbackWrapper = require("./callback_wrapper"),
        FunctionCallbackWrapper = require("./function_callback_wrapper"),
        Utils = require("./utils"), StructField = require("./struct_field"),
        PassTypeWrapper = require("./pass_type_wrapper"),
        bridjs = require("./native_lib"), NativeFunctionWrapper,
        SignatureWrapper = require("./signature_wrapper"), defaultStackSize = 4096, 
        AbstractStruct = require("./abstract_struct"), UnionStruct = require("./union_struct"),
        Struct = require("./struct"), ArrayStruct = require("./array_struct"),
        log4js = require("log4js"),Compiler = require("./compiler"),
        log = log4js.getLogger("BridJS"), name, newName, value;

if (bridjs) {

    /*
     function mergeNativeArguments(argsObject) {
     var args = new Array(argsObject.length),j;
     
     //args[0] = klass._bridjs_impl_vm;
     for (j = 0; j < argsObject.length; ++j) {
     args[j] = argsObject[j];
     }
     
     return args;
     }*/
    function getNativeType(klass) {
        var type;

        if (klass instanceof SignatureWrapper) {
            type = bridjs.dc.Signature.POINTER_TYPE;
        } else if (klass instanceof PassTypeWrapper) {
            type = klass.getPassType();
        } else {
            type = klass;
        }

        return type;
    }
    function setNativeFunction(klass, name) {
        var nativeFunction, callMethod, args,
                dyncall = bridjs.dc, dynload = bridjs.dl, symbol, j,
                property = klass.prototype[name], callAsyncMthod, wrapReturn,
                returnType = property.getReturnType(), bindName, instanceCache;

        if (typeof (property.getBindName()) === "string") {
            bindName = property.getBindName();
        } else {
            bindName = name;
        }

        instanceCache = property.hasCacheInstance();

        symbol = dynload.findSymbol(klass._bridjs_impl_lib, bindName);

        if (symbol) {
            args = new Array(2 + property.getArgumentLength());
            args[0] = symbol;
            args[1] = getNativeType(returnType);
            for (j = 0; j < property.getArgumentLength(); ++j) {
                args[j + 2] = getNativeType(property.getArgumentType(j));
            }

            nativeFunction = new dyncall.NativeFunction(args);
            callMethod = dyncall.NativeFunction.prototype.call;
            callAsyncMthod = dyncall.NativeFunction.prototype.callAsync;

            if (returnType instanceof PassTypeWrapper) {
                /*Wrap pointer to struct*/
                var type = returnType.getType();

                wrapReturn = Utils.createTypeWrap(type, instanceCache);
            } else {
                wrapReturn = function(value) {
                    return value;
                };
            }

            //wrapReturn = Utils.createTypeWrapper(returnType);

            klass.prototype[name] = function() {
                var r;
                try {
                    r = wrapReturn(callMethod.call(nativeFunction, klass._bridjs_impl_vm,
                            arguments));
                } catch (e) {
                    throw new Error(e);
                }

                return r;
            };
            klass.prototype._bridjs_impl_async[name] = function() {
                var lastIndex = arguments.length - 1, callback = arguments[lastIndex];
                arguments[lastIndex] = new CallbackWrapper(callback, returnType, instanceCache);
                //console.log(arguments);
                return callAsyncMthod.call(nativeFunction, 4069,
                        arguments);
            };
        } else {
            throw new BridjsException("Fail to locate function: " + bindName);
        }
    }

    function defineReadOnlyProperty(object, newName, value) {
        Object.defineProperty(object, newName, {
            get: function() {
                return value;
            }
        });
    }

    for (name in  bridjs.dc.Signature) {
        newName = name.replace("_TYPE", '').toLowerCase();
        defineReadOnlyProperty(bridjs.dc.Signature, newName, bridjs.dc.Signature[name]);
        defineReadOnlyProperty(bridjs.dc.Signature, newName.toUpperCase(), bridjs.dc.Signature[name]);
        defineReadOnlyProperty(bridjs.dc.Signature, newName+"_t", bridjs.dc.Signature[name]);
        defineReadOnlyProperty(bridjs.dc.Signature, newName.charAt(0).toUpperCase() + newName.substr(1), bridjs.dc.Signature[name]);
    }

    module.exports = my.Class({
        STATIC: {
            rawBindings: bridjs,
            dyncall: bridjs.dc,
            dynload: bridjs.dl,
            dyncallback: bridjs.dcb,
            dc: bridjs.dc,
            dl: bridjs.dl,
            dcb: bridjs.dcb,
            test: bridjs.test,
            Pointer: bridjs.dc.Pointer,
            Struct: Struct,
            Signature: bridjs.dc.Signature,
            utils: bridjs.utils,
            DEFAULT_STACK_SIZE: defaultStackSize,
            LIBRARY_PATH: bridjs.LIBRARY_PATH,
            symbols: function(libPath) {
                var symbols = new Array();
                var syms = bridjs.dl.symsInit(libPath);
                try {
                    var count = bridjs.dl.symsCount(syms);
                    for (var i = 0; i < count; i++) {
                        var name = bridjs.dl.symsName(syms, i);
                        if (name)
                            symbols.push(name);
                    }
                } finally {
                    bridjs.dl.symsCleanup(syms);
                }
                return symbols;
            },
            register: function(klass, libraryPath, config) {
                var propertyNames, property, dyncall = bridjs.dc,
                        dynload = bridjs.dl, name, i;
                try {
                    if (typeof (config) !== "object") {
                        config = {};
                    }

                    if (typeof (config.statckSize) !== "number"
                            || config.statckSize <= 0) {
                        config.stackSize = defaultStackSize;
                    }
                    //console.log(config);
                    klass._bridjs_impl_vm = dyncall.newCallVM(config.stackSize);
                    klass._bridjs_impl_lib = dynload.loadLibrary(libraryPath);
                    if (klass._bridjs_impl_lib) {
                        if (typeof (klass.prototype._bridjs_impl_async) !== "object") {
                            klass.prototype._bridjs_impl_async = {};
                        }
                        propertyNames = Object.getOwnPropertyNames(klass.prototype);
                        //console.log(propertyNames);
                        for (i = 0; i < propertyNames.length; ++i) {
                            name = propertyNames[i];
                            property = klass.prototype[name];
                            //console.log(name+", "+property);
                            try {
                                if (property instanceof SignatureWrapper) {
                                    setNativeFunction(klass, name);
                                }
                            } catch (e) {
                                log.fatal("Fail to bind native function: " + name);

                                throw e;
                            }
                        }
                    } else {
                        throw new BridjsException("Fail to load library from: " + libraryPath);
                    }
                } catch (e) {
                    this.unregister(klass);
                    log.warn("Fail to register module :" + klass + ", exception = " + e);
                    throw e;
                }
            },
            unregister: function(klass) {
                if (klass._bridjs_impl_vm) {
                    bridjs.dc.free(klass._bridjs_impl_vm);
                    klass._bridjs_impl_vm = null;
                }

                if (klass._bridjs_impl_lib) {
                    bridjs.dl.freeLibrary(klass._bridjs_impl_lib);
                    klass._bridjs_impl_lib = null;
                }
            },
            defineFunction: function() {
                return new SignatureWrapper(arguments);
            },
            async: function(object) {
                return object._bridjs_impl_async;
            },
            structField: function(type, order) {
                Utils.checkType(type);

                return new StructField(type, order);
            },
            structArrayField: function(type, length, order) {
                return module.exports.structField(Struct.defineStructArray(type,length), order);
            },
            defineStruct: function(elements, constructor) {
                var structClass;
                //console.log(typeof (elements.constructor));
                //if(typeof(elements.constructor)!=="function"){
                elements.constructor = function(pointer) {
                    structClass.Super.call(this, pointer);
                    if (typeof (constructor) === "function") {
                        constructor.call(this, pointer);
                    }
                };
                //}

                structClass = my.Class(Struct, elements);

                return structClass;
            },
            defineUnion: function(elements, constructor) {
                var structClass;
                //console.log(typeof (elements.constructor));
                //if(typeof(elements.constructor)!=="function"){
                elements.constructor = function(pointer) {
                    structClass.Super.call(this, pointer);
                    if (typeof (constructor) === "function") {
                        constructor.call(this, pointer);
                    }
                };
                //}

                structClass = my.Class(UnionStruct, elements);

                return structClass;
            },
            defineArray: function(type, length) {
                Utils.checkType(type);

                if (typeof (length) === "number") {
                    var structClass = my.Class(ArrayStruct, {
                        constructor: function(pointer) {
                            structClass.Super.call(this, type, length,pointer);
                        }
                    });

                    return structClass;
                } else {
                    throw new BridjsException("Illegal length argument: " + length);
                }
            },
            newArray: function(type, length, pointer) {
                var Array = module.exports.defineArray(type, length);

                return new Array(pointer);
            },
            byPointer: function(klass) {
                return Compiler.byPointer(klass);
            },
            getStructPointer: function(struct) {
                return Struct.getStructPointer(struct);
            },
            getStructSize: function(struct) {
                return struct._bridjs_impl_get_size();
            },
            newCallback: function(functionDefinition, callbackFunction, hasInstanceCache) {
                if (functionDefinition instanceof SignatureWrapper) {
                    var returnType = functionDefinition.getReturnType(),
                            callbackWrapper, argumentTypes =
                            new Array(functionDefinition.getArgumentLength()),
                            nativeArgumentTypes =
                            new Array(functionDefinition.getArgumentLength()), i,
                            type, instanceCache = hasInstanceCache;

                    for (i = 0; i < functionDefinition.getArgumentLength(); ++i) {
                        type = functionDefinition.getArgumentType(i);
                        argumentTypes[i] = type;
                        nativeArgumentTypes[i] = getNativeType(type);
                    }

                    if (typeof (instanceCache) === "undefined") {
                        instanceCache = true;
                    }

                    callbackWrapper = new FunctionCallbackWrapper(callbackFunction,
                            argumentTypes, instanceCache);

                    return bridjs.dcb.newCallback(returnType, nativeArgumentTypes,
                            callbackWrapper);
                } else {
                    throw new BridjsException("Illegal argument exception, unknown function definition: "
                            + functionDefinition);
                }
            },
            defineModule: function(properties, libraryPath, config) {
                var klass = my.Class(properties);

                module.exports.register(klass, libraryPath, config);

                return klass;
            },
            defineNativeValue: function(type) {
                var structClass = module.exports.defineStruct({
                    value: module.exports.structField(type, 0),
                    get: function() {
                        return this.value;
                    },
                    set: function(value) {
                        this.value = value;
                    },
                    toString: function() {
                        return "" + this.value;
                    }
                }, function(value) {
                    //To do, We can not figure out the pointer is for value or Struct's address
                    if (typeof (value) !== "undefined" && !(value instanceof bridjs.dc.Pointer)) {
                        this.set(value);
                    }
                });


                return structClass;
            },
            newNativeValue: function(type) {
                var n = module.exports.defineNativeValue(type);

                return new n();
            },
            getTypeSize: function(klass) {
                if (klass instanceof Struct) {
                    /*Struct's instance, return pointer immediately*/
                    return module.exports.getStructSize(klass);
                } else if (typeof (klass) === "function") {
                    /*Struct's class, return PassTypeWrapper*/
                    var size, oldValue = Struct.isAllowToAllocateBuffer();
                    Struct.setAllowToAllocateBuffer(false);
                    size = module.exports.getStructSize(new klass());
                    Struct.setAllowToAllocateBuffer(oldValue);
                    return size;
                } else if (klass instanceof SignatureWrapper) {
                    return bridjs.utils.getTypeSize(bridjs.dc.Pointer);
                } else {
                    return bridjs.utils.getTypeSize(klass);
                }
            },
            sizeof: function(klass) {
                return module.exports.getTypeSize(klass);
            },
            fill: function(struct, value) {
                var buffer = module.exports.byPointer(struct);

                if (buffer instanceof Buffer) {
                    buffer.fill(value);
                } else {
                    throw new BridjsException("Only support buffer pointer type: " + buffer);
                }
            },
            toString: function(buffer, encoding) {
                var end;
                if (buffer instanceof Struct) {
                    buffer = module.exports.getStructPointer(buffer);

                    if (buffer instanceof bridjs.dc.Pointer) {
                        return bridjs.utils.pointerToString(buffer);
                    }
                }

                for (end = 0; end < buffer.length; ++end) {
                    if (buffer[end] === 0) {
                        break;
                    }
                }

                return buffer.toString(encoding, 0, end);
            }
        }
    });

    module.exports.NativeValue = my.Class({
    });

    for (name in  bridjs.dc.Signature) {
        newName = name.replace("_TYPE", '').toLowerCase();
        value = module.exports.defineNativeValue(bridjs.dc.Signature[name]);
        defineReadOnlyProperty(module.exports.NativeValue, newName, value);
        defineReadOnlyProperty(module.exports.NativeValue, name.toUpperCase(), value);
        defineReadOnlyProperty(module.exports.NativeValue, newName.charAt(0).toUpperCase() + newName.substr(1), value);
    }

} else {
    throw new BridjsException("Fail to initalize bridjs from: " + libarryPath);
}

