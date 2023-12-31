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

package com.powerslide.remake;

import android.view.Surface;
import android.content.res.AssetManager;

public class OgreActivityJNI {	
	public native static void create(AssetManager mgr, String dataDir);	
	public native static void destroy();	
	public native static void initWindow(Surface surface, Object obj);
	public native static void termWindow();
	public native static boolean renderOneFrame();
    
    public native static void pause();
    
    public native static void handleActionDown(final int pID, final float pX, final float pY);
    public native static void handleActionUp(final int pID, final float pX, final float pY);
    public native static void handleActionMove(final int pID, final float pX, final float pY);
    public native static void handleKeyUp(final int keyCode, final int text);
    public native static boolean handleBackPressed();
}
