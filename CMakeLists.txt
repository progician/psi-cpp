cmake_minimum_required( VERSION 3.9 )

project( private-set-intersection LANGUAGES CXX )
set( CMAKE_CXX_STANDARD 14 )
set( CMAKE_CXX_STANDARD_REQUIRED ON )
set( CMAKE_CXX_EXTENSIONS OFF )


#===-----------------------------------------------------------------------===
# CryptoCom library
add_library( CryptoCom INTERFACE )
target_include_directories( CryptoCom INTERFACE include )


#===-----------------------------------------------------------------------===
# Unit testing
add_executable( UnitTests
  unittest/CyclicRingTest.cpp
  unittest/ElGamalTest.cpp
  unittest/ExponentialElGamalTest.cpp
  unittest/ObliviousEvaluationTest.cpp
  unittest/PolynomialTest.cpp
  unittest/UnitTestMain.cpp )

target_include_directories( UnitTests PRIVATE unittest )

target_link_libraries( UnitTests PRIVATE CryptoCom )

add_custom_target( check COMMAND UnitTests )


#===-----------------------------------------------------------------------===
# External tests
# add_executable( RoundTrip
#   test/psi-test.cpp )
# 
# target_link_libraries( RoundTrip
#   PRIVATE
#     CryptoCom )
