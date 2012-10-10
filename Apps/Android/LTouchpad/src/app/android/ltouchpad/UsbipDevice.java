package app.android.ltouchpad;

import java.io.IOException;
import android.util.Log;

public abstract class UsbipDevice {
	
	private static final int MAX_CLIENT = 5;
	private static final String STR_USBIPD_START = "usbipd start";
	private static final String STR_BINDD_START = "bind to usbip, complete!";
	private static boolean isServerOn;
	
	private boolean connection;
	private boolean clientConnection;
	
	private ThreadExec proc_USBIPD;
	private ThreadExec proc_BINDD;
	private static int sock_bindd;
	private static int sock_usbipd;
	private String usbip_port;
	protected RecvThread recvThread;
	
	public static final int ERRCODE_USBIPD = 1;
	public static final int ERRCODE_BINDD = 2;
	public static final int ERRCODE_USBIPCONNECT = 3;
	public static final int ERRCODE_CLIENTCONNECT = 4;
	public static final int ERRCODE_DISCONNECT = 5;
	
	public native int server_bind(int max_client);
	public native int listen_client();
	public native String recv_sock(int sock);
	
	protected native int connect_usbip();
	protected native int recv_enum();
	protected native int send_enum(int desc);
	protected native int is_sendable();
	protected native int recv_ack();
	protected native int send_control(byte[] buffer);
	protected native String get_message();
	
	protected abstract void onBinddOpen();
	protected abstract void onBinddSuccess();
	protected abstract void onClientSuccess();
	protected abstract void onError(int errCode, String errMsg);
	
	public UsbipDevice(String usbip_port) {
		System.loadLibrary("usb-device");
		this.usbip_port = usbip_port;
		recvThread = new RecvThread(this);
		connection = false;
	}
	
	private boolean startListen() {
		if( !isServerOn ) {
			int ret;
			Log.i("LTouchPad", "ShellProc startListen");
			
			if((ret = server_bind(MAX_CLIENT))<0) {
				Log.e("LTouchPad", "bind_error " + ret);
				return false;
			}
		}
		isServerOn = true;
		return true;
	}
	
	public void on() {
		if(!isServerOn) {
			startListen();
		}
		openUsbipd();
	}
	
	public boolean isUsbipConnect() {
		return connection;
	}
	
	public boolean isClientConnect() {
		return clientConnection;
	}
	
	public void connectUsbip() {
		if(connect_usbip()<0) {
			Log.d("LTouchPad", "connect usbip Error");
			connection = false;
			onError(ERRCODE_USBIPCONNECT, null);
			//sendHandler(Handler_What_ErrorMessage, Handler_ErrorMsg_Usbip);
		}
		else {
			Log.d("LTouchPad", "connect usbip Success");
			connection = true;
			onBinddSuccess();
		}
	}
	
	protected void closeConnection() {
		clientConnection = false;
		Log.d("LTouchPad", "closeConnection()");
	}
	
	public boolean recvAck() {
		int ret;
		if(!clientConnection)
			return false;
		
		ret=recv_ack();
		Log.d("LTouchPad", "recvAck : " + ret);
		if(ret<0) {
			Log.e("LTouchPad", "client disconnected");
			onError(ERRCODE_DISCONNECT, null);
			return false;
		}
		else
			return true;
	}
	
	private void openUsbipd() {
		new Thread ( new Runnable() {
			public void run() {
				Log.d("LTouchPad", "USBIP listen_start");
				sock_usbipd = listen_client();
				if(sock_usbipd<0) {
					Log.e("LTouchPad", "listen_usbipd error");
					return;
				}
				Log.i("LTouchPad", "listen success");
				recvUSBIPD();
			} // USBIPD Thread run()
		}).start();
		proc_USBIPD = new ThreadExec("usbipd", this);
		proc_USBIPD.start();
	}
	
	private void openBindd() {
		new Thread ( new Runnable() {
			public void run() {
				Log.d("LTouchPad", "BINDDRIVER listen_start");
				sock_bindd = listen_client();
				if(sock_bindd<0) {
					Log.e("LTouchPad", "listen_binddriver error");
					return;
				}
				Log.i("LTouchPad", "bind listen success");
				recvBINDDRIVER();
			} // BINDDRIVER Thread run()
		}).start(); // BINDDRIVER Thread
		proc_BINDD = new ThreadExec( "usbip_bind_driver --usbip", this);
		proc_BINDD.setPort(usbip_port);
		proc_BINDD.start();
	}
	
