
/*
   The top-level Tour server, which waits for client connections
   and creates TourServerHandler threads to handle them.

   Details about each client are maintained in a TourGroup object
   which is referenced by each thread.
*/

import java.net.*;
import java.io.*;


public class Server
{
  static final int PORT = 5000;
  
  private Group tg;

  public Server()
  // wait for a client connection, spawn a thread, repeat
  {
    tg = new Group();
    try {
      ServerSocket serverSock = new ServerSocket(PORT);
      Socket clientSock;

      while (true) {
        System.out.println("Waiting for a client...");
        clientSock = serverSock.accept();
        new ServerHandler(clientSock, tg).start();
      }
    }
    catch(Exception e)
    {  System.out.println(e);  }
  }  // end of TourServer()


  // -----------------------------------

  public static void main(String args[]) 
  {  new Server();  }

} // end of Server class

