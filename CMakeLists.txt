cmake_minimum_required (VERSION 2.8)
project (AutoReject)

SET(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} "${CMAKE_SOURCE_DIR}/cmake")

SET(COMPILER_WARNINGS "-Wall -Wno-unused-parameter -Wformat -Wformat-security -Wparentheses -Wshadow -Wno-unknown-pragmas -Wno-deprecated")

IF(CMAKE_COMPILER_IS_GNUCXX)
	SET(RELEASE_COMPILER_FLAGS "-fexpensive-optimizations -frerun-cse-after-loop -fcse-follow-jumps -finline-functions -fschedule-insns2 -fthread-jumps -fforce-addr -fstrength-reduce -funroll-loops")
	IF (${MACHINE} MATCHES "x86_64" OR ${MACHINE} MATCHES "i686")
		SET(RELEASE_COMPILER_FLAGS "${RELEASE_COMPILER_FLAGS} -mfpmath=sse")
	ENDIF()
ELSEIF(${CMAKE_CXX_COMPILER_ID} STREQUAL "Clang")
	SET(RELEASE_COMPILER_FLAGS "-funroll-loops")
ENDIF()

set (CMAKE_CXX_FLAGS "${COMPILER_WARNINGS} -g ${CMAKE_CXX_FLAGS}")
set (CMAKE_CXX_FLAGS_RELEASE "${COMPILER_WARNINGS} -O3 ${RELEASE_COMPILER_FLAGS}")
set (CMAKE_CXX_FLAGS_DEBUG "${COMPILER_WARNINGS} -g")

set (CXX_SRC_SUFFIX "cpp")
set (CXX_HEADER_SUFFIX "h")

include (CheckCXX11)

IF(_HAS_CXX11_FLAG)
	SET(HAVE_CXX11 1)
	SET(CMAKE_CXX_FLAGS "${CXX11_COMPILER_FLAGS} ${CMAKE_CXX_FLAGS}")
	SET(SWIG_CXX_COMPILER_FLAGS "${CXX11_COMPILER_FLAGS} ${SWIG_CXX_COMPILER_FLAGS}")
ENDIF()

IF(_HAS_CXX0X_FLAG)
	SET(HAVE_CXX0X 1)
	SET(CMAKE_CXX_FLAGS "${CXX11_COMPILER_FLAGS} ${CMAKE_CXX_FLAGS}")
	SET(SWIG_CXX_COMPILER_FLAGS "${CXX11_COMPILER_FLAGS} ${SWIG_CXX_COMPILER_FLAGS}")
ENDIF()

include_directories(src rapidxml-1.13)
add_subdirectory (src)

option (TESTING "Run tests" ON)

if (TESTING)
  enable_testing ()
  # Download and unpack googletest at configure time
  configure_file(gtest.in
                 googletest-download/CMakeLists.txt)
  execute_process(COMMAND ${CMAKE_COMMAND} -G "${CMAKE_GENERATOR}" .
    WORKING_DIRECTORY ${CMAKE_BINARY_DIR}/googletest-download )
  execute_process(COMMAND ${CMAKE_COMMAND} --build .
    WORKING_DIRECTORY ${CMAKE_BINARY_DIR}/googletest-download )

  # Prevent GoogleTest from overriding our compiler/linker options
  # when building with Visual Studio
  set(gtest_force_shared_crt ON CACHE BOOL "" FORCE)

  # Add googletest directly to our build. This adds
  # the following targets: gtest, gtest_main, gmock
  # and gmock_main
  add_subdirectory(${CMAKE_BINARY_DIR}/googletest-src
                   ${CMAKE_BINARY_DIR}/googletest-build)

  # The gtest/gmock targets carry header search path
  # dependencies automatically when using CMake 2.8.11 or
  # later. Otherwise we have to add them here ourselves.
  if (CMAKE_VERSION VERSION_LESS 2.8.11)
    include_directories("${gtest_SOURCE_DIR}/include"
                        "${gmock_SOURCE_DIR}/include")
  endif(CMAKE_VERSION VERSION_LESS 2.8.11)

  add_subdirectory(test)
  # Now simply link your own targets against gtest, gmock,
  # etc. as appropriate

endif (TESTING)


