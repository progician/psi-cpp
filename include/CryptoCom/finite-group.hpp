#pragma once

#include <boost/operators.hpp>
#include <random>
#include <stdint.h>

namespace FiniteGroup {

  struct Elem;

  struct Group
  {
    uint64_t const order;
    uint64_t const generator;
    uint64_t fixedRandom;
    mutable std::random_device randomDevice;
    mutable std::uniform_int_distribution< uint64_t > distribution;

    Group() = delete;

    Group( uint64_t const o, uint64_t const g )
    : order( o )
    , generator( g )
    , fixedRandom( 0ull )
    , distribution( 0, o ) { }

    Group( Group const& other ) = default;

    Elem random() const;
    Elem g() const;
    Elem operator()( int64_t const plainValue ) const;
    Elem minusOne() const;
    Elem plusOne() const;

    bool
    operator ==( Group const& other ) const;
  };


  struct Elem
  : public boost::addable< Elem >
  , public boost::subtractable< Elem >
  , public boost::multipliable< Elem >
  , public boost::xorable< Elem, int64_t >
  , public boost::xorable< Elem >
  , public boost::dividable< Elem >
  , public boost::equality_comparable< Elem >
  {
    uint64_t value;
    Group const& group;

    Elem() = delete;

    Elem( uint64_t const v, Group const& g )
    : value( v )
    , group( g ) { }

    Elem( Elem const& other ) =  default;

    Elem&
    operator =( Elem const& other );

    Elem& operator +=( Elem const& other );
    Elem& operator -=( Elem const& other );
    Elem& operator *=( Elem const& other );
    Elem& operator ^=( int64_t const other );
    Elem& operator ^=( Elem const& other );
    Elem& operator /=( Elem const& other );

    bool operator ==( Elem const& other ) const;

    Elem
    inverse() const;
  };
} // End of namespace 'FiniteGroup'

namespace fg = FiniteGroup;
