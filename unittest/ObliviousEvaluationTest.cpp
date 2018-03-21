#include <CryptoCom/ObliviousEvaluation.hpp>
#include <catch/catch.hpp>


namespace {
  struct NoEncryption {
    using Cipher = int32_t;
    using Ring = int32_t;
    using RNG = std::function< int32_t() >;


    static Ring Decipher( Ring const& e ) { return e; }

    static std::tuple< int32_t, int32_t >
    KeyPairOf( std::function< int32_t() > rng ) {
      return std::make_tuple( 1, 1 );
    }

    static Cipher
    Encrypt( Ring const& key, Ring const& plainText, RNG ) {
      return plainText;
    }

    static Cipher
    Decrypt( Ring const& key, Cipher const& encryptedMessage ) {
      return encryptedMessage;
    }
  };
}


TEST_CASE( "Private set intersection using oblivious polynomial evaluation, where we use plain text with no encryption" ) {
  using namespace CryptoCom::ObliviousEvaluation;
  using TestClientSet = ClientSet< int32_t, int32_t, NoEncryption >;

  TestClientSet client {
    TestClientSet::Ring{5}, TestClientSet::Ring{7},
    { 3, 6 }, []() { return 1; } };

  SECTION( "preparing a set is to create an polynomial with encrypted coefficients" ) {
    auto const polynomial = client.forServer();
    REQUIRE( polynomial.size() == 3 );
    CHECK( polynomial[ 2 ] == 1  );
    CHECK( polynomial[ 1 ] == -9 );
    CHECK( polynomial[ 0 ] == 18 );
  }


  SECTION( "extracting intersection from evaluated polynomial set" ) {
    auto const intersection = client.intersection( { 2, 3, 12 }, 11 );
    REQUIRE( intersection.size() == 1 );
    CHECK( intersection.count( 3 ) );
  }
}
