

/* A threaded TourServerHandler deals with a client.

   Details about a client are maintained in a TourGroup 
   object, which is referenced by all the threads.

   The TourGroup object handles message broadcasting
   via its broadcast() method.

   Possible client messages:
      * create name xPosn zPosn   -- a new client wishes to join the world

	  * detailsFor toAddr toPort xPosn zPosn rotRadians
            -- pass on info about this client's sprite to 
               the client at (toAddr,toPort)

      * bye           -- client is disconnecting

      * <text>        -- broadcast text with the client's name at its front


*/

import java.net.*;
import java.io.*;
import java.util.*;


public class ServerHandler extends Thread
{
  private Socket clientSock;
  private String cliAddr;
  private String userName;
  private int port;

  private Group tg;    // shared by all threads

  public ServerHandler(Socket s, Group tg)
  {
    this.tg = tg;
    clientSock = s;
    userName = "?";
    cliAddr = clientSock.getInetAddress().getHostAddress();
    port = clientSock.getPort();
    System.out.println("Client connection from (" + 
                 cliAddr + ", " + port + ")");
  } // end of TourServerHandler()


  public void run()
  // process messages from the client
  {
    try {
      // Get I/O streams from the socket
      BufferedReader in  = new BufferedReader( 
	  		new InputStreamReader( clientSock.getInputStream() ) );
      PrintWriter out = 
			new PrintWriter( clientSock.getOutputStream(), true );  // autoflush

      tg.addPerson(cliAddr, port, out);   // add client details to TourGroup

      processClient(in, out);        // interact with client
 
      // the client has finished when execution reaches here
      tg.delPerson(cliAddr, port, userName + " bye");         // remove client details
      clientSock.close();
      System.out.println("Client " + userName + " (" + 
                 cliAddr + ", " +  port +  ") connection closed");
    }
    catch(Exception e)
    {  System.out.println(e);  }
  }  // end of run()


   private void processClient(BufferedReader in, PrintWriter out)
   /* Stop when the input stream closes (is null) or "bye" is sent
      Otherwise pass the input to doRequest(). */
   {
     String line;
     boolean done = false;
     try {
       while (!done) {
         if((line = in.readLine()) == null)
           done = true;
         else {
           // System.out.println(userName + " received msg: " + line);
           if (line.trim().equals("bye"))
             done = true;
           else 
             doRequest(line.trim(), out);
         }
       }
     }
     catch(IOException e)
     {  System.out.println(e);  }
   }  // end of processClient()


  private void doRequest(String line, PrintWriter out)
  /*  The input line can be :
            create ...      -- there is a new client
			detailsFor ...
      or	text		    -- which is broadcast, with the client's
							   name at its front 
  */
  {
    if (line.startsWith("create"))
      sendCreate(line);
    else if (line.startsWith("detailsFor"))
      sendDetails(line);
    else  // use TourGroup object to broadcast the message
      tg.broadcast(cliAddr, port, userName + " " + line);
  }  // end of doRequest()


  private void sendCreate(String line)
  /* A new client wishes to join the world.

     This requires the client to find out about the existing
     clients, and to add itself to the other clients' worlds.

     Message format: create name xPosn zPosn

     Store the user's name, extracted from the "create" message
  */
  { StringTokenizer st = new StringTokenizer(line);
    st.nextToken();                  // skip 'create' word
    userName = st.nextToken();
    String xPosn = st.nextToken();   // don't parse
    String zPosn = st.nextToken();   // don't parse

    // request details from other clients
    tg.broadcast(cliAddr, port, "wantDetails " + cliAddr + " " + port);

    // tell other clients about the new one
    tg.broadcast(cliAddr, port, "create " + userName + " "+xPosn+" "+zPosn);
  } // end of sendCreate()



  private void sendDetails(String line)
  /* Send the details on this line to the named client.
     Input msg format:   
          detailsFor toAddr toPort xPosn zPosn rotRadians
     Output to TourGroup: 
          detailsFor userName xPosn zPosn rotRadians
  */
  { StringTokenizer st = new StringTokenizer(line);
    st.nextToken();                       // skip 'detailsFor' word
    String toAddr = st.nextToken();
    int toPort = Integer.parseInt( st.nextToken() );
    String xPosn = st.nextToken();        // don't parse
    String zPosn = st.nextToken();        // don't parse
    String rotRadians = st.nextToken();   // don't parse

    tg.sendTo(toAddr, toPort, 
			"detailsFor " + userName + " " + xPosn + " " + zPosn + 
								" " + rotRadians);
  } // end of sendDetails()

}  // end of TourServerHandler class
