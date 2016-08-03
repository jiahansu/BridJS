#
# Generated Makefile - do not edit!
#
# Edit the Makefile in the project folder instead (../Makefile). Each target
# has a -pre and a -post target defined where you can add customized code.
#
# This makefile implements configuration specific macros and targets.


# Environment
MKDIR=mkdir
CP=cp
GREP=grep
NM=nm
CCADMIN=CCadmin
RANLIB=ranlib
CC=gcc
CCC=g++
CXX=g++
FC=gfortran
AS=as

# Macros
CND_PLATFORM=GNU-MacOSX
CND_DLIB_EXT=dylib
CND_CONF=Darwin_X64
CND_DISTDIR=dist
CND_BUILDDIR=build

# Include project Makefile
include Makefile

# Object Directory
OBJECTDIR=${CND_BUILDDIR}/${CND_CONF}/${CND_PLATFORM}

# Object Files
OBJECTFILES= \
	${OBJECTDIR}/_ext/56252444/array_struct_v8.o \
	${OBJECTDIR}/_ext/56252444/bridjs_module.o \
	${OBJECTDIR}/_ext/56252444/dyncall_v8.o \
	${OBJECTDIR}/_ext/56252444/dyncall_v8_utils.o \
	${OBJECTDIR}/_ext/56252444/dyncallback_v8.o \
	${OBJECTDIR}/_ext/56252444/dynload_v8.o \
	${OBJECTDIR}/_ext/56252444/native_function_v8.o \
	${OBJECTDIR}/_ext/56252444/pointer_v8.o \
	${OBJECTDIR}/_ext/56252444/struct_v8.o \
	${OBJECTDIR}/_ext/56252444/test.o \
	${OBJECTDIR}/_ext/56252444/union_struct_v8.o


# C Compiler Flags
CFLAGS=-m64

# CC Compiler Flags
CCFLAGS=-m64
CXXFLAGS=-m64

# Fortran Compiler Flags
FFLAGS=-m64

# Assembler Flags
ASFLAGS=-arch x86_64

# Link Libraries and Options
LDLIBSOPTIONS=../../deps/dyncall-0.8/darwin/libdyncall_s.a ../../deps/dyncall-0.8/darwin/libdyncallback_s.a ../../deps/dyncall-0.8/darwin/libdynload_s.a

# Build Targets
.build-conf: ${BUILD_SUBPROJECTS}
	"${MAKE}"  -f nbproject/Makefile-${CND_CONF}.mk ${CND_DISTDIR}/../../../build/Release/bridjs.node

${CND_DISTDIR}/../../../build/Release/bridjs.node: ../../deps/dyncall-0.8/darwin/libdyncall_s.a

${CND_DISTDIR}/../../../build/Release/bridjs.node: ../../deps/dyncall-0.8/darwin/libdyncallback_s.a

${CND_DISTDIR}/../../../build/Release/bridjs.node: ../../deps/dyncall-0.8/darwin/libdynload_s.a

${CND_DISTDIR}/../../../build/Release/bridjs.node: ${OBJECTFILES}
	${MKDIR} -p ${CND_DISTDIR}/../../../build/Release
	${LINK.cc} -o ${CND_DISTDIR}/../../../build/Release/bridjs.node ${OBJECTFILES} ${LDLIBSOPTIONS} -undefined dynamic_lookup -dynamiclib -install_name bridjs.node -fPIC

${OBJECTDIR}/_ext/56252444/array_struct_v8.o: ../../src/array_struct_v8.cc 
	${MKDIR} -p ${OBJECTDIR}/_ext/56252444
	${RM} "$@.d"
	$(COMPILE.cc) -g -Wall -I../../deps/dyncall-0.8/include -I/usr/include/nodejs/deps/v8/include -I/usr/include/nodejs/src -I/usr/include/nodejs/deps/uv/include -I/usr/include/nodejs -I../../../../../.node-gyp/6.2.0/include/node -I../../node_modules/nan -std=c++11 -fPIC  -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/_ext/56252444/array_struct_v8.o ../../src/array_struct_v8.cc

${OBJECTDIR}/_ext/56252444/bridjs_module.o: ../../src/bridjs_module.cc 
	${MKDIR} -p ${OBJECTDIR}/_ext/56252444
	${RM} "$@.d"
	$(COMPILE.cc) -g -Wall -I../../deps/dyncall-0.8/include -I/usr/include/nodejs/deps/v8/include -I/usr/include/nodejs/src -I/usr/include/nodejs/deps/uv/include -I/usr/include/nodejs -I../../../../../.node-gyp/6.2.0/include/node -I../../node_modules/nan -std=c++11 -fPIC  -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/_ext/56252444/bridjs_module.o ../../src/bridjs_module.cc

${OBJECTDIR}/_ext/56252444/dyncall_v8.o: ../../src/dyncall_v8.cc 
	${MKDIR} -p ${OBJECTDIR}/_ext/56252444
	${RM} "$@.d"
	$(COMPILE.cc) -g -Wall -I../../deps/dyncall-0.8/include -I/usr/include/nodejs/deps/v8/include -I/usr/include/nodejs/src -I/usr/include/nodejs/deps/uv/include -I/usr/include/nodejs -I../../../../../.node-gyp/6.2.0/include/node -I../../node_modules/nan -std=c++11 -fPIC  -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/_ext/56252444/dyncall_v8.o ../../src/dyncall_v8.cc

