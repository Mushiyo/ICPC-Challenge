/** 
    Example player for Bumper Boat Rally.  The player with the baton
    tries to head toward the next rally point.  The rest just sail
    around randomly.  We pass the baton sometimes, if another boat in
    the fleet is close enough.
    
    ICPC Challenge
    Sturgill, NC State University
*/

import java.util.Scanner;
import java.util.Random;
import java.util.ArrayList;
import java.awt.geom.Point2D;

public class HotPotato {
  /** Number of boats on each player's side. */
  public static final int BOATS_PER_PLAYER = 5;
  
  /** How close the boat center has to come to the waypoint to touch it. */
  public static final double RALLY_POINT_SOFTNESS = 3;
  
  /** Maximum distance the baton can be passed. */
  public static final double BATON_PASS_LIMIT = 10;
  
  /** Force at the edge of the field, linear in the distance past the edge. */
  public static final double BORDER_SPRING_FORCE_PER_UNIT = 0.5;
  
  /** Friction, linear in the speed.*/
  public static final double LINEAR_FRICTION_PER_UNIT_SPEED = 0.25;
  
  /** Rotational Friction, linear in the rotation veloicty. */
  public static final double ROTATIONAL_FRICTION_PER_UNIT_SPIN = 0.25;

  /** Constant for the red index */
  public static final int RED = 0;

  /** Constant for the blue index */
  public static final int BLUE = 1;

  /** Source of Randomness */
  private static Random rnd = new Random();

  /** Simple representation for a boat. */
  private static class Boat {
    // Radius
    double radius;

    // Thrust position WRT orgin-centric boat.
    Point2D tPos = new Point2D.Double();

    // Thrust force for this boat.
    double thrust;

    // Left/right turn range for the thrust, in radians.
    double tRange;

    // Mass
    double mass;

    // Position of the boat
    Point2D pos = new Point2D.Double();

    // Heading, measured CCW from positive X.
    double heading;

    // Velocity
    Point2D vel = new Point2D.Double();

    // Rotational speed, CCW.
    double spin;

    // To where are we trying to take this boat?
    Point2D target = new Point2D.Double();
  };

  /** Return a new vector containing the sum of a and b. */
  static Point2D sum( Point2D a, Point2D b ) {
    return new Point2D.Double( a.getX() + b.getX(), 
                               a.getY() + b.getY() );
  }

  /** Return a new vector containing the difference between a and b. */
  static Point2D diff( Point2D a, Point2D b ) {
    return new Point2D.Double( a.getX() - b.getX(), 
                               a.getY() - b.getY() );
  }

  /** Return a new vector containing a scaled by scaling factor s. */
  static Point2D scale( Point2D a, double s ) {
    return new Point2D.Double( a.getX() * s, a.getY() * s );
  }

  /** Return a new vector thats the given vector rotated by r CCW. */
  static Point2D rotate( Point2D a, double r ) {
    double s = Math.sin( r );
    double c = Math.cos( r );
    return new Point2D.Double( a.getX() * c - a.getY() * s,
                               a.getX() * s + a.getY() * c );
  }

  /** Return the magnitude of vector a. */
  static double mag( Point2D a ) {
    return Math.sqrt( a.getX() * a.getX() + a.getY() * a.getY() );
  }

  /** Return a new vector containing normalized version of a. */
  static Point2D norm( Point2D a ) {
    double m = mag( a );
    return new Point2D.Double( a.getX() / m,
                               a.getY() / m );
  }

  /** Return a ccw perpendicular vector for a. */
  static Point2D perp( Point2D a ) {
    return new Point2D.Double( -a.getY(), a.getX() );
  }

  /** Return the dot product of a and b. */
  static double dot( Point2D a, Point2D b ) {
    return a.getX() * b.getX() + a.getY() * b.getY();
  }

  /** Return the cross product of a and b. */
  static double cross( Point2D a, Point2D b ) {
    return a.getX() * b.getY() - a.getY() * b.getX();
  }

  /** Return a vector pointing in the same direction as v, but with
      magnitude no greater than d. */
  static Point2D limit( Point2D v, double d ) {
    double m = mag( v );
    if ( m > d )
      return new Point2D.Double( d * v.getX() / m, d * v.getY() / m );
    return v;
  }

