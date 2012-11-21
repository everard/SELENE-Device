#!/bin/sh

PROJECT_NAME=$1
PLATFORM_NAME=$2

if !([ "$PROJECT_NAME"=~^[A-Za-z0-9\-]+$ ]); then
        echo 'Error: project name must be specified and can have only A-Z, a-z, 0-9 and - characters.';
        exit 0;
fi;

if !([ "$PLATFORM_NAME"=~^[A-Za-z0-9\-]+$ ]); then
        echo 'Error: platform name must be specified and can have only A-Z, a-z, 0-9 and - characters.';
        exit 0;
fi;

# delete everything in directory
rm -rf $PROJECT_NAME

# init directory
mkdir -p $PROJECT_NAME
mkdir -p $PROJECT_NAME/jni
mkdir -p $PROJECT_NAME/res/values
mkdir -p $PROJECT_NAME/src/com/$PROJECT_NAME

echo "Project name    :  $PROJECT_NAME"
echo "Android platform:  $PLATFORM_NAME"

touch $PROJECT_NAME/AndroidManifest.xml
touch $PROJECT_NAME/Makefile
touch $PROJECT_NAME/jni/Android.mk
touch $PROJECT_NAME/jni/Application.mk
touch $PROJECT_NAME/res/values/strings.xml
touch $PROJECT_NAME/src/com/$PROJECT_NAME/MainActivity.java

echo "original:
	rm -rf bin gen libs obj build.xml local.properties proguard-project.txt project.properties" > $PROJECT_NAME/Makefile

echo "<?xml version=\"1.0\" encoding=\"utf-8\"?>
<resources>
    <string name=\"app_name\">$PROJECT_NAME</string>
</resources>" > $PROJECT_NAME/res/values/strings.xml

echo "<?xml version=\"1.0\" encoding=\"utf-8\"?>
<!-- BEGIN_INCLUDE(manifest) -->
<manifest xmlns:android=\"http://schemas.android.com/apk/res/android\"
        package=\"com.$PROJECT_NAME\"
        android:versionCode=\"1\"
        android:versionName=\"1.0\">

    <!-- This is the platform API where NativeActivity was introduced. -->
    <uses-sdk android:minSdkVersion=\"9\" />

    <!-- This .apk has Java code for loading shared libraries, so set hasCode to true. -->
    <application android:label=\"@string/app_name\" android:hasCode=\"true\">

        <!-- Our activity is the derived class from NativeActivity framework class.
             This will take care of integrating with our NDK code. -->
        <activity android:name=\".MainActivity\"
                android:label=\"@string/app_name\"
                android:configChanges=\"orientation|keyboardHidden\">
            <!-- Tell NativeActivity the name of or .so -->
            <meta-data android:name=\"android.app.lib_name\"
                    android:value=\"$PROJECT_NAME\" />
            <intent-filter>
                <action android:name=\"android.intent.action.MAIN\" />
                <category android:name=\"android.intent.category.LAUNCHER\" />
            </intent-filter>
        </activity>
    </application>

</manifest> 
<!-- END_INCLUDE(manifest) -->" > $PROJECT_NAME/AndroidManifest.xml

echo "package com.$PROJECT_NAME;

import android.app.NativeActivity;

public class MainActivity extends NativeActivity
{
        static
        {
                System.loadLibrary(\"gnustl_shared\");
                System.loadLibrary(\"$PROJECT_NAME\");
        }
}" > $PROJECT_NAME/src/com/$PROJECT_NAME/MainActivity.java

echo "LOCAL_PATH := \$(call my-dir)
include \$(CLEAR_VARS)

LOCAL_MODULE    := $PROJECT_NAME
LOCAL_SRC_FILES := \$(notdir \$(wildcard \$(LOCAL_PATH)/*.cpp))
LOCAL_LDLIBS    := -llog -landroid -lEGL -lGLESv1_CM
#LOCAL_WHOLE_STATIC_LIBRARIES = libraries
LOCAL_STATIC_LIBRARIES = android_native_app_glue

include \$(BUILD_SHARED_LIBRARY)

\$(call import-module,android/native_app_glue)" > $PROJECT_NAME/jni/Android.mk

echo "APP_MODULES  := $PROJECT_NAME
APP_PLATFORM := $PLATFORM_NAME
APP_ABI := all
APP_STL := gnustl_shared
APP_CPPFLAGS := -std=gnu++11 -Wall
APP_GNUSTL_FORCE_CPP_FEATURES := exceptions rtti" > $PROJECT_NAME/jni/Application.mk
