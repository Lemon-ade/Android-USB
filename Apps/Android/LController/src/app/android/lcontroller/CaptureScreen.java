package app.android.lcontroller;

public class CaptureScreen {
	private final int PORT = 4001;
	private final int JNI_SUCCESS = 0;
	private final int JNI_FAIL = -1;
	
	private boolean isServerOn;
	private boolean isAccepting;
	private String clientIP;
	private captureThread captureThr;
	private boolean captureStop;
	
	private native int TCPConnect_listen_server(int port);
	private native String TCPConnect_accept_client();
	private native int TCPConnect_recv_msg();
	private native int TCPConnect_send_screen();
	private native int TCPConnect_close();
	private native String getErrorMsg(); 
	
	public CaptureScreen() {
		System.loadLibrary("ndk-control");
		isServerOn = false;
		isAccepting = false;
		captureStop = false;
		captureThr = new captureThread();
	}
	
	public boolean serverOn() {
		int ret;
		if(isServerOn)
			return true;
		ret = TCPConnect_listen_server(PORT);
		if(ret == JNI_SUCCESS) {
			isServerOn = true;
			return true;
		}
		else {
			isServerOn = false;
			return false;
		}
	}
	
	public String accept() {
		if( !isServerOn || isAccepting || isConnect() )
			return null;
		
		isAccepting = true;
		clientIP = TCPConnect_accept_client();
		isAccepting = false;
		
		if(clientIP!=null) {
			captureThr.start();
			return clientIP;
		}
		else
			return null;
	}
	
	public boolean serverOff() {
		return true;
	}
	
	public boolean isConnect() {
		if(clientIP!=null)
			return true;
		else
			return false;
	}
	
	public boolean disconnect() {
		if(!isConnect())
			return false;
		
		captureStop = true;
		return true;
	}
	
	private class captureThread extends Thread {
		public captureThread() {
			captureStop = false;
		}
		
		public void run() {
			while(!captureStop) {
				if(TCPConnect_recv_msg()<0) {
					String test = getErrorMsg();
					break;
				}
				if(TCPConnect_send_screen()<0) {
					String test = getErrorMsg();
					break;
				}
			}
			TCPConnect_close();
			clientIP = null;
		}
	}
	
	public String getErrMsg() {
		return getErrorMsg();
	}
}
