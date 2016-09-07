#include "elgamal.hpp"
#include "finite-group.hpp"
#include "polynomial.hpp"
#include "private-intersection.hpp"

#include <algorithm>

#include <boost/asio.hpp>
#include <boost/program_options.hpp>

#include <fstream>
#include <iostream>
#include <set>
#include <stdexcept>
#include <vector>

namespace program_options = boost::program_options;
using boost::asio::ip::tcp;


std::set< int32_t >
buildInputSet( std::istream* inputStream )
{
	std::set< int32_t > results;
	while ( !inputStream->eof() ) {
		int32_t inputNumber = -1;
		(*inputStream) >> inputNumber;
		if ( inputStream->fail() )
			continue;
		results.insert( inputNumber );
	}

	return results;
}


std::ostream&
operator <<( std::ostream& ostr, std::set< int64_t > const& v )
{
	for ( const int32_t n: v ) {
		ostr << n << " ";
	}

	return ostr;
}


int
main( int argc, char** argv )
{
  /*program_options::options_description description;
	description.add_options()
    ( "port,p", program_options::value<uint16_t>()->default_value( 6666 ),
			"The shared memory identifier through which the PSI peers communicate" )
		( "help,h", "produce this help message" )
		( "input-set-file", "input set file name. optional, if missing it will default to the standard input" );

	program_options::positional_options_description positionalOptions;
	positionalOptions.add( "input-set-file", 1 );

	program_options::variables_map variablesMap;
	program_options::store(
	  program_options::command_line_parser( argc, argv )
		  .options( description )
		  .positional( positionalOptions )
		  .run(), variablesMap );
	program_options::notify( variablesMap );

	if ( variablesMap.count( "help" ) > 0 ) {
		std::cout << description << std::endl;
		return 0;
	}

	std::istream* inputStream = &std::cin;
	bool cleanUpInputStream = false;
	if ( variablesMap.count( "input-set-file" ) > 0 ) {
		inputStream = new std::ifstream( variablesMap[ "input-set-file" ].as<std::string>() );
		if ( inputStream->fail() ) {			
			std::cerr << "error: " << strerror( errno ) << std::endl;
			delete inputStream;
			return 0;
		}
		cleanUpInputStream = true;
	}

	std::set< int32_t > inputSet = buildInputSet( inputStream );
	if ( cleanUpInputStream )
		delete inputStream;

  uint16_t portNumber = variablesMap[ "port" ].as<uint16_t>();

  std::cout << "port number: " << boost::lexical_cast<std::string>( portNumber ) << std::endl; */
  // std::cout << "input set: " << inputSet << std::endl;

  std::vector< int64_t > const clientSet = { 2, 4, 6, 8, 10, 12, 14, 16, 18, 20, 22 };
  // std::vector< int64_t > const serverSet = { 3, 6, 9, 12, 15, 18, 21 };
  fg::Group const cryptoGroup( 2250635938ull, 3 );
  auto const keys = ElGamal::keygen( cryptoGroup );
  std::cout << "keys: " << std::get<0>( keys ).value << " " << std::get<1>( keys ).value << std::endl;

  /*auto const clientPoly = poly::fromRoots( clientSet.begin(), clientSet.end(), int64_t( -1 ), int64_t( 1 ) );
  for ( auto const e : clientSet ) {
    int64_t res = poly::eval< int64_t, int64_t, std::vector< int64_t >::const_iterator >( e, clientPoly.begin(), clientPoly.end() );
    if ( res != 0 )
      std::cerr << "failed root: " << e << std::endl;
  } */

  auto const fromClient = PrivateIntersection::prepareLocalSet( clientSet.begin(), clientSet.end(), std::get<1>( keys ) );

  auto const gOnZero = cryptoGroup.g() ^ 0;
  for ( auto const e : clientSet ) {
    auto const fe = cryptoGroup( e );

    ElGamal::Cipher const res = poly::eval< ElGamal::Cipher, fg::Elem, std::vector< ElGamal::Cipher >::const_iterator >( fe, fromClient.begin(), fromClient.end() );
    fg::Elem const decrypted = ElGamal::decrypt( std::get<1>( keys ), res );
    if ( decrypted != gOnZero )
      std::cerr << "failed root: " << e << std::endl;
  }


  /* auto const fromServer = PrivateIntersection::obliviousEvaluation( serverSet.begin(), serverSet.end(), fromClient );
  auto const intersection = PrivateIntersection::extractIntersection< int64_t, std::vector< int64_t >::const_iterator >( clientSet.begin(), clientSet.end(), fromServer, std::get<0>( keys ) );

  std::cout << intersection << std::endl; */

  // std::cout << std::endl;
  // std::cout << "Looking for counter part for PSI calculation..." << std::endl;

	return 0;
}
