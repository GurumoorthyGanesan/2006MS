

/* Maintains info about all the current clients.

  A single TourGroup object is used by all the 
  TourServerHandler threads, so methods which
  manipulate the tourPeople ArrayList must be synchronised
  so that concurrent updates are prevented.

  TourGroup handles 
    * the addition/removal of client details
    * broadcasting, but not to the original sender
    * sendTo() for communication with one client

*/

import java.io.*;
import java.util.*;


public class Group
{
  private ArrayList People;    
          // holds a list of TouristInfo objects


  public Group()
  {  People = new ArrayList();  }


  synchronized public void addPerson(String cliAddr, int port, PrintWriter out)
  {  People.add( new Info(cliAddr, port, out));  }


  synchronized public void delPerson(String cliAddr, int port,
													String byeMsg)
  // remove tourist and send 'bye' msg to all the others
  { Info c;
    for(int i=0; i < People.size(); i++) {
      c = (Info) People.get(i);
      if (c.matches(cliAddr, port)) {
        People.remove(i);
        broadcast(cliAddr, port, byeMsg);
        break;
      }
    }
  }  // end of delPerson()


  synchronized public void broadcast(String cliAddr, int port, String msg)
  // broadcast to everyone but original msg sender
  {
    Info c;
    for(int i=0; i < People.size(); i++) {
      c = (Info) People.get(i);
      if (!c.matches(cliAddr, port))
        c.sendMessage(msg);
    }
  }  // end of broadcast()



  synchronized public void sendTo(String cliAddr, int port, String msg)
  // send msg to a single cliAddr/port
  { Info c;
    for(int i=0; i < People.size(); i++) {
      c = (Info) People.get(i);
      if (c.matches(cliAddr, port)) {
        c.sendMessage(msg);
        break;
      }
    }
  }  // end of sendTo()


}  // end of TourGroup class
