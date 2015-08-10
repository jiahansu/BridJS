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
if (typeof define !== 'function') {
    var define = require('amdefine')(module);
}

// Functions which will be available to external callers
define([ "myclass","log4js", "./struct_field", "./native_lib", 'xregexp', "./utils", "./bridjs_exception"], 
    function(my, log4js, StructField, bridjs, xregexp, Utils, BridjsException){
        
    var AbstractStruct, log = log4js.getLogger("AbstractStruct"), structImplMap = {}, 
            XRegExp = xregexp.XRegExp, arrayExp, ArrayStruct, allowToAllocateBuffer = true;
        arrayExp = XRegExp("(?<type>\\w+)\\[(?<count>\\d)\\]");
        
        
    function setStructField(instance, structImpl, structField, order, bufferWrapper, subStruct) {

        if (subStruct) {
            Object.defineProperty(instance, structField.getName(), {
                get: function() {
                    return subStruct;
                },
                set: function(value) {
                    AbstractStruct.getStructPointer(value).copy(
                            AbstractStruct.getStructPointer(subStruct));
                }
            });
        } else {
            Object.defineProperty(instance, structField.getName(), {
                get: function() {
                    return structImpl.getField(order, bufferWrapper.buffer);
                },
                set: function(value) {
                    structImpl.setField(order, value, bufferWrapper.buffer);
                }
            });
        }
    };
    
    AbstractStruct = my.Class({
        STATIC: {
            implMap:structImplMap,
            
            setAllowToAllocateBuffer:function(b){
                allowToAllocateBuffer = b;
            },
            isAllowToAllocateBuffer:function(){
                return allowToAllocateBuffer;
            },
            getStructPointer: function(struct) {
                return struct._bridjs_impl_get_pointer();
            },
            defineStructArray: function(type, length) {
                Utils.checkType(type);

                if (typeof (length) === "number") {
                    var structClass = my.Class(ArrayStruct, {
                        constructor: function() {
                            structClass.Super.call(this, type, length);
                        }
                    });

                    return structClass;//module.exports.structField(structClass, order);
                } else {
                    throw new BridjsException("Illegal length argument: " + length);
                }
            },
            setArrayStructClass:function(klass){
                ArrayStruct = klass;
            }
        },
        constructor: function(pointer) {
            var name, field, array = [], types, signature, i,
                    type, subStruct, subStructs = {}, structImpl, buffer,
                    bufferWrapper = {}, oldValue, match;

            for (name in this) {
                field = this[name];
                //log.info(field);
                if (field instanceof StructField) {
                    field.setName(name);
                    array.push(field);
                }else if (field.type!==undefined){
                    
                    match = XRegExp.exec(field.type, arrayExp);
                    //log.info(field, match);
                    if (match && match.type && (match.count!==undefined) ) {
                        field = new StructField(AbstractStruct.defineStructArray( bridjs.dc.Signature[match.type],
                        parseInt(match.count)), field.order);
                        //log.info(field, match);
                    }else{
                        field = new StructField(field.type, 
                                            field.order);
                    }
                    field.setName(name);
                    array.push(field);
                }
            }
            array.sort(StructField.compare);
            types = new Array(array.length);
            signature = new Array(types.length);
            
            if (array.length > 0) {
                for (i = 0; i < array.length; ++i) {
                        type = array[i].getType();
                        
                        if (typeof (type) === "string") {
                            if(type.length===1){
                                types[i] = type;
                            }else{
                                types[i] = bridjs.dc.Signature[type]; 
                            }
                        } else {
                            /*We will assign buffer to sub struct later*/
                            oldValue = AbstractStruct.isAllowToAllocateBuffer();
                            //log.info("b ", oldValue);
                            AbstractStruct.setAllowToAllocateBuffer(false);

                            try {
                                subStruct = new type();
                            } catch (e) {
                                log.warn("Fail to create sub-struct field, name = ",array[i].getName(),", exception: ",e.stack);
                                /*Throw exception for stack trace*/
                                throw new BridjsException(e);
                            }

                            AbstractStruct.setAllowToAllocateBuffer(oldValue);
                            //log.info("a ",AbstractStruct.isAllowToAllocateBuffer());
                            types[i] = subStruct._bridjs_impl_get_struct_impl();
                            subStructs[i] = subStruct;
                        }
                    
                    /*Type will be a string or a bridjs.dyncall.Struct object*/
                    //log.info(types[i]);
                    signature.push(types[i].toString());
                }

                signature = signature.join('');
                //console.log(signature);
                /*Reuse Native Struct object*/
                structImpl = structImplMap[signature];
                if (typeof (structImpl) !== "object") {
                    var klass = this.getImplClass();
                    structImpl = new klass(types);
                    structImplMap[signature] = structImpl;
                }
                if (typeof (pointer) === "object") {
                    buffer = pointer;
                } else if (AbstractStruct.isAllowToAllocateBuffer()) {
                    buffer = new Buffer(structImpl.getSize());
                } else {
                    buffer = null;
                }
                //console.log(this._bridjs_impl_buffer);
                for (i = 0; i < array.length; ++i) {
                    field = array[i];
                    setStructField(this, structImpl, field, i, bufferWrapper, subStructs[i]);
                }
            }

            this._bridjs_impl_get_struct_impl = function() {
                return structImpl;
            };
            this._bridjs_impl_get_size = function() {
                return structImpl.getSize();
            };
            this._bridjs_impl_get_sub_struct = function() {
                return subStructs;
            };

            this._bridjs_impl_get_pointer = function() {
                return bufferWrapper.buffer;
            };
            this._bridjs_impl_update_buffer = function(buffer) {
                //if (typeof (bufferWrapper.buffer) !== "object" || true) {
                var subStructs = this._bridjs_impl_get_sub_struct(), index,
                        subStruct, structImpl = this._bridjs_impl_get_struct_impl(),
                        start, end;
                if (buffer) {
                    bufferWrapper.buffer = buffer;

                    for (index in subStructs) {
                        subStruct = subStructs[index];
                        start = structImpl.getFieldOffset(index);
                        end = start + subStruct._bridjs_impl_get_size();
                        //console.log(index + ", " + start+", "+end);
                        subStruct._bridjs_impl_update_buffer(bufferWrapper.buffer.slice(start, end));
                    }
                }
                /*
                 } else {
                 throw new BridJSException("Buffer was allocated: " + this._bridjs_impl_buffer);
                 }*/
            };
            
            this.getCloneBuffer = function(){
                return new Buffer(bufferWrapper.buffer);
            };
            //log.info(AbstractStruct.isAllowToAllocateBuffer(), types);
            if (AbstractStruct.isAllowToAllocateBuffer()) {
                this._bridjs_impl_update_buffer(buffer);
            }
            /*
             this.toString= function(){
             return structImpl.toString();
             };*/
        },
        
        getImplClass:function(){
            throw new BridjsException("Not implement");
        }
    });
    
    return AbstractStruct;
});


