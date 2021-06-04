cmake_minimum_required(VERSION 3.20)

project(Irulan)
include(Ctest)

set(CMAKE_CXX_STANDARD 17)
add_compile_options(-pedantic)

add_executable(StaticConstruction   test/StaticConstruction.cc)
add_executable(StaticBasicOperators test/StaticBasicOperators.cc)
add_executable(StaticIndex          test/StaticIndex.cc)
add_executable(StaticInitList       test/StaticInitList.cc)

add_test(StaticConstruction   StaticConstruction)
add_test(StaticBasicOperators StaticBasicOperators)
add_test(StaticIndex          StaticIndex)
add_test(StaticInitList       StaticInitList)
