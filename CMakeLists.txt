PROJECT(GIJZELIJZER)
CMAKE_MINIMUM_REQUIRED(VERSION 2.6)
SET(CMAKE_COLOR_MAKEFILE ON)

IF (MSVC)
    ADD_DEFINITIONS(/D _CRT_SECURE_NO_WARNINGS)
ENDIF()

#-------------------------------------------------------------------------------
# Find 3rd party libs
#-------------------------------------------------------------------------------
SET(VSTSDK_PATH ${GIJZELIJZER_SOURCE_DIR}/../vstsdk2.4 CACHE PATH "Path to the Steinberg VST SDK")
SET(VSTSDK_INCLUDE_DIR ${VSTSDK_PATH})
SET(VSTSDK_PLUGIN_SOURCE
    ${VSTSDK_PATH}/public.sdk/source/vst2.x/audioeffectx.cpp
    ${VSTSDK_PATH}/public.sdk/source/vst2.x/audioeffect.cpp
    ${VSTSDK_PATH}/public.sdk/source/vst2.x/vstplugmain.cpp
)

SET(VSTSDK_HOST_SOURCE
    ${VSTSDK_PATH}/pluginterfaces/vst2.x/aeffectx.cpp
)

#-------------------------------------------------------------------------------
# Set what to compile how
#-------------------------------------------------------------------------------
SET(GIJZELIJZER_SOURCE
    src/gijzelijzer.cpp
    src/gijzelijzer.h
    src/gijzelijzermain.cpp
    resources/gijzelijzer.rc
    resources/vstplug.def
    ${VSTSDK_PLUGIN_SOURCE}
)

ADD_LIBRARY(gijzelijzer SHARED ${GIJZELIJZER_SOURCE})

#-------------------------------------------------------------------------------
# Include 3rd party library headers
#-------------------------------------------------------------------------------
INCLUDE_DIRECTORIES(
    ${VSTSDK_INCLUDE_DIR}
)

#-------------------------------------------------------------------------------
# Link libraries
#-------------------------------------------------------------------------------
TARGET_LINK_LIBRARIES(gijzelijzer
    ${VSTSDK_LIBRARIES}
)
