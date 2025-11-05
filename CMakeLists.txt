cmake_minimum_required(VERSION 3.15)

# package
set(BEHH_VERSION_MAJOR 0)
set(BEHH_VERSION_MINOR 19)
set(BEHH_VERSION_PATCH 0)
set(BEHH_VERSION_RELEASE_DATE 20251105)

# package project
project(behh
    VERSION "${BEHH_VERSION_MAJOR}.${BEHH_VERSION_MINOR}.${BEHH_VERSION_PATCH}"
    DESCRIPTION "module library treatment for c++ backend project"
    HOMEPAGE_URL "https://github.com/prothegee/behh"
    LANGUAGES CXX
)

# option/s
## build tests, default false/OFF
option(BEHH_BUILD_TESTS                         "build project test/s" OFF)
## build tools, default false/OFF
option(BEHH_BUILD_TOOLS                         "build project tool/s" OFF)
## build vendor stb, default false/OFF
option(BEHH_BUILD_VENDOR_STB                    "build vendor stb" OFF)
## build vendor sdl, default false/OFF
option(BEHH_BUILD_VENDOR_SDL3                   "build vendor sdl3" OFF)
## build vendor haru, default false/OFF
option(BEHH_BUILD_VENDOR_HARU                   "build vendor haru" OFF)
## build vendor nanosvg, default false/OFF
option(BEHH_BUILD_VENDOR_NANOSVG                "build vendor nanosvg" OFF)
## build vendor zxing-cpp, default false/OFF
option(BEHH_BUILD_VENDOR_ZXING_CPP              "build vendor zxing-cpp" OFF)
## build vendor couchbase-cxx-client, default false/OFF
option(BEHH_BUILD_VENDOR_COUCHBASE_CXX_CLIENT   "build vendor couchbase-client-cxx" OFF)
### build use vcpkg, default false/OFF
option(BEHH_BUILD_VCPKG                         "build use vcpkg" OFF)

## rules of options
### build vcpkg_triplet, default false/OFF, should be string value
set(BEHH_BUILD_VCPKG_TRIPLET)

## build tests as bool
if(BEHH_BUILD_TESTS)
    set(BEHH_BUILD_TESTS true)
else()
    set(BEHH_BUILD_TESTS false)
endif()

## build tools as bool
if(BEHH_BUILD_TOOLS)
    set(BEHH_BUILD_TOOLS true)
else()
    set(BEHH_BUILD_TOOLS false)
endif()

## build vcpkg as bool
if(BEHH_BUILD_VCPKG)
    set(BEHH_BUILD_VCPKG true)
else()
    set(BEHH_BUILD_VCPKG false)
endif()

## build type
if(NOT CMAKE_BEHH_BUILD_TYPE)
    set(CMAKE_BEHH_BUILD_TYPE "Debug")
endif()

## cxx standard required
if(NOT DEFINED CMAKE_CXX_STANDARD_REQUIRED)
    set(CMAKE_CXX_STANDARD_REQUIRED ON)
endif()
## cxx extensions
if(NOT DEFINED CMAKE_CXX_EXTENSIONS)
    set(CMAKE_CXX_EXTENSIONS OFF)
endif()


## cxx standard
### by default build is set for gnu linux environment
if(NOT DEFINED CMAKE_CXX_STANDARD)
    # automatic standard detection from compiler flags
    set(_detected_standard 17)
    # check for Unix-style flags (-std=c++XX or -std=gnu++XX)
    string(REGEX MATCH "-std=([c|gnu]\\+\\+)([0-9]+)" _ "${CMAKE_CXX_FLAGS}")
    if(CMAKE_MATCH_2)
        set(_detected_standard ${CMAKE_MATCH_2})
    endif()
    # check for MSVC flags (/std:c++XX or /std:c++latest)
    if(NOT _detected_standard AND MSVC)
        string(REGEX MATCH "/std:c\\+\\+latest" _ "${CMAKE_CXX_FLAGS}")
        if(CMAKE_MATCH_0)
            set(_detected_standard latest) # map /std:c++latest to C++20
        else()
            string(REGEX MATCH "/std:c\\+\\+([0-9]+)" _ "${CMAKE_CXX_FLAGS}")
            if(CMAKE_MATCH_1)
                set(_detected_standard ${CMAKE_MATCH_1})
            endif()
        endif()
    endif()
    set(CMAKE_CXX_STANDARD ${_detected_standard} CACHE STRING "C++ standard to use (17, 20, etc.)")
else()
    set(CMAKE_CXX_STANDARD "${CMAKE_CXX_STANDARD}" CACHE STRING "C++ standard to use (17, 20, etc.)" FORCE)
endif()

## cxx flags
### by default build is set for gnu linux environment
if(NOT DEFINED CMAKE_CXX_FLAGS)
    if(MSVC)
        if(CMAKE_CXX_STANDARD GREATER_EQUAL 20)
            set(CMAKE_CXX_FLAGS "/std:c++latest /EHsc")
        else()
            set(CMAKE_CXX_FLAGS "/std:c++${CMAKE_CXX_STANDARD} /EHsc")
        endif()
    else()
        set(CMAKE_CXX_FLAGS "-std=c++${CMAKE_CXX_STANDARD}")
    endif()
else()
    # clean existing flags
    string(REGEX REPLACE "-std=[^ ]+" "" CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS}")
    string(REGEX REPLACE "/std:c\\+\\+[^ ]+" "" CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS}")
    string(REGEX REPLACE "/EH[^ ]+" "" CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS}") # remove old /EH flags
    string(REPLACE "/MP" "" CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS}")  # remove /MP
    if(MSVC)
        if(CMAKE_CXX_STANDARD GREATER_EQUAL 20)
            set(CMAKE_CXX_FLAGS "/std:c++latest /EHsc")
        else()
            set(CMAKE_CXX_FLAGS "/std:c++${CMAKE_CXX_STANDARD} /EHsc")
        endif()
    else()
        set(CMAKE_CXX_FLAGS "-std=c++${CMAKE_CXX_STANDARD}")
    endif()
