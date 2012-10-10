package app.android.ltouchpad;

import android.util.Log;
import android.view.MotionEvent;
import android.view.View;
import android.view.View.OnTouchListener;

public class TouchListenerPad implements OnTouchListener {
	private boolean isTouchMove;
	private boolean isActionDown;
	private Point pPoint = new Point(0,0);
	private Point movePoint = new Point(0,0);

	public boolean onTouch(View v, MotionEvent event) {
		switch(event.getAction()) {
		case MotionEvent.ACTION_DOWN :
			Log.d("LTouchPad", "Pad/onTouch : ACTION_DOWN");
			if(isActionDown)
				return false;
			isActionDown = true;
			isTouchMove = false;
			pPoint.x = (int)event.getX();
			pPoint.y = (int)event.getY();
			return true;
		case MotionEvent.ACTION_MOVE :
			Log.d("LTouchPad", "Pad/onTouch : ACTION_MOVE");
			isTouchMove = true;
			int x2 = (int)event.getX();
			int y2 = (int)event.getY();
			movePoint.add(x2-pPoint.x, y2-pPoint.y);
			movePoint = LTouchpad.Move(movePoint);
			pPoint.x = x2;
			pPoint.y = y2;
			return false;
		case MotionEvent.ACTION_UP :
			Log.d("LTouchPad", "Pad/onTouch : ACTION_UP");
			if(!isActionDown)
				return false;
			isActionDown = false;
			if(!isTouchMove) {
				// Click Event
				LTouchpad.LButtonClick();
			}
			else {
				// Move Event end
				movePoint.set(0, 0);
			}
			return true;
		default :
			return false;
		}
	}
}
