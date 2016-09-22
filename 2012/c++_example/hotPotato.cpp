// Example player for Bumper Boat Rally.  The boat with the baton
// tries to head toward the next rally point.  The rest just sail
// around randomly.  We pass the baton sometimes, if another boat in
// the fleet is close enough.
//
// ICPC Challenge
// Sturgill, NC State University

#include "Util.h"
#include <vector>
#include <iostream>
#include <list>
#include <cstdlib>

using namespace std;

/** Simple representation for a boat. */
struct Boat {
  // Radius
  double radius;

  // Thrust position WRT orgin-centric boat.
  Vector2D tPos;

  // Thrust force for this boat.
  double thrust;

  // Left/right turn range for the thrust, in radians.
  double tRange;

  // Mass
  double mass;

  // Position of the boat
  Vector2D pos;

  // Heading, measured CCW from positive X.
  double heading;

  // Velocity
  Vector2D vel;

  // Rotational speed, CCW.
  double spin;

  // To where are we trying to take this boat?
  Vector2D target;

  Boat() {
  }
};

int main() {
  // Size of the field.  It depends on the game.
  double fieldSize;

  // List of all boats
  vector< Boat > bList( 2 * BOATS_PER_PLAYER );
  
  // Course, first my course, then the opponent.
  vector< vector< Vector2D > > course( 2, vector< Vector2D >() );
  
  // Read size of the field.
  cin >> fieldSize;

  // Read initial boat configurations.
  for ( int i = 0; i < bList.size(); i++ ) {
    cin >> bList[ i ].radius
        >> bList[ i ].tPos.x >> bList[ i ].tPos.y
        >> bList[ i ].thrust >> bList[ i ].tRange
        >> bList[ i ].mass;
  }

  // Read the courses.
  int courseLen;
  cin >> courseLen;
  for ( int i = 0; i < course.size(); i++ ) {
    course[ i ] = vector< Vector2D >( courseLen );
    for ( int j = 0; j < courseLen; j++ )
      cin >> course[ i ][ j ].x >> course[ i ][ j ].y;
  }

  // current score for each player.
  int score[ 2 ];

  // Number of waypoints touched on this lap.
  int progress[ 2 ];

  // Which boat has the baton.
  int baton[ 2 ];

  int turnNum;
  cin >> turnNum;
  while ( cin && turnNum >= 0 ) {
    cin >> score[ RED ] >> score[ BLUE ];
    cin >> progress[ RED ] >> progress[ BLUE ];
    cin >> baton[ RED ] >> baton[ BLUE ];

    // Read all boat locations, orientations, etc.
    for ( unsigned int i = 0; i < bList.size(); i++ ) {
      cin >> bList[ i ].pos.x >> bList[ i ].pos.y 
          >> bList[ i ].heading
          >> bList[ i ].vel.x >> bList[ i ].vel.y 
          >> bList[ i ].spin;
    }

    // Target for passing the baton.
    int passTarget = -1;
    
    for ( unsigned int i = 0; i < BOATS_PER_PLAYER; i++ ) {
      Boat &boat = bList[ i ];

      // If I have the baton, 
      if ( baton[ RED ] == i ) {
        boat.target = course[ RED ][ progress[ RED ] ];

        // See if we can pass the baton.
        vector< int > nearby;
        for ( int j = 0; j < BOATS_PER_PLAYER; j++ )
          if ( i != j && ( boat.pos - bList[ j ].pos ).mag() < BATON_PASS_LIMIT )
            nearby.push_back( j );

        // We have some nearby boats.  Decide if I want to pass the baton,
        // and choose someone to pass to.
        if ( nearby.size() > 0 && rand() % 10 == 0 ) {
          passTarget = nearby[ rand() % nearby.size() ];

          boat.target.x = fieldSize * rand() / RAND_MAX;
          boat.target.y = fieldSize * rand() / RAND_MAX;
        }
      } else {
        // Every 20 turns, choose a new target location.
        if ( turnNum % 20 == 0 ) {
          boat.target.x = fieldSize * rand() / RAND_MAX;
          boat.target.y = fieldSize * rand() / RAND_MAX;
        }
      }

      // Make a guess for where the boat will be when it comes to rest, if we apply
      // thrust in the neutral position.
      Vector2D newVel = boat.vel + 
        Vector2D( 1, 0 ).rotate( boat.heading ) * boat.thrust;
      Vector2D restPos = boat.pos + newVel * ( 1.0 / LINEAR_FRICTION_PER_UNIT_SPEED );
      
      // If I'm the one with the baton, head to the next waypoint.
      Vector2D goalDir = boat.target - restPos;

      // Make a guess for how far we're going to spin, if we don't do
      // anything about it.
      double restHeading = boat.heading + boat.spin * 1.0 / ROTATIONAL_FRICTION_PER_UNIT_SPIN;
      Vector2D restDir = Vector2D( 1, 0 ).rotate( restHeading );
      
      // How much to try to turn left or right.
      double turnAmount = 1.5;

      // Only turn if we're not heading toward the goal.
      if ( goalDir.norm() * restDir > 0.9 ) {
        turnAmount = 0.0;
      }
      
      if ( boat.tPos.x > 0 ) {
        // Compute thrust angle for no turn.
        double ctrAngle = atan2( boat.tPos.y, boat.tPos.x );
      
        // Try to turn to correct our heading.
        if ( goalDir.norm().cross( restDir ) > 0 ) {
          cout << "1 " << ctrAngle - turnAmount << " ";
        } else {
          cout << "1 " << ctrAngle + turnAmount << " ";
        }
      } else {
        // Compute thrust angle for no turn.
        double ctrAngle = atan2( -boat.tPos.y, -boat.tPos.x );
      
        // Try to turn to correct our heading.
        if ( goalDir.norm().cross( restDir ) > 0 ) {
          cout << "1 " << ctrAngle + turnAmount << " ";
        } else {
          cout << "1 " << ctrAngle - turnAmount << " ";
        }
      }
    }

    // Pass the baton (if we decided to above)
    cout << passTarget << endl;

    cin >> turnNum;
  }
}
