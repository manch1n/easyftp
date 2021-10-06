
include_directories(${CMAKE_CURRENT_SOURCE_DIR}/include)
aux_source_directory(${CMAKE_CURRENT_SOURCE_DIR}/lib LIB_SRCS)
set(LIBRARY_OUTPUT_PATH ${PROJECT_SOURCE_DIR}/lib)
set(CMAKE_CXX_STANDARD 17)
add_library(block_static STATIC ${LIB_SRCS})