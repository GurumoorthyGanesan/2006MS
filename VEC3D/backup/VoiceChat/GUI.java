/**
 * <p>Title: GUI </p>
 * <p>Description: A graphical user interface for the VoiceChat client.</p>
 * @author Jacob Hallenborg
 * @version 2002-09-22
 */



import java.awt.*;
import java.awt.event.*;
import javax.swing.*;

class GUI extends JFrame implements Runnable, ActionListener {

  private JPanel outerPane, statusPane, buttonPane;
  private JLabel rLabel, sLabel;
  private VoiceChat voiceChat;
  private JButton configButton;
  private JButton exitButton;

  /**
   * The constructor takes the VoiceChat client as argument.
   * @param voiceChat main program
   */
  public GUI(VoiceChat voiceChat) {
    this.voiceChat=voiceChat;
    // Wakes up the receiver, which is waiting
    synchronized(this) {
      notify();
    }
  }

  /**
   * Sets the value of the sending label.
   * @param str sendingstatus
   */
  void setSendingLabel(String str) {
    sLabel.setText(str);
  }

  /**
   * Sets the value of the receiving label.
   * @param str receiving status
   */
  void setReceivingLabel(String str) {
    rLabel.setText(str);
  }

  /**
   * Remove the player visual component.
   * @param comp the visual component to add
   */
  void removeVisComp(Component comp) {
    //if(comp!=null)
      //getContentPane().remove(comp);
  }

  /**
   * Adds the player visual component.
   * @param comp the visual component to remove
   */
  void addVisComp(Component comp) {
    getContentPane().add(comp, BorderLayout.SOUTH);
    setVisible(true);
  }

  /**
   * Stops the GUI
   */
  void halt() {
    dispose();
  }

  /**
   * Handles the button events.
   * @param e the button event
   */
  public void actionPerformed(ActionEvent e) {
    if(e.getSource()==exitButton)
      voiceChat.exit();
    else if(e.getSource()==configButton) {
      voiceChat.configure();
    }
  }


  /**
   * Starts the GUI thread
   */
  public void run() {
    rLabel = new JLabel("-------------", SwingConstants.LEFT);
    sLabel = new JLabel("-------------", SwingConstants.RIGHT);

    configButton = new JButton("Configure");
    configButton.addActionListener(this);

    exitButton = new JButton("Exit");
    exitButton.addActionListener(this);

    outerPane = new JPanel();
    outerPane.setLayout(new BoxLayout(outerPane, BoxLayout.Y_AXIS));

    statusPane = new JPanel();
    statusPane.setLayout(new BoxLayout(statusPane, BoxLayout.X_AXIS));
    statusPane.setBorder(BorderFactory.createLineBorder(new Color(255,106,0),2));
    statusPane.add(Box.createRigidArea(new Dimension(10,0)));
    statusPane.add(rLabel);
    statusPane.add(Box.createRigidArea(new Dimension(25,0)));
    statusPane.add(sLabel);
    statusPane.add(Box.createRigidArea(new Dimension(10,0)));

    buttonPane = new JPanel();
    buttonPane.setLayout(new BoxLayout(buttonPane, BoxLayout.X_AXIS));
    buttonPane.add(configButton);
    buttonPane.add(Box.createRigidArea(new Dimension(10,0)));
    buttonPane.add(exitButton);

    outerPane.setBorder(BorderFactory.createEmptyBorder(10,10,10,10));
    outerPane.add(statusPane);
    outerPane.add(Box.createRigidArea(new Dimension(0,20)));
    outerPane.add(buttonPane);
    outerPane.add(Box.createRigidArea(new Dimension(0,20)));

    getContentPane().add(outerPane);
    setTitle("VoiceChat");
    setLocation(200,200);
    pack();
    setVisible(true);
  }
}