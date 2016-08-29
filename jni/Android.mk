LOCAL_PATH := $(call my-dir)

include $(CLEAR_VARS)

OPENCV_CAMERA_MODULES=off
OPENCV_LIB_TYPE=STATIC
include $(LOCAL_PATH)/../jni-libs/opencv/jni/OpenCV.mk

TENSORFLOW_CFLAGS	  := -frtti \
  -fstack-protector-strong \
  -fpic \
  -ffunction-sections \
  -funwind-tables \
  -no-canonical-prefixes \
  -fno-canonical-system-headers \
  '-march=armv7-a' \
  '-mfpu=vfpv3-d16' \
  '-mfloat-abi=softfp' \
  '-std=c++11' '-mfpu=neon' -O2 \
  '-DMIN_LOG_LEVEL=0'

LOCAL_MODULE    := label_image
LOCAL_SRC_FILES := main.cpp
LOCAL_CFLAGS    := $(TENSORFLOW_CFLAGS)
LOCAL_LDLIBS := \
    -Wl,-whole-archive \
    $(LOCAL_PATH)/../jni-libs/$(TARGET_ARCH_ABI)/libandroid_tensorflow_kernels.lo \
	$(LOCAL_PATH)/../jni-libs/$(TARGET_ARCH_ABI)/libandroid_tensorflow_lib.lo \
	$(LOCAL_PATH)/../jni-libs/$(TARGET_ARCH_ABI)/libandroid_tensorflow_lib_lite.lo \
	$(LOCAL_PATH)/../jni-libs/$(TARGET_ARCH_ABI)/libprotos_all_cc.a \
	$(LOCAL_PATH)/../jni-libs/$(TARGET_ARCH_ABI)/libprotobuf.a \
	$(LOCAL_PATH)/../jni-libs/$(TARGET_ARCH_ABI)/libprotobuf_lite.a \
	$(LOCAL_PATH)/../jni-libs/$(TARGET_ARCH_ABI)/libre2.a \
	-Wl,-no-whole-archive \
	$(NDK_ROOT)/sources/cxx-stl/gnu-libstdc++/4.9/libs/$(TARGET_ARCH_ABI)/libgnustl_static.a \
	$(NDK_ROOT)/sources/cxx-stl/gnu-libstdc++/4.9/libs/$(TARGET_ARCH_ABI)/libsupc++.a \
	-llog -landroid -lm -lz -ljnigraphics -pthread -no-canonical-prefixes '-march=armv7-a' -Wl,--fix-cortex-a8 -Wl,-S \

LOCAL_C_INCLUDES += $(LOCAL_PATH)/../include/google \
	$(LOCAL_PATH)/../include/eigen_archive \
	$(LOCAL_PATH)/../include

LOCAL_CPPFLAGS += -std=c++11

NDK_MODULE_PATH := $(call my-dir)

include $(BUILD_EXECUTABLE)