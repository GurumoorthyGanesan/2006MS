

// ColouredTiles creates a coloured quad array of tiles.
// No lighting since no normals or Material used

import javax.media.j3d.*;
import javax.vecmath.*;
import java.util.ArrayList;


public class Tiles extends Shape3D 
{
  private QuadArray plane;


  public Tiles(boolean mode,ArrayList coords, Color3f col) 
  /* mode = true => color , mode = false -> material*/
  {
    plane = new QuadArray(coords.size(),GeometryArray.COORDINATES | GeometryArray.COLOR_3);
    createGeometry(coords,col);
    createAppearanceColor();
    
  }    

  private void createGeometry(ArrayList coords, Color3f col)
  { 
    int numPoints = coords.size();
    
    Point3f[] points = new Point3f[numPoints];
    coords.toArray(points);
    plane.setCoordinates(0,points);
    
    Color3f cols[]= new Color3f[numPoints];
    for (int i=0;i < numPoints;i++)
    	cols[i] = col;
    
    plane.setColors(0,cols);
    setGeometry(plane);
  }  // end of createGeometry()


  private void createAppearanceColor()
  {
    Appearance app = new Appearance();
    
    PolygonAttributes pa = new PolygonAttributes();
    // so can see the color tiles from both sides
    pa.setCullFace(PolygonAttributes.CULL_NONE);
    
    app.setPolygonAttributes(pa);
    
    setAppearance(app);
  }  // end of createAppearance()


} // end of ColouredTiles class
