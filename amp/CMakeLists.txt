CMAKE_MINIMUM_REQUIRED(VERSION 2.6)

# Name & Version
PROJECT(AMP)
SET(MAJOR_VERSION "0")
SET(MINOR_VERSION "1")

# System architecture detection
IF(${CMAKE_SYSTEM_PROCESSOR} MATCHES "i[3-6]86")
	SET(i386 1)
	SET(LIB_SUFFIX "" CACHE STRING "Choose the suffix of the lib folder (if any) : None 32")
ELSEIF(${CMAKE_SYSTEM_PROCESSOR} MATCHES "x86_64|amd64")
	SET(x86_64 1)
	SET(LIB_SUFFIX "" CACHE STRING "Choose the suffix of the lib folder (if any) : None 64")
ELSEIF(${CMAKE_SYSTEM_PROCESSOR} MATCHES "ppc")
	SET(ppc 1)
	SET(LIB_SUFFIX "" CACHE STRING "Choose the suffix of the lib folder (if any) : None ppc")
ELSE()
	MESSAGE(SEND_ERROR "Platform ${CMAKE_SYSTEM_PROCESSOR} not supported")
ENDIF()

SET(BINDIR "bin")
SET(LIBDIR "lib${LIB_SUFFIX}")

IF(NOT CMAKE_BUILD_TYPE)
	SET(CMAKE_BUILD_TYPE Release CACHE STRING
		"Choose the type of build, options are: Debug Release" FORCE)
ENDIF()

