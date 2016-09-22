#ifndef __CHALLENGE_UTIL_H__
#define __CHALLENGE_UTIL_H__

// Supporting functions for a simple player.  Feel free to use this
// code or write better code for yourself.
//
// ICPC Challenge
// Sturgill, NC State University

#include <cmath>
#include <iostream>

/** Number of boats on each player's side. */
const int BOATS_PER_PLAYER = 5;

/** How close the boat center has to come to the waypoint to touch it. */
const double RALLY_POINT_SOFTNESS = 3;

/** Maximum distance the baton can be passed. */
const double BATON_PASS_LIMIT = 10;

/** Force at the edge of the field, linear in the distance past the edge. */
const double BORDER_SPRING_FORCE_PER_UNIT = 0.5;

/** Friction, linear in the speed.*/
const double LINEAR_FRICTION_PER_UNIT_SPEED = 0.25;

/** Rotational Friction, linear in the rotation veloicty. */
const double ROTATIONAL_FRICTION_PER_UNIT_SPIN = 0.25;

/** Color values for the two sides and for neutral.  The player can
    always think of itself as red. */
enum GameColor {
  RED = 0, BLUE = 1
};

/** Simple 2D Point/Vector representation along with common utility
    functions. */
struct Vector2D {
  /** X coordinate of this point/vector. */
  double x;

  /** Y coordinate of this point/vector. */
  double y;

  /** Initialize with given coordinates. */
  Vector2D( double xv = 0, double yv = 0 ) {
    x = xv;
    y = yv;
  }

  /** Return the squared magnitude of this vector. */
  double squaredMag() const {
    return x * x + y * y;
  }

  /** Return the magnitude of this vector. */
  double mag() const {
    return std::sqrt( x * x + y * y );
  }

  /** Return a unit vector pointing in the same direction as this. */
  Vector2D norm() const {
    double m = mag();
    return Vector2D( x / m, y / m );
  }

  /** Return a CCW perpendicular to this vector.*/
  Vector2D perp() const {
    return Vector2D( -y, x );
  }

  /** Return a rotated version of this vector, by the given angle, in
      radians. */
  Vector2D rotate( double a ) const {
    double s = sin( a );
    double c = cos( a );
    return Vector2D( x * c - y * s,
                     x * s + y * c );
  }

  /** Return a cross product of this and b. */
  double cross( Vector2D const &b ) const {
    return x * b.y - y * b.x;
  }

  /** Return a vector pointing in the same direction as this, but with
      magnitude no greater than d. */
  Vector2D limit( double d ) const {
    double m = mag();
    if ( m > d )
      return Vector2D( d * x / m, d * y / m );
    else
      return *this;
  }
};

/** Return a vector that's the sum of a and b. */
Vector2D operator+( Vector2D const &a, Vector2D const &b ) {
  return Vector2D( a.x + b.x, a.y + b.y );
}

/** Return a vector that's a minus b. */
Vector2D operator-( Vector2D const &a, Vector2D const &b ) {
  return Vector2D( a.x - b.x, a.y - b.y );
}

/** Return a copy of a that's a scaled by b. */
Vector2D operator*( Vector2D const &a, double b ) {
  return Vector2D( a.x * b, a.y * b );
}

/** Return a copy of a that's a scaled by b. */
Vector2D operator*( double b, Vector2D const &a ) {
  return Vector2D( b * a.x, b * a.y );
}

/** Return the dot product of a and b. */
double operator*( Vector2D const &a, Vector2D const &b ) {
  return a.x * b.x + a.y * b.y;
}

/** Return the dot product of a and b. */
std::ostream &operator<<( std::ostream &stream, Vector2D const &a ) {
  stream << "( " << a.x << ", " << a.y << " )"; 
  return stream;
}

#endif