endif()

## rest of the world is debug except Release
set(BEHH_IS_DEBUG true)
if(CMAKE_BEHH_BUILD_TYPE STREQUAL "Release")
    set(BEHH_IS_DEBUG false) 
elseif(CMAKE_BEHH_BUILD_TYPE STREQUAL "MinSizeRel")
    set(BEHH_IS_DEBUG false) 
endif()

## system name
if(NOT DEFINED BEHH_SYSTEM_NAME)
    set(BEHH_SYSTEM_NAME "")
endif()
string(TOLOWER "${CMAKE_SYSTEM_NAME}" BEHH_SYSTEM_NAME)
### rules of os
if(${BEHH_SYSTEM_NAME} STREQUAL "macos")
    set(BEHH_SYSTEM_NAME "osx")
elseif(${BEHH_SYSTEM_NAME} STREQUAL "darwin")
    set(BEHH_SYSTEM_NAME "osx")
else()
    # force otherwise
    string(TOLOWER "${CMAKE_SYSTEM_NAME}" BEHH_SYSTEM_NAME)
endif()

## system process
if(NOT DEFINED BEHH_SYSTEM_PROCESSOR)
    set(BEHH_SYSTEM_PROCESSOR "")
endif()
string(TOLOWER "${CMAKE_SYSTEM_PROCESSOR}" BEHH_SYSTEM_PROCESSOR)

# include base
include(CheckIncludeFileCXX)
include(GenerateExportHeader)
include(CMakeDependentOption)
include(CMakeFindDependencyMacro)
include(CMakePackageConfigHelpers)
include(FindPackageHandleStandardArgs)

# files
## headers files
if(NOT DEFINED BEHH_HEADERS_FILES)
    set(BEHH_HEADERS_FILES)
endif()
### constants
set(BEHH_HEADERS_FILES_CONSTANTS
    src/inc/behh/constants/string_const.hh
)
list(APPEND BEHH_HEADERS_FILES ${BEHH_HEADERS_FILES_CONSTANTS})
### enums
### functions
set(BEHH_HEADERS_FILES_FUNCTIONS
    src/inc/behh/functions/communication.hh
    src/inc/behh/functions/cryptography.hh
    src/inc/behh/functions/date_and_time.hh
    src/inc/behh/functions/generate.hh
    src/inc/behh/functions/system.hh
    src/inc/behh/functions/utility.hh
)
list(APPEND BEHH_HEADERS_FILES ${BEHH_HEADERS_FILES_FUNCTIONS})
### interfaces
set(BEHH_HEADERS_FILES_INTERFACES
    src/inc/behh/interfaces/ipostgresql_core_interface.hh
    src/inc/behh/interfaces/iscylladb_core_interface.hh
)
list(APPEND BEHH_HEADERS_FILES ${BEHH_HEADERS_FILES_INTERFACES})
### modules
### types
set(BEHH_HEADERS_FILES_TYPES
    src/inc/behh/types/auth_type.hh
    src/inc/behh/types/look_type.hh
)
list(APPEND BEHH_HEADERS_FILES ${BEHH_HEADERS_FILES_TYPES})

## sources files
if(NOT DEFINED BEHH_SOURCES_FILES)
    set(BEHH_SOURCES_FILES)
endif()

list(APPEND
    BEHH_SOURCES_FILES
        src/behh.cc
        # constants
        # enums
        # functions
        src/communication.cc
        src/cryptography.cc
        src/date_and_time.cc
        src/generate.cc
        src/system.cc
        src/utility.cc
        # interfaces
        src/ipostgresql_core_interface.cc
        src/iscylladb_core_interface.cc
        src/icouchbase_core_interface.cc
        # modules
)

# list of used behh vendor/thirdparty lib
if(NOT DEFINED BEHH_USED_VENDORS_DIR)
    set(BEHH_USED_VENDORS_DIR)
endif()

# packages, lib related
## 
## base toolchain
if(NOT "${CMAKE_TOOLCHAIN_FILE}" STREQUAL "")
    message(NOTICE "-- ${PROJECT_NAME}: using CMAKE_TOOLCHAIN_FILE as `${CMAKE_TOOLCHAIN_FILE}`")
