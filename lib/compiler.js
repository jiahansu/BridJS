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
define([ "myclass","log4js", 'xregexp', "./native_lib", "./abstract_struct", "./pass_type_wrapper", "./bridjs_exception"], 
    function(my, log4js, xregexp, nativeLib, AbstractStruct, PassTypeWrapper, Exception){
        
    var Compiler, XRegExp = xregexp.XRegExp, functionExp,
    log = log4js.getLogger("BridJS"), parameterExp, pointerExp, byPointer, Signature = nativeLib.dc.Signature, returnTypeExp;
    //(?<returnType> \\w+)\\s+(\\w+\\s+){0,1}
    functionExp = XRegExp("(?<returnSignature>(\\w+\\**\\s+)+)(?<name>\\w*)(?<callbackName>\\(.*\\*\\w+\\))?\\s*\\((?<parameters>.*)\\)");
    parameterExp = XRegExp("\\s*(const)?\\s*(?<type>\\w+)(\\s+\\w+)*");
    returnTypeExp = XRegExp("\\s*(?<type>\\w+)\\**\\s*");
    pointerExp = new RegExp("\\*", "g");
    
            function byPointer(klass) {
                var type = typeof (klass);

                if (klass instanceof AbstractStruct) {
                    /*Struct's instance, return pointer immediately*/
                    return AbstractStruct.getStructPointer(klass);
                } else if (type === "function") {
                    /*Struct's class, return PassTypeWrapper*/
                    return new PassTypeWrapper(klass, nativeLib.dc.Signature.POINTER_TYPE);
                } else if (klass instanceof nativeLib.dc.Pointer) {
                    return Signature.pointer;
                } else if (type === "undefined") {
                    throw new GazeException("Undefined is not a valid pointer type");
                } else {
                    if(klass === Signature.char){
                        return Signature.string;
                    }else{
                       return nativeLib.dc.Signature.POINTER_TYPE; 
                    }
                }
            }
            
            function getType(name, srtuctTable) {
                var type = Signature[name];

                if ((type === undefined || type === null) && srtuctTable) {
                    type = srtuctTable[name];
                }
                
                if(type === undefined || type === null){
                    throw new Exception("Unknown parameter type: "+name);
                }
                
                return type;
            }
            function getReturnType(fullSignature, structTable){
                var typeStrs = fullSignature.split(' '), i, type, match;

                for (i in typeStrs) {
                    match = XRegExp.exec(typeStrs[i], parameterExp );
                    try{
                        type = getType(match.type, structTable);
                    }catch(e){
                        //do nothing
                    }
                    //log.info(match, type);
                    if(isValidType(type)){
                        break;
                    }
                }
                
                return type;
            }
            function pointerFilter(type, fullSignature) {
                var numStars = (fullSignature.match(pointerExp) || []).length, newType;
                
                if((type instanceof nativeLib.dc.Pointer) || 
                        (typeof(type.getAddress)==="function")){    
                    newType = (Signature.pointer);
                }else if (numStars <= 0) {
                    newType = (type);
                } else if (numStars === 1) {
                    newType = (byPointer(type));
                } else {
                    newType = (Signature.pointer);
                }
                //log.info(type, fullSignature, newType, (typeof(type.getAddress)==="function"));
                /*
                if(newType===Signature.char && numStars===1){
                    newType = Signature.string;
                }*/

                return newType;
            }
            function isValidType(type){
                var typeName = typeof(type);
                //log.info(type, typeName, typeof(type.isSignatureWrapper()));
                return (typeName ==="string") || (typeName === "function") 
                        || (typeName === "object")
                        || (type instanceof nativeLib.dc.Pointer) 
                        || (type instanceof PassTypeWrapper);
            }
    Compiler = my.Class({
        STATIC:{
            compileFunction:function(functionDesc, srtuctTable, returnInfo){
                var parts = XRegExp.exec(functionDesc,functionExp), parameterStrs, 
                        i, match, parameters = [], type, returnType;
                
                //log.info(parts);
                if(parts){
                    if(typeof(srtuctTable)!=="object"){
                        srtuctTable = {};
                    }
                    returnType = pointerFilter(getReturnType(parts.returnSignature, srtuctTable)
                    ,parts.returnSignature);
                    
                    if(isValidType(returnType)){
                        parameterStrs = parts.parameters.split(',');

                        for(i in parameterStrs){
                            match = XRegExp.exec(parameterStrs[i], parameterExp );
                            if(match){
                                type = getType(match.type, srtuctTable);
                                //log.info(match);
                                if(isValidType(type)){
                                    parameters.push(pointerFilter(type, parameterStrs[i]));
                                }else{
                                    throw new Exception("Unknown parameter type: "+match.type);
                                }
                            }
                        }
                    }else{
                       throw new Exception("Unrecognized function declaration : "+functionDesc); 
                    }

                    if(typeof(returnInfo) ==="object"){
                        returnInfo.name = parts.name;
                        returnInfo.returnType = returnType;
                    }
                }else{
                    throw new Exception("Unknown function pattern: "+functionDesc);
                }
                return parameters;
            },
            
            byPointer: function(klass) {
                return byPointer(klass);
            }
        }
    });
    
    return Compiler;
});

