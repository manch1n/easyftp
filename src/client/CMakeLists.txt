include_directories(${CMAKE_CURRENT_SOURCE_DIR}/)
aux_source_directory(${CMAKE_CURRENT_SOURCE_DIR}/ CLIENT_SRCS)
set(EXECUTABLE_OUTPUT_PATH ${PROJECT_SOURCE_DIR}/build)
add_executable(myftpclient ${CLIENT_SRCS} ${COMMON_SRCS})