endif()
##
## package manager: vcpkg
set(BEHH_USING_VCPKG false)
if(BEHH_BUILD_VCPKG)
    set(BEHH_VCPKG_DIR)
    set(BEHH_VCPKG_TRIPLET)
    set(BEHH_VCPKG_INSTALLED_DIR)

    # check VCPKG_DIR env. var.
    if(NOT "$ENV{VCPKG_DIR}" STREQUAL "")
        set(BEHH_VCPKG_DIR "$ENV{VCPKG_DIR}")
    endif()

    # check VCPKG_ROOT env. var.
    if(NOT "$ENV{VCPKG_ROOT}" STREQUAL "")
        set(BEHH_VCPKG_DIR "$ENV{VCPKG_ROOT}")
    endif()

    if(BEHH_BUILD_VCPKG AND BEHH_VCPKG_DIR STREQUAL "")
        message(FATAL_ERROR "-- ${PROJECT_NAME}: you are attempting to use vcpkg, but environment variables of `VCPKG_DIR/VCPKG_ROOT` is empty")
    endif()

    # include toolchain
    include("${BEHH_VCPKG_DIR}/scripts/buildsystems/vcpkg.cmake")

    # check vcpkg triplet env. var.
    if(NOT "$ENV{VCPKG_TRIPLET}" STREQUAL "")
        set(BEHH_VCPKG_TRIPLET "$ENV{VCPKG_TRIPLET}")
    endif()
    if(NOT "$ENV{VCPKG_DEFAULT_TRIPLET}" STREQUAL "")
        set(BEHH_VCPKG_TRIPLET "$ENV{VCPKG_DEFAULT_TRIPLET}")
    endif()

    # check vcpkg triplet defined var.
    if(NOT "${VCPKG_TRIPLET}" STREQUAL "")
        set(BEHH_VCPKG_TRIPLET "${VCPKG_TRIPLET}")
    endif()
    if(NOT "${VCPKG_DEFAULT_TRIPLET}" STREQUAL "")
        set(BEHH_VCPKG_TRIPLET "${VCPKG_DEFAULT_TRIPLET}")
    endif()
    if(NOT "${BEHH_BUILD_VCPKG_TRIPLET}" STREQUAL "")
        set(BEHH_VCPKG_TRIPLET "${BEHH_BUILD_VCPKG_TRIPLET}")
    endif()

    if(BEHH_VCPKG_TRIPLET STREQUAL "")
        message(FATAL_ERROR "-- ${PROJECT_NAME}: attempt to use vcpkg but triplet is not defined/set, use `-DVCPKG_TRIPLET=processor_type:system_os_name` or `-DVCPKG_DEFAULT_TRIPLET=processor_type:system_os_name` or set those on your environment variable from -DBEHH_BUILD_VCPKG_TRIPLET=processor_type:system_os_name") 
    endif()

    # check and set
    set(BEHH_VCPKG_INSTALLED_DIR "${BEHH_VCPKG_DIR}/installed/${BEHH_VCPKG_TRIPLET}")
    if(EXISTS "${BEHH_VCPKG_INSTALLED_DIR}")
        # link bin dirs
        include_directories(    "${BEHH_VCPKG_INSTALLED_DIR}/bin")
        link_directories(BEFORE "${BEHH_VCPKG_INSTALLED_DIR}/bin")
        include_directories(    "${BEHH_VCPKG_INSTALLED_DIR}/debug/bin")
        link_directories(BEFORE "${BEHH_VCPKG_INSTALLED_DIR}/debug/bin")

        # include dirs
        include_directories(    "${BEHH_VCPKG_INSTALLED_DIR}/include")
        link_directories(BEFORE "${BEHH_VCPKG_INSTALLED_DIR}/include")
        include_directories(    "${BEHH_VCPKG_INSTALLED_DIR}/debug/include")
        link_directories(BEFORE "${BEHH_VCPKG_INSTALLED_DIR}/debug/include")

        # link lib dirs
        include_directories(    "${BEHH_VCPKG_INSTALLED_DIR}/lib")
        link_directories(BEFORE "${BEHH_VCPKG_INSTALLED_DIR}/lib")
        include_directories(    "${BEHH_VCPKG_INSTALLED_DIR}/debug/lib")
        link_directories(BEFORE "${BEHH_VCPKG_INSTALLED_DIR}/debug/lib")

        # link share dirs
        include_directories(    "${BEHH_VCPKG_INSTALLED_DIR}/share")
        link_directories(BEFORE "${BEHH_VCPKG_INSTALLED_DIR}/share")
    else()
        message(FATAL_ERROR "-- ${PROJECT_NAME}: vpckg error")
    endif()

    set(BEHH_USING_VCPKG true)
endif()
##
## curl executeable
set(BEHH_USING_CURL_EXECUTEABLE false)
set(BEHH_USING_CURL_EXECUTEABLE_PATH)
if(WIN32)
    find_file(BEHH_USING_CURL_EXECUTEABLE_PATH "curl.exe")
else()
    # assume unix stlye
    find_file(BEHH_USING_CURL_EXECUTEABLE_PATH "curl")
endif()
if(BEHH_USING_CURL_EXECUTEABLE_PATH)
    set(BEHH_USING_CURL_EXECUTEABLE true)
    message(NOTICE "-- ${PROJECT_NAME}: curl found in \"${BEHH_USING_CURL_EXECUTEABLE_PATH}\"")
endif()
##
## jsoncpp
set(BEHH_USING_JSONCPP false)

find_package(JsonCpp)
find_package(JsonCpp CONFIG QUIET)
if(NOT JsonCpp_FOUND)
    find_package(jsoncpp CONFIG QUIET)
endif()
if(NOT JsonCpp_FOUND AND NOT jsoncpp_FOUND)
    find_package(Jsoncpp CONFIG QUIET)
endif()
if(NOT JsonCpp_FOUND AND NOT jsoncpp_FOUND AND NOT Jsoncpp_FOUND)
    find_package(PkgConfig QUIET)
    if(PKG_CONFIG_FOUND)
        pkg_check_modules(JSONCPP jsoncpp)
        if(JSONCPP_FOUND)
            set(JsonCpp_FOUND TRUE)
        endif()
    endif()
endif()
if(JsonCpp_FOUND OR jsoncpp_FOUND OR Jsoncpp_FOUND OR JSONCPP_FOUND)
    set(BEHH_USING_JSONCPP true)
    message(NOTICE "-- ${PROJECT_NAME}: adding jsoncpp since package found")
endif()
##
## openssl
set(BEHH_USING_OPENSSL false)

find_package(OpenSSL)
if(OpenSSL_FOUND)
    set(BEHH_USING_OPENSSL true)
    message(NOTICE "-- ${PROJECT_NAME}: adding openssl since package found")
