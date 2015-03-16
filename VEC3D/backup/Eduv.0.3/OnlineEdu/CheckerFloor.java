

/* The floor is a blue and green chessboard, with a small red square
   at the (0,0) position on the (X,Z) plane, and with numbers along
   the X- and Z- axes.
*/

import java.awt.*;
import javax.media.j3d.*;
import com.sun.j3d.utils.geometry.Text2D;
import javax.vecmath.*;
import java.util.ArrayList;


public class CheckerFloor
{
  private final static int FLOOR_LEN = 20;  // should be even

  // colours for floor, etc
  private final static Color3f blue = new Color3f(0.1f, 0.3f, 0.0f);
  private final static Color3f green = new Color3f(0.0f, 0.5f, 0.5f);
  private final static Color3f medRed = new Color3f(1.0f, 0.0f, 0.0f);
  private final static Color3f white = new Color3f(1.0f, 1.0f, 1.0f);
  private final static Color3f wall = new Color3f(0.85f, 0.85f, 0.85f);
  private final static Color3f ceil = new Color3f(0.65f, 0.75f, 0.85f);

  private BranchGroup floorBG;


  public CheckerFloor()
  // create tiles(mode true = color ,mode false = material), add origin marker, then the axes labels
  {
    ArrayList blueFloorCoords  = new ArrayList();
    ArrayList greenFloorCoords = new ArrayList();
    ArrayList ceil1Coords = new ArrayList();
    ArrayList ceil2Coords = new ArrayList();
    
    ArrayList WallCoords   = new ArrayList();
    floorBG = new BranchGroup();
    
    boolean isBlue;
    int t = FLOOR_LEN/2;
    for (int z = -t; z <= t-1;z++){
    	isBlue = (z%2==0) ? true : false;
    	for (int x= -t; x <= t-1;x++){
    		// the floor
    		if (isBlue) {
    			createFloorCoords(t-3,x,z,ceil1Coords);
    			createFloorCoords(0,x,z,blueFloorCoords);
    		}else{
    			createFloorCoords(t-3,x,z,ceil2Coords);
    			createFloorCoords(0,x,z,greenFloorCoords);
    		}
    		isBlue = !isBlue;
    		// back wall
    		createFaceCoords(-t,x,z,WallCoords);
    		// front wall
    		//createFaceCoords(t,x,z,WallCoords);
    	}
    }
    
   
    for (int z = -t; z <= t-1;z++)
    	for (int y= 0; y <= t-1;y++){
    		createWallCoords(t,y,z,WallCoords); // right side
    		createWallCoords(-t,y,z,WallCoords);// left side
	}
    
    // add Color tiles
    floorBG.addChild(new Tiles(true,ceil1Coords,wall));
    floorBG.addChild(new Tiles(true,ceil2Coords,ceil));
    floorBG.addChild(new Tiles(true,blueFloorCoords,blue));
    floorBG.addChild(new Tiles(true,greenFloorCoords,green));
    
    floorBG.addChild(new Tiles(false,WallCoords,wall));
    
    
   // addOriginMarker();
   // labelAxes();
  }  // end of CheckerFloor()


  private void createWallCoords(int s,int y, int z, ArrayList coords){
  	 // points created in counter-clockwise order
    Point3f p1 = new Point3f(s,y, z+1.0f);
    Point3f p2 = new Point3f(s,y+1.0f, z+1.0f);
    Point3f p3 = new Point3f(s,y+1.0f, z);
    Point3f p4 = new Point3f(s,y,z);
    
    coords.add(p1); coords.add(p2);
    coords.add(p3); coords.add(p4);
  } // end of createWallCoords()
  
  private void createFaceCoords(int s,int x, int y, ArrayList coords)
  // Coords for a single blue or green square, 
  // its left hand corner at (x,0,z)
  {
    // points created in counter-clockwise order
    Point3f p1 = new Point3f(x,  	y+1.0f,	s);
    Point3f p2 = new Point3f(x+1.0f,y+1.0f,	s);
    Point3f p3 = new Point3f(x+1.0f,y,		s);
    Point3f p4 = new Point3f(x,		y,		s);
    
    coords.add(p1); coords.add(p2);
    coords.add(p3); coords.add(p4);
  }  // end of createFaceCoords

