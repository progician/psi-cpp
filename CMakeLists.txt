cmake_minimum_required(VERSION 3.4)
project( private-set-intersection )
file( GLOB peer_SRCS
  "*.cpp"
  "*.hpp" )
add_executable( peer ${peer_SRCS} )
set_property( TARGET peer PROPERTY CXX_STANDARD 11 )
target_link_libraries( peer cryptopp boost_program_options boost_system )