	private void recvUSBIPD() {
		new Thread ( new Runnable() {
			String str=null;
			
			public void run() {
				while(true) {
					str = recv_sock(sock_usbipd);
					if(str==null || str=="") {
						Log.e("LTouchPad", "recvUSBIPD Error");
						onError(ERRCODE_USBIPD, "CONNCECT ERROR");
						break;
					}
					else if(str.startsWith("err")) {
						Log.e("LTouchPad", "usbipd err: " + str);
						onError(ERRCODE_USBIPD, str);
					}
					else {
						if(str.startsWith(STR_USBIPD_START))
							openBindd();
						Log.d("LTouchPad", "recv(usbipd) : " + str);
					}
				}
			}
		}).start();
	}
	
	private void recvBINDDRIVER() {
		new Thread ( new Runnable() {
			String str=null;
			
			public void run() {
				while(true) {
					str = recv_sock(sock_bindd);
					if(str==null || str=="") {
						Log.e("LTouchPad", "recvBINDDRIVER Error");
						onError(ERRCODE_BINDD, "BINDD CONNCECT ERROR");
						break;
					}
					else if(str.startsWith("err")) {
						Log.e("LTouchPad", "recv err: " + str);
						onError(ERRCODE_BINDD, str);
					}
					else {
						if(str.startsWith(STR_BINDD_START)) {
							// bind driver start
						}
						Log.d("LTouchPad", "recv(bindd) : " + str);
					}
				}
			}
		}).start();
	}
	
	protected void process_cmd() {
		
		new Thread(new Runnable() {
			int desc;
    		public void run() {
    			for(int i=0; i<7; i++) {
    				Log.d("LTouchPad", "recv_enum (i:" + i+")");
    				desc = recv_enum();
    				Log.d("LTouchPad", "recv_enum (desc:" + desc+")");
    				new Thread(new Runnable() {
    		    		public void run() {
    		    			Log.d("LTouchPad", "send_enum (desc:" + desc+")");
    		    			if(send_enum(desc)<0) {
    		    				clientConnection = false;
    		    				onError(ERRCODE_CLIENTCONNECT, null);
    		    			}
    		    			else
    		    				Log.d("LTouchPad", "send_enum (desc:" + desc+") success");
    		    		}
    		    	}).start();
    			}
    			Log.d("LTouchPad", "process_cmd() success");
    			clientConnection =true;
    			recvThread.start();
    			onClientSuccess();
    		}
    	}).start();
	}
	
	class RecvThread extends Thread {
		private UsbipDevice usbipDevice;
		
		public RecvThread(UsbipDevice usbipDevice) {
			this.usbipDevice = usbipDevice;
		}
		
		public void run() {
			while(true) {
				if(!usbipDevice.recvAck()) {
					closeConnection();
					break;
				}
			}
		}
	}
}

class ThreadExec extends Thread {
	String cmd;
	String usbipPort;
	Process proc;
	Runtime runtime;
	UsbipDevice usbipDevice;

	public ThreadExec(String cmd, UsbipDevice usbipDevice) {
		this.cmd = cmd;
		this.usbipDevice = usbipDevice;
		usbipPort = "";
		runtime = Runtime.getRuntime();
	}
	
	public void setPort(String port) {
		this.usbipPort = port;
	}
	
	public String getPort() {
		return usbipPort;
	}
	
	public void destroyProc() {
		if(proc==null)
			return;
		try {
			Log.d("LTouchPad", "destroyProc()");
			proc.getInputStream().close();
			proc.getOutputStream().close();
			proc.getErrorStream().close();
			try {
				proc.waitFor();
			} catch (InterruptedException e) {
				// TODO Auto-generated catch block
				e.printStackTrace();
			}
			proc.destroy();
			proc = null;
		} catch (IOException e) {
			Log.e("LTouchPad", "IOException-exec()");
			e.printStackTrace();
		}
	}

	public void run() {
		try {
			proc = runtime.exec("su");
			proc = runtime.exec(cmd + " " + usbipPort);
			Log.d("LTouchPad", cmd + " " + usbipPort + " end");
			if(!usbipPort.equals("")) {
				try {
					Thread.sleep(500);
				} catch (InterruptedException e) {
					// TODO Auto-generated catch block
					e.printStackTrace();
				}
				usbipDevice.onBinddOpen();
			}
			destroyProc();
		} catch (IOException e) {
			Log.e("LTouchPad", "IOException-exec()");
			e.printStackTrace();
		}
	}
}