${OBJECTDIR}/_ext/56252444/dyncall_v8_utils.o: ../../src/dyncall_v8_utils.cc 
	${MKDIR} -p ${OBJECTDIR}/_ext/56252444
	${RM} "$@.d"
	$(COMPILE.cc) -g -Wall -I../../deps/dyncall-0.8/include -I/usr/include/nodejs/deps/v8/include -I/usr/include/nodejs/src -I/usr/include/nodejs/deps/uv/include -I/usr/include/nodejs -I../../../../../.node-gyp/6.2.0/include/node -I../../node_modules/nan -std=c++11 -fPIC  -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/_ext/56252444/dyncall_v8_utils.o ../../src/dyncall_v8_utils.cc

${OBJECTDIR}/_ext/56252444/dyncallback_v8.o: ../../src/dyncallback_v8.cc 
	${MKDIR} -p ${OBJECTDIR}/_ext/56252444
	${RM} "$@.d"
	$(COMPILE.cc) -g -Wall -I../../deps/dyncall-0.8/include -I/usr/include/nodejs/deps/v8/include -I/usr/include/nodejs/src -I/usr/include/nodejs/deps/uv/include -I/usr/include/nodejs -I../../../../../.node-gyp/6.2.0/include/node -I../../node_modules/nan -std=c++11 -fPIC  -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/_ext/56252444/dyncallback_v8.o ../../src/dyncallback_v8.cc

${OBJECTDIR}/_ext/56252444/dynload_v8.o: ../../src/dynload_v8.cc 
	${MKDIR} -p ${OBJECTDIR}/_ext/56252444
	${RM} "$@.d"
	$(COMPILE.cc) -g -Wall -I../../deps/dyncall-0.8/include -I/usr/include/nodejs/deps/v8/include -I/usr/include/nodejs/src -I/usr/include/nodejs/deps/uv/include -I/usr/include/nodejs -I../../../../../.node-gyp/6.2.0/include/node -I../../node_modules/nan -std=c++11 -fPIC  -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/_ext/56252444/dynload_v8.o ../../src/dynload_v8.cc

${OBJECTDIR}/_ext/56252444/native_function_v8.o: ../../src/native_function_v8.cc 
	${MKDIR} -p ${OBJECTDIR}/_ext/56252444
	${RM} "$@.d"
	$(COMPILE.cc) -g -Wall -I../../deps/dyncall-0.8/include -I/usr/include/nodejs/deps/v8/include -I/usr/include/nodejs/src -I/usr/include/nodejs/deps/uv/include -I/usr/include/nodejs -I../../../../../.node-gyp/6.2.0/include/node -I../../node_modules/nan -std=c++11 -fPIC  -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/_ext/56252444/native_function_v8.o ../../src/native_function_v8.cc

${OBJECTDIR}/_ext/56252444/pointer_v8.o: ../../src/pointer_v8.cc 
	${MKDIR} -p ${OBJECTDIR}/_ext/56252444
	${RM} "$@.d"
	$(COMPILE.cc) -g -Wall -I../../deps/dyncall-0.8/include -I/usr/include/nodejs/deps/v8/include -I/usr/include/nodejs/src -I/usr/include/nodejs/deps/uv/include -I/usr/include/nodejs -I../../../../../.node-gyp/6.2.0/include/node -I../../node_modules/nan -std=c++11 -fPIC  -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/_ext/56252444/pointer_v8.o ../../src/pointer_v8.cc

${OBJECTDIR}/_ext/56252444/struct_v8.o: ../../src/struct_v8.cc 
	${MKDIR} -p ${OBJECTDIR}/_ext/56252444
	${RM} "$@.d"
	$(COMPILE.cc) -g -Wall -I../../deps/dyncall-0.8/include -I/usr/include/nodejs/deps/v8/include -I/usr/include/nodejs/src -I/usr/include/nodejs/deps/uv/include -I/usr/include/nodejs -I../../../../../.node-gyp/6.2.0/include/node -I../../node_modules/nan -std=c++11 -fPIC  -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/_ext/56252444/struct_v8.o ../../src/struct_v8.cc

${OBJECTDIR}/_ext/56252444/test.o: ../../src/test.cc 
	${MKDIR} -p ${OBJECTDIR}/_ext/56252444
	${RM} "$@.d"
	$(COMPILE.cc) -g -Wall -I../../deps/dyncall-0.8/include -I/usr/include/nodejs/deps/v8/include -I/usr/include/nodejs/src -I/usr/include/nodejs/deps/uv/include -I/usr/include/nodejs -I../../../../../.node-gyp/6.2.0/include/node -I../../node_modules/nan -std=c++11 -fPIC  -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/_ext/56252444/test.o ../../src/test.cc

${OBJECTDIR}/_ext/56252444/union_struct_v8.o: ../../src/union_struct_v8.cc 
	${MKDIR} -p ${OBJECTDIR}/_ext/56252444
	${RM} "$@.d"
	$(COMPILE.cc) -g -Wall -I../../deps/dyncall-0.8/include -I/usr/include/nodejs/deps/v8/include -I/usr/include/nodejs/src -I/usr/include/nodejs/deps/uv/include -I/usr/include/nodejs -I../../../../../.node-gyp/6.2.0/include/node -I../../node_modules/nan -std=c++11 -fPIC  -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/_ext/56252444/union_struct_v8.o ../../src/union_struct_v8.cc

# Subprojects
.build-subprojects:

# Clean Targets
.clean-conf: ${CLEAN_SUBPROJECTS}
	${RM} -r ${CND_BUILDDIR}/${CND_CONF}
	${RM} ${CND_DISTDIR}/../../../build/Release/bridjs.node

# Subprojects
.clean-subprojects:

# Enable dependency checking
.dep.inc: .depcheck-impl

include .dep.inc
