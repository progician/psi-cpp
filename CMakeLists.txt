cmake_minimum_required( VERSION 3.9 )

project( private-set-intersection )
set( CMAKE_CXX_STANDARD 14 )
set( CMAKE_CXX_STANDARD_REQUIRED ON )
set( CMAKE_CXX_EXTENSIONS OFF )
set( CMAKE_MODULE_PATH ${CMAKE_CURRENT_SOURCE_DIR}/share/cmake/Modules )

find_package( Boost REQUIRED )
find_package( Catch REQUIRED )

#===-----------------------------------------------------------------------===
# CryptoCom library
add_library( CryptoCom STATIC
  src/elgamal.cpp
  src/finite-group.cpp
  src/private-intersection.cpp )

target_include_directories( CryptoCom
  PUBLIC
    include )

target_link_libraries( CryptoCom
  Boost::boost )


#===-----------------------------------------------------------------------===
# Unit testing
add_executable( UnitTests
  unittest/CyclicGroupTest.cpp
  unittest/UnitTestMain.cpp )

target_link_libraries( UnitTests
  PRIVATE
    CryptoCom
    Catch::catch )


#===-----------------------------------------------------------------------===
# External tests
add_executable( RoundTrip
  test/psi-test.cpp )

target_link_libraries( RoundTrip
  PRIVATE
    CryptoCom )
