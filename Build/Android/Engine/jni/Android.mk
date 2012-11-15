LOCAL_PATH := $(call my-dir)/../../../../Source/Engine
include $(CLEAR_VARS)

SEARCH_DIRS = /* /*/* /*/*/* /*/*/*/* /*/*/*/*/* /*/*/*/*/*/*
LOCAL_MODULE    := engine
LOCAL_SRC_FILES := $(patsubst $(LOCAL_PATH)/%, %, $(foreach DIR, $(SEARCH_DIRS), $(wildcard $(LOCAL_PATH)$(DIR).cpp)))

include $(BUILD_STATIC_LIBRARY)