endif()
##
## drogon
set(BEHH_USING_DROGON false)

find_package(Drogon CONFIG)
if(Drogon_FOUND)
    set(BEHH_USING_DROGON true)
    message(NOTICE "-- ${PROJECT_NAME}: adding drogon since package found")
endif()
##
## argon2
set(BEHH_USING_ARGON2 false)
set(BEHH_ARGON2_HEADER)
set(BEHH_ARGON2_SHARED)
set(BEHH_ARGON2_LIBRARY)

if(BEHH_USING_VCPKG)
    find_package(unofficial-argon2 CONFIG)

    if(unofficial-argon2_FOUND)
        set(BEHH_USING_ARGON2 true)
        message(NOTICE "-- ${PROJECT_NAME}: adding argon2 from vcpkg")
    endif()
else() 
    find_file(BEHH_ARGON2_HEADER "argon2.h")
    if(WIN32)
        find_library(BEHH_ARGON2_SHARED "argon2.dll")
        find_library(BEHH_ARGON2_LIBRARY "argon2.lib")
    else()
        find_library(BEHH_ARGON2_SHARED "libargon2.so")
        find_library(BEHH_ARGON2_LIBRARY "libargon2.a")
    endif()

    if(BEHH_ARGON2_HEADER AND BEHH_ARGON2_SHARED AND BEHH_ARGON2_LIBRARY)
        set(BEHH_USING_ARGON2 true)
        message(NOTICE "-- ${PROJECT_NAME}: adding argon2 since header, shared & lib found")
    else()
        if(NOT BEHH_ARGON2_HEADER)
            message(NOTICE "-- ${PROJECT_NAME}: still couldn't found argon2 header")
        endif(NOT BEHH_ARGON2_HEADER)

        if(NOT BEHH_ARGON2_SHARED)
            message(NOTICE "-- ${PROJECT_NAME}: still couldn't found argon2 shared object/runtime")
        endif(NOT BEHH_ARGON2_SHARED)

        if(NOT BEHH_ARGON2_LIBRARY)
            message(NOTICE "-- ${PROJECT_NAME}: still couldn't found argon2 library")
        endif(NOT BEHH_ARGON2_LIBRARY)
    endif()
endif()

##
## postgresql
set(BEHH_USING_POSTGRESQL false)

find_package(PostgreSQL)
if(PostgreSQL_FOUND)
    set(BEHH_USING_POSTGRESQL true)
    message(NOTICE "-- ${PROJECT_NAME}: adding postgresql since package found")
else()
    set(BEHH_LIBPQ_FE_HEADER)
    set(BEHH_POSTGRESQL_LIB)

    find_file(BEHH_LIBPQ_FE_HEADER "libpq-fe.h")

    if(BEHH_LIBPQ_FE_HEADER)
        set(BEHH_USING_POSTGRESQL true)
        message(NOTICE "-- ${PROJECT_NAME}: adding postgresql since header found")
    endif()
endif()
##
## scylladb
set(BEHH_USING_SCYLLADB false)
set(BEHH_CASSANDRA_HEADER)
set(BEHH_SCYLLA_CPP_LIB)

find_file(BEHH_CASSANDRA_HEADER "cassandra.h")

if(WIN32)
    find_file(BEHH_SCYLLA_CPP_LIB "scylla-cpp-driver.lib")
else()
    find_file(BEHH_SCYLLA_CPP_LIB "libscylla-cpp-driver.so")
endif()

if(BEHH_CASSANDRA_HEADER AND BEHH_SCYLLA_CPP_LIB)
    set(BEHH_USING_SCYLLADB true)
    message(NOTICE "-- ${PROJECT_NAME}: adding scylladb since deps found")
endif()

if(BEHH_USING_SCYLLADB)
    get_filename_component(BEHH_SCYLLA_CPP_LIB_DIR "${BEHH_SCYLLA_CPP_LIB}" DIRECTORY)
    get_filename_component(BEHH_CASSANDRA_HEADER_DIR "${BEHH_CASSANDRA_HEADER}" DIRECTORY)

    include_directories(${BEHH_SCYLLA_CPP_LIB_DIR})
    include_directories(${BEHH_CASSANDRA_HEADER_DIR})

    link_directories(BEFORE "${BEHH_SCYLLA_CPP_LIB_DIR}")
    link_directories(BEFORE "${BEHH_CASSANDRA_HEADER_DIR}")
endif(BEHH_USING_SCYLLADB)
##
## stb
set(BEHH_USING_STB false)

if(BEHH_BUILD_VENDOR_STB)
    if(NOT DEFINED ${Stb_INCLUDE_DIR})
        set(Stb_INCLUDE_DIR "")
    endif()

    set(BEHH_VENDOR_STB_DIR "${PROJECT_SOURCE_DIR}/vendors/stb")

    if(NOT ${BEHH_VENDOR_STB_DIR} STREQUAL "")
        set(BEHH_USING_STB true)
        set(Stb_INCLUDE_DIR ${BEHH_VENDOR_STB_DIR})
        include_directories(${Stb_INCLUDE_DIR})
    endif()

    set(BEHH_USING_STB true)
    message(NOTICE "-- ${PROJECT_NAME}: adding stb")
endif()
##
## sdl3
set(BEHH_USING_SDL3 false)

