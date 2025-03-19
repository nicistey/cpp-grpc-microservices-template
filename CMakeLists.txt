cmake_minimum_required(VERSION 3.20)
project(GRPCMicroservices LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)
 
# По умолчанию vcpkg или системные пути помогут найти библиотеки.
set(CMAKE_PREFIX_PATH "${CMAKE_PREFIX_PATH};C:/vcpkg/installed/x64-windows")
find_package(gRPC CONFIG REQUIRED)
find_package(Protobuf CONFIG REQUIRED)
find_package(GTest CONFIG REQUIRED)
 

set(PROTO_FILE ${CMAKE_CURRENT_SOURCE_DIR}/proto/templateProto.proto)
set(GENERATED_PROTO_DIR ${CMAKE_CURRENT_BINARY_DIR}/generated)
file(MAKE_DIRECTORY ${GENERATED_PROTO_DIR})
set(gRPC_CPP_PLUGIN_EXECUTABLE "C:/vcpkg/installed/x64-windows/tools/grpc/grpc_cpp_plugin.exe")

add_custom_command(
    OUTPUT ${GENERATED_PROTO_DIR}/templateProto.pb.cc ${GENERATED_PROTO_DIR}/templateProto.pb.h
           ${GENERATED_PROTO_DIR}/templateProto.grpc.pb.cc ${GENERATED_PROTO_DIR}/templateProto.grpc.pb.h
    COMMAND ${Protobuf_PROTOC_EXECUTABLE}
    ARGS --grpc_out ${GENERATED_PROTO_DIR} --cpp_out ${GENERATED_PROTO_DIR}
         -I ${CMAKE_CURRENT_SOURCE_DIR}/proto
         --plugin=protoc-gen-grpc=${gRPC_CPP_PLUGIN_EXECUTABLE}
         ${PROTO_FILE}
    DEPENDS ${PROTO_FILE}
)

add_library(proto_files STATIC
    ${GENERATED_PROTO_DIR}/templateProto.pb.cc
    ${GENERATED_PROTO_DIR}/templateProto.grpc.pb.cc
)
target_include_directories(proto_files PUBLIC ${GENERATED_PROTO_DIR})
target_link_libraries(proto_files PUBLIC protobuf::libprotobuf gRPC::grpc++)

add_executable(server server.cpp)
target_link_libraries(server PRIVATE proto_files)

add_executable(client client.cpp)
target_link_libraries(client PRIVATE proto_files)



 



