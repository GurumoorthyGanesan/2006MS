/**
 * <p>Title: VoiceChat</p>
 * @author Y.Y.L
 * @Data 07/25/04
 * <p>Description: A voice chat client</p>
 * <p>The client consist of four parts: Receiver, Sender, GUI and Configuration </p>
 */

// IMPORT
import java.awt.*;
import java.net.*;
import javax.media.*;
import javax.swing.*;
import javax.vecmath.*;
import javax.media.j3d.*;

import com.sun.j3d.utils.applet.MainFrame;
import com.sun.j3d.loaders.Scene;
import com.sun.j3d.utils.behaviors.vp.*;
import com.sun.j3d.utils.universe.*;
import com.sun.j3d.loaders.objectfile.ObjectFile;
import com.sun.j3d.utils.applet.MainFrame;
import javax.swing.JOptionPane;


public class VoiceChat extends java.applet.Applet {
  private Receiver receiver;
  private Sender sender;
  private Thread guiThread;
  GUI gui;
  private Configuration config;
  private String remoteIp;
  private MediaLocator localLocator, remoteLocator;
  private int localPort, remotePort;
  private JPanel buttonPanel,TextAreaPane;
  private JTextField TextAreaField;
  
  private SimpleUniverse u;
  private BoundingSphere bounds;
  
   /** Initialization method that will be called after the applet is loaded
     *  into the browser.
     */
    public void init() {
    	// Build the VoiceChat
    	try{
    		// @param args [<local port> <remote IP> <remote port>]
    		VoiceChatMain(""+2000, ""+InetAddress.getLocalHost().getHostAddress(), ""+2000);
    	}catch (UnknownHostException ex) {
			System.err.println("The arguments are invalid: "+ex);
      	}
      	
      	// Build the 3DWorld
    	Build3DWorld();
    } // end of init() 
    
    /** 
     * Build3DWorld method 
     */
    private void Build3DWorld(){
    	OrbitBehavior B;
        ViewingPlatform ourView;
        
        GraphicsConfiguration config = SimpleUniverse.getPreferredConfiguration();
        bounds = new BoundingSphere(new Point3d(0.0, 0.0, 0.0), 100.0);
      
        //---Layout management-----------------------------------
        setLayout(new BorderLayout());
        Canvas3D c = new Canvas3D(config);
           
        setLayout(new BorderLayout());
   		TextAreaPane = new JPanel();
    	TextAreaField = new JTextField("Input", 32);
    	TextAreaPane.add(new JLabel("Messages :"));
    	TextAreaPane.setLayout(new BoxLayout(TextAreaPane, BoxLayout.X_AXIS));
    	TextAreaPane.add(TextAreaField);
 
    	add(TextAreaPane,BorderLayout.SOUTH);
    	// -------------------------------------------------------
    	add(new Button("East"), BorderLayout.EAST);
        add(c,BorderLayout.CENTER);
        //--------------------------------------------------------
        SimpleUniverse u = new SimpleUniverse(c);
        
 BranchGroup scene = createSceneGraph(); 
 
	 	u.addBranchGraph(scene);
         
        ourView = u.getViewingPlatform();

		// 設定視埠
        Transform3D locator = new Transform3D();
        locator.setTranslation(new Vector3f(0, 3f, -3f));
        locator.lookAt(new Point3d(0d, 3d, -3d), new Point3d(0d, 0d, 5d), new Vector3d(0d, 1d, 0d));
        locator.invert();
        ourView.getViewPlatformTransform().setTransform(locator);
        
        // 根據mouse 調整視埠
        B = new OrbitBehavior(c);
        B.setSchedulingBounds(bounds);
        ourView.setViewPlatformBehavior(B);
        System.out.println("Demo ...");
        
    } // end of method Build3DWorld
    
