package com.SimpleAnimation;

import android.app.NativeActivity;

public class MainActivity extends NativeActivity
{
        static
        {
                System.loadLibrary("gnustl_shared");
                System.loadLibrary("SimpleAnimation");
        }
}
