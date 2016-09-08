#include "finite-group.hpp"

#include <boost/assert.hpp>
#include <stdexcept>
#include <tuple>


static std::tuple< int64_t, int64_t, int64_t >
extendedGCD( int64_t const a, int64_t const b )
{
  if ( a == 0 )
    return std::tuple< int64_t, int64_t, int64_t >( b, 0, 1 );
  else {
    auto const prev = extendedGCD( b % a, a );
    auto const g = std::get<0>( prev );
    auto const x = std::get<1>( prev );
    auto const y = std::get<2>( prev );
    return std::make_tuple( g, y - ( ( b / a ) * x ), x );
  }
}


static int64_t
inverseModulo( const int64_t a, int64_t const b )
{
  auto const egcd = extendedGCD( a, b );
  auto const g = std::get<0>( egcd );
  auto const x = std::get<1>( egcd );

  if ( g == 1 ) {
    int64_t result;
    if ( x < 0 )
      result = b + x;
    else
      result = std::abs( x ) % b;

    return result;
  }

  throw std::invalid_argument( "a and b are relative primes" );
}


namespace FiniteGroup {

  Elem
  Group::random() const
  {
    if ( fixedRandom != 0 )
      return Elem( fixedRandom, *this );

    std::mt19937 randomNumberGenerator( randomDevice() );
    return Elem( distribution( randomNumberGenerator ), *this );
  }

  Elem
  Group::g() const
  { return Elem( generator, *this ); }


  Elem
  Group::operator()( int64_t const plainValue ) const
  {
    return Elem( uint64_t( plainValue % order ), *this );
  }

  Elem
  Group::minusOne() const
  { return Elem( order - 1, *this ); }


  Elem
  Group::plusOne() const
  { return Elem( 1, *this ); }


  bool
  Group::operator==( Group const& other ) const
  {
    return order == other.order &&
           generator == other.generator;
  }


  Elem&
  Elem::operator=( Elem const& other )
  {
    BOOST_ASSERT( group == other.group );
    value = other.value;
    return *this;
  }

  Elem&
  Elem::operator +=( Elem const& other )
  {
    BOOST_ASSERT( group == other.group );
    value = ( __uint128_t( value ) + __uint128_t( other.value ) ) % group.order;
    return *this;
  }


  Elem&
  Elem::operator -=( Elem const& other )
  {
    BOOST_ASSERT( group == other.group );

    if ( value < other.value ) {
      value = group.order - ( other.value - value );
    }
    else {
      value = value - other.value;
    }

    return *this;
  }


  Elem&
  Elem::operator *=( Elem const& other )
  {
    BOOST_ASSERT( group == other.group );
    __uint128_t const multipliedValue = __uint128_t( value ) * __uint128_t( other.value );
    value = uint64_t( multipliedValue % __uint128_t( group.order ) );
    return *this;
  }


  Elem&
  Elem::operator ^=( int64_t const exponent )
  {
    if ( exponent < 0 ) {
      *this ^= std::abs( exponent );
      *this = this->inverse();
      return *this;
    }

    Elem res( 1, group );
    Elem base = *this;

    int64_t e = exponent;
    while ( e > 0 ) {
      if ( ( e & 1 ) == 1 ) {
        res *= base;
        e--;
      }

      base *= base;
      e >>= 1;
    }

    return *this = res;
  }


  Elem&
  Elem::operator^=( Elem const& exponent )
  {
    BOOST_ASSERT( group == exponent.group );
    return *this ^= int64_t( exponent.value );
  }


  Elem
  Elem::inverse() const
  {
    auto const resultValue = uint64_t( inverseModulo( int64_t( value ), int64_t( group.order ) ) );
    return Elem( resultValue, group );
  }


  Elem&
  Elem::operator /=( Elem const& other )
  {
    auto const invOther = other.inverse();
    return *this *= invOther;
  }


  bool
  Elem::operator==( Elem const& other ) const
  {
    return value == other.value && group == other.group;
  }
}