  private void createFloorCoords(int s,int x, int z, ArrayList coords)
  // Coords for a single blue or green square, 
  // its left hand corner at (x,0,z)
  {
    // points created in counter-clockwise order
    Point3f p1 = new Point3f(x, 	 	s, 	z+1.0f);
    Point3f p2 = new Point3f(x+1.0f, 	s, 	z+1.0f);
    Point3f p3 = new Point3f(x+1.0f, 	s, 	z);
    Point3f p4 = new Point3f(x, 		s,	z);
    
    coords.add(p1); coords.add(p2);
    coords.add(p3); coords.add(p4);
  }  // end of createFloorCoords()


  private void addOriginMarker()
  // A red square centered at (0,0,0), of length 0.5
  {  // points created counter-clockwise, a bit above the floor
    Point3f p1 = new Point3f(-0.25f, 0.01f, 0.25f);
    Point3f p2 = new Point3f(0.25f, 0.01f, 0.25f);
    Point3f p3 = new Point3f(0.25f, 0.01f, -0.25f);    
    Point3f p4 = new Point3f(-0.25f, 0.01f, -0.25f);

    ArrayList oCoords = new ArrayList();
    oCoords.add(p1); oCoords.add(p2);
    oCoords.add(p3); oCoords.add(p4);

    floorBG.addChild( new Tiles(true,oCoords, medRed) );
  } // end of addOriginMarker();


  private void labelAxes()
  // Place numbers along the X- and Z-axes at the integer positions
  {
    
    // bright yellow text
    Appearance apText = new Appearance();
	Material m = new Material(medRed, medRed,  medRed,  white, 64f);
    m.setLightingEnable(true);
	apText.setMaterial(m);

    // create y-axis oriented 3D text for name
    Font3D f3d = new Font3D( new Font("Verdana", Font.PLAIN, 1),
                             new FontExtrusion() );
    Text3D txt = new Text3D(f3d, "VEC3D");
    txt.setAlignment(Text3D.ALIGN_CENTER);

    OrientedShape3D textShape = new OrientedShape3D();
    textShape.setGeometry(txt);
    textShape.setAppearance(apText);
	textShape.setAlignmentAxis( 0.0f, 1.0f, 0.0f);

    // create a transform group for the object's name
    TransformGroup nameTG = new TransformGroup();
    Transform3D toRot = new Transform3D();
    Transform3D nameT3d = new Transform3D();
    // Assuming uniform size chars, set scale to fit string in view
  //  nameT3d.setScale( 1.2/(userName.length()) );
  
  	toRot.rotY(2.5);	// 160 degree
    nameT3d.mul(toRot);
    nameT3d.setTranslation( new Vector3d(-7.0f, 3.0f, 0) );
    
    nameTG.setTransform(nameT3d);
    nameTG.addChild( textShape ); 
    floorBG.addChild(nameTG);
    /*Vector3d pt = new Vector3d();
    for (int i=-FLOOR_LEN/2; i <= FLOOR_LEN/2; i++) {
      pt.x = i;
      floorBG.addChild( makeText(pt,""+i) );   // along x-axis
    }

    pt.x = 0;
    for (int i=-FLOOR_LEN/2; i <= FLOOR_LEN/2; i++) {
      pt.z = i;
      floorBG.addChild( makeText(pt,""+i) );   // along z-axis
    }*/
  }  // end of labelAxes()


  private TransformGroup makeText(Vector3d vertex, String text)
  // Create a Text2D object at the specified vertex
  {
    Text2D message = new Text2D(text, white, "SansSerif", 10, Font.BOLD );
       // 36 point bold Sans Serif

    TransformGroup tg = new TransformGroup();
    Transform3D t3d = new Transform3D();
    t3d.setTranslation(vertex);
    tg.setTransform(t3d);
    tg.addChild(message);
    return tg;
  } // end of getTG()


  public BranchGroup getBG()
  {  return floorBG;  }


}  // end of CheckerFloor class

