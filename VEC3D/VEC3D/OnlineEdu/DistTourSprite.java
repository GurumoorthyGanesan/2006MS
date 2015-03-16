
// DistTourSprite.java
// Andrew Davison, July 2003, dandrew@ratree.psu.ac.th

/* DistTourSprite moves and rotates a sprite representing
   a client from another machine. 

   It is very similar to TourSprite, but doesn't
   send its moves/rotations to the server.

   The methods defined here are called by the TourWatcher, in 
   response to move/rotation messages received from the 
   server.
*/


public class DistTourSprite extends Sprite3D
{
  private final static double MOVERATE = 0.3;
  private final static double ROTATE_AMT = Math.PI / 16.0;


  public DistTourSprite(String userName, String fnm, Obstacles obs,
								double xPosn, double zPosn)
  { 
    super(userName, fnm, obs);
    setPosition(xPosn, zPosn);
  }  // end of DistTourSprite()


  // moves
  public boolean moveForward()
  { return moveBy(0.0, MOVERATE); }

  public boolean moveBackward()
  { return moveBy(0.0, -MOVERATE); }

  public boolean moveLeft()
  { return moveBy(-MOVERATE,0.0); }

  public boolean moveRight()
  { return moveBy(MOVERATE,0.0); }


  // rotations in Y-axis only
  public void rotClock()
  { doRotateY(-ROTATE_AMT); }   // clockwise

  public void rotCounterClock()
  { doRotateY(ROTATE_AMT); }  // counter-clockwise

}  // end of DistTourSprite