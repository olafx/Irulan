cmake_minimum_required(VERSION 3.20)

project(Irulan
    VERSION 0
    LANGUAGES CXX
)

set(CMAKE_CXX_STANDARD 17)

add_library(Irulan INTERFACE)

target_include_directories(Irulan INTERFACE 
    $<BUILD_INTERFACE:${PROJECT_SOURCE_DIR}/include>
    $<INSTALL_INTERFACE:include>
)

include(CMakePackageConfigHelpers)

write_basic_package_version_file("${PROJECT_BINARY_DIR}/IrulanConfigVersion.cmake"
    VERSION 0
    COMPATIBILITY ExactVersion
)

install(TARGETS Irulan
    EXPORT IrulanTargets
    LIBRARY DESTINATION lib COMPONENT Runtime
    ARCHIVE DESTINATION lib COMPONENT Development
    RUNTIME DESTINATION bin COMPONENT Runtime
    PUBLIC_HEADER DESTINATION include COMPONENT Development
    BUNDLE DESTINATION bin COMPONENT Runtime
)

include(CMakePackageConfigHelpers)

configure_package_config_file(
    "${PROJECT_SOURCE_DIR}/cmake/IrulanConfig.cmake.in"
    "${PROJECT_BINARY_DIR}/IrulanConfig.cmake"
    INSTALL_DESTINATION lib/cmake/Irulan
)

install(EXPORT IrulanTargets DESTINATION lib/cmake/Irulan)

install(FILES "${PROJECT_BINARY_DIR}/IrulanConfigVersion.cmake"
              "${PROJECT_BINARY_DIR}/IrulanConfig.cmake"
        DESTINATION lib/cmake/Irulan)

install(DIRECTORY ${PROJECT_SOURCE_DIR}/include/ DESTINATION include)

add_executable(DynamicConstruction test/DynamicConstruction.cc)
add_executable(DynamicIndex test/DynamicIndex.cc)
add_executable(DynamicOperators test/DynamicOperators.cc)
add_executable(DynamicWrapping test/DynamicWrapping.cc)
add_executable(DynamicEfficientShape test/DynamicEfficientShape.cc)
add_executable(StaticConstruction test/StaticConstruction.cc)
add_executable(StaticIndex test/StaticIndex.cc)
add_executable(StaticInitList test/StaticInitList.cc)
add_executable(StaticOperators test/StaticOperators.cc)

enable_testing()

add_test(DynamicConstruction DynamicConstruction)
add_test(DynamicIndex DynamicIndex)
add_test(DynamicOperators DynamicOperators)
add_test(DynamicWrapping DynamicWrapping)
add_test(DynamicEfficientShape DynamicEfficientShape)
add_test(StaticConstruction StaticConstruction)
add_test(StaticIndex StaticIndex)
add_test(StaticInitList StaticInitList)
add_test(StaticOperators StaticOperators)
