cmake_minimum_required(VERSION 3.5)

set(PROJECT_MASTER http_server)
set(PROJECT_PRUEBACLASE rpc_pruebaclase)
set(PROJECT_REMOTEFILE rpc_remotefile)

set(CMAKE_BUILD_TYPE Debug)
set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_BUILD_TYPE Debug)

project(${PROJECT_MASTER} LANGUAGES CXX)
add_executable(http_server main.cpp httpserver.cpp httpserver.h utils.cpp utils.h)

project(${PROJECT_PRUEBACLASE})
#[[add_executable(${PROJECT_PRUEBACLASE} "main_slave.cpp" "pruebaclase_imp.cpp"
    "mpi_manager.h" "mpi_manager.cpp"
    "pruebaclase_imp.h" "pruebaclase.cpp" "pruebaclase.h"
    )]]


project(${PROJECT_REMOTEFILE})
#[[add_executable(${PROJECT_REMOTEFILE} "main_remotefile.cpp"
    "mpi_manager.h" "mpi_manager.cpp"
    "remotefile.h" "remotefile.cpp"
    "remotefile_imp.cpp" "remotefile_imp.h")̣]]

LINK_DIRECTORIES(/usr/lib/x86_64-linux-gnu/)
target_link_libraries(http_server mysqlcppconn mysqlclient pthread z m rt atomic dl)

find_package(MPI REQUIRED)
include_directories(${MPI_INCLUDE_PATH})
target_link_libraries(${PROJECT_MASTER} ${MPI_LIBRARIES})
#target_link_libraries(${PROJECT_PRUEBACLASE} ${MPI_LIBRARIES})
#target_link_libraries(${PROJECT_REMOTEFILE} ${MPI_LIBRARIES})
