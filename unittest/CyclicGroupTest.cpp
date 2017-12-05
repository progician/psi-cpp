#include <catch/catch.hpp>


struct SimpleCyclicGroupTraits {
  static const int64_t order = 0x800000;
  static const int64_t generator = 2;
};


TEST_CASE( "In cyclic groups" ) {
  SECTION( "when elements are added" ) {
    
  }
}
