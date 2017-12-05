find_path( Catch_ROOT
  NAMES
    include/catch/catch.hpp
  DOC
    "The relative root of the Catch C++ unit testing framework." )

if ( ${Catch_ROOT} STREQUAL "Catch_ROOT-NOTFOUND")
  message( FATAL_ERROR "Catch C++ unit testing framework was not found" )
endif()

add_library( Catch::catch INTERFACE IMPORTED )
set_target_properties( Catch::catch
  PROPERTIES
    INTERFACE_INCLUDE_DIRECTORIES ${Catch_ROOT}/include )

