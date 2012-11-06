package app.android.ltouchpad;

import android.util.Log;
import android.view.MotionEvent;
import android.view.View;
import android.view.View.OnTouchListener;

public class TouchListenerButton implements OnTouchListener {
	
	public boolean onTouch(View v, MotionEvent event) {
		
		switch(event.getAction()) {
		case MotionEvent.ACTION_DOWN :
			Log.d("LTouchPad", "Button/onTouch : ACTION_DOWN");
			if(v.getId()==R.id.frame_lbtn)
				LTouchpad.LButtonDown();
			else // R.id.frame_Rbtn
				LTouchpad.RButtonDown();
			return true;
		case MotionEvent.ACTION_UP :
			Log.d("LTouchPad", "Button/onTouch : ACTION_UP");
			if(v.getId()==R.id.frame_lbtn) {
				LTouchpad.LButtonUp();
			}
			else // R.id.frame_Rbtn
				LTouchpad.RButtonUp();
			return true;
		default :
			if(event.getPointerCount()>1) {
				Log.d("LTouchPad", "Button/onTouch : MultiTouch - " + event.getAction());
				LTouchpad.MultiTouch(v, event);
			}
			return false;
		}
	}
}
