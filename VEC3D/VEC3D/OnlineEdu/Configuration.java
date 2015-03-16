/**
 * <p>Title: Configuration </p>
 * <p>Description: A window for configuring the values och Receiver and Sender.</p>
 * @author Jacob Hallenborg
 * @version 2002-09-22
 */



import java.awt.*;
import java.awt.event.*;
import javax.swing.*;

public class Configuration extends JDialog implements ActionListener {

  private VoiceChat voiceChat;
  private JPanel outerPane, localPortPane, remoteIpPane, remotePortPane, buttonPane;
  private JLabel titleLabel, localPortLabel, remoteIpLabel, remotePortLabel, messageLabel;
  private JTextField localPortField, remotePortField, remoteIpField;
  private JButton okButton, cancelButton;


  /**
   * The constructor takes the VoiceChat client as argument.
   * @param voiceChat
   */
  public Configuration(VoiceChat voiceChat) {
    this.voiceChat=voiceChat;

    outerPane = new JPanel();
    localPortPane = new JPanel();
    remoteIpPane = new JPanel();
    remoteIpPane = new JPanel();
    buttonPane = new JPanel();
    remotePortPane = new JPanel();
    messageLabel = new JLabel("", SwingConstants.CENTER);
    messageLabel.setAlignmentX(Component.CENTER_ALIGNMENT);
    outerPane.setLayout(new BoxLayout(outerPane, BoxLayout.Y_AXIS));
    outerPane.setBorder(BorderFactory.createEmptyBorder(10, 10, 10, 10));
    localPortPane.setLayout(new BoxLayout(localPortPane, BoxLayout.X_AXIS));
    remoteIpPane.setLayout(new BoxLayout(remoteIpPane, BoxLayout.X_AXIS));
    remotePortPane.setLayout(new BoxLayout(remotePortPane, BoxLayout.X_AXIS));
    buttonPane.setLayout(new BoxLayout(buttonPane, BoxLayout.X_AXIS));
    titleLabel = new JLabel("   Configuration   ", SwingConstants.CENTER);
    titleLabel.setAlignmentX(Component.CENTER_ALIGNMENT);

    localPortField = new JTextField(""+voiceChat.getLocalPort(), 6);
    remotePortField = new JTextField(""+voiceChat.getRemotePort(), 6);
    remoteIpField = new JTextField(voiceChat.getRemoteIp(), 10);
    okButton = new JButton("Ok");
    cancelButton = new JButton("Cancel");
    okButton.addActionListener(this);
    cancelButton.addActionListener(this);
    titleLabel.setBorder(BorderFactory.createLineBorder(new Color(255,106,0),2));

    localPortPane.add(new JLabel("Local port:"));
    localPortPane.add(Box.createRigidArea(new Dimension(63,0)));
    localPortPane.add(localPortField);

    remoteIpPane.add(new JLabel("Remote IP:"));
    remoteIpPane.add(Box.createRigidArea(new Dimension(30,0)));
    remoteIpPane.add(remoteIpField);

    remotePortPane.add(new JLabel("Remote port:"));
    remotePortPane.add(Box.createRigidArea(new Dimension(50,10)));
    remotePortPane.add(remotePortField);

    buttonPane.setBorder(BorderFactory.createEmptyBorder(15,15,5,5));
    buttonPane.add(okButton);
    buttonPane.add(cancelButton);

    outerPane.add(titleLabel);
    outerPane.add(Box.createRigidArea(new Dimension(0,20)));
    outerPane.add(localPortPane);
    outerPane.add(remoteIpPane);
    outerPane.add(remotePortPane);
    outerPane.add(buttonPane);
    outerPane.add(messageLabel);
    outerPane.add(Box.createRigidArea(new Dimension(0,10)));

    getContentPane().add(outerPane);
    setResizable(false);
    pack();
    setVisible(true);
  }

  /**
   * Handles the button events
   * @param e
   */
  public void actionPerformed(ActionEvent e) {
    if(e.getSource()==okButton)
      if(checkAndSetValues()) {
        voiceChat.restart();
        dispose();
      }
    if(e.getSource()==cancelButton)
      dispose();
  }

  // Checks the user input. If a value is invalid, an error message is shown.
  // Otherwise, the values are set in VoiceChat which reinitializes.
  private boolean checkAndSetValues() {

    // Local port
    int localPort;
    try {
      localPort = Integer.parseInt(localPortField.getText());
      if(localPort < 1024 || localPort > 65535)
	throw new NumberFormatException("");
    }
    catch(NumberFormatException e) {
      messageLabel.setText("The local port value is invalid");
      messageLabel.setForeground(Color.red);
      return false;
    }

    // Remote port
    int remotePort;
    try {
      remotePort = Integer.parseInt(remotePortField.getText());
      if(remotePort < 1024 || remotePort > 65535)
	throw new NumberFormatException("");
    }
    catch(NumberFormatException e) {
      messageLabel.setText("The remote port value is invalid");
      return false;
    }

    // Remote IP
    String remoteIp= remoteIpField.getText();
    String[] ipParts =  remoteIp.split("\\.");
    if(ipParts.length!=4) {
      messageLabel.setText("The remote IP value is invalid");
      return false;
    }
    for(int i=0; i<4 ; i++) {
      int part = Integer.parseInt(ipParts[i]);
      if(part < 0 || part > 255) {
	messageLabel.setText("The remote IP value is invalid");
	return false;
      }
    }

    // Sets the values
    voiceChat.setRemoteIp(remoteIp);
    voiceChat.setRemotePort(remotePort);
    voiceChat.setLocalPort(localPort);
    return true;
  }

}