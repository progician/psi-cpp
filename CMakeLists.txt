cmake_minimum_required(VERSION 3.4)
project( private-set-intersection )
file( GLOB psi_SRCS
  "*.cpp"
  "*.hpp" )
add_executable( psi-test ${psi_SRCS} )
set_property( TARGET psi-test PROPERTY CXX_STANDARD 11 )
