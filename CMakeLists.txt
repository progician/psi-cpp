cmake_minimum_required( VERSION 3.9 )

project( private-set-intersection LANGUAGES CXX )
set( CMAKE_CXX_STANDARD 14 )
set( CMAKE_CXX_STANDARD_REQUIRED ON )
set( CMAKE_CXX_EXTENSIONS OFF )

find_package( Boost REQUIRED )
find_package( Catch REQUIRED )

#===-----------------------------------------------------------------------===
# CryptoCom library
add_library( CryptoCom INTERFACE )
target_include_directories( CryptoCom INTERFACE include )
target_link_libraries( CryptoCom INTERFACE Boost::boost )


#===-----------------------------------------------------------------------===
# Unit testing
add_executable( UnitTests
  unittest/CyclicRingTest.cpp
  unittest/ElGamalTest.cpp
  unittest/UnitTestMain.cpp )

target_link_libraries( UnitTests
  PRIVATE
    CryptoCom
    Catch::catch )


#===-----------------------------------------------------------------------===
# External tests
# add_executable( RoundTrip
#   test/psi-test.cpp )
# 
# target_link_libraries( RoundTrip
#   PRIVATE
#     CryptoCom )
