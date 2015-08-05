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
define([ "myclass","log4js", "./compiler", "./native_lib", "./utils"], 
    function(my, log4js, Compiler, bridjs, Utils){
        
     var SignatureWrapper = my.Class(bridjs.dc.Signature, {
        constructor: function(args) {
            var returnType, argumentTypes, i, functionName, instanceCache = false;
            if (args.length >= 1) {
                
                if(args.length<=2 && typeof(args[0])==="string" && args[0].length>=5 && 
                       (args.length<=1 || typeof(args[1])==="object" ) ){
                    var returnInfo = {};
                    
                    argumentTypes = Compiler.compileFunction(args[0], args[1],returnInfo);
                    returnType = returnInfo.returnType;
                    functionName = returnInfo.name;
                }else{
                    returnType = Utils.checkType(args[0]);
                    argumentTypes = new Array(args.length - 1);

                    for (i = 1; i < args.length; ++i) {
                        argumentTypes[i - 1] = Utils.checkType(args[i]);
                    }
                }
                
                /*No arguments case*/
                if(argumentTypes[0]===bridjs.dc.Signature.void){
                    argumentTypes = [];
                }
                
                this.getReturnType = function() {
                    return returnType;
                };
                this.getArgumentLength = function() {
                    return argumentTypes.length;
                };
                this.getArgumentType = function(index) {
                    if (index >= 0 && index < argumentTypes.length) {
                        return argumentTypes[index];
                    } else {
                        throw new BridjsException("OutOfBoundary, length = " + argumentTypes.length + ", index = " + index);
                    }
                };
                this.bind = function(name) {
                    functionName = name;

                    return this;
                };
                this.getBindName = function() {
                    return functionName;
                };
                this.cacheInstance = function(cache) {
                    instanceCache = cache;

                    return this;
                };
                this.hasCacheInstance = function() {
                    return instanceCache;
                };
            } else {
                throw new BridjsException("Illegal agguments to describe a valid function signature");
            }
        },
        
        isSignatureWrapper:function(){
            return true;
        }
    });
    //log4js.getLogger("ggyy").info(SignatureWrapper);
    return SignatureWrapper;
});