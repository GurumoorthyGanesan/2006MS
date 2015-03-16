
/* Sets up a Tour3D world, but one including visitors from 
   other machines.

   New net-related code:
     * makeContact() for making contact with the TourServer;

     * closeLink() to break the link with the server and exit;

     * addVisitor() for creating distributed tourist(s),
       called by TourWatcher when the server sends it a request;

     * sendDetails() for sending sprite info to the server

  Minor differences from Tour3D:
     * no alien
     * no full-screen
     * no keyboard-driven quitting; use the close box instead
     * the frame is 500x300 rather than 512x512
     * light blue background, not a texture
     * position the tourist with command-line args
*/

import javax.swing.*;
import java.awt.*;
import java.io.*;
import java.net.*;
import java.text.DecimalFormat;

import com.sun.j3d.utils.universe.*;
import com.sun.j3d.utils.geometry.*;
import com.sun.j3d.utils.image.TextureLoader;
import javax.media.j3d.*;
import javax.vecmath.*;

public class Content3D extends JPanel
// Holds the 3D canvas where the loaded image is displayed
{
  private static final int BOUNDSIZE = 100;  // larger than world

  private static final int PORT = 5000;      // server details
  private static final String HOST = "goodspeed.csie.ndhu.edu.tw";


  private SimpleUniverse su;
  private BranchGroup sceneBG;
  private BoundingSphere bounds;   // for environment nodes

  private Obstacles obs;
  private TourSprite bob;   // the tourist

  private Socket sock;
  private BufferedReader in;     // IO for the client
  private PrintWriter out;  
  private DecimalFormat df;   // for simpler output

  private static final int WIDTH = 800;     // size of panel
  private static final int HEIGHT = 600; 

  public Content3D(String userName, String tourFnm, 
                                      double xPosn, double zPosn)
  // construct the 3D canvas
  {
  	setLayout( new BorderLayout() );
    setOpaque( false );
    df = new DecimalFormat("0.####");  // 4 dp

    setPreferredSize( new Dimension(WIDTH, HEIGHT));
    GraphicsConfiguration config =
					SimpleUniverse.getPreferredConfiguration();
    Canvas3D canvas3D = new Canvas3D(config);
    add("Center", canvas3D);

    canvas3D.setFocusable(true);
    canvas3D.requestFocus();
   
    su = new SimpleUniverse(canvas3D);
    createSceneGraph(userName, tourFnm, xPosn, zPosn);

    su.addBranchGraph( sceneBG );
  } // end of WrapNetTour3D()


  void createSceneGraph(String userName, String tourFnm,
									double xPosn, double zPosn) 
  // initilise the scene
  { 
    sceneBG = new BranchGroup();
    bounds = new BoundingSphere(new Point3d(0,0,0), BOUNDSIZE); 

    // to allow clients to be added/removed from the world at run time
    sceneBG.setCapability(Group.ALLOW_CHILDREN_READ);
    sceneBG.setCapability(Group.ALLOW_CHILDREN_WRITE);
    sceneBG.setCapability(Group.ALLOW_CHILDREN_EXTEND);


    LightScene();         // add the lights
    addBackground();      // add the sky
    sceneBG.addChild( new CheckerFloor().getBG() );  // add the floor

    Scenery(tourFnm);      // add scenery and obstacles

    Connect();     // contact server (after Obstacles object created)

 	// add the user-controlled 3D sprite
    addRoles(userName, xPosn, zPosn);      
					  
    sceneBG.compile();   // fix the scene
  } // end of createSceneGraph()
	
// ------------------------------------------------------------------//
// ------------------------- Light ----------------------------------//
// -----------------------------------------------------------------// 
  private void LightScene() {
  	/* ambient light,directional lights */
  	Color3f white = new Color3f(1.0f,1.0f,1.0f);
  	
  	// Set up the ambient light
  	AmbientLight ambientLig = new AmbientLight(white);
  	ambientLig.setInfluencingBounds(bounds);
  	sceneBG.addChild(ambientLig);

	// Set up the directional lights
	// left, down, backwards
	Vector3f DirLig1 = new Vector3f(-1.0f, -1.0f, -1.0f); 
	// right,down, forwards
	Vector3f DirLig2 = new Vector3f(1.0f , -1.0f, 1.0f);
	
	DirectionalLight lig1 = new DirectionalLight(white,DirLig1);
	lig1.setInfluencingBounds(bounds);
	sceneBG.addChild(lig1);
	
	DirectionalLight lig2 = new DirectionalLight(white,DirLig2);
	lig2.setInfluencingBounds(bounds);
	sceneBG.addChild(lig2);
	
 } // end of LightScene
 
 
 
// -----------------------------------------------------------------//
// ------------------------- backgroudn ----------------------------//
// -----------------------------------------------------------------// 
 private void addBackground(){
 	/* Add a blue sky */
 	TextureLoader bgTex = new TextureLoader( "models/bigSky.jpg",null);
 	
 	Background bk = new Background(bgTex.getImage());
 	bk.setImageScaleMode(Background.SCALE_FIT_MAX);
 	bk.setApplicationBounds(bounds);
 //	bk.setColor(0.17f , 0.65f, 0.92f);
 	sceneBG.addChild(bk);
 	
 } // end of addBackground
 
 
 
// ---------------------------------------------------------------------//
// ------------------------- Obstacle ----------------------------------//
// ---------------------------------------------------------------------// 
 private void Scenery(String classFnm){
 	/*	Parse the class file which contains the name of scenery object to load,
 		and the position of obstacles.
 		format :
 		<object file>
 		[-o (x1,z1) (x2,z2) ... (xn,zn)]
 	*/
 	obs = new Obstacles();
 	LoaderManager LoaderMan;
 //	String classFile = "models/" + classFnm;
 	String classFile= System.getProperty("user.dir")+"/models/"+ classFnm;
 // String classFile = "http://goodspeed.csie.ndhu.edu.tw:8080:/OnlineEdu/models/"+ classFnm;
 	System.out.println("Loading class file : " + classFile );
 	try {
 		BufferedReader br = new BufferedReader(new FileReader(classFile));
 		String line;
 		while ((line = br.readLine())!= null){
 			System.out.println(line);
 			if (line.startsWith("-o"))
 				obs.store(line.substring(2).trim());	// save obstacle info
 			else{
 				LoaderMan = new LoaderManager(line.trim(),true);	// load scenery
 				sceneBG.addChild(LoaderMan.getTG());		// add to world
 			}	
 		} // end of while
 		br.close();
 		sceneBG.addChild(obs.getObsGroup());
 	}catch(IOException e){
 		System.out.println("Error reading class file : "+ classFile);
 		System.exit(1);
 	}
 } // end of makeScenery
 
// ------------------------------------------------------------------//
// ------------------------- role -----------------------------------//
// ------------------------------------------------------------------// 
private void addRoles(String userName, double xPos,double zPos){
	/* create sprite for this client */
	// sprite
	bob = new TourSprite(userName,"PEOPLE1.3ds",obs,xPos,zPos,out);
	sceneBG.addChild(bob.getBG());
	
	ViewingPlatform vp = su.getViewingPlatform();
	TransformGroup viewerTG = vp.getViewPlatformTransform(); // view position
	// sprite's controls
	TouristControls rcs = new TouristControls(bob,viewerTG);
	rcs.setSchedulingBounds(bounds);
	
	sceneBG.addChild(rcs);
	
} // end of addRole

public DistTourSprite addVisitor(String userName,double xPos,double zPos,double rotRads){
	/* Create a sprite for a visitor to the world.
	   This method is called from TourWatcher, in response to a 
	   requst form the TourServer.
	*/
	DistTourSprite dtSprite = new DistTourSprite(userName,"PEOPLE1.3ds",obs,xPos,zPos);
	if (rotRads != 0)
		dtSprite.setCurrRotation(rotRads);
		
	BranchGroup sBG = dtSprite.getBG();
	
	sBG.compile();
	try{
		Thread.sleep(200);
	}catch(InterruptedException e){}
	sceneBG.addChild(sBG);
	
	// just in case, but problem seems solved
	if (!sBG.isLive())
		System.out.println("Visitor sprite is not live");
	else
		System.out.println("Visitor sprite is now live");
	
	return dtSprite;
	
} // end of DistTourSprite



// ------------------------------------------------------------------//
// ------------------------- network --------------------------------//
// ------------------------------------------------------------------// 
 private void Connect(){
 	/* Connect the server, and set up a Watcher to monitor the server */
 	try{
 		sock = new Socket(HOST,PORT);
 		in = new BufferedReader( new InputStreamReader( sock.getInputStream()));
 		// autoflush
 		out = new PrintWriter(sock.getOutputStream(),true);
 		
 		new Watcher(this,in,obs).start(); // start watching for server msgs
 	}catch(Exception e){
 		System.out.println("No connect to server");
 		System.exit(0);
 	}
 } // end of Connected
 
 public void sendDetails(String cliAddr,String strPort){
 	/* Send details of local sprite to the client at location 
 	   Message format :
 	   details for cliAddr strPort xPos zPos rotRats
 	*/
 	Point3d currLoc = bob.getCurrLoc();
 	double currRotation = bob.getCurrRotation();
 	String msg = new String("detailsFor "+ cliAddr +
 	 " " + strPort + " "+
 	 df.format(currLoc.x) +" "+ df.format(currLoc.z)+" ");
 	 out.println(msg);
 } // end of sendDetails
 
 public void closeLink(){
 	/* kill Watcher thread */
 	try{
 		out.println("bye");
 		sock.close();
 	}catch(Exception e){
 		System.out.println("Link terminated");
 		System.exit(0);
 	}
 } // end of closeLink
} // end of Content3D class