# Copyright (C) 2010 Chia-I Wu <olvaffe@gmail.com>
# Copyright (C) 2010-2011 LunarG Inc.
#
# Permission is hereby granted, free of charge, to any person obtaining a
# copy of this software and associated documentation files (the "Software"),
# to deal in the Software without restriction, including without limitation
# the rights to use, copy, modify, merge, publish, distribute, sublicense,
# and/or sell copies of the Software, and to permit persons to whom the
# Software is furnished to do so, subject to the following conditions:
#
# The above copyright notice and this permission notice shall be included
# in all copies or substantial portions of the Software.
#
# THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
# IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
# FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.  IN NO EVENT SHALL
# THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
# LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
# FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER
# DEALINGS IN THE SOFTWARE.

LOCAL_PATH := $(call my-dir)

include $(CLEAR_VARS)

MALI_ARCHITECTURE_UTGARD := 0
MALI_SUPPORT_AFBC_WIDEBLK ?= 0
MALI_USE_YUV_AFBC_WIDEBLK ?= 0

# AFBC buffers should be initialised after allocation in all rk platforms.
GRALLOC_INIT_AFBC?=1

# not use afbc layer by default.
USE_AFBC_LAYER = 0

ifeq ($(strip $(TARGET_BOARD_PLATFORM)),rk3399)
USE_AFBC_LAYER = 0
endif

# enable AFBC by default
MALI_AFBC_GRALLOC := 1

DISABLE_FRAMEBUFFER_HAL ?= 1
GRALLOC_DEPTH ?= GRALLOC_32_BITiS
GRALLOC_FB_SWAP_RED_BLUE ?= 0

#If cropping should be enabled for AFBC YUV420 buffers
AFBC_YUV420_EXTRA_MB_ROW_NEEDED ?= 0

ifeq ($(GRALLOC_ARM_NO_EXTERNAL_AFBC),1)
LOCAL_CFLAGS += -DGRALLOC_ARM_NO_EXTERNAL_AFBC=1
endif

ifeq ($(strip $(TARGET_BOARD_PLATFORM_GPU)), mali-t720)
MALI_AFBC_GRALLOC := 0
LOCAL_CFLAGS += -DMALI_PRODUCT_ID_T72X=1
endif

ifeq ($(strip $(TARGET_BOARD_PLATFORM_GPU)), mali-t760)
MALI_AFBC_GRALLOC := 1
LOCAL_CFLAGS += -DMALI_PRODUCT_ID_T76X=1
endif

ifeq ($(strip $(TARGET_BOARD_PLATFORM_GPU)), mali-t860)
MALI_AFBC_GRALLOC := 1
LOCAL_CFLAGS += -DMALI_PRODUCT_ID_T86X=1
endif

ifeq ($(MALI_AFBC_GRALLOC), 1)
AFBC_FILES = gralloc_buffer_priv.cpp
else
MALI_AFBC_GRALLOC := 0
AFBC_FILES =
endif

LOCAL_SRC_FILES := \
	gralloc_drm.cpp \
	gralloc_drm_rockchip.cpp \
	mali_gralloc_formats.cpp \
	$(AFBC_FILES)

LOCAL_C_INCLUDES := \
	external/libdrm \
	external/libdrm/include/drm \
	hardware/libhardware/include

LOCAL_SHARED_LIBRARIES := \
	libdrm \
	liblog \
	libcutils \
	libutils \
	libdrm_rockchip

MAJOR_VERSION := \
	"RK_GRAPHICS_VER=commit-id:$(shell cd $(LOCAL_PATH) && git log  -1 --oneline | awk '{print $$1}')"

LOCAL_CFLAGS += \
	-DRK_DRM_GRALLOC=1 \
	-DRK_DRM_GRALLOC_DEBUG=0 \
	-DENABLE_ROCKCHIP \
	-DPLATFORM_SDK_VERSION=$(PLATFORM_SDK_VERSION) \
	-D$(GRALLOC_DEPTH) \
	-DMALI_ARCHITECTURE_UTGARD=$(MALI_ARCHITECTURE_UTGARD) \
	-DDISABLE_FRAMEBUFFER_HAL=$(DISABLE_FRAMEBUFFER_HAL) \
	-DMALI_AFBC_GRALLOC=$(MALI_AFBC_GRALLOC) \
	-DMALI_SUPPORT_AFBC_WIDEBLK=$(MALI_SUPPORT_AFBC_WIDEBLK) \
	-DAFBC_YUV420_EXTRA_MB_ROW_NEEDED=$(AFBC_YUV420_EXTRA_MB_ROW_NEEDED) \
	-DGRALLOC_INIT_AFBC=$(GRALLOC_INIT_AFBC) \
	-DRK_GRAPHICS_VER=\"$(MAJOR_VERSION)\" \
	-DUSE_AFBC_LAYER=$(USE_AFBC_LAYER) \
	-DGRALLOC_LIBRARY_BUILD=1 \
	-DGRALLOC_USE_GRALLOC1_API=0

ifeq ($(TARGET_USES_HWC2),true)
    LOCAL_CFLAGS += -DUSE_HWC2
endif

ifeq ($(GRALLOC_FB_SWAP_RED_BLUE),1)
LOCAL_CFLAGS += -DGRALLOC_FB_SWAP_RED_BLUE
endif

ifeq ($(GRALLOC_ARM_NO_EXTERNAL_AFBC),1)
LOCAL_CFLAGS += -DGRALLOC_ARM_NO_EXTERNAL_AFBC=1
endif

LOCAL_MODULE := libgralloc_drm
LOCAL_MODULE_TAGS := optional
LOCAL_PROPRIETARY_MODULE := true

include $(BUILD_SHARED_LIBRARY)

# -------------------------------------------------------------------- #
include $(CLEAR_VARS)
LOCAL_SRC_FILES := \
	gralloc.cpp

LOCAL_C_INCLUDES := \
	external/libdrm \
	external/libdrm/include/drm \
	hardware/libhardware/include

LOCAL_SHARED_LIBRARIES := \
	libgralloc_drm \
	liblog \
	libutils

# for glFlush/glFinish
LOCAL_SHARED_LIBRARIES += \
	libGLESv1_CM

LOCAL_CFLAGS += \
	-DRK_DRM_GRALLOC=1 \
	-DRK_DRM_GRALLOC_DEBUG=0 \
	-DMALI_AFBC_GRALLOC=1 \
	-DPLATFORM_SDK_VERSION=$(PLATFORM_SDK_VERSION)

ifeq ($(TARGET_USES_HWC2),true)
    LOCAL_CFLAGS += -DUSE_HWC2
endif

LOCAL_MODULE := gralloc.$(TARGET_BOARD_PLATFORM)
LOCAL_MODULE_TAGS := optional
LOCAL_MODULE_RELATIVE_PATH := hw
LOCAL_PROPRIETARY_MODULE := true

include $(BUILD_SHARED_LIBRARY)
