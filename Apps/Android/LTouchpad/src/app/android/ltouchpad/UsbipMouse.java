package app.android.ltouchpad;

import android.os.Handler;
import android.os.Message;
import android.util.Log;

public class UsbipMouse extends UsbipDevice {
	
	private boolean moveflag;
	private static Handler handler;
	
	private byte[] buffer;
	private boolean isBtnLeftDown;
	private boolean isBtnRightDown;
	private boolean isBtnScrollDown;
	
	public static final int Handler_What_UsbipConnected = 2221;
	public static final int Handler_What_ClientConnected = 2222;
	public static final int Handler_What_ErrorMessage = 2223;
	public static final String Handler_ErrorMsg_Usbip = "UsbipConnect Error";
	public static final String Handler_ErrorMsg_Cleint = "ClientConnect Error";
	public static final String Handler_ErrorMsg_Disconnect = "Client disconnected";
	
	public UsbipMouse(String usbip_port, Handler handler) {
		super(usbip_port);
		UsbipMouse.handler = handler;
		moveflag = false;
		buffer = new byte[4];
	}
	
	private static void sendHandler(int what, String str) {
		if(handler==null)
			return;
		Message msg = new Message();
		msg.what = what;
		if(str!=null)
			msg.obj = str;
		handler.sendMessage(msg);
	}
	
	@Override
	protected void onError(int errCode, String errMsg) {
		Log.d("LTouchPad", "onError (" + errCode + ") : " + errMsg);
		
		switch(errCode) {
		case UsbipDevice.ERRCODE_USBIPD :
			sendHandler(Handler_What_ErrorMessage, Handler_ErrorMsg_Disconnect);
			break;
		case UsbipDevice.ERRCODE_BINDD :
			sendHandler(Handler_What_ErrorMessage, Handler_ErrorMsg_Usbip);
			break;
		case UsbipDevice.ERRCODE_USBIPCONNECT :
			sendHandler(Handler_What_ErrorMessage, Handler_ErrorMsg_Usbip);
			break;		
		case UsbipDevice.ERRCODE_CLIENTCONNECT :
			sendHandler(Handler_What_ErrorMessage, Handler_ErrorMsg_Cleint);
			break;
		}
	}
	
	@Override
	protected void onBinddOpen() {
		Log.d("LTouchPad", "onBinddOpen");
		connectUsbip();
	}
	
	@Override
	protected void onBinddSuccess() {
		Log.d("LTouchPad", "onBinddSuccess");
		sendHandler(Handler_What_UsbipConnected, null);
	}
	
	@Override
	protected void onClientSuccess() {
		Log.d("LTouchPad", "client connect success");
		sendHandler(Handler_What_ClientConnected, null);
	}
	
	private int move(int x, int y) {
		setBuffer_move(x, y, 0);
		if(send_control(buffer)<0)
			return -1;
		setBuffer_move(0, 0, 0);
		return send_control(buffer);
	}
	private int btn_left(boolean down) {
		if(setBuffer_btn(true,false,false,down))
			return send_control(buffer);
		else
			return -1;
	}
	private int btn_right(boolean down) {
		if(setBuffer_btn(false,true,false,down))
			return send_control(buffer);
		else
			return -1;
	}
	private int btn_scroll(boolean down) {
		if(setBuffer_btn(false,false,true,down))
			return send_control(buffer);
		else
			return -1;
	}
	private int move_scroll(boolean down) {
		setBuffer_move(0,0,(down)?-1:1);
		if(send_control(buffer)<0)
			return -1;
		setBuffer_move(0,0,0);
		return send_control(buffer);
	}
	private void setBuffer_move(int x, int y, int scroll) {
		buffer[1] = (byte) (0x00 + x);
		buffer[2] = (byte) (0x00+y);
		buffer[3] = (byte) (0x00+scroll);
	}
	private boolean setBuffer_btn(boolean left, boolean right,
			boolean scroll, boolean down) {
		if(left) {
			if(down==isBtnLeftDown)
				return false;
			buffer[0] += (down)? 0x01 : -0x01;
			isBtnLeftDown = !isBtnLeftDown;
		}
		else if(right) {
			if(down==isBtnRightDown)
				return false;
			buffer[0] += (down)? 0x02 : -0x02;
			isBtnRightDown = !isBtnRightDown;
		}
		else if(scroll) {
			if(down==isBtnScrollDown)
				return false;
			buffer[0] += (down)? 0x04 : -0x04;
			isBtnScrollDown = !isBtnScrollDown;
		}
		else {
			return false;
		}
		return true;		
	}
	
	public boolean moveTouch(int x, int y) {
		int ret;
		if(!isClientConnect())
			return false;
		
		if(!moveflag) {
			moveflag = true;
			ret = move(x,y);
			if(ret<0) {
				closeConnection();
				moveflag = false;
				return false;
			}
			else {
				moveflag = false;
				return true;
			}
		}
		else
			return false;
	}
	
	public boolean btnLeft(boolean down) {
		if(!isClientConnect())
			return false;
		
		if(btn_left(down)<0)
			return false;
		else
			return true;
	}
	
	public boolean btnRight(boolean down) {
		if(!isClientConnect())
			return false;
		
		if(btn_right(down)<0)
			return false;
		else
			return true;
	}
	
	public boolean btnScroll(boolean down) {
		if(!isClientConnect())
			return false;
		
		if(btn_scroll(down)<0)
			return false;
		else
			return true;
	}
	
	public boolean moveScroll(boolean down) {
		if(!isClientConnect())
			return false;
		
		if(move_scroll(down)<0)
			return false;
		else
			return true;
	}
	
	public boolean isSendable() {
		if(!isClientConnect())
			return false;
		
		if(is_sendable()==1)
			return true;
		else
			return false;
	}
}