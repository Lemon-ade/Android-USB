/*
 * Copyright (C) 2008-2009 Google Inc.
 * 
 * Licensed under the Apache License, Version 2.0 (the "License"); you may not
 * use this file except in compliance with the License. You may obtain a copy of
 * the License at
 * 
 * http://www.apache.org/licenses/LICENSE-2.0
 * 
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS, WITHOUT
 * WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied. See the
 * License for the specific language governing permissions and limitations under
 * the License.
 * 
 * 
 * @author
 * Kang In-gu
 * Department of Computer Engineering, HansungUniversity, Korea.
 * e-mail : 2002gig@naver.com
 */

package app.android.lcontroller;

import java.io.IOException;
import java.net.Socket;
import java.util.ArrayList;

import android.app.Activity;
import android.content.Intent;
import android.os.Bundle;
import android.os.Handler;
import android.os.Message;
import android.util.Log;
import android.view.KeyEvent;
import android.view.View;
import android.widget.AdapterView;
import android.widget.AdapterView.OnItemClickListener;
import android.widget.ArrayAdapter;
import android.widget.Button;
import android.widget.ListView;
import android.widget.TextView;
import android.widget.Toast;
import app.android.lcontroller.R;

public class LController extends Activity {
	private static LController instance;
	
	private static String myIP;
	private static TextView ipText;
	private static Button btnServer;
	private static ListView connectList;
	private static ArrayList<String> mArrayList;
	private static ArrayAdapter<String> mConnectAdapter;
	
	public static final int HANDLER_WHAT_SETMYIP = 1;
	public static final int HANDLER_WHAT_RESETSERVERBTN = 2;
	public static final int HANDLER_WHAT_RESETCLIP = 3;
	public static final int HANDLER_WHAT_TOASTVIEW = 4;
	private static final String STR_SERVER_ON = "Server TurnOn";
	private static final String STR_SERVER_OFF = "Server TurnOff";
	private static final String STR_SETMYIP_ERROR = "IP address not found.";
	
	private Handler handler = new Handler() {
		public void handleMessage(Message msg) {
			switch(msg.what) {
			case HANDLER_WHAT_SETMYIP :
				setMyIP((String)msg.obj); break;
			case HANDLER_WHAT_RESETSERVERBTN :
				resetBtnServer(); break;
			case HANDLER_WHAT_RESETCLIP :
				resetClientIP(); break;
			case HANDLER_WHAT_TOASTVIEW :
				toastView((String)msg.obj); break;
			default :
				Log.w("MessagePCViewer", "handler msg : 'what' not found");
				break;
			}
		}
	};
	
    /** Called when the activity is first created. */
    @Override
    public void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.main);
        instance = this;
        
        ipText = (TextView) findViewById(R.id.ip_text);
        btnServer = (Button) findViewById(R.id.btn_server);
        
        connectList = (ListView) findViewById(R.id.listView_client_ip);
        if(mArrayList==null) {
        	mArrayList = new ArrayList<String>();
        }
        mConnectAdapter = new ArrayAdapter<String>(this,
        		android.R.layout.simple_list_item_1, mArrayList);
        connectList.setAdapter(mConnectAdapter);
        connectList.setOnItemClickListener(mOnItemClick);
        
        resetMyIP();
		startService(new Intent(this, MessageManager.class));
    }
    
    public static LController getInstance() {
    	return instance;
    }
    
    public OnItemClickListener mOnItemClick = new OnItemClickListener() {
		public void onItemClick(AdapterView<?> arg0, View arg1, int arg2,
				long arg3) {
		}
    };
    
    public void toast(String str) {
    	Message msg = new Message();
    	msg.what = HANDLER_WHAT_TOASTVIEW;
    	msg.obj = str;
    	handler.sendMessage(msg);
    }
    
    private void toastView(String str) {
		Toast.makeText(this, str, Toast.LENGTH_SHORT).show();
	}

    public void mBtnClick(View v) {
    	switch(v.getId()) {
    	case R.id.btn_server : 
    		MessageManager.setConnect();
    		if(btnServer.getText().equals(STR_SERVER_ON)) {
    			// server turn on
    			MessageManager.setCheckServerOn(true);
    		}
    		else {
    			// server turn off
    			MessageManager.setCheckServerOn(false);
    			MessageManager.stop();
    		}
    		break;
    	case R.id.btn_close_client :
    		MessageManager.closeConnect();
    		break;
    	default :
    		break;
    	}
    	Message msg = new Message();
    	msg.what = HANDLER_WHAT_RESETSERVERBTN;
    	handler.sendMessage(msg);
    }
    
    private void resetMyIP() {
    	new Thread(new Runnable() {
			 public void run() {
				 String tmpMyIP = "";
				 try {
			    		Log.d("MessagePCViewer", "in getMyIP()");
						Socket socket = new Socket("www.google.com", 80);
						Log.d("MessagePCViewer", "myIP : " + socket.getLocalAddress());
						if(socket!=null) {
							tmpMyIP = socket.getLocalAddress().toString();
						}
				 } catch (IOException e) {
					 e.printStackTrace();
					 Log.e("MessagePCViewer", "resetMyIP : socket make error");
				 }
				 Message msg = new Message();
				 msg.what = HANDLER_WHAT_SETMYIP;
				 if(tmpMyIP==null || tmpMyIP.equals("")) {
					 msg.obj = STR_SETMYIP_ERROR;
				 }
				 else {
					 msg.obj = tmpMyIP;
				 }
				 handler.sendMessage(msg);
			 }
		 }).start();
    }
    
    private static void setMyIP(String IP) {
    	myIP = IP;
    	if(ipText==null) {
    		Log.w("MessagePCViewer", "setMyIP : ipText is null");
    		return;
    	}
    	ipText.setText(myIP);
    }
    
    // add the accepted Client IP
    public void addClientIP(String IP) {
    	if(IP!=null) {
    		if(mArrayList==null) mArrayList = new ArrayList<String>();
    		mArrayList.add(IP);
    		Message msg = new Message();
    		msg.what = HANDLER_WHAT_RESETCLIP;
    		handler.sendMessage(msg);
    	}
    }
    
    // mConnectAdapter reset
    private static void resetClientIP() {
    	if(mConnectAdapter!=null) { // UI usable
    		mConnectAdapter.notifyDataSetChanged();   	}
    }
    
    private static void resetBtnServer() {
    	if(btnServer==null)
    		return;
    	if(MessageManager.getCheckServerOn()) { // server On
    		btnServer.setText(STR_SERVER_OFF);
    	}
    	else {
    		btnServer.setText(STR_SERVER_ON);
    	}
    }
    
    public boolean onKeyDown(int keyCode, KeyEvent event) {
    	switch (keyCode) {
        	case KeyEvent.KEYCODE_BACK:
        		toast("run on background");
        		moveTaskToBack(true);
        		return true;
    	}
    	return super.onKeyDown(keyCode, event);
    }
}