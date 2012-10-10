package app.android.lcontroller;

import android.os.Bundle;
import android.app.Activity;
import android.view.Menu;
import android.view.MenuItem;
import android.view.View;
import android.widget.Toast;
import android.support.v4.app.NavUtils;

public class LController extends Activity {
	CaptureScreen capture;

    @Override
    public void onCreate(Bundle savedInstanceState) {
    	super.onCreate(savedInstanceState);
       setContentView(R.layout.activity_lcontroller);
        
       capture = new CaptureScreen();
    }

    @Override
    public boolean onCreateOptionsMenu(Menu menu) {
        getMenuInflater().inflate(R.menu.activity_lcontroller, menu);
        return true;
    }
    
    public void onButton(View v) {
		switch(v.getId()) {
		case R.id.btnServer :
			if(capture.serverOn())
				Toast.makeText(this, "서버ON", Toast.LENGTH_SHORT).show();
		    else {
		    	Toast.makeText(this, "서버ON 실패 : "
		    			+ capture.getErrMsg(), Toast.LENGTH_SHORT).show();
		    }
			break;
		case R.id.btnConnect :
			String str = capture.accept();
		    if(str != null)
		    Toast.makeText(this, "연결성공", Toast.LENGTH_SHORT).show();
		    else {
		    	   Toast.makeText(this, "연결실패 : "
		    			+ capture.getErrMsg(), Toast.LENGTH_SHORT).show();
		    }
		    break;
		default :
			break;
		}
	}
}