# Compiler defaults flags for different profiles
SET(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -std=c99")

SET(CMAKE_C_FLAGS_DEBUG "-g")
SET(CMAKE_C_FLAGS_RELEASE "-O3 -DNDEBUG")

INCLUDE_DIRECTORIES(src/c/)

# Setting the output directories, so we can build all profiles without mixmatching
SET(CMAKE_RUNTIME_OUTPUT_DIRECTORY "${PROJECT_BINARY_DIR}/${CMAKE_SYSTEM_PROCESSOR}/${CMAKE_BUILD_TYPE}/bin")
SET(CMAKE_LIBRARY_OUTPUT_DIRECTORY "${PROJECT_BINARY_DIR}/${CMAKE_SYSTEM_PROCESSOR}/${CMAKE_BUILD_TYPE}/lib${LIB_SUFFIX}")

# Common source for amp lib
SET(AMP_LIB_SRC 
    src/c/amp/amp_barrier_common.c
    src/c/amp/amp_condition_variable_common.c
    src/c/amp/amp_memory.c
    src/c/amp/amp_mutex_common.c
    src/c/amp/amp_platform_common.c
    src/c/amp/amp_semaphore_common.c
    src/c/amp/amp_thread_array.c
    src/c/amp/amp_thread_common.c
    src/c/amp/amp_thread_local_slot_common.c
)

# Type of barriers
IF(USE_GENERIC_BROADCAST_BARRIERS)
    ADD_DEFINITIONS(-DAMP_USE_GENERIC_BROADCAST_BARRIERS)
    SET(AMP_LIB_SRC ${AMP_LIB_SRC} src/c/amp/amp_barrier_generic_broadcast.c)
ELSE()
    ADD_DEFINITIONS(-DAMP_USE_GENERIC_SIGNAL_BARRIERS)
    SET(AMP_LIB_SRC ${AMP_LIB_SRC} src/c/amp/amp_barrier_generic_signal.c)
ENDIF()

# Add Windows specific
IF(WIN32)
    ADD_DEFINITIONS(-DAMP_USE_WINTHREADS)
    
    SET(AMP_LIB_SRC ${AMP_LIB_SRC} 
        src/c/amp/amp_condition_variable_winthreads.c
        src/c/amp/amp_mutex_winthreads.c
        src/c/amp/amp_internal_platform_win_system_info.c
        src/c/amp/amp_internal_platform_win_system_logical_processor_information.c
        src/c/amp/amp_semaphore_winthreads.c
        src/c/amp/amp_thread_local_slot_winthreads.c
        src/c/amp/amp_thread_winthreads.c
    )
    # ADD_DEFINITIONS(-DAMP_USE_WINVISTA_CONDITION_VARIABLES)
    # src/c/amp/amp_condition_variable_winvista.c
    # src/c/amp/amp_platform_windows_min_host_sdk_win2000.c
    # src/c/amp/amp_platform_windows_min_host_sdk_winserver2003_or_winxp_sp3_or_winxp_pro_x64.c
ELSE()
    # pthreads versions of these are shared across all other platforms
    ADD_DEFINITIONS(-DAMP_USE_PTHREADS)
    SET(AMP_LIB_SRC ${AMP_LIB_SRC} 
        src/c/amp/amp_condition_variable_pthreads.c
        src/c/amp/amp_mutex_pthreads.c
        src/c/amp/amp_thread_local_slot_pthreads.c
        src/c/amp/amp_thread_pthreads.c
    )
    
    # Different flavors of *NIX
    IF(APPLE)
        ADD_DEFINITIONS(-DAMP_USE_LIBDISPATCH_SEMAPHORES)
        SET(AMP_LIB_SRC ${AMP_LIB_SRC} 
            src/c/amp/amp_semaphore_libdispatch.c
            src/c/amp/amp_platform_sysctl.c
        )
    ELSEIF(CMAKE_SYSTEM_NAME STREQUAL "Linux" AND CMAKE_COMPILER_IS_GNUCC)
        ADD_DEFINITIONS(-DAMP_USE_POSIX_1003_1B_SEMAPHORES -DSEM_VALUE_MAX=2147483647)
        SET(AMP_LIB_SRC ${AMP_LIB_SRC} 
            src/c/amp/amp_semaphore_posix_1003_1b.c
            src/c/amp/amp_platform_gnuc.c
        )
    ELSEIF(UNIX)
        SET(AMP_LIB_SRC ${AMP_LIB_SRC}
            src/c/amp/amp_platform_sysconf.c
            src/c/amp/amp_semaphore_pthreads.c
        )
    ELSE()
        SET(AMP_LIB_SRC ${AMP_LIB_SRC} 
            src/c/amp/amp_platform_unknown.c
            src/c/amp/amp_semaphore_pthreads.c
        )
    ENDIF()
    
ENDIF()

# amp library
ADD_LIBRARY(amp STATIC ${AMP_LIB_SRC})
SET_TARGET_PROPERTIES(amp PROPERTIES OUTPUT_NAME amp)
SET_TARGET_PROPERTIES(amp PROPERTIES VERSION "${MAJOR_VERSION}.${MINOR_VERSION}")
IF(WIN32)
    TARGET_LINK_LIBRARIES(amp)
ELSE()
    TARGET_LINK_LIBRARIES(amp pthread)
ENDIF()

# Test suite
SET(CMAKE_CXX_FLAGS_DEBUG "-g")
SET(CMAKE_CXX_FLAGS_RELEASE "-O3 -DNDEBUG")

# Test suite sources
SET(AMP_TEST_SRC
    test/amp_barrier_test.cpp
    test/amp_condition_variable_test.cpp
    test/amp_mutex_test.cpp
    test/amp_platform_test.cpp
    test/amp_semaphore_test.cpp
    test/amp_stddef_test.cpp
    test/amp_thread_array_test.cpp
    test/amp_thread_local_slot_test.cpp
    test/amp_thread_test.cpp
    test/tests_main.cpp
)


IF(COMPILE_TESTS)
    IF(NOT LIBUNITTESTPP_DIR)
        SET(LIBUNITTESTPP_DIR "~/Downloads/UnitTest++")
    ENDIF()
    
    LINK_DIRECTORIES(${CMAKE_LIBRARY_OUTPUT_DIRECTORY} ${LIBUNITTESTPP_DIR})
    INCLUDE_DIRECTORIES(${LIBUNITTESTPP_DIR}/src)
    
    IF(WIN32)
        INCLUDE_DIRECTORIES(${LIBUNITTESTPP_DIR}/src/Win32)
    ELSE()
        INCLUDE_DIRECTORIES(${LIBUNITTESTPP_DIR}/src/Posix)
    ENDIF()
    
	ADD_EXECUTABLE(amp_tests ${AMP_TEST_SRC})
	TARGET_LINK_LIBRARIES(amp_tests amp UnitTest++)
ENDIF()