NDK_TOOLCHAIN_VERSION := 4.9
APP_ABI := armeabi-v7a
APP_PLATFORM := android-21
APP_STL := gnustl_static
APP_CFLAGS += -Werror=return-type -Wno-error=format-security
APP_CPPFLAGS += -std=c++11