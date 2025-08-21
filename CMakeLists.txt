cmake_minimum_required(VERSION 3.16)
project(BAMalyzer LANGUAGES C CXX)
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

include(FetchContent)
FetchContent_Declare(nlohmann_json GIT_REPOSITORY https://github.com/nlohmann/json.git GIT_TAG v3.11.2)
set(CLI11_INSTALL OFF)
FetchContent_Declare(CLI11 GIT_REPOSITORY https://github.com/CLIUtils/CLI11.git GIT_TAG v2.3.2)
FetchContent_MakeAvailable(nlohmann_json CLI11)
find_package(OpenMP REQUIRED)

set(HTSLIB_SOURCE_DIR ${CMAKE_SOURCE_DIR}/htslib)

add_executable(bamalyzer src/main.cpp src/BamAnalyzer.cpp)

target_include_directories(bamalyzer PUBLIC
    "include"
    "${nlohmann_json_SOURCE_DIR}/include"
    "${CLI11_SOURCE_DIR}/include"
    "${HTSLIB_SOURCE_DIR}/htslib"  
    "${HTSLIB_SOURCE_DIR}"         
)

target_link_libraries(bamalyzer PRIVATE
    "${HTSLIB_SOURCE_DIR}/libhts.a"
    CLI11::CLI11
    OpenMP::OpenMP_CXX
    -lz -lbz2 -llzma -lcurl -lssl -lcrypto
)

target_compile_options(bamalyzer PRIVATE -Wall -Wextra -pedantic)