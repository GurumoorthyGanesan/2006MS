

/* Watch and respond to messages coming from the server.
   Possible messages:

    * create <name> xPosn zPosn
         // create a visitor to the world on this machine

    * wantDetails cliAddr port
         // the client at (cliAddr, port) wants info about this client

    * detailsFor userName xPosn zPosn rotRadians
         // another client sends details about itself to this client

    * <name> <command>
         // <command> can be one of:
            forward | back | left | right | rotCClock | rotClock | bye


   A 'create' message is received from a new client when it is first
   created. The new client wants to be added to the local world.

   A 'wantDetails' message is received from a new client when it wants
   to find out about the current client. 
   A 'detailsFor' message will be sent out in response.

   A 'detailsFor' message is received from an existing client, after this
   client has sent out a 'wantDetails' request to the server. 
   For each 'detailsFor' message received, a new sprite will be 
   created.

   The other commands are mostly to move the sprites representing
   other clients.
*/

import java.io.*;
import java.net.*;
import java.util.*;


public class Watcher extends Thread
{
  private HashMap visitors;   // stores (user name, sprite object) pairs
  private Content3D w3d;
  private BufferedReader in;
  private Obstacles obs;


  public Watcher(Content3D w3d, BufferedReader i, Obstacles obs)
  { this.w3d = w3d;
    in = i;  
    this.obs = obs;
    visitors = new HashMap();   // no visitors yet
  }


  public void run()
  // read server messages and act on them.
  { String line;
    try {
      while ((line = in.readLine()) != null) {
        // System.out.println( line);
        if (line.startsWith("create"))
          createVisitor( line.trim() );
        else if (line.startsWith("wantDetails"))  // another client wants info
          sendDetails( line.trim() );
       else if (line.startsWith("detailsFor"))  // info for this client
          receiveDetails( line.trim() );
        else 
          doCommand( line.trim() );
      }
    }
    catch(Exception e)    // socket closure will cause termination of while
    { System.out.println("Link to Server Lost"); 
      System.exit( 0 );
    }
  }  // end of run()



  private void createVisitor(String line)
  /* Create a visitor to the world on this machine.
     Message format:
		create <name> xPosn zPosn

     The visitor has the name <name>, and their sprite will
     be located at (xPosn ,zPosn). The sprite will be the robot,
     oriented along the +z-axis.

     <name> and its sprite are stored in the visitors HashMap
  */
  { StringTokenizer st = new StringTokenizer(line);

    st.nextToken();   // skip 'create' word
    String userName = st.nextToken();
    double xPosn = Double.parseDouble( st.nextToken() );
    double zPosn = Double.parseDouble( st.nextToken() );
    System.out.println("Create: " + userName + "(" + xPosn + ", " + zPosn + ")" );

    if (visitors.containsKey(userName))
      System.out.println("Duplicate name -- ignoring it");   // should tell server
    else {
      System.out.println("Creating Sprite");
      DistTourSprite dtSprite = 
			w3d.addVisitor(userName, xPosn, zPosn, 0);
      visitors.put( userName, dtSprite);
    }
  }  // end of createVisitor()



  private void sendDetails(String line)
  /* Msg format:
		wantDetails cliAddr port
     The client at (cliAddr, port) wants info about this client.
     The reason is to render the client as a sprite.
     
     Ask WrapNetTour3D (w3d) to send the details in a 
    'detailsFor' message.
  */
  { StringTokenizer st = new StringTokenizer(line);
    st.nextToken(); // skip 'wantDetails' word
    String cliAddr = st.nextToken();
    String strPort = st.nextToken();   // don't parse

    w3d.sendDetails(cliAddr, strPort);
  }  // end of sendDetails()



  private void receiveDetails(String line)
  /* Another client sends details about itself to this client.
     The reason is to render the other client here on this machine.

     Message format:
		detailsFor userName xPosn zPosn rotRadians

     (xPosn,zPosn) is the position of the sprite, with rotRadians
     its rotation away from facing front.

     Store the visitor's name and sprite in the visitors HashMap
  */
  { StringTokenizer st = new StringTokenizer(line);
    st.nextToken(); // skip 'detailsFor' word
    String userName = st.nextToken();
    double xPosn = Double.parseDouble( st.nextToken() );
    double zPosn = Double.parseDouble( st.nextToken() );
    double rotRadians = Double.parseDouble( st.nextToken() );

    if (visitors.containsKey(userName))
      System.out.println("Duplicate name -- ignoring it");   // should tell server
    else {
      System.out.println("Making sprite for " + userName);
      DistTourSprite dtSprite = 
			w3d.addVisitor(userName, xPosn, zPosn, rotRadians);
      visitors.put( userName, dtSprite);
    }

  }  // end of receiveDetails()


  private void doCommand(String line)
  /* Message formats:
		<name> <command>
     where <command> can be one of:
        forward | back | left | right | rotCClock | rotClock | bye

     Most of these commands are to duplicate the movement of a
     client's sprite in the copy here.

     Also check if <name> is in the visitors HashMap
  */ 
  { StringTokenizer st = new StringTokenizer(line);
    String userName = st.nextToken();
    String command = st.nextToken();

    DistTourSprite dtSprite = (DistTourSprite) visitors.get(userName);
    if (dtSprite == null)
      System.out.println(userName + " is not here");
    else {
      if (command.equals("forward"))
        dtSprite.moveForward();
      else if (command.equals("back"))
        dtSprite.moveBackward();
      else if (command.equals("left"))
        dtSprite.moveLeft();
      else if (command.equals("right"))
        dtSprite.moveRight();
      else if (command.equals("rotCClock"))
        dtSprite.rotCounterClock();
      else if (command.equals("rotClock"))
        dtSprite.rotClock();
      else if (command.equals("bye")) {
        System.out.println("Removing info on " + userName);
        dtSprite.detach();
        visitors.remove(userName);
      }
      else
        System.out.println("Do not recognise the command");
    }
  }  // end of doCommand()


}  // end of Watcher