if(BEHH_BUILD_VENDOR_SDL3)
    if(BEHH_USING_VCPKG)
        find_package(SDL3 CONFIG REQUIRED)
    else()
        set(BEHH_VENDOR_SDL3_DIR "${PROJECT_SOURCE_DIR}/vendors/sdl3")

        include_directories(${BEHH_VENDOR_SDL3_DIR}/include)

        link_directories(AFTER "${CMAKE_BINARY_DIR}/BEHH_SDL3")

        set(SDL_STATIC true)
        set(SDL_TEST_LIBRARY false)

        add_subdirectory(${BEHH_VENDOR_SDL3_DIR} BEHH_SDL3)
    endif()

    set(BEHH_USING_SDL3 true)
    message(NOTICE "-- ${PROJECT_NAME}: adding sdl3")
    list(APPEND BEHH_USED_VENDORS_DIR "BEHH_SDL3")
endif()
##
## haru
set(BEHH_USING_HARU false)

if(BEHH_BUILD_VENDOR_HARU)
    if(BEHH_USING_VCPKG)
        set(LIBHPDF_SHARED false)
        set(LIBHPDF_STATIC true)
        set(LIBHPDF_EXAMPLES false)

        find_package(unofficial-libharu CONFIG REQUIRED)
    else()
        set(BEHH_VENDOR_LIBHARU_DIR "${PROJECT_SOURCE_DIR}/vendors/libharu")

        set(BEHH_VENDOR_LIBHARU_DIR_SOURCE_OUT ${CMAKE_BINARY_DIR}/BEHH_LIBHARU/src)
        set(BEHH_VENDOR_LIBHARU_DIR_INCLUDE_OUT ${CMAKE_BINARY_DIR}/BEHH_LIBHARU/include)

        set(LIBHPDF_SHARED false)
        set(LIBHPDF_STATIC true)
        set(LIBHPDF_EXAMPLES false)

        include_directories(${BEHH_VENDOR_LIBHARU_DIR}/include)

        include_directories(${BEHH_VENDOR_LIBHARU_DIR_SOURCE_OUT})
        include_directories(${BEHH_VENDOR_LIBHARU_DIR_INCLUDE_OUT})

        link_directories(AFTER "${BEHH_VENDOR_LIBHARU_DIR_SOURCE_OUT}")

        add_subdirectory(${BEHH_VENDOR_LIBHARU_DIR} BEHH_LIBHARU)
    endif()

    set(BEHH_USING_HARU true)
    message(NOTICE "-- ${PROJECT_NAME}: adding haru hpdf")
    list(APPEND BEHH_USED_VENDORS_DIR "BEHH_LIBHARU")
endif()
##
## nanosvg
set(BEHH_USING_NANOSVG false)

if(BEHH_BUILD_VENDOR_NANOSVG)
    if(BEHH_USING_VCPKG)
        find_package(NanoSVG CONFIG REQUIRED)
    else()
        set(BEHH_VENDOR_NANOSVG_DIR "${PROJECT_SOURCE_DIR}/vendors/nanosvg")

        if(NOT EXISTS "${CMAKE_BINARY_DIR}/BEHH_NANOSVG/nanosvg")
            file(MAKE_DIRECTORY "${CMAKE_BINARY_DIR}/BEHH_NANOSVG/nanosvg")
        endif()

        set(BEHH_VENDOR_NANOSVG_DIR_NANOSVG)

        file(COPY "${BEHH_VENDOR_NANOSVG_DIR}/src/" DESTINATION "${CMAKE_BINARY_DIR}/BEHH_NANOSVG/nanosvg")

        set(BEHH_VENDOR_NANOSVG_DIR_NANOSVG "${CMAKE_BINARY_DIR}/BEHH_NANOSVG")

        include_directories(${BEHH_VENDOR_NANOSVG_DIR_NANOSVG})

        add_subdirectory(${BEHH_VENDOR_NANOSVG_DIR} BEHH_NANOSVG)
    endif()

    set(BEHH_USING_NANOSVG true)
    message(NOTICE "-- ${PROJECT_NAME}: adding nanosvg")
    list(APPEND BEHH_USED_VENDORS_DIR "BEHH_NANOSVG")
endif()
##
## zxing-cpp
set(BEHH_USING_ZXING_CPP false)

if(BEHH_BUILD_VENDOR_ZXING_CPP)
    set(ZXING_EXPERIMENTAL_API true)
    set(ZXING_EXAMPLES false)
    set(ZXING_BLACKBOX_TESTS false)
    set(ZXING_UNIT_TESTS false)

    if(BEHH_USING_VCPKG)
        find_package(ZXing CONFIG REQUIRED)
    else()
        set(BEHH_VENDOR_ZXING_CPP_DIR "${PROJECT_SOURCE_DIR}/vendors/zxing-cpp")

        if(NOT EXISTS "${CMAKE_BINARY_DIR}/BEHH_ZXING_CPP/ZXing")
            file(MAKE_DIRECTORY "${CMAKE_BINARY_DIR}/BEHH_ZXING_CPP/ZXing")
        endif()

        set(BEHH_VENDOR_ZXING_CPP_DIR_ZXING)

        file(COPY "${BEHH_VENDOR_ZXING_CPP_DIR}/core/src/" DESTINATION "${CMAKE_BINARY_DIR}/BEHH_ZXING_CPP/ZXing")

        set(BEHH_VENDOR_ZXING_CPP_DIR_ZXING_DIR "${CMAKE_BINARY_DIR}/BEHH_ZXING_CPP")

        include_directories(${BEHH_VENDOR_ZXING_CPP_DIR_ZXING_DIR})

        add_subdirectory(${BEHH_VENDOR_ZXING_CPP_DIR} BEHH_ZXING_CPP)
    endif()

    set(BEHH_USING_ZXING_CPP true)
    message(NOTICE "-- ${PROJECT_NAME}: adding zxing-cpp")
    list(APPEND BEHH_USED_VENDORS_DIR "BEHH_ZXING_CPP")
