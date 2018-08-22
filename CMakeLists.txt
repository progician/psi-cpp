cmake_minimum_required(VERSION 3.9)

project(CryptoCom LANGUAGES CXX)
set(CMAKE_CXX_STANDARD 14)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

if (UNIX)
  add_compile_options(-Wall -Wpedantic)
endif()


#===-----------------------------------------------------------------------===
# CryptoCom library
add_library(CryptoCom INTERFACE)
target_include_directories(CryptoCom INTERFACE include)


#===-----------------------------------------------------------------------===
# Unit testing
add_executable(UnitTests
  unittest/CyclicRingTest.cpp
  unittest/ElGamalTest.cpp
  unittest/ExponentialElGamalTest.cpp
  unittest/ObliviousEvaluationTest.cpp
  unittest/PolynomialTest.cpp
  unittest/UnitTestMain.cpp
)
target_include_directories(UnitTests PRIVATE unittest)
target_link_libraries(UnitTests PRIVATE CryptoCom)
add_custom_command(
  TARGET UnitTests
  POST_BUILD
  COMMENT "Running unit tests..."
  COMMAND UnitTests
)


#===-----------------------------------------------------------------------===
# External tests
enable_testing()
add_executable(RoundTrip test/RoundTrip.cpp)
target_link_libraries(RoundTrip PRIVATE CryptoCom)
target_include_directories(RoundTrip PRIVATE unittest)
add_test(NAME RoundTrip COMMAND RoundTrip)