
/*
 * <p>Title: Receiver </p>
 * @Author Jacob Hallenborg
 * @Data 24/07/04
 * <p>Description: Receives an RTP-stream and shows it for the user 
 * through a Player.</p>
 */



import java.io.*;
import javax.media.*;
import java.net.*;
import java.awt.*;
import javax.swing.*;

public class Receiver extends Thread implements ControllerListener {
  private Player player;
  private VoiceChat voiceChat;
  private Component visComp;

  /**
   * The constructor takes the VoiceChat client as argument.
   * @param voiceChat main program
   */
  Receiver(VoiceChat voiceChat) {
    this.voiceChat = voiceChat;
  }

  /**
   * Rerurns the player.
   * @return player
   */
  Player getPlayer() {
    return player;
  }

  /**
   * Stops the player.
   */
  void halt() {
    if(player!=null) {
      voiceChat.gui.removeVisComp(visComp);
      player.stop();
      player.deallocate();
      player.close();
    }
    if(voiceChat.gui!=null)
      voiceChat.gui.setReceivingLabel("-----------");
  }

  /**
   *  Starts the receiver thread.
   */
  public void run() {
    try {
      player = Manager.createPlayer(voiceChat.getLocalLocator());
      player.addControllerListener(this);
      player.realize();
      player.prefetch();
      player.start();
    }
    catch(Exception e) {
      System.err.println(e.getMessage()); e.printStackTrace();
      voiceChat.exit();
    }
   }



   /**
    * Waits for the player to reach Started-status before invoking methods from it.
    * @param e StartEvent
    */
   public void controllerUpdate(ControllerEvent e) {
     if(e instanceof StartEvent) {
       // Waits for the GUI to be ready, before invoking methods from it.
       synchronized(voiceChat.gui) {
	 try {
	   while(voiceChat.gui==null) {
	     voiceChat.gui.wait();
	   }
	 }
	 catch (InterruptedException ex) {
	   System.err.println("7"+ex);
	   voiceChat.exit();
	 }
       }
       voiceChat.gui.setReceivingLabel("Receiving");
       visComp = player.getControlPanelComponent();
       voiceChat.gui.addVisComp(visComp);
       voiceChat.gui.pack();
     }
   }

}