endif()
##
## couchbase-cxx-client
set(BEHH_USING_COUCHBASE_CXX_CLIENT false)

if(BEHH_BUILD_VENDOR_COUCHBASE_CXX_CLIENT)
    set(COUCHBASE_CXX_CLIENT_BUILD_TESTS false)
    set(COUCHBASE_CXX_CLIENT_BUILD_DOCS false)
    set(COUCHBASE_CXX_CLIENT_BUILD_EXAMPLES false)
    set(COUCHBASE_CXX_CLIENT_BUILD_TOOLS false)

    if(WIN32)
        add_definitions(-DWIN32)

        set(COUCHBASE_CXX_CLIENT_BUILD_STATIC ON)
    endif(WIN32)

    set(BEHH_VENDOR_COUCHBASE_CXX_CLIENT_DIR "${PROJECT_SOURCE_DIR}/vendors/couchbase-cxx-client")

    # couchbase it self
    include_directories("${BEHH_VENDOR_COUCHBASE_CXX_CLIENT_DIR}")
    link_directories(AFTER "${BEHH_VENDOR_COUCHBASE_CXX_CLIENT_DIR}")

    ## taocpp-json
    include_directories("${CMAKE_BINARY_DIR}/_deps/json-src/include")
    link_directories(AFTER "${CMAKE_BINARY_DIR}/_deps/json-src/include")

    ## taocpp-pegtl
    include_directories("${CMAKE_BINARY_DIR}/_deps/json-src/external/PEGTL/include")
    link_directories(AFTER "${CMAKE_BINARY_DIR}/_deps/json-src/external/PEGTL/include")

    ## spdlog
    include_directories("${CMAKE_BINARY_DIR}/_deps/spdlog-src/include")
    link_directories(AFTER "${CMAKE_BINARY_DIR}/_deps/spdlog-src/include")

    ## msvc need /utf-8
    if(MSVC)
        # check /utf-8
        string(FIND "${CMAKE_CXX_FLAGS}" "/utf-8" POS_UTF8_SLASH)

        if(POS_UTF8_SLASH EQUAL -1)
            set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /utf-8")
            message(NOTICE "-- behh: adding /utf-8 flags to msvc")
        endif()
    endif(MSVC)

    add_subdirectory(${BEHH_VENDOR_COUCHBASE_CXX_CLIENT_DIR} BEHH_COUCHBASE_CXX_CLIENT)

    set(BEHH_USING_COUCHBASE_CXX_CLIENT true)
    message(NOTICE "-- ${PROJECT_NAME}: adding couchbase-cxx-client")
    list(APPEND BEHH_USED_VENDORS_DIR "BEHH_COUCHBASE_CXX_CLIENT")

    # tmp
    if(BEHH_IS_DEBUG)
        message(AUTHOR_WARNING "-- ${PROJECT_NAME}: adding [taocpp-json, taocpp-pegtl, spdlog] packages & their dependencies, can make error for this couchbase-cxx-client")
        # maybe force something if BEHH_USING_COUCHBASE_CXX_CLIENT, do not add it
    endif()
endif()

# project lib
## object core lib
add_library(${PROJECT_NAME} OBJECT ${BEHH_SOURCES_FILES})
## static core lib
add_library(behh_static STATIC $<TARGET_OBJECTS:${PROJECT_NAME}>)
## shared code lib
add_library(behh_shared SHARED $<TARGET_OBJECTS:${PROJECT_NAME}>)
### lib/s name
#### lib output
set(BEGE_OUTPUT_NAME "behh")
#### libs windows and else
if(WIN32)
    set_target_properties(behh_static PROPERTIES
        ARCHIVE_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}"
        OUTPUT_NAME "${BEGE_OUTPUT_NAME}"
    )
    set_target_properties(behh_shared PROPERTIES
        ARCHIVE_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/bin"
        OUTPUT_NAME "${BEGE_OUTPUT_NAME}"
    )
else()
    set_target_properties(behh_static PROPERTIES
        OUTPUT_NAME "${BEGE_OUTPUT_NAME}"
    )
    set_target_properties(behh_shared PROPERTIES
        OUTPUT_NAME "${BEGE_OUTPUT_NAME}"
    )
endif()

## installed options
### reserved: TODO - install configs

# compiler options
if(CMAKE_CXX_COMPILER_ID STREQUAL "GNU")
    set(BEHH_COMPILER_ID    1)
    set(BEHH_COMPILER_GNU   true)
    set(BEHH_COMPILER_MSVC  false)
    set(BEHH_COMPILER_CLANG false)

    target_compile_options(${PROJECT_NAME} PRIVATE
        -Wall
        -Wextra
        -Wpedantic
        -Wno-unused-value
        -Wno-unused-variable
        -Wno-reorder
        -Wno-parentheses
        -Wno-switch
        -Wno-format
        -Wno-invalid-offsetof
        -Wno-multichar
        -Wno-char-subscripts
        -Wno-empty-body
        -Wno-unused-function
        -Wno-deprecated-declarations
        -lm
        -fPIC
    )
elseif(CMAKE_CXX_COMPILER_ID STREQUAL "MSVC")
    set(BEHH_COMPILER_ID    2)
    set(BEHH_COMPILER_GNU   false)
    set(BEHH_COMPILER_MSVC  true)
    set(BEHH_COMPILER_CLANG false)

    target_compile_options(${PROJECT_NAME} PRIVATE
        /W4
        /wd4996
    )
