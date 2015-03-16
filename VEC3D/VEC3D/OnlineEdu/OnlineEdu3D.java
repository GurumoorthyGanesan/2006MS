
/*
 * Title : Online Education.
 * function :
 * 1. Setup up a virtual classes, allow anybody login from other machine.
 * 2. Everybody can using voice talking head to communication with each other.
 * 3. FACE detection.(???)
 */ 

import javax.media.j3d.*;
import javax.vecmath.*;
import javax.swing.*;
import java.awt.event.*;
import java.awt.*;
import com.sun.j3d.utils.universe.*;
import com.sun.j3d.utils.geometry.*;
import com.sun.j3d.utils.applet.MainFrame;

public class OnlineEdu3D extends JApplet
{
  private String userName = null; 	// username
  private String userPSW = null;	// user's password
  private String MapFnm = null;		// map
  private double xPosn = 0.0;   	// default starting coord
  private double zPosn = 0.0;
  private Content3D C3d;
	
  public OnlineEdu3D() {
  	/* construct the canvas3D */
  	
   	// processArgs(args);
    userName = "Alan";
    MapFnm = "tour1.txt";
   
  	Container c = getContentPane();
    c.setLayout( new BorderLayout() );
    C3d = new Content3D(userName, MapFnm, xPosn, zPosn);
    c.add(C3d, BorderLayout.CENTER);

    ActionListener al = new ActionListener() {
       public void actionPerformed(ActionEvent e){ 
       	C3d.closeLink(); 
       	}
     };
    show();
  }

  public void init(){
  }
  
  public static void main(String args[]){ 
  	new MainFrame(new OnlineEdu3D(),800,600); 
  }

} // end of NetTour3D class
