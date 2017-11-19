cmake_minimum_required( VERSION 3.9 )

project( private-set-intersection )
set( CMAKE_CXX_STANDARD 14 )
set( CMAKE_CXX_STANDARD_REQUIRED ON )
set( CMAKE_CXX_EXTENSIONS OFF )

find_package( Boost REQUIRED )

#===-----------------------------------------------------------------------===
# CryptoCom library
add_library( CryptoCom
  src/elgamal.cpp
  src/finite-group.cpp
  src/private-intersection.cpp )

target_include_directories( CryptoCom
  PUBLIC
    include )

target_link_libraries( CryptoCom
  Boost::boost )


#===-----------------------------------------------------------------------===
# External tests
add_executable( RoundTrip
  test/psi-test.cpp )

target_link_libraries( RoundTrip
  PRIVATE
    CryptoCom )
