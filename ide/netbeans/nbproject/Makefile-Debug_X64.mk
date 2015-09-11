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
CND_CONF=Debug_X64
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
	${OBJECTDIR}/_ext/1445274692/test.o \
	${OBJECTDIR}/_ext/1445274692/union_struct_v8.o


# C Compiler Flags
CFLAGS=-m64

# CC Compiler Flags
CCFLAGS=-m64
CXXFLAGS=-m64

# Fortran Compiler Flags
FFLAGS=-m64

# Assembler Flags
ASFLAGS=--64

# Link Libraries and Options
LDLIBSOPTIONS=../../deps/dyncall-0.8/linux_x64/libdyncall_s.a ../../deps/dyncall-0.8/linux_x64/libdyncallback_s.a ../../deps/dyncall-0.8/linux_x64/libdynload_s.a

# Build Targets
.build-conf: ${BUILD_SUBPROJECTS}
	"${MAKE}"  -f nbproject/Makefile-${CND_CONF}.mk ${CND_DISTDIR}/../../../build/Release/bridjs.node

${CND_DISTDIR}/../../../build/Release/bridjs.node: ../../deps/dyncall-0.8/linux_x64/libdyncall_s.a

${CND_DISTDIR}/../../../build/Release/bridjs.node: ../../deps/dyncall-0.8/linux_x64/libdyncallback_s.a

${CND_DISTDIR}/../../../build/Release/bridjs.node: ../../deps/dyncall-0.8/linux_x64/libdynload_s.a

${CND_DISTDIR}/../../../build/Release/bridjs.node: ${OBJECTFILES}
	${MKDIR} -p ${CND_DISTDIR}/../../../build/Release
	${LINK.cc} -o ${CND_DISTDIR}/../../../build/Release/bridjs.node ${OBJECTFILES} ${LDLIBSOPTIONS} -shared -fPIC

${OBJECTDIR}/_ext/1445274692/array_struct_v8.o: ../../src/array_struct_v8.cc 
	${MKDIR} -p ${OBJECTDIR}/_ext/1445274692
	${RM} "$@.d"
	$(COMPILE.cc) -g -Wall -I../../deps/dyncall-0.8/include -I/home/jiahan/.node-gyp/4.0.0/include/node -std=c++11 -fPIC  -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/_ext/1445274692/array_struct_v8.o ../../src/array_struct_v8.cc

${OBJECTDIR}/_ext/1445274692/bridjs_module.o: ../../src/bridjs_module.cc 
	${MKDIR} -p ${OBJECTDIR}/_ext/1445274692
	${RM} "$@.d"
	$(COMPILE.cc) -g -Wall -I../../deps/dyncall-0.8/include -I/home/jiahan/.node-gyp/4.0.0/include/node -std=c++11 -fPIC  -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/_ext/1445274692/bridjs_module.o ../../src/bridjs_module.cc

${OBJECTDIR}/_ext/1445274692/dyncall_v8.o: ../../src/dyncall_v8.cc 
	${MKDIR} -p ${OBJECTDIR}/_ext/1445274692
	${RM} "$@.d"
	$(COMPILE.cc) -g -Wall -I../../deps/dyncall-0.8/include -I/home/jiahan/.node-gyp/4.0.0/include/node -std=c++11 -fPIC  -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/_ext/1445274692/dyncall_v8.o ../../src/dyncall_v8.cc

${OBJECTDIR}/_ext/1445274692/dyncall_v8_utils.o: ../../src/dyncall_v8_utils.cc 
	${MKDIR} -p ${OBJECTDIR}/_ext/1445274692
	${RM} "$@.d"
	$(COMPILE.cc) -g -Wall -I../../deps/dyncall-0.8/include -I/home/jiahan/.node-gyp/4.0.0/include/node -std=c++11 -fPIC  -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/_ext/1445274692/dyncall_v8_utils.o ../../src/dyncall_v8_utils.cc

${OBJECTDIR}/_ext/1445274692/dyncallback_v8.o: ../../src/dyncallback_v8.cc 
	${MKDIR} -p ${OBJECTDIR}/_ext/1445274692
	${RM} "$@.d"
	$(COMPILE.cc) -g -Wall -I../../deps/dyncall-0.8/include -I/home/jiahan/.node-gyp/4.0.0/include/node -std=c++11 -fPIC  -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/_ext/1445274692/dyncallback_v8.o ../../src/dyncallback_v8.cc

${OBJECTDIR}/_ext/1445274692/dynload_v8.o: ../../src/dynload_v8.cc 
	${MKDIR} -p ${OBJECTDIR}/_ext/1445274692
	${RM} "$@.d"
	$(COMPILE.cc) -g -Wall -I../../deps/dyncall-0.8/include -I/home/jiahan/.node-gyp/4.0.0/include/node -std=c++11 -fPIC  -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/_ext/1445274692/dynload_v8.o ../../src/dynload_v8.cc

${OBJECTDIR}/_ext/1445274692/native_function_v8.o: ../../src/native_function_v8.cc 
	${MKDIR} -p ${OBJECTDIR}/_ext/1445274692
	${RM} "$@.d"
	$(COMPILE.cc) -g -Wall -I../../deps/dyncall-0.8/include -I/home/jiahan/.node-gyp/4.0.0/include/node -std=c++11 -fPIC  -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/_ext/1445274692/native_function_v8.o ../../src/native_function_v8.cc

${OBJECTDIR}/_ext/1445274692/pointer_v8.o: ../../src/pointer_v8.cc 
	${MKDIR} -p ${OBJECTDIR}/_ext/1445274692
	${RM} "$@.d"
	$(COMPILE.cc) -g -Wall -I../../deps/dyncall-0.8/include -I/home/jiahan/.node-gyp/4.0.0/include/node -std=c++11 -fPIC  -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/_ext/1445274692/pointer_v8.o ../../src/pointer_v8.cc

${OBJECTDIR}/_ext/1445274692/struct_v8.o: ../../src/struct_v8.cc 
	${MKDIR} -p ${OBJECTDIR}/_ext/1445274692
	${RM} "$@.d"
	$(COMPILE.cc) -g -Wall -I../../deps/dyncall-0.8/include -I/home/jiahan/.node-gyp/4.0.0/include/node -std=c++11 -fPIC  -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/_ext/1445274692/struct_v8.o ../../src/struct_v8.cc

${OBJECTDIR}/_ext/1445274692/test.o: ../../src/test.cc 
	${MKDIR} -p ${OBJECTDIR}/_ext/1445274692
	${RM} "$@.d"
	$(COMPILE.cc) -g -Wall -I../../deps/dyncall-0.8/include -I/home/jiahan/.node-gyp/4.0.0/include/node -std=c++11 -fPIC  -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/_ext/1445274692/test.o ../../src/test.cc

${OBJECTDIR}/_ext/1445274692/union_struct_v8.o: ../../src/union_struct_v8.cc 
	${MKDIR} -p ${OBJECTDIR}/_ext/1445274692
	${RM} "$@.d"
	$(COMPILE.cc) -g -Wall -I../../deps/dyncall-0.8/include -I/home/jiahan/.node-gyp/4.0.0/include/node -std=c++11 -fPIC  -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/_ext/1445274692/union_struct_v8.o ../../src/union_struct_v8.cc

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
