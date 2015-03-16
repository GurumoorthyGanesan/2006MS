/**
 * <p>Title: VoiceChat</p>
 * <p>Description: A voice chat client</p>
 * <p>The client consist of four parts: Receiver, Sender, GUI and Configuration </p>
 * @author Yong Yuan Lin
 * @version 09/23/04
 */



import java.net.*;
import javax.media.*;
import javax.swing.*;


class VoiceChat extends JFrame {
  private Receiver receiver;
  private Sender sender;
  private Thread guiThread;
  GUI gui;
  private Configuration config;
  private String remoteIp;
  private MediaLocator localLocator, remoteLocator;
  private int localPort, remotePort;

  /**
   * The constructor takes three arguments from the commandline
   * @param localPort port to listen to
   * @param remoteIp address to sendd to
   * @param remotePort port to send to
   */
  VoiceChat(String localPort, String remoteIp, String remotePort)  {
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
    dispose();
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
   * <p> Usage: java -jar VoiceChat.jar [<local port> <remote IP> <remote port>]
   * If no argument list is supplied localhost and port 2000 will be used.
   * The list och command line arguments is optional, as the values
   * can be set from the program when it's running </p>
   * @param args [<local port> <remote IP> <remote port>]
   */
  public static void main(String[] args) {
    if(args.length==3)
      new VoiceChat(args[0], args[1], args[2]);
    else
      if(args.length==0)
	try {
	new VoiceChat(""+2000, ""+InetAddress.getLocalHost().getHostAddress(), ""+2000);
      }
      catch (UnknownHostException ex) {
	System.err.println("The arguments are invalid: "+ex);
      }

    else {
      System.out.println("Usage: java -jar VoiceChat.jar [<local port> <remote IP> <remote port>]"
			 +"\n If no argument list is supplied localhost and port 2000 will be used.");
      System.exit(1);
    }
  }
}