elseif(CMAKE_CXX_COMPILER_ID MATCHES "[Cc]lang")
    set(BEHH_COMPILER_ID    3)
    set(BEHH_COMPILER_GNU   false)
    set(BEHH_COMPILER_MSVC  false)
    set(BEHH_COMPILER_CLANG true)

    target_compile_options(${PROJECT_NAME} PRIVATE
        -Wall
        -Wextra
        -Wpedantic
        -Wno-unused-value
        -Wno-unused-variable
        -Wno-reorder
        -Wno-parentheses
        -Wno-switch
        -Wno-format
        -Wno-dynamic-class-memaccess
        -Wno-unused-private-field
        -Wno-invalid-offsetof
        -Wno-multichar
        -Wno-char-subscripts
        -Wno-null-conversion
        -Wno-empty-body
        -Wno-unused-lambda-capture
        -Wno-unused-function
        -Wno-tautological-constant-out-of-range-compare
        -Wno-deprecated-declarations
        -fPIC
    )
else()
    set(BEHH_COMPILER_ID    0)
    set(BEHH_COMPILER_GNU   false)
    set(BEHH_COMPILER_MSVC  false)
    set(BEHH_COMPILER_CLANG false)

    message(NOTICE "-- ${PROJECT_NAME}: -DCMAKE_CXX_COMPILER_ID is not supported as ${CMAKE_CXX_COMPILER_ID}")
endif()

if(BEHH_COMPILER_GNU) # gcc 12.2
    target_link_libraries(${PROJECT_NAME} PRIVATE -lm)

    if(EXISTS "/usr/lib/x86_64-linux-gnu/libm.so")
        target_link_libraries(${PROJECT_NAME} PRIVATE "/usr/lib/x86_64-linux-gnu/libm.so") # somehow build not error
    else()
        message(WARNING "-- ${PROJECT_NAME}: for gcc has error, be warn")
    endif()
endif()

## warning
if(CMAKE_CXX_FLAGS STREQUAL "")
    message(WARNING "-- ${PROJECT_NAME} cxx flags is empty")
endif()

# behh config
configure_file(
    ${PROJECT_SOURCE_DIR}/src/inc/behh/config.in.hh
    ${PROJECT_SOURCE_DIR}/src/inc/behh/config.hh
)

# behh export
generate_export_header(behh
    EXPORT_FILE_NAME
        ${PROJECT_SOURCE_DIR}/src/inc/behh/export.hh
)
generate_export_header(behh_static
    EXPORT_FILE_NAME
        ${PROJECT_SOURCE_DIR}/src/inc/behh/export_static.hh
)
generate_export_header(behh_shared
    EXPORT_FILE_NAME
        ${PROJECT_SOURCE_DIR}/src/inc/behh/export_shared.hh
)

# behh shared definitions
target_compile_definitions(behh_shared PRIVATE BEHH_SHARED)

# target include dirs
## core include: src/inc
target_include_directories(${PROJECT_NAME}
    PUBLIC
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}>/src/inc
)

# target source
## dont expand this yet
target_sources(${PROJECT_NAME}
    PRIVATE
        ${CMAKE_CURRENT_SOURCE_DIR}/src
)