  public static void main( String[] args ) {
    // Scanner to parse input from the game engine.
    Scanner in = new Scanner( System.in );

    // Size of the field.  It depends on the game.
    double fieldSize = in.nextDouble();

    // Read boat configurations.
    Boat[] bList = new Boat [ 2 * BOATS_PER_PLAYER ];
    
    for ( int i = 0; i < bList.length; i++ ) {
      bList[ i ] = new Boat();
      bList[ i ].radius = in.nextDouble();
      double x = in.nextDouble();
      double y = in.nextDouble();
      bList[ i ].tPos.setLocation( x, y );
      bList[ i ].thrust = in.nextDouble();
      bList[ i ].tRange = in.nextDouble();
      bList[ i ].mass = in.nextDouble();
    }

    int courseLen = in.nextInt();
    Point2D[][] course = new Point2D [ 2 ][ courseLen ];
    for ( int i = 0; i < 2; i++ )
      for ( int j = 0; j < courseLen; j++ ) {
        double x = in.nextDouble();
        double y = in.nextDouble();
        course[ i ][ j ] = new Point2D.Double( x, y );
      }

    // current score for each player.
    int[] score = new int [ 2 ];

    // Number of waypoints touched on this lap.
    int[] progress = new int [ 2 ];

    // Which boat has the baton.
    int[] baton = new int [ 2 ];

    // For each game turn ...
    int turnNum = in.nextInt();
    while ( turnNum >= 0 ) {
      // Read current score, waypoints touched and who has the baton.
      score[ RED ] = in.nextInt();
      score[ BLUE ] = in.nextInt();
      progress[ RED ] = in.nextInt();
      progress[ BLUE ] = in.nextInt();
      baton[ RED ] = in.nextInt();
      baton[ BLUE ] = in.nextInt();

      for ( int i = 0; i < bList.length; i++ ) {
        double x = in.nextDouble();
        double y = in.nextDouble();
        bList[ i ].pos.setLocation( x, y );
        bList[ i ].heading = in.nextDouble();

        x = in.nextDouble();
        y = in.nextDouble();
        bList[ i ].vel.setLocation( x, y );
        bList[ i ].spin = in.nextDouble();
      }

      // Target for passing the baton.
      int passTarget = -1;

      for ( int i = 0; i < BOATS_PER_PLAYER; i++ ) {
        Boat boat = bList[ i ];

        // If I have the baton.
        if ( baton[ RED ] == i ) {
          boat.target.setLocation( course[ RED ][ progress[ RED ] ] );

          // Find nearby boats.
          ArrayList< Integer > nearby = new ArrayList< Integer >();
          for ( int j = 0; j < BOATS_PER_PLAYER; j++ )
            if ( i != j &&
                 mag( diff( boat.pos, bList[ j ].pos ) ) < BATON_PASS_LIMIT )
              nearby.add( j );
          
          // We have some nearby boats.  Do we want to pass the baton?
          if ( nearby.size() > 0 && rnd.nextDouble() < 0.1 ) {
            passTarget = nearby.get( rnd.nextInt( nearby.size() ) );
            boat.target.setLocation( fieldSize * rnd.nextDouble(),
                                     fieldSize * rnd.nextDouble() );
          }
        } else {
          // Every 20 turns, choose a new target location.
          if ( turnNum % 20 == 0 ) {
            boat.target.setLocation( fieldSize * rnd.nextDouble(),
                                     fieldSize * rnd.nextDouble() );
          }
        }
        
        // Make a guess for where the boat will be when it comes to rest, if we apply
        // thrust in the neutral position.
        Point2D newVel = sum( boat.vel,
                              rotate( new Point2D.Double( boat.thrust, 0 ), boat.heading ) );
        // Figure out where the boat will stop if we do nothing.
        Point2D restPos = sum( boat.pos, scale( newVel, 1.0 / LINEAR_FRICTION_PER_UNIT_SPEED ) );
        
        // Which way to the target?
        Point2D goalDir = diff( boat.target, restPos );
        
        // Figure out how far we're going to spin, if we do nothing.
        double restHeading = boat.heading + boat.spin * 1.0 / ROTATIONAL_FRICTION_PER_UNIT_SPIN;
        Point2D restDir = rotate( new Point2D.Double( 1, 0 ), restHeading );

        // How much to try to turn left or right.
        double turnAmount = 1.5;

        // Only turn if we're not heading toward the goal.
        if ( dot( norm( goalDir ), restDir ) > 0.9 ) {
          turnAmount = 0.0;
        }
        
        if ( boat.tPos.getX() > 0 ) {
          // Compute thrust angle for no turn.
          double ctrAngle = Math.atan2( boat.tPos.getY(), boat.tPos.getX() );

          // Try to turn to correct our heading.
          if ( cross( norm( goalDir ), restDir ) > 0 ) {
            System.out.print( "1 " + ( ctrAngle - turnAmount ) + " " );
          } else {
            System.out.print( "1 " + ( ctrAngle + turnAmount ) + " " );
          }
        } else {
          // Compute thrust angle for no turn.
          double ctrAngle = Math.atan2( -boat.tPos.getY(), -boat.tPos.getX() );
        
          // Try to turn to correct our heading.
          if ( cross( norm( goalDir ), restDir ) > 0 ) {
            System.out.print( "1 " + ( ctrAngle + turnAmount ) + " " );
          } else {
            System.out.print( "1 " + ( ctrAngle - turnAmount ) + " " );
          }
        }
      }

      System.out.println( passTarget );

      turnNum = in.nextInt();
    }
  }
}
