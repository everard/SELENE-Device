LOCAL_PATH := $(call my-dir)/../../../../../Source

include $(CLEAR_VARS)
LOCAL_MODULE := engine
LOCAL_SRC_FILES := ../Build/Android/Engine/obj/local/$(TARGET_ARCH_ABI)/libengine.a
LOCAL_EXPORT_C_INCLUDES := $(LOCAL_PATH)/Engine $(LOCAL_PATH)/Platforms/Android
include $(PREBUILT_STATIC_LIBRARY)

include $(CLEAR_VARS)

MY_SEARCH_DIRS = /* /*/* /*/*/* /*/*/*/* /*/*/*/*/* /*/*/*/*/*/*
MY_SOURCE_DIRS = /Demos/SimpleApplication
LOCAL_MODULE    := SimpleApplication
LOCAL_SRC_FILES := $(foreach SRC_DIR, $(MY_SOURCE_DIRS), $(patsubst $(LOCAL_PATH)/%, %, $(foreach DIR, $(MY_SEARCH_DIRS), $(wildcard $(LOCAL_PATH)$(SRC_DIR)$(DIR).cpp))))
LOCAL_LDLIBS    := -llog -landroid -lEGL -lGLESv2
LOCAL_WHOLE_STATIC_LIBRARIES = engine
LOCAL_STATIC_LIBRARIES = android_native_app_glue

include $(BUILD_SHARED_LIBRARY)

$(call import-module,android/native_app_glue)
