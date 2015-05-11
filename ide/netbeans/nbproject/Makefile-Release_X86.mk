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
CND_PLATFORM=GNU-Linux-x86
CND_DLIB_EXT=so
CND_CONF=Release_X86
CND_DISTDIR=dist
CND_BUILDDIR=build

# Include project Makefile
include Makefile

# Object Directory
OBJECTDIR=${CND_BUILDDIR}/${CND_CONF}/${CND_PLATFORM}

# Object Files
OBJECTFILES= \
	${OBJECTDIR}/_ext/1445274692/array_struct_v8.o \
	${OBJECTDIR}/_ext/1445274692/bridjs_module.o \
	${OBJECTDIR}/_ext/1445274692/dyncall_v8.o \
	${OBJECTDIR}/_ext/1445274692/dyncall_v8_utils.o \
	${OBJECTDIR}/_ext/1445274692/dyncallback_v8.o \
	${OBJECTDIR}/_ext/1445274692/dynload_v8.o \
	${OBJECTDIR}/_ext/1445274692/native_function_v8.o \
	${OBJECTDIR}/_ext/1445274692/pointer_v8.o \
	${OBJECTDIR}/_ext/1445274692/struct_v8.o \
	${OBJECTDIR}/_ext/1445274692/test.o


# C Compiler Flags
CFLAGS=-m32

# CC Compiler Flags
CCFLAGS=-m32
CXXFLAGS=-m32

# Fortran Compiler Flags
FFLAGS=-s -m32

# Assembler Flags
ASFLAGS=--32

# Link Libraries and Options
LDLIBSOPTIONS=../../deps/dyncall-0.8/linux_ia32/libdyncall_s.a ../../deps/dyncall-0.8/linux_ia32/libdyncallback_s.a ../../deps/dyncall-0.8/linux_ia32/libdynload_s.a

# Build Targets
.build-conf: ${BUILD_SUBPROJECTS}
	"${MAKE}"  -f nbproject/Makefile-${CND_CONF}.mk ${CND_DISTDIR}/../../../linux_ia32/bridjs.node

${CND_DISTDIR}/../../../linux_ia32/bridjs.node: ../../deps/dyncall-0.8/linux_ia32/libdyncall_s.a

${CND_DISTDIR}/../../../linux_ia32/bridjs.node: ../../deps/dyncall-0.8/linux_ia32/libdyncallback_s.a

${CND_DISTDIR}/../../../linux_ia32/bridjs.node: ../../deps/dyncall-0.8/linux_ia32/libdynload_s.a

${CND_DISTDIR}/../../../linux_ia32/bridjs.node: ${OBJECTFILES}
	${MKDIR} -p ${CND_DISTDIR}/../../../linux_ia32
	${LINK.cc} -o ${CND_DISTDIR}/../../../linux_ia32/bridjs.node ${OBJECTFILES} ${LDLIBSOPTIONS} -shared -s -fPIC

${OBJECTDIR}/_ext/1445274692/array_struct_v8.o: ../../src/array_struct_v8.cc 
	${MKDIR} -p ${OBJECTDIR}/_ext/1445274692
	${RM} "$@.d"
	$(COMPILE.cc) -O3 -Wall -s -I../../deps/dyncall-0.8/include -I/usr/include/nodejs/deps/v8/include -I/usr/include/nodejs/src -I/usr/include/nodejs/deps/uv/include -I/usr/include/nodejs -I/home/jiahan/.node-gyp/0.10.26/deps/v8/include -I/home/jiahan/.node-gyp/0.10.26/src -I/home/jiahan/.node-gyp/0.10.26/deps/uv/include -std=c++11 -fPIC  -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/_ext/1445274692/array_struct_v8.o ../../src/array_struct_v8.cc

${OBJECTDIR}/_ext/1445274692/bridjs_module.o: ../../src/bridjs_module.cc 
	${MKDIR} -p ${OBJECTDIR}/_ext/1445274692
	${RM} "$@.d"
	$(COMPILE.cc) -O3 -Wall -s -I../../deps/dyncall-0.8/include -I/usr/include/nodejs/deps/v8/include -I/usr/include/nodejs/src -I/usr/include/nodejs/deps/uv/include -I/usr/include/nodejs -I/home/jiahan/.node-gyp/0.10.26/deps/v8/include -I/home/jiahan/.node-gyp/0.10.26/src -I/home/jiahan/.node-gyp/0.10.26/deps/uv/include -std=c++11 -fPIC  -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/_ext/1445274692/bridjs_module.o ../../src/bridjs_module.cc

${OBJECTDIR}/_ext/1445274692/dyncall_v8.o: ../../src/dyncall_v8.cc 
	${MKDIR} -p ${OBJECTDIR}/_ext/1445274692
	${RM} "$@.d"
	$(COMPILE.cc) -O3 -Wall -s -I../../deps/dyncall-0.8/include -I/usr/include/nodejs/deps/v8/include -I/usr/include/nodejs/src -I/usr/include/nodejs/deps/uv/include -I/usr/include/nodejs -I/home/jiahan/.node-gyp/0.10.26/deps/v8/include -I/home/jiahan/.node-gyp/0.10.26/src -I/home/jiahan/.node-gyp/0.10.26/deps/uv/include -std=c++11 -fPIC  -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/_ext/1445274692/dyncall_v8.o ../../src/dyncall_v8.cc

