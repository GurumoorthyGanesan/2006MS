

/* Sprite3D loads a 3D image, placing it at (0,0,0). 
   We assume that the object's actual position is (0,0) in the XZ plane. 
   The Y position will vary but probably the base of the object is 
   touching the XZ plane.

   Movements are restricted to the XZ plane and rotations
   around the Y-axis.

   An object cannot move off the floor, or travel through obstacles 
   (as defined in the Obstacles object).

   Very sinmilar to Sprite3D in Tour3D. 
   New net-related code:
     - a userName above the sprite that stays oriented along +z axis;
     - return a detachable branchgroup for the scene;
     - store the current rotation around the y-axis, and methods to return
       and set it.
*/

import java.text.DecimalFormat;
import java.util.*;
import java.io.*;
import java.awt.Font;

import javax.media.j3d.*;
import javax.vecmath.*;


public class Sprite3D 
{
  private final static double OBS_FACTOR = 0.5;  
             // used to reduce radius of bounding sphere around Sprite
             // when testing for intersection with obstacles

  private BranchGroup objectBG;       // for adding/removing from scene
  private TransformGroup objectTG;    // TG which the loaded object hangs off
  private Transform3D t3d, toMove, toRot;    // for manipulating objectTG's transform
  private Switch visSwitch;           // to make object visible/invisible

  private double radius;
  private boolean isActive;   // is the sprite active?
  private double currRotation;

  private Obstacles obs;      // obstacles in the way of an object
  private DecimalFormat df;   // for simpler output during debugging


  public Sprite3D(String userName, String fnm, Obstacles obs)
  { 
    df = new DecimalFormat("0.###");  // 3 dp
    this.obs = obs;
    currRotation = 0.0;

    // load object and coords
    LoaderManager LoaderMan = new LoaderManager(fnm, true);

    // get objLoc and radius of loaded object
    // hopefully objLoc is close to (0, ??, 0)
    Vector3d objLoc = LoaderMan.getLoc();
    radius = LoaderMan.getScale();      // assume radius == scale
    // printTuple(objLoc, "objLoc");
    // System.out.println("radius: " + df.format(radius));

    // create switch for visibility 
    visSwitch = new Switch();
    visSwitch.setCapability(Switch.ALLOW_SWITCH_WRITE);
    visSwitch.addChild( LoaderMan.getTG() );     // add object to switch
    visSwitch.setWhichChild( Switch.CHILD_ALL );   // make visible

    // create a transform group for the object and its name
    objectTG = new TransformGroup();
    objectTG.setCapability(TransformGroup.ALLOW_TRANSFORM_READ);
    objectTG.setCapability(TransformGroup.ALLOW_TRANSFORM_WRITE);
    objectTG.addChild( visSwitch );
    objectTG.addChild( makeName(userName) );  // object's name and TG

    // branch group for object
    objectBG = new BranchGroup();
    objectBG.setCapability( BranchGroup.ALLOW_DETACH );		
    objectBG.addChild(objectTG);

    t3d = new Transform3D();
    toMove = new Transform3D();
    toRot = new Transform3D();
    isActive = true;
  }  // end of Sprite3D()


  private TransformGroup makeName(String userName)
  /* The name of the client, floating above the sprite, and always
     facing the viewer. */
  {
    // bright yellow text
    Color3f black = new Color3f(0.0f, 0.0f, 0.0f);
    Color3f brightYellow = new Color3f(0.5f,0.5f, 0.0f);
    Appearance apText = new Appearance();
	Material m = new Material(black, black, brightYellow, brightYellow, 64f);
    m.setLightingEnable(true);
	apText.setMaterial(m);

    // create y-axis oriented 3D text for name
    Font3D f3d = new Font3D( new Font("Verdana", Font.PLAIN, 1),
                             new FontExtrusion() );
    Text3D txt = new Text3D(f3d, userName);
    txt.setAlignment(Text3D.ALIGN_CENTER);

    OrientedShape3D textShape = new OrientedShape3D();
    textShape.setGeometry(txt);
    textShape.setAppearance(apText);
	textShape.setAlignmentAxis( 0.0f, 1.0f, 0.0f);

    // create a transform group for the object's name
    TransformGroup nameTG = new TransformGroup();
    Transform3D nameT3d = new Transform3D();
    // Assuming uniform size chars, set scale to fit string in view
    nameT3d.setScale( 1.2/(userName.length()) );
    nameT3d.setTranslation( new Vector3d(0, radius*2, 0) );
    nameTG.setTransform(nameT3d);
    nameTG.addChild( textShape ); 

    return nameTG;
  }  // end of makeName()



