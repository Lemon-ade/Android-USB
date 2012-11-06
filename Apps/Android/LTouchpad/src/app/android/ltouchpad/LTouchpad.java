package app.android.ltouchpad;

import java.io.IOException;
import java.net.Socket;

import android.app.Activity;
import android.content.Context;
import android.graphics.drawable.Drawable;
import android.os.Bundle;
import android.os.Handler;
import android.os.Message;
import android.os.Vibrator;
import android.util.Log;
import android.view.KeyEvent;
import android.view.Menu;
import android.view.MenuItem;
import android.view.MotionEvent;
import android.view.View;
import android.view.View.OnTouchListener;
import android.view.WindowManager;
import android.widget.Button;
import android.widget.FrameLayout;
import android.widget.ProgressBar;
import android.widget.TextView;
import android.widget.Toast;

public class LTouchpad extends Activity {
	
	private static UsbipMouse usbipMouse;
	
	private static Button btnState;
	private static TextView textConnect;
	private static ProgressBar pbarConnect; 
	private static int state;
	private static String result;
	
	private static Vibrator vibe;
	
	private static OnTouchListener tlistenerPad;
	private static OnTouchListener tlistenerScroll;
	private static OnTouchListener tlistenerButton;
	
	private static FrameLayout fTouch;
	private static FrameLayout fScroll;
	private static FrameLayout fLbutton;
	private static FrameLayout fRbutton;
	
	private static Drawable Drawable_Lbtn;
	private static Drawable Drawable_LbtnClicked;
	private static Drawable Drawable_Rbtn;
	private static Drawable Drawable_RbtnClicked;
	
	public static final String HANDLE_MSG_CONNECTING = "connecting";
    public static final String HANDLE_MSG_SUCCESS = "success";
    public static final String HANDLE_MSG_FAILED = "failed";
    
    public static final int HANDLE_WHAT_CONNECT_USBIP = 1;
    public static final int HANDLE_WHAT_GET_MYIP = 2;
    public static final int HANDLE_WHAT_CONNECT_CLIENT = 3;
    public static final int HANDLE_WHAT_RESETSERVERBTN = 4;
    public static final int HANDLE_WHAT_RESETCLIP = 5;
    public static final int HANDLE_WHAT_DISCONNECT = 6;
    public static final int TIME_VIBE = 100;
	
