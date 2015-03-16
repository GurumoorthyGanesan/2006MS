
/* Obstacles stores a 2D boolean array (obs) representing the
   XZ plane. The row index is the z-axis, the column index
   is the x-axis. obs[0][0] is the back left hand corner of
   the plane, which is the (x,z) point (0-FLOOR_LEN/2, 0-FLOOR_LEN/2).

   Obstacles can only be positioned at integer positions due
   to the use of array subscripts to mark the obstacle positions.
*/

import java.util.*;
import javax.media.j3d.*;
import javax.vecmath.*;
import com.sun.j3d.utils.geometry.*;


public class Obstacles
{
  private final static float RADIUS = 0.1f;   // radius of obstacle
  private final static float HEIGHT = 1.0f;   // height of obstacle

  private final static int FLOOR_LEN = 20;  
      // should be the same as the floor size in CheckerFloor, and be even

  // colours for obs material
  private final static Color3f black = new Color3f(0.0f, 0.0f, 0.0f);
  private final static Color3f specular = new Color3f(1.0f, 1.0f, 1.0f);
  private final static Color3f red = new Color3f(0.9f, 0.1f, 0.2f);


  private boolean obs[][];    // whether a (x,z) has an obstacle
  private BoundingSphere obsBounds[][];   // the bounding sphere for obs
  private Group obsGroup;     // group of 3d obstacles shown in world


  public Obstacles()
  {
    obs = new boolean[FLOOR_LEN+1][FLOOR_LEN+1];
    obsBounds = new BoundingSphere[FLOOR_LEN+1][FLOOR_LEN+1];
    // clear obstracles array
    for (int z=0; z <= FLOOR_LEN; z++)
    	for (int x=0; x <= FLOOR_LEN; x++){
    		obs[z][x] = false;
    		obsBounds[z][x] = null;
    	}
    	
    obsGroup = new Group();
  }  // end of Obstacles()


  public void store(String line)
  /* The line will have the format:
        (x1,z1) (x2,z2) ...
  */
  {
    int x=0, z=0;
    String coordStr;
    StringTokenizer points;
    StringTokenizer coords = new StringTokenizer(line);
    while (coords.hasMoreTokens()) {
      coordStr = coords.nextToken();    // should be a single (x,z)
      points = new StringTokenizer(coordStr, "(,)");
      try {
        x = Integer.parseInt(points.nextToken());
        z = Integer.parseInt(points.nextToken());
      }
      catch (NumberFormatException ex){ 
        System.out.println("Incorrect format for obstacle data in tours file"); 
        break;
      } 
      markObstacle(x,z);
    }
  }  // end of store()


  private void markObstacle(int x, int z)
  {
    if ((x < -FLOOR_LEN/2) || (x > FLOOR_LEN/2)) {
      System.out.println("Obstacle x point out of bounds: " + x);
      x = 0;
    }
    if ((z < -FLOOR_LEN/2) || (z > FLOOR_LEN/2)) {
      System.out.println("Obstacle z point out of bounds: " + z);
      z = 0;
    }
    obs[z+(FLOOR_LEN/2)][x+(FLOOR_LEN/2)] = true;
    obsBounds[z+(FLOOR_LEN/2)][x+(FLOOR_LEN/2)] =
			new BoundingSphere( new Point3d(x, 0.0, z), RADIUS);

   obsGroup.addChild( makeObs(x, z));

  }  // end of markObstacle()


  public void print()
  {
    // header - markers for each 5 units, and a line
    for(int x=(-FLOOR_LEN/2); x <= (FLOOR_LEN/2); x++) {
      if (x == 0)
        System.out.print("0");
      else if (x%5 == 0)
        System.out.print("*");
      else
        System.out.print(" ");
    }
    System.out.println("");
    for(int x=(-FLOOR_LEN/2); x <= (FLOOR_LEN/2); x++)
      System.out.print("-");
    System.out.println("");

    // body of table: O's and a z-axis at the end of each line
    for (int z=0; z <= FLOOR_LEN; z++) {
      for(int x=0; x <= FLOOR_LEN; x++) {
        if (obs[z][x])
          System.out.print("O");
        else
          System.out.print(" ");
      }
      if ((z-FLOOR_LEN/2)%5 == 0)
        System.out.println("| " + (z-FLOOR_LEN/2));
      else 
        System.out.println("|");
    }

    // footer: a line
    for(int x=(-FLOOR_LEN/2); x <= (FLOOR_LEN/2); x++)
      System.out.print("-");
    System.out.println("");
  }  // end of print()


  public boolean nearObstacle(Point3d pos, double radius)
  /* posn is not allowed to be outside the floor area.
     Also, every obstacle's bounding sphere is checked against pos
  */
  {
    if ((pos.x < -FLOOR_LEN/2) || (pos.x > FLOOR_LEN/2) ||
        (pos.z < -FLOOR_LEN/2) || (pos.z > FLOOR_LEN/2))     // off the floor
      return true;

    // check if near any obstacles
    BoundingSphere bs = new BoundingSphere( pos, radius);
    for (int z=0; z <= FLOOR_LEN; z++)
      for(int x=0; x <= FLOOR_LEN; x++)
        if (obs[z][x]) {
          if (obsBounds[z][x].intersect(bs))
            return true;
        }
     return false;
   }  // end of nearObstacle()



  private TransformGroup makeObs(int x, int z)
  // a red cylinder whose base is on (x,z)
  {
    // create obstacle node
    Appearance obsApp = new Appearance();
    Material mat = new Material(black, black, red, specular, 100.f);
    mat.setLightingEnable(true);
    obsApp.setMaterial( mat );
    Cylinder obs = new Cylinder( RADIUS, HEIGHT, 
						Cylinder.GENERATE_NORMALS, obsApp);

    // fix obs's position
    TransformGroup posnTG = new TransformGroup();
    Transform3D trans = new Transform3D();
    trans.setTranslation( new Vector3d(x, HEIGHT/2, z) );
    posnTG.setTransform(trans);
    posnTG.addChild(obs); 
    return posnTG;
  }  // end of makeObs()


  public Group getObsGroup()
  {  return obsGroup;  }



}  // end of Obstacles class