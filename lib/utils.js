/*
 * BridJS - Dynamic and blazing-fast native interop for JavaScript.
 * https://github.com/jiahansu/BridJS
 *
 * Copyright (c) 2013-2013, Olivier Chafik (http://ochafik.com/) and Jia-Han Su (https://github.com/jiahansu/BridJS)
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
var my = require('myclass'),
        log4js = require("log4js"), log = log4js.getLogger("Utils"),
        BridjsException = require("./bridjs_exception"),
        PassTypeWrapper = require("./pass_type_wrapper");

var Utils = my.Class({
    STATIC: {
        DEFAULT_ENCODING: 'UTF-8',
        DEBUG_MODE: true,
        NANO_TO_SECONDS: 1.0 / (1000 * 1000 * 1000),
        timeSeconds: function() {
            var time = process.hrtime();

            return time[0] + (time[1] * Utils.NANO_TO_SECONDS);
        },
        fill: function(object, value) {
            var key;

            for (key in object) {
                object[key] = value;
            }
        },
        clone: function(obj) {
            var key, newObj = {};

            for (key in obj) {
                newObj[key] = obj[key];
            }

            return newObj;
        },
        checkType: function(type) {
            var typeName = typeof (type);
            if ((typeName === "string") || typeName === "object" || typeName === "function") {
                return type;
            } else {
                throw new BridjsException("Illegal type value: " + type);
            }
        },
        createTypeWrap: function(klass, instanceCache) {
            var instance = null;
            
            if(instanceCache){
                return function(v) {
                    /*Reuse instance*/
                    if (instance) {
                        instance._bridjs_impl_update_buffer(v);
                    } else {
                        instance = new klass(v);
                    }
                    return instance;
                };
            }else{
                return function(v){
                   return new klass(v);
                };
            }
        }
    }
});

module.exports = Utils;