# dependencies for static & shared
## loop dependencies target
foreach(BEHH_TARGET behh_static behh_shared)
    if(CMAKE_CXX_COMPILER_ID STREQUAL "GNU")
        target_compile_options(${BEHH_TARGET} PRIVATE
            -Wall
            -Wextra
            -Wpedantic
            -Wno-unused-value
            -Wno-unused-variable
            -Wno-reorder
            -Wno-parentheses
            -Wno-switch
            -Wno-format
            -Wno-invalid-offsetof
            -Wno-multichar
            -Wno-char-subscripts
            -Wno-empty-body
            -Wno-unused-function
            -Wno-deprecated-declarations
            -lm
            -fPIC
        )
    elseif(CMAKE_CXX_COMPILER_ID STREQUAL "MSVC")
        target_compile_options(${BEHH_TARGET} PRIVATE
            /W4
            /wd4996
        )
    elseif(CMAKE_CXX_COMPILER_ID MATCHES "[Cc]lang")
        target_compile_options(${BEHH_TARGET} PRIVATE
            -Wall
            -Wextra
            -Wpedantic
            -Wno-unused-value
            -Wno-unused-variable
            -Wno-reorder
            -Wno-parentheses
            -Wno-switch
            -Wno-format
            -Wno-dynamic-class-memaccess
            -Wno-unused-private-field
            -Wno-invalid-offsetof
            -Wno-multichar
            -Wno-char-subscripts
            -Wno-null-conversion
            -Wno-empty-body
            -Wno-unused-lambda-capture
            -Wno-unused-function
            -Wno-tautological-constant-out-of-range-compare
            -Wno-deprecated-declarations
            -fPIC
        )
    else()
        message(NOTICE "-- ${BEHH_TARGET}: -DCMAKE_CXX_COMPILER_ID is not supported as ${CMAKE_CXX_COMPILER_ID}")
    endif()

    ## jsoncpp
    if(BEHH_USING_JSONCPP)
        if(NOT BEHH_BUILD_VCPKG)
            target_link_libraries(${BEHH_TARGET} PRIVATE jsoncpp)
        else()
            find_package(jsoncpp CONFIG REQUIRED)
            target_link_libraries(${BEHH_TARGET} PRIVATE JsonCpp::JsonCpp)
        endif()
    endif()
    ##
    ## openssl
    if(BEHH_USING_OPENSSL)
        target_link_libraries(${BEHH_TARGET} PRIVATE OpenSSL::SSL OpenSSL::Crypto)
    endif()
    ##
    ## drogon
    if(BEHH_USING_DROGON)
        target_link_libraries(${BEHH_TARGET} PRIVATE Drogon::Drogon)
    endif()
    ##
    ## argon2
    if(BEHH_USING_ARGON2)
        if(BEHH_BUILD_VCPKG)
            target_link_libraries(${BEHH_TARGET} PRIVATE unofficial::argon2::libargon2)
        else()
            target_link_libraries(${BEHH_TARGET} PRIVATE argon2)
        endif()
    endif()
    ##
    ## postgresql
    if(BEHH_USING_POSTGRESQL)
        target_link_libraries(${BEHH_TARGET} PRIVATE PostgreSQL::PostgreSQL)
    endif()
    ##
    ## scylladb
    if(BEHH_USING_SCYLLADB)
        target_link_libraries(${BEHH_TARGET} PRIVATE scylla-cpp-driver)
    endif()
    ##
    ## stb
    if(BEHH_USING_STB)
        target_include_directories(${BEHH_TARGET} PRIVATE ${Stb_INCLUDE_DIR})
    endif()
    ##
    ## sdl
    if(BEHH_USING_SDL3)
        if(BEHH_BUILD_VCPKG)
            target_link_libraries(${BEHH_TARGET} PRIVATE SDL3::SDL3)
        else()
            set_target_properties(SDL3-static PROPERTIES POSITION_INDEPENDENT_CODE ON)
            set_target_properties(SDL_uclibc PROPERTIES POSITION_INDEPENDENT_CODE ON)
            set_target_properties(SDL3_Headers PROPERTIES POSITION_INDEPENDENT_CODE ON)

            target_link_libraries(${BEHH_TARGET} PRIVATE SDL3-static SDL_uclibc SDL3_Headers)
        endif()
    endif()
    ##
    ## haru
    if(BEHH_USING_HARU)
        if(BEHH_BUILD_VCPKG)
            target_link_libraries(${BEHH_TARGET} PRIVATE unofficial::libharu::hpdf)
        else()
            set_target_properties(hpdf PROPERTIES POSITION_INDEPENDENT_CODE ON)

            target_link_libraries(${BEHH_TARGET} PRIVATE hpdf)

            target_include_directories(${BEHH_TARGET} PRIVATE ${BEHH_VENDOR_LIBHARU_DIR_SOURCE_OUT})
            target_include_directories(${BEHH_TARGET} PRIVATE ${BEHH_VENDOR_LIBHARU_DIR_INCLUDE_OUT})
        endif()
    endif()
    ##
    ## nanosvg
    if(BEHH_USING_NANOSVG)
        if(BEHH_BUILD_VCPKG)
            target_link_libraries(${BEHH_TARGET} PRIVATE NanoSVG::nanosvg NanoSVG::nanosvgrast)
        else()
            set_target_properties(nanosvg PROPERTIES POSITION_INDEPENDENT_CODE ON)
            set_target_properties(nanosvgrast PROPERTIES POSITION_INDEPENDENT_CODE ON)

            target_link_libraries(${BEHH_TARGET} PRIVATE nanosvg nanosvgrast)
        endif()
    endif()
    ##
    ## zxing-cpp
    if(BEHH_USING_ZXING_CPP)
        target_link_libraries(${BEHH_TARGET} PRIVATE ZXing::Core ZXing::ZXing)
    endif()
    ##
    ## zxing-cpp
    if(BEHH_USING_COUCHBASE_CXX_CLIENT)
        if(WIN32)
            # 
            target_link_libraries(${BEHH_TARGET} PRIVATE "${CMAKE_BINARY_DIR}/BEHH_COUCHBASE_CXX_CLIENT/asio.lib")

            target_link_libraries(${BEHH_TARGET} PRIVATE couchbase_cxx_client_static)
        else()
            target_link_libraries(${BEHH_TARGET} PRIVATE couchbase_cxx_client)
        endif()
    endif()
endforeach()

# version shared lib
set_target_properties(behh_shared PROPERTIES
    VERSION "${BEHH_VERSION_MAJOR}.${BEHH_VERSION_MINOR}.${BEHH_VERSION_PATCH}"
    SOVERSION "${BEHH_VERSION_MAJOR}.${BEHH_VERSION_MINOR}.${BEHH_VERSION_PATCH}"
)

# build tests
if(BEHH_BUILD_TESTS)
    add_subdirectory(tests BEHH_TESTS)
endif(BEHH_BUILD_TESTS)

# build tools
if(BEHH_BUILD_TOOLS)
    # n/a
endif(BEHH_BUILD_TOOLS)

# check if not treat as main project ( treat as sub dir ) as last notice
if(NOT CMAKE_SOURCE_DIR STREQUAL CMAKE_CURRENT_SOURCE_DIR)
    message(AUTHOR_WARNING "${PROJECT_NAME}@prothegee: behh CMakeLists.txt is not main/root project")
    message(NOTICE "-- ${PROJECT_NAME}: please add below dir to your project `include_directories` & `link_directories` if you intend to use some behh vendor/thirdparties module:")

    foreach(BEHH_VENDOR_DIR IN LISTS BEHH_USED_VENDORS_DIR)
        message(NOTICE "-- ${BEHH_VENDOR_DIR}, example:
   include_directories(    \"$\{CMAKE_BINARY_DIR}/${BEHH_VENDOR_DIR}\")
   link_directories(BEFORE \"$\{CMAKE_BINARY_DIR}/${BEHH_VENDOR_DIR}\")
")
    endforeach()
endif()

# check
## cxx flags
if(CMAKE_CXX_FLAGS STREQUAL "")
    message(WARNING "-- ${PROJECT_NAME}: cxx flags is empty for some reason")
else()
    message(NOTICE "-- ${PROJECT_NAME}: build info\n   -DCMAKE_CXX_FLAGS=\"${CMAKE_CXX_FLAGS}\"")
endif()
