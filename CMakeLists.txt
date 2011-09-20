PROJECT(GIJZELIJZER)
CMAKE_MINIMUM_REQUIRED(VERSION 2.6)
SET(CMAKE_COLOR_MAKEFILE ON)


IF (MSVC)
    ADD_DEFINITIONS(/D _CRT_SECURE_NO_WARNINGS)
ENDIF()

#-------------------------------------------------------------------------------
# Find 3rd party libs
#-------------------------------------------------------------------------------
IF(WIN32)
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

    SET(GTEST_PATH ${GIJZELIJZER_SOURCE_DIR}/../gtest-1.6.0 CACHE PATH "Path to the Google Test Library")
    SET(GTEST_INCLUDE_DIR ${GTEST_PATH}/include)
    SET(GTEST_INCLUDE_DIRS ${GTEST_INCLUDE_DIR})
    SET(GTEST_LIB_DIR ${GTEST_PATH}/lib)
    SET(GTEST_BIN_DIR ${GTEST_PATH}/bin)
    SET(GTEST_LIBRARY gtest)
    SET(GTEST_LIBRARIES ${GTEST_LIBRARY})
    SET(GTEST_MAIN_LIBRARY gtest)
    SET(GTEST_MAIN_LIBRARIES ${GTEST_MAIN_LIBRARY})
    SET(GTEST_BOTH_LIBRARIES ${GTEST_LIBRARIES} ${GTEST_MAIN_LIBRARIES})
ENDIF()

#-------------------------------------------------------------------------------
# Set what to compile how
#-------------------------------------------------------------------------------
SET(GIJZELIJZER_SOURCE
    src/vstxsynth.cpp
    src/vstxsynthproc.cpp
    resources/vstxsynth.rc
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

#-------------------------------------------------------------------------------
# Testing
#-------------------------------------------------------------------------------
SET(ENABLE_TESTS FALSE CACHE BOOL "Enable to make the tests")

IF (ENABLE_TESTS)
    LINK_DIRECTORIES(${GTEST_LIB_DIR})
    find_package(GTest REQUIRED)
    ENABLE_TESTING()
    INCLUDE_DIRECTORIES(${GTEST_INCLUDE_DIRS})
    ADD_EXECUTABLE(gijzelijzertest
        #${VSTSDK_PLUGIN_SOURCE}
        test/src/vstapi.cpp
        test/src/minieditor.cpp
    )
    TARGET_LINK_LIBRARIES(gijzelijzertest ${GTEST_BOTH_LIBRARIES})
    ADD_TEST(AllTestsGijzelijzer gijzelijzertest)
ENDIF ()