  public BranchGroup getBG()
  {  return objectBG; }


  public void detach()
  // remove this sprite from the scene
  {  objectBG.detach(); }



  public void setPosition(double xPos, double zPos)
  { Point3d currLoc = getCurrLoc();
    double xMove = xPos - currLoc.x;
    double zMove = zPos - currLoc.z;
    moveBy(xMove, zMove);
  } // end of setPosition()



  public boolean moveBy(double x, double z)
  /* Move the sprite by offsets x and z, but only if within the floor
     and there is no obstacle nearby. */
  {
    if (isActive()) {
      Point3d nextLoc = tryMove(new Vector3d(x, 0, z));
      if (obs.nearObstacle(nextLoc, radius*OBS_FACTOR))
        return false;
      else {
        doMove( new Vector3d(x, 0, z) );   // inefficient recalc
        return true;
      }
    }
    else    // not active
      return false;
  }  // end of moveBy()



  private void doMove(Vector3d theMove)
  // Move the sprite by the amount in theMove
  {
    objectTG.getTransform( t3d );
    toMove.setTranslation(theMove);    // overwrite previous trans
    t3d.mul(toMove);
    objectTG.setTransform(t3d);
  }  // end of doMove()



  private Point3d tryMove(Vector3d theMove)
  /* Calculate the effect of the given translation but
     do not update the sprite's position until it's been
     tested.
  */
  { objectTG.getTransform( t3d );
    toMove.setTranslation(theMove);
    t3d.mul(toMove);
    Vector3d trans = new Vector3d();
    t3d.get( trans );
    // printTuple(trans, "nextLoc");
    return new Point3d( trans.x, trans.y, trans.z);
  }  // end of tryMove()


  public void doRotateY(double radians)
  // Rotate the sprite by radians amount around its y-axis
  {
    objectTG.getTransform( t3d );
    toRot.rotY(radians);   // overwrite previous rotation
    t3d.mul(toRot);
    objectTG.setTransform(t3d);
    currRotation += radians;
  } // end of doRotateY()


  public Point3d getCurrLoc()
  {        
    objectTG.getTransform( t3d );
    Vector3d trans = new Vector3d();
    t3d.get( trans );
    // printTuple(trans, "currLoc");
    return new Point3d( trans.x, trans.y, trans.z);
  } // end of getCurrLoc()


  public void setCurrRotation(double rot)
  { double rotChange = rot - currRotation;
    doRotateY(rotChange);
  } // end of setCurrRotation()


  public double getCurrRotation()
  {  return currRotation;  }


  public boolean isActive() 
  {  return isActive;  }

  public void setActive(boolean b)
  // Activity changes affect the sprite's visibility 
  { isActive = b;
    if (!isActive)
       visSwitch.setWhichChild( Switch.CHILD_NONE );   // make invisible
    else if (isActive)
      visSwitch.setWhichChild( Switch.CHILD_ALL );   // make visible
  } // end of setActive()



  private void printTuple(Tuple3d t, String id)
  // used for debugging
  {
    System.out.println(id + " x: " + df.format(t.x) + 
				", " + id + " y: " + df.format(t.y) +
				", " + id + " z: " + df.format(t.z));
  }  // end of printTuple()


}  // end of Sprite3D class
