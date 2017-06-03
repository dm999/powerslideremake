/*
-----------------------------------------------------------------------------
This source file is part of OGRE
(Object-oriented Graphics Rendering Engine)
For the latest info, see http://www.ogre3d.org/

Copyright (c) 2000-2013 Torus Knot Software Ltd

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in
all copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
THE SOFTWARE.
-----------------------------------------------------------------------------
*/

package org.ogre3d.android;

import org.ogre3d.android.OgreActivityJNI;

import android.app.Activity;
import android.hardware.Sensor;
import android.hardware.SensorEvent;
import android.hardware.SensorEventListener;
import android.os.Bundle;
import android.os.Handler;
import android.view.Surface;
import android.view.SurfaceHolder;
import android.view.SurfaceHolder.Callback;
import android.view.SurfaceView;
import android.content.res.AssetManager;
import android.view.MotionEvent;

public class MainActivity extends Activity implements SensorEventListener {
	protected Handler handler = null;
	protected SurfaceView surfaceView = null;
	protected Surface lastSurface = null;

	private Runnable renderer = null;
	private boolean paused = false;
	private boolean initOGRE = false;
	private AssetManager assetMgr = null;
	
	@Override
	public void onCreate(Bundle savedInstanceState) {
		super.onCreate(savedInstanceState);
		handler = new Handler();
		sysInit();
	}

	@Override
	protected void onPause() {
        
		super.onPause();
        
        Runnable pauser = new Runnable() {
			public void run() {
				OgreActivityJNI.pause();
			}
		};
        handler.post(pauser);
        
		handler.removeCallbacks(renderer);
		paused = true;
	}

	@Override
	protected void onResume() {
		super.onResume();
		paused = false;
		handler.post(renderer);
	}
	
	@Override
	protected void onDestroy() {
		super.onDestroy();

		Runnable destroyer = new Runnable() {
			public void run() {
				OgreActivityJNI.destroy();
			}
		};
		handler.post(destroyer);
	}

	private void sysInit() {
		final Runnable initRunnable = new Runnable() {
			public void run() {
				if (!initOGRE) {
					initOGRE = true;
					
					if(assetMgr == null) {
						assetMgr = getResources().getAssets();
					}
					
					OgreActivityJNI.create(assetMgr);

					renderer = new Runnable() {
						public void run() {

							if (paused)
								return;

							if (!wndCreate && lastSurface != null) {
								wndCreate = true;
								OgreActivityJNI.initWindow(lastSurface);
								handler.post(this);
								return;
							}

							if (initOGRE && wndCreate)
								OgreActivityJNI.renderOneFrame();

							handler.post(this);
						}
					};

					handler.post(renderer);
				}
			}

		};

		SurfaceView view = new SurfaceView(this);
		SurfaceHolder holder = view.getHolder();
		// holder.setType(SurfaceHolder.SURFACE_TYPE_GPU);
		surfaceView = view;

		holder.addCallback(new Callback() {
			public void surfaceCreated(SurfaceHolder holder) {
				if (holder.getSurface() != null
						&& holder.getSurface().isValid()) {
					lastSurface = holder.getSurface();
					handler.post(initRunnable);
				}
			}

			public void surfaceDestroyed(SurfaceHolder holder) {
				if (initOGRE && wndCreate) {
					wndCreate = false;
					lastSurface = null;
					handler.post(new Runnable() {
						public void run() {
							OgreActivityJNI.termWindow();
						}
					});
				}
			}

			public void surfaceChanged(SurfaceHolder holder, int format,
					int width, int height) {

			}
		});
		setContentView(surfaceView);
	}

	boolean wndCreate = false;

	public void onAccuracyChanged(Sensor sensor, int accuracy) {

	}

	public void onSensorChanged(SensorEvent event) {
		if (event.sensor.getType() == Sensor.TYPE_ACCELEROMETER) {
		}
	}
    
    public boolean onTouchEvent(MotionEvent event) {
        
        /*
        final int pointerNumber = event.getPointerCount();
        final int[] ids = new int[pointerNumber];
        final float[] xs = new float[pointerNumber];
        final float[] ys = new float[pointerNumber];

        for (int i = 0; i < pointerNumber; i++) 
        {
            ids[i] = event.getPointerId(i);
            xs[i] = event.getX(i);
            ys[i] = event.getY(i);
        }*/
        
        switch(event.getAction() & MotionEvent.ACTION_MASK){
            
        case MotionEvent.ACTION_DOWN:
            final int idPointerDown = event.getPointerId(0);
            final float xPointerDown = event.getX();
            final float yPointerDown = event.getY();
            handler.post(new Runnable() {
                public void run() {
                    OgreActivityJNI.handleActionDown(idPointerDown, xPointerDown, yPointerDown);
                }
            });
            break;
            
        case MotionEvent.ACTION_UP:
            final int idPointerUp = event.getPointerId(0);
            final float xPointerUp = event.getX();
            final float yPointerUp = event.getY();
            handler.post(new Runnable() {
                public void run() {
                    OgreActivityJNI.handleActionUp(idPointerUp, xPointerUp, yPointerUp);
                }
            });
            break;
        case MotionEvent.ACTION_POINTER_DOWN:
            final int indexPointer2Down = (event.getAction() & MotionEvent.ACTION_POINTER_INDEX_MASK) >> MotionEvent.ACTION_POINTER_INDEX_SHIFT;
            final int idPointer2Down = event.getPointerId(indexPointer2Down);
            final float xPointer2Down = event.getX(indexPointer2Down);
            final float yPointer2Down = event.getY(indexPointer2Down);
            handler.post(new Runnable() {
                public void run() {
                    OgreActivityJNI.handleActionDown(idPointer2Down, xPointer2Down, yPointer2Down);
                }
            });
            break;
        case MotionEvent.ACTION_POINTER_UP:
            final int indexPointer2Up = (event.getAction() & MotionEvent.ACTION_POINTER_INDEX_MASK) >> MotionEvent.ACTION_POINTER_INDEX_SHIFT;
            final int idPointer2Up = event.getPointerId(indexPointer2Up);
            final float xPointer2Up = event.getX(indexPointer2Up);
            final float yPointer2Up = event.getY(indexPointer2Up);
            handler.post(new Runnable() {
                public void run() {
                    OgreActivityJNI.handleActionUp(idPointer2Up, xPointer2Up, yPointer2Up);
                }
            });
            break;
        case MotionEvent.ACTION_MOVE:
            final int indexPointer2Move = (event.getAction() & MotionEvent.ACTION_POINTER_INDEX_MASK) >> MotionEvent.ACTION_POINTER_INDEX_SHIFT;
            final int idPointer2Move = event.getPointerId(indexPointer2Move);
            final float xPointer2Move = event.getX(indexPointer2Move);
            final float yPointer2Move = event.getY(indexPointer2Move);
            handler.post(new Runnable() {
                public void run() {
                    OgreActivityJNI.handleActionMove(idPointer2Move, xPointer2Move, yPointer2Move);
                }
            });
            break;
        }
        return true;
    }

	static {
        System.loadLibrary("openal");
		System.loadLibrary("PowerslideRebuild");
	}
}
