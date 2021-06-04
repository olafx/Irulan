cmake_minimum_required(VERSION 3.20)

project(Irulan)
include(Ctest)

set(CMAKE_CXX_STANDARD 17)
add_compile_options(-pedantic)


add_executable(StaticConstruction   test/StaticConstruction.cc)
add_executable(StaticBasicOperators test/StaticBasicOperators.cc)
add_executable(StaticIndex          test/StaticIndex.cc)
add_executable(StaticInitList       test/StaticInitList.cc)

add_executable(HybridConstruction   test/HybridConstruction.cc)
add_executable(HybridBasicOperators test/HybridBasicOperators.cc)
add_executable(HybridIndex          test/HybridIndex.cc)
add_executable(HybridInitList       test/HybridInitList.cc)

add_executable(DynamicConstruction   test/DynamicConstruction.cc)
add_executable(DynamicBasicOperators test/DynamicBasicOperators.cc)
add_executable(DynamicIndex          test/DynamicIndex.cc)


add_test(StaticConstruction   StaticConstruction)
add_test(StaticBasicOperators StaticBasicOperators)
add_test(StaticIndex          StaticIndex)
add_test(StaticInitList       StaticInitList)

add_test(HybridConstruction   HybridConstruction)
add_test(HybridBasicOperators HybridBasicOperators)
add_test(HybridIndex          HybridIndex)
add_test(HybridInitList       HybridInitList)

add_test(DynamicConstruction   DynamicConstruction)
add_test(DynamicBasicOperators DynamicBasicOperators)
add_test(DynamicIndex          DynamicIndex)
