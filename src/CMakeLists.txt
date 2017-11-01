FILE(GLOB_RECURSE LIBFACS_SRC *.${CXX_SRC_SUFFIX})
FILE(GLOB_RECURSE LIBFACS_HEADER *.${CXX_HEADER_SUFFIX})
#list(REMOVE_ITEM LIBFACS_SRC "app.cpp")

add_library(libfacs_io OBJECT ${LIBFACS_SRC})
set_target_properties(libfacs_io PROPERTIES
	POSITION_INDEPENDENT_CODE TRUE)

add_library(facs_io STATIC $<TARGET_OBJECTS:libfacs_io>)

add_executable(bd2flowjo app.cpp)
target_link_libraries(bd2flowjo facs_io)

INSTALL(TARGETS facs_io
  ARCHIVE DESTINATION lib
  LIBRARY DESTINATION lib
  COMPONENT libraries)

INSTALL(TARGETS bd2flowjo
	RUNTIME DESTINATION bin
	COMPONENT development)