    private BranchGroup createSceneGraph() 
    {
        BranchGroup objRoot = new BranchGroup();
        TransformGroup dudeGroup = new TransformGroup();
       
        dudeGroup.setCapability(TransformGroup.ALLOW_TRANSFORM_WRITE);
        dudeGroup.setCapability(TransformGroup.ALLOW_TRANSFORM_READ);
      
       	/*
       	 * Load shape file
       	 */
    
        String filePath = "http://134.208.27.166:8080/VoiceChat/";
     
        // Create a loader
        ObjectFile loader = new ObjectFile(ObjectFile.RESIZE);

        Scene scene = null;
        Shape3D shape;
        // default we have three different object
		GeometryArray geoms[] = new GeometryArray[3];


		for(int i=0; i<3; i++) 
	    	geoms[i] = null;

        try
        {
        	// loading file (three)
            java.net.URL objFiles[] = new java.net.URL[3];
            objFiles[0] = new java.net.URL(filePath+"models/msFigureStep1.obj");
            objFiles[1] = new java.net.URL(filePath+"models/msFigureStanding.obj");
            objFiles[2] = new java.net.URL(filePath+"models/msFigureStep2.obj");

            for(int i=0; i<3;i++){
                try {
                      scene = loader.load(objFiles[i]);
                }
                catch (Exception e){
                     e.printStackTrace();
                     System.exit(1);
                }
                BranchGroup b = scene.getSceneGroup();
                shape = (Shape3D) b.getChild(0);
                geoms[i] = (GeometryArray) shape.getGeometry();
                }
         }
        catch (Exception e)
        {
                System.err.println("could not load file! "+filePath);
                e.printStackTrace();
        }

		
       	Morph ourMorph = new Morph(geoms);
	    ourMorph.setCapability(Morph.ALLOW_WEIGHTS_READ);
	    ourMorph.setCapability(Morph.ALLOW_WEIGHTS_WRITE);
        ourMorph.setCapability(Morph.ALLOW_GEOMETRY_ARRAY_READ);
        
        // appearance 設定
        Appearance app = new Appearance();
        Color3f objColor = new Color3f(0.7f, 0.2f, 0.8f);
        Color3f black = new Color3f(0.0f, 0.0f, 0.0f);
        app.setMaterial(new Material(objColor, black, objColor, black, 80.0f));
        ourMorph.setAppearance(app);
        

		Alpha morphAlpha = new Alpha(-1, Alpha.INCREASING_ENABLE |
				     Alpha.DECREASING_ENABLE,
				     0, 0,
				     600, 100, 100,
				     600, 100, 100);

        dudeGroup.addChild(ourMorph);  
        
        // 設定移動物件的控制
		MoveMorph mBeh = new MoveMorph(morphAlpha, ourMorph, dudeGroup);
		mBeh.setSchedulingBounds(bounds);
        
        dudeGroup.addChild(mBeh);

		// 一般光源設定
        Color3f alColour = new Color3f(0.4f, 0.4f, 0.4f);
		AmbientLight aLgt = new AmbientLight(alColour);
        aLgt.setInfluencingBounds(bounds);
        
        Color3f DirColour = new Color3f(0.35f, 0.35f, 0.3f);
		Vector3f DirVec  = new Vector3f(0.5f, -0.2f, 0.6f);

		DirectionalLight DirLig = new DirectionalLight(DirColour, DirVec);
		DirLig.setInfluencingBounds(bounds);

		// load Geometry background & texture
        String texturePath=filePath+"stone.jpg"; 
        SimpleLand s = new SimpleLand(10, 10, texturePath);
        
        objRoot.addChild(DirLig);
        objRoot.addChild(aLgt);
        objRoot.addChild(dudeGroup);
        objRoot.addChild(s);
        objRoot.compile();
        return objRoot;
    }
    
        
    public void destroy() {
		u.removeAllLocales();
    }
    
 
  public VoiceChat(){
  }

   /**
   * The  VoicChatMain method
   * @param localPort port to listen to
   * @param remoteIp address to sendd to
   * @param remotePort port to send to
   */
  public void VoiceChatMain(String localPort, String remoteIp, String remotePort)  {
    try {
      if(Integer.parseInt(localPort) < 1024  || Integer.parseInt(localPort) > 65535 ||
	 Integer.parseInt(remotePort) < 1024 || Integer.parseInt(remotePort) > 65535) {
	System.err.println("Invalid port number. Value must be between 1024 and 65535");
	System.exit(1);
      }
      this.localPort = Integer.parseInt(localPort);
      this.remoteIp = remoteIp;
      this.remotePort = Integer.parseInt(remotePort);
      initialize();
    }
    catch (NumberFormatException ex) {
      System.err.println("The arguments are invalid: "+ex);
      exit();
    }
  }
  
  // Initializes all the parts of the client
  private void initialize() {
    try {
      localLocator = new MediaLocator("rtp://" + InetAddress.getLocalHost().getHostAddress()
				      + ":" + localPort + "/audio");
      remoteLocator = new MediaLocator("rtp://" + remoteIp + ":" + remotePort +"/audio");
    }
    catch (UnknownHostException ex) {
      System.err.println("There was a problem with the addresses: "+ex);
      exit();
    }
    // Paints the GUI, only when the client is started the first time
    if(gui==null) {
      gui = new GUI(this);
      guiThread = new Thread(gui);
      guiThread.start();
    }
    receiver = new Receiver(this);
    receiver.start();
    sender = new Sender(this);
    sender.start();
  }

  /**
   * Stops all parts, except for the GUI
   */
  void halt() {
    config=null;
    if(sender!=null)
      sender.halt();
    sender = null;
    if(receiver!=null)
      receiver.halt();
    receiver = null;
  }

  /**
   * Stops and restarts all parts, except for the GUI
   */
  void restart() {
    halt();
    initialize();
  }

  /**
   * Halts the client
   */
  void exit() {
    halt();
    if(gui!=null)
      gui.halt();
    guiThread=null;
//    dispose();
    System.exit(0);
  }

  /**
   * Returns remoteIp
   */
  String getRemoteIp() { return remoteIp;}

  /**
   * Returns localLocator
   */
  MediaLocator getLocalLocator() { return localLocator;}

  /**
   * Returns remoteLocator
   */
  MediaLocator getRemoteLocator() { return remoteLocator;}

  /**
   * Returns localPort
   */
  int getLocalPort() { return localPort; }

  /**
   * Returns @remotePort
   */
  int getRemotePort() { return remotePort; }


  /**
   * Set the local port to @param port
   */
  void setLocalPort(int port) {
    localPort=port;
  }

  /**
   * Sets the remote port to @param port
   */
  void setRemotePort(int port) {
    remotePort=port;
  }

  /**
   * Sets th remote IP to @param ip
   */
  void setRemoteIp(String ip) {
    remoteIp=ip;
  }

  /**
   * Opens the configuration window
   */
  void configure() { config = new Configuration(this); }

  /**
   * <p> Usage: java -jar VoiceChat.jar
   */
  public static void main(String[] args) {
		new MainFrame(new VoiceChat(),400,300);
  } // end of method main()
}