${OBJECTDIR}/_ext/1445274692/dyncall_v8_utils.o: ../../src/dyncall_v8_utils.cc 
	${MKDIR} -p ${OBJECTDIR}/_ext/1445274692
	${RM} "$@.d"
	$(COMPILE.cc) -O3 -Wall -s -I../../deps/dyncall-0.8/include -I/usr/include/nodejs/deps/v8/include -I/usr/include/nodejs/src -I/usr/include/nodejs/deps/uv/include -I/usr/include/nodejs -I/home/jiahan/.node-gyp/0.10.26/deps/v8/include -I/home/jiahan/.node-gyp/0.10.26/src -I/home/jiahan/.node-gyp/0.10.26/deps/uv/include -std=c++11 -fPIC  -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/_ext/1445274692/dyncall_v8_utils.o ../../src/dyncall_v8_utils.cc

${OBJECTDIR}/_ext/1445274692/dyncallback_v8.o: ../../src/dyncallback_v8.cc 
	${MKDIR} -p ${OBJECTDIR}/_ext/1445274692
	${RM} "$@.d"
	$(COMPILE.cc) -O3 -Wall -s -I../../deps/dyncall-0.8/include -I/usr/include/nodejs/deps/v8/include -I/usr/include/nodejs/src -I/usr/include/nodejs/deps/uv/include -I/usr/include/nodejs -I/home/jiahan/.node-gyp/0.10.26/deps/v8/include -I/home/jiahan/.node-gyp/0.10.26/src -I/home/jiahan/.node-gyp/0.10.26/deps/uv/include -std=c++11 -fPIC  -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/_ext/1445274692/dyncallback_v8.o ../../src/dyncallback_v8.cc

${OBJECTDIR}/_ext/1445274692/dynload_v8.o: ../../src/dynload_v8.cc 
	${MKDIR} -p ${OBJECTDIR}/_ext/1445274692
	${RM} "$@.d"
	$(COMPILE.cc) -O3 -Wall -s -I../../deps/dyncall-0.8/include -I/usr/include/nodejs/deps/v8/include -I/usr/include/nodejs/src -I/usr/include/nodejs/deps/uv/include -I/usr/include/nodejs -I/home/jiahan/.node-gyp/0.10.26/deps/v8/include -I/home/jiahan/.node-gyp/0.10.26/src -I/home/jiahan/.node-gyp/0.10.26/deps/uv/include -std=c++11 -fPIC  -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/_ext/1445274692/dynload_v8.o ../../src/dynload_v8.cc

${OBJECTDIR}/_ext/1445274692/native_function_v8.o: ../../src/native_function_v8.cc 
	${MKDIR} -p ${OBJECTDIR}/_ext/1445274692
	${RM} "$@.d"
	$(COMPILE.cc) -O3 -Wall -s -I../../deps/dyncall-0.8/include -I/usr/include/nodejs/deps/v8/include -I/usr/include/nodejs/src -I/usr/include/nodejs/deps/uv/include -I/usr/include/nodejs -I/home/jiahan/.node-gyp/0.10.26/deps/v8/include -I/home/jiahan/.node-gyp/0.10.26/src -I/home/jiahan/.node-gyp/0.10.26/deps/uv/include -std=c++11 -fPIC  -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/_ext/1445274692/native_function_v8.o ../../src/native_function_v8.cc

${OBJECTDIR}/_ext/1445274692/pointer_v8.o: ../../src/pointer_v8.cc 
	${MKDIR} -p ${OBJECTDIR}/_ext/1445274692
	${RM} "$@.d"
	$(COMPILE.cc) -O3 -Wall -s -I../../deps/dyncall-0.8/include -I/usr/include/nodejs/deps/v8/include -I/usr/include/nodejs/src -I/usr/include/nodejs/deps/uv/include -I/usr/include/nodejs -I/home/jiahan/.node-gyp/0.10.26/deps/v8/include -I/home/jiahan/.node-gyp/0.10.26/src -I/home/jiahan/.node-gyp/0.10.26/deps/uv/include -std=c++11 -fPIC  -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/_ext/1445274692/pointer_v8.o ../../src/pointer_v8.cc

${OBJECTDIR}/_ext/1445274692/struct_v8.o: ../../src/struct_v8.cc 
	${MKDIR} -p ${OBJECTDIR}/_ext/1445274692
	${RM} "$@.d"
	$(COMPILE.cc) -O3 -Wall -s -I../../deps/dyncall-0.8/include -I/usr/include/nodejs/deps/v8/include -I/usr/include/nodejs/src -I/usr/include/nodejs/deps/uv/include -I/usr/include/nodejs -I/home/jiahan/.node-gyp/0.10.26/deps/v8/include -I/home/jiahan/.node-gyp/0.10.26/src -I/home/jiahan/.node-gyp/0.10.26/deps/uv/include -std=c++11 -fPIC  -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/_ext/1445274692/struct_v8.o ../../src/struct_v8.cc

${OBJECTDIR}/_ext/1445274692/test.o: ../../src/test.cc 
	${MKDIR} -p ${OBJECTDIR}/_ext/1445274692
	${RM} "$@.d"
	$(COMPILE.cc) -O3 -Wall -s -I../../deps/dyncall-0.8/include -I/usr/include/nodejs/deps/v8/include -I/usr/include/nodejs/src -I/usr/include/nodejs/deps/uv/include -I/usr/include/nodejs -I/home/jiahan/.node-gyp/0.10.26/deps/v8/include -I/home/jiahan/.node-gyp/0.10.26/src -I/home/jiahan/.node-gyp/0.10.26/deps/uv/include -std=c++11 -fPIC  -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/_ext/1445274692/test.o ../../src/test.cc

# Subprojects
.build-subprojects:

# Clean Targets
.clean-conf: ${CLEAN_SUBPROJECTS}
	${RM} -r ${CND_BUILDDIR}/${CND_CONF}
	${RM} ${CND_DISTDIR}/../../../linux_ia32/bridjs.node

# Subprojects
.clean-subprojects:

# Enable dependency checking
.dep.inc: .depcheck-impl

include .dep.inc
