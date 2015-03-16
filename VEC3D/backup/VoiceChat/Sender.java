/**
 * <p>Title: Sender </p>
 * <p>Description: Captures sound from microphone device and sends as RTP-stream</p>
 * @author Jacob Hallenborg
 * @version 2002-09-22
 */


import java.util.*;
import javax.media.*;
import javax.media.format.*;
import javax.media.protocol.*;


class Sender extends Thread {
  private VoiceChat voiceChat;
  private CaptureDeviceManager manager;
  private CaptureDevice device;
  private Format format;
  private Vector vector;
  private MediaLocator captureIn, dest;
  private DataSource source;
  private final ContentDescriptor CONTENT_DESCRIPTOR =
      new ContentDescriptor(ContentDescriptor.RAW_RTP);
  private Processor processor;
  private DataSink sink;
  private String url;

  /**
   * The constructor takes the VoiceChat client as argument.
   * @param voiceChat
   */
  Sender(VoiceChat voiceChat){
    this.voiceChat = voiceChat;
    try {
      manager = new CaptureDeviceManager();
      format = new AudioFormat("linear", 44100, 16, 2);
      vector = CaptureDeviceManager.getDeviceList(format);
      if(vector.size()==0) {
	System.err.println("Audio format error.");
	voiceChat.exit();
      }
      captureIn = ((CaptureDeviceInfo) vector.elementAt(0)).getLocator();
      source = Manager.createDataSource(captureIn);
    }
    catch(Exception e) {
      System.err.println("5:\t"+e);
      System.err.println(e.getMessage());
      e.printStackTrace();
      voiceChat.exit();
    }
  }

  /**
   * Stops the processor.
   */
  void halt() {
    processor.close();
    processor.stop();
    if(voiceChat.gui!=null)
      voiceChat.gui.setSendingLabel("-----------");
  }

  /**
   * Starts the processor.
   */
  public void run() {
    try {
      dest = voiceChat.getRemoteLocator();
      processor = Manager.createRealizedProcessor(
	  new ProcessorModel(source, new Format[] {format}, CONTENT_DESCRIPTOR));
      sink = Manager.createDataSink(processor.getDataOutput(), dest);
      sink.open();
      sink.start();
      processor.start();
      // waits for the GUI to be ready, before invoking it's methods
      synchronized(voiceChat.gui) {
	while(voiceChat.gui==null)
	  voiceChat.gui.wait();
      }
      voiceChat.gui.setSendingLabel("Sending");
    }
    catch(Exception ex) {
      System.err.println("6:\t"+ex);
      System.err.println(ex.getMessage());
      voiceChat.exit();
    }

  }

}