LOCAL_PATH := $(call my-dir)/../../../../Source
include $(CLEAR_VARS)

MY_SEARCH_DIRS = /* /*/* /*/*/* /*/*/*/* /*/*/*/*/* /*/*/*/*/*/*
MY_SOURCE_DIRS = /Engine /Platforms/Android
LOCAL_MODULE    := engine
LOCAL_SRC_FILES := $(foreach SRC_DIR, $(MY_SOURCE_DIRS), $(patsubst $(LOCAL_PATH)/%, %, $(foreach DIR, $(MY_SEARCH_DIRS), $(wildcard $(LOCAL_PATH)$(SRC_DIR)$(DIR).cpp))))

include $(BUILD_STATIC_LIBRARY)
