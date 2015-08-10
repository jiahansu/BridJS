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
 function defineReadOnlyProperty(object, newName, value) {
        Object.defineProperty(object, newName, {
            get: function() {
                return value;
            }
        });
    }

// Functions which will be available to external callers
define([ "myclass","log4js", "./native_lib",  "./struct", "./bridjs_exception"], 
    function(my, log4js, bridjs, Struct, BridjsException){
    var log = log4js.getLogger("ArrayStruct");    
    var ArrayStruct = my.Class(Struct, {
        constructor: function(type, length, pointer) {
            var signature, structImpl, bufferWrapper = {}, buffer;

            ArrayStruct.Super.call(this);
            //console.log(length);
            signature = [type, '(' + length + ')'].join('');
            //console.log(signature);
            /*Reuse Native Struct object*/
            structImpl = Struct.implMap[signature];
            if (typeof (structImpl) !== "object") {
                structImpl = new bridjs.dc.ArrayStruct(type, length);
                Struct.implMap[signature] = structImpl;
            }
            if (typeof (pointer) === "object") {
                buffer = pointer;
            } else if (Struct.isAllowToAllocateBuffer()) {
                buffer = new Buffer(structImpl.getSize());
            } else {
                buffer = null;
            }
            defineReadOnlyProperty(this, "length", length);
            this._bridjs_impl_get_struct_impl = function() {
                return structImpl;
            };
            this._bridjs_impl_get_size = function() {
                return structImpl.getSize();
            };
            this._bridjs_impl_get_pointer = function() {
                return bufferWrapper.buffer;
            };
            this._bridjs_impl_update_buffer = function(buffer) {
                if (typeof (bufferWrapper.buffer) !== "object") {
                    bufferWrapper.buffer = buffer;
                } else {
                    throw new BridjsException("Buffer was allocated: " + this._bridjs_impl_buffer);
                }
            };

            if (Struct.isAllowToAllocateBuffer()) {
                this._bridjs_impl_update_buffer(buffer);
            }
        },
        get: function(index) {
            var buffer = ArrayStruct.getStructPointer(this);

            return this._bridjs_impl_get_struct_impl().getField(index, buffer);
        },
        set: function(index, value) {
            var buffer = ArrayStruct.getStructPointer(this);

            return this._bridjs_impl_get_struct_impl().setField(index, value, buffer);
        }
    });
    
    Struct.setArrayStructClass(ArrayStruct);
    
    return ArrayStruct;
});