    /** Called when the activity is first created. */
    @Override
    public void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.main);
        
        getWindow().addFlags(WindowManager.LayoutParams.FLAG_KEEP_SCREEN_ON);
        
        btnState = (Button)findViewById(R.id.btn_state);
        textConnect = (TextView)findViewById(R.id.text_connect);
        pbarConnect = (ProgressBar)findViewById(R.id.progressBar_connect);
        
        tlistenerPad = new TouchListenerPad();
        tlistenerScroll = new TouchListenerScroll();
        tlistenerButton = new TouchListenerButton();
        
        fTouch = (FrameLayout)findViewById(R.id.frame_touch);
        fTouch.setOnTouchListener(tlistenerPad);
        fScroll = (FrameLayout)findViewById(R.id.frame_scroll);
        fScroll.setOnTouchListener(tlistenerScroll);
        fLbutton = (FrameLayout)findViewById(R.id.frame_lbtn);
        fLbutton.setOnTouchListener(tlistenerButton);
        fRbutton = (FrameLayout)findViewById(R.id.frame_rbtn);
        fRbutton.setOnTouchListener(tlistenerButton);
        
        Drawable_Lbtn = getResources().getDrawable(R.drawable.left);
        Drawable_LbtnClicked = getResources().getDrawable(R.drawable.left_clicked);
        Drawable_Rbtn = getResources().getDrawable(R.drawable.right);
        Drawable_RbtnClicked = getResources().getDrawable(R.drawable.right_clicked);
        
        vibe = (Vibrator)getSystemService(Context.VIBRATOR_SERVICE);
        
        btnState.setVisibility(Button.GONE);
        pbarConnect.setVisibility(ProgressBar.VISIBLE);
        state = HANDLE_WHAT_CONNECT_USBIP;
        result = HANDLE_MSG_CONNECTING;
        textConnect.setText("Connecting USBIP...");
        
        DeviceOn();
    }
    
    private void DeviceOn() {
    	if(usbipMouse==null)
    		usbipMouse = new UsbipMouse("2-1.2", handler);
       usbipMouse.on();
    }
    
    public static Handler handler = new Handler() {
		public void handleMessage(Message msg) {
			String strMsg = (String)msg.obj;
			
			switch(msg.what) {
			
			case UsbipMouse.Handler_What_UsbipConnected :
				state = HANDLE_WHAT_CONNECT_USBIP;
		       result = HANDLE_MSG_SUCCESS;
				getMyIP();
				break;
				
			case HANDLE_WHAT_GET_MYIP :
				state = HANDLE_WHAT_GET_MYIP;
				if(strMsg.equals(HANDLE_MSG_CONNECTING)) {
					result = strMsg;
					pbarConnect.setVisibility(ProgressBar.VISIBLE);
					textConnect.setText("getting my IP address");
					btnState.setVisibility(Button.GONE);
				}
				else if(strMsg.equals(HANDLE_MSG_FAILED)) {
					result = strMsg;
					pbarConnect.setVisibility(ProgressBar.INVISIBLE);
					textConnect.setText("my IP address error");
					btnState.setText("re try");
					btnState.setVisibility(Button.VISIBLE);
				}
				else {
					result = HANDLE_MSG_SUCCESS;
					pbarConnect.setVisibility(ProgressBar.INVISIBLE);
					btnState.setVisibility(Button.GONE);
					textConnect.setText("waiting PC (server IP : " + strMsg + " )");
					connectClient();
				}
				break;
				
			case HANDLE_WHAT_CONNECT_CLIENT :
				state = HANDLE_WHAT_CONNECT_CLIENT;
				result = HANDLE_MSG_CONNECTING;
				pbarConnect.setVisibility(ProgressBar.VISIBLE);
				btnState.setVisibility(Button.GONE);
				break;
				
			case UsbipMouse.Handler_What_ClientConnected :
				state = HANDLE_WHAT_CONNECT_CLIENT;
				result = HANDLE_MSG_SUCCESS;
				pbarConnect.setVisibility(ProgressBar.INVISIBLE);
				textConnect.setText("LTouchpad connected");
				btnState.setText("EXIT");
				btnState.setVisibility(Button.VISIBLE);
				break;
				
			case UsbipMouse.Handler_What_ErrorMessage :
				if(strMsg.equals(UsbipMouse.Handler_ErrorMsg_Usbip)) {
					state = HANDLE_WHAT_CONNECT_USBIP;
					result = HANDLE_MSG_FAILED;
					pbarConnect.setVisibility(ProgressBar.INVISIBLE);
					textConnect.setText("Usbip connect error");
					btnState.setText("re try");
					btnState.setVisibility(Button.VISIBLE);
				}
				else if(strMsg.equals(UsbipMouse.Handler_ErrorMsg_Cleint)) {
					state = HANDLE_WHAT_CONNECT_CLIENT;
					result = HANDLE_MSG_FAILED;
					pbarConnect.setVisibility(ProgressBar.INVISIBLE);
					textConnect.setText("Client connect error");
					btnState.setText("re try");
					btnState.setVisibility(Button.VISIBLE);
				}
				else if(strMsg.equals(UsbipMouse.Handler_ErrorMsg_Disconnect)) {
					state = HANDLE_WHAT_DISCONNECT;
					pbarConnect.setVisibility(ProgressBar.INVISIBLE);
					textConnect.setText("Disconnected Client");
					btnState.setText("re start");
					btnState.setVisibility(Button.VISIBLE);
				}
				break;
			}
		}
	};
	
	private static void getMyIP() {
    	Message msg = new Message();
    	msg.what = HANDLE_WHAT_GET_MYIP;
    	msg.obj = HANDLE_MSG_CONNECTING;
    	handler.sendMessage(msg);
    	
    	new Thread(new Runnable() {
			 public void run() {
				 String tmpMyIP = "";
				 try {
			    		Log.d("LTouchPad", "in getMyIP()");
						Socket socket = new Socket("www.google.com", 80);
						Log.d("LTouchPad", "myIP : " + socket.getLocalAddress());
						if(socket!=null) {
							tmpMyIP = socket.getLocalAddress().toString();
						}
				 } catch (IOException e) {
					 e.printStackTrace();
					 Log.e("LTouchPad", "resetMyIP : socket make error");
				 }
				 Message msg = new Message();
				 msg.what = HANDLE_WHAT_GET_MYIP;
				 if(tmpMyIP==null || tmpMyIP.equals("")) {
					 msg.obj = HANDLE_MSG_FAILED;
				 }
				 else {
					 msg.obj = tmpMyIP;
				 }
				 handler.sendMessage(msg);
			 }
		 }).start();
    }
	
	public static void connectClient() {
		Log.d("LTouchPad", "call connectClient");
    	Message msg = new Message();
    	msg.what = HANDLE_WHAT_CONNECT_CLIENT;
    	msg.obj = HANDLE_MSG_CONNECTING;
    	handler.sendMessage(msg);
    	usbipMouse.process_cmd();
    }
    
    public void onButton(View v) {
    	switch(state) {
    	case HANDLE_WHAT_CONNECT_USBIP :
    		// re try
    		DeviceOn();
    		break;
    	case HANDLE_WHAT_GET_MYIP :
    		// re try
    		getMyIP();
    		break;
    	case HANDLE_WHAT_CONNECT_CLIENT :
    		if(result.equals(HANDLE_MSG_SUCCESS)) {
    			// exit program
    			finish();
    		}
    		else if(result.equals(HANDLE_MSG_FAILED)){
    			// re try
    			connectClient();
    		}
    		break;
    	case HANDLE_WHAT_DISCONNECT :
    		// re start
    		DeviceOn();
    		break;
    	}
    }
    
    public static boolean LButtonDown() {
    	Log.i("LTouchPad", "LButtonDown()");
    	if(usbipMouse.btnLeft(true)) {
    		vibe.vibrate(TIME_VIBE);
    		fLbutton.setBackgroundDrawable(Drawable_LbtnClicked);
    		return true;
    	}
    	else
    		return false;
    }
    
    public static boolean LButtonUp() {
    	Log.i("LTouchPad", "LButtonUp()");
    	if(usbipMouse.btnLeft(false)) {
    		vibe.vibrate(TIME_VIBE);
    		fLbutton.setBackgroundDrawable(Drawable_Lbtn);
    		return true;
    	}
    	else
    		return false;
    }
    
    public static boolean LButtonClick() {
    	if(LButtonDown())
    		return LButtonUp();
    	else
    		return false;
    }
    
    public static boolean RButtonDown() {
    	Log.i("LTouchPad", "RButtonDown()");
    	if(usbipMouse.btnRight(true)) {
    		vibe.vibrate(TIME_VIBE);
    		fRbutton.setBackgroundDrawable(Drawable_RbtnClicked);
    		return true;
    	}
    	else
    		return false;
    }
    
    public static boolean RButtonUp() {
    	Log.i("LTouchPad", "RButtonUp()");
    	if(usbipMouse.btnRight(false)) {
    		vibe.vibrate(TIME_VIBE);
    		fRbutton.setBackgroundDrawable(Drawable_Rbtn);
    		return true;
    	}
    	else
    		return false;
    }
    
    public static boolean RButtonClick() {
    	if(RButtonDown())
    		return RButtonUp();
    	else
    		return false;
    }
    
    public static boolean SButtonClick() {
    	Log.i("LTouchPad", "SButtonClick()");
    	if(usbipMouse.btnScroll(true)) {
    		if(usbipMouse.btnScroll(false))
    			return true;
    	}
    	return false;
    }
    
    public static boolean ScrollDown() {
    	Log.i("LTouchPad", "ScrollDown()");
    	if(!usbipMouse.isSendable()) {
    		Log.w("LTouchPad", "can't send state");
    		return false;
    	}
    	if(usbipMouse.moveScroll(true))
    		return true;
    	else
    		return false;
    }
    
    public static boolean ScrollUp() {
    	Log.i("LTouchPad", "ScrollUp()");
    	if(!usbipMouse.isSendable()) {
    		Log.w("LTouchPad", "can't send state");
    		return false;
    	}
    	if(usbipMouse.moveScroll(false))
    		return true;
    	else
    		return false;
    }
    
    public static Point Move(Point movePoint) {
    	if(!usbipMouse.isClientConnect()) {
    		movePoint.set(0, 0);
    		return movePoint;
    	}
    	if(!usbipMouse.isSendable()) {
    		Log.w("LTouchPad", "can't send state");
    		return movePoint;
    	}
    	
    	//movePoint.setHalf();
    	movePoint.cutMaxAbsnum(30);
    	if(movePoint.getBigSize()<1)
    		return movePoint;
    	
    	Log.i("LTouchPad", "Move("+movePoint.x+","+movePoint.y+")");
    	if(usbipMouse.moveTouch(movePoint.x,movePoint.y)) {
    		movePoint.set(0, 0);
    	}
    	else {
    		Log.e("LTouchPad", "move() error!");
    	}
    	return movePoint;
    }
    
    public static boolean MultiTouch(View v, MotionEvent event) {
    	if(event.getPointerCount()<2)
    		return false;
    	
    	Point p = new Point((int)event.getX(1), (int)event.getY(1));
    	p = LTouchpad.getAbsPoint(v, p);
    	// need check p case
    	switch(event.getAction()) {
    	case MotionEvent.ACTION_POINTER_2_DOWN :
    		event.setAction(MotionEvent.ACTION_DOWN);
    		break;
    	case MotionEvent.ACTION_MOVE :
    		break;
    	case MotionEvent.ACTION_POINTER_2_UP :
    		event.setAction(MotionEvent.ACTION_UP);
    		break;
    	case MotionEvent.ACTION_POINTER_1_UP :
    		event.setAction(MotionEvent.ACTION_UP);
    		tlistenerButton.onTouch(v, event);
    		break;
    	}
    	event.setLocation(event.getX(event.getPointerCount()-1),
    			event.getY(event.getPointerCount()-1));
    	tlistenerPad.onTouch(v, event);
    	
    	return true;
    }
    
    public static Point getAbsPoint(View v, Point p) {
    	View parentView = v.getRootView();
    	Point point = new Point(0,0);
    	int sumX = 0;
    	int sumY = 0;
    	boolean isEnd = false;
    	
    	while(!isEnd) {
    		sumX += v.getLeft();
    		sumY += v.getTop();
    		
    		v = (View)v.getParent();
    		if(parentView == v) {
    			isEnd = true;
    		}
    	}
    	
    	if(p!=null) {
    		point.x += p.x;
    		point.y += p.y;
    	}
    	point.x += sumX;
		point.y += sumY;
    	return point;
    }
    
    public boolean onCreateOptionsMenu(Menu menu) {
    	super.onCreateOptionsMenu(menu);
    	menu.setQwertyMode(true);
    	
    	menu.addSubMenu(0,0,0,"LTouchpad 종료");
    	return true;
    }
    
    public boolean onOptionsItemSelected(MenuItem item) {
    	switch(item.getItemId()) {
    	case 0 :
    		finish();
    		return true;
    	}
    	return false;
    }
    
    public boolean onKeyDown(int keyCode, KeyEvent event) {
    	switch (keyCode) {
        	case KeyEvent.KEYCODE_BACK:
        		Toast.makeText(this, "to exit LTouchpad, use menu-exit."
        				, Toast.LENGTH_LONG).show();
        		moveTaskToBack(true);
        		return true;
    	}
    	return super.onKeyDown(keyCode, event);
    }
}