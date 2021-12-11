/*
 * Copyright (C) 2010-2011 Chia-I Wu <olvaffe@gmail.com>
 * Copyright (C) 2010-2011 LunarG Inc.
 *
 * Permission is hereby granted, free of charge, to any person obtaining a
 * copy of this software and associated documentation files (the "Software"),
 * to deal in the Software without restriction, including without limitation
 * the rights to use, copy, modify, merge, publish, distribute, sublicense,
 * and/or sell copies of the Software, and to permit persons to whom the
 * Software is furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included
 * in all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.  IN NO EVENT SHALL
 * THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
 * FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER
 * DEALINGS IN THE SOFTWARE.
 */

/**
 * @file gralloc_drm.h
 * 定义提供给 gralloc.cpp 调用的 gralloc_drm_device 的功能接口.
 */

#ifndef _GRALLOC_DRM_H_
#define _GRALLOC_DRM_H_

#include <hardware/gralloc.h>
#include <system/graphics.h>
#include <mali_gralloc_formats.h>
#ifdef __cplusplus
extern "C" {
#endif

#define ALIGN(val, align) (((val) + (align) - 1) & ~((align) - 1))


enum {
    /* buffer may be used as a cursor */
    GRALLOC_USAGE_ROT_MASK              = 0x0F000000U,

    GRALLOC_USAGE_TO_USE_SINGLE_BUFFER  = 0x0B000000U,

    /* mali p010 format */
    GRALLOC_USAGE_TO_USE_ARM_P010       = 0x0A000000U,
    /* would like to use a fbdc(afbc) format. */
    GRALLOC_USAGE_TO_USE_FBDC_FMT       = 0x09000000U,
    /* use Physically Continuous memory */
    GRALLOC_USAGE_TO_USE_PHY_CONT      = 0x08000000U,
};

/**
 * perform operation commands for rk gralloc.
 * Helpers for using the non-type-safe perform() extension functions. Use
 * these helpers instead of calling perform() directly in your application.
 */
enum {
  /****************Implement****************/
  GRALLOC_MODULE_PERFORM_GET_HADNLE_PHY_ADDR       = 0x08100001U,
  GRALLOC_MODULE_PERFORM_GET_HADNLE_PRIME_FD       = 0x08100002U,
  GRALLOC_MODULE_PERFORM_GET_HADNLE_ATTRIBUTES     = 0x08100004U,
  GRALLOC_MODULE_PERFORM_GET_INTERNAL_FORMAT       = 0x08100006U,
  GRALLOC_MODULE_PERFORM_GET_HADNLE_WIDTH          = 0x08100008U,
  GRALLOC_MODULE_PERFORM_GET_HADNLE_HEIGHT         = 0x0810000AU,
  GRALLOC_MODULE_PERFORM_GET_HADNLE_STRIDE         = 0x0810000CU,
  GRALLOC_MODULE_PERFORM_GET_HADNLE_BYTE_STRIDE    = 0x0810000EU,
  GRALLOC_MODULE_PERFORM_GET_HADNLE_FORMAT         = 0x08100010U,
  GRALLOC_MODULE_PERFORM_GET_HADNLE_SIZE           = 0x08100012U,
  
  GRALLOC_MODULE_PERFORM_GET_RK_ASHMEM             = 0x08100014U,
  GRALLOC_MODULE_PERFORM_SET_RK_ASHMEM             = 0x08100016U,
  
  /* perform(const struct gralloc_module_t *mod,
   *     int op,
   *     buffer_handle_t buffer,
   *     int *usage);
   */
  GRALLOC_MODULE_PERFORM_GET_USAGE = 0x0feeff03,
   
  
  /****************Not Implement****************/
  GRALLOC_MODULE_PERFORM_GET_DRM_FD                = 0x08000002U,
  /* perform(const struct gralloc_module_t *mod,
   *       int op,
   *       int drm_fd,
   *       buffer_handle_t buffer,
   *       struct hwc_drm_bo *bo);
   */
  GRALLOC_MODULE_PERFORM_DRM_IMPORT                = 0xffeeff00U,

  /* perform(const struct gralloc_module_t *mod,
   *       int op,
   *       buffer_handle_t buffer,
   *       void (*free_callback)(void *),
   *       void *priv);
   */
  GRALLOC_MODULE_PERFORM_SET_IMPORTER_PRIVATE      = 0xffeeff01U,

  /* perform(const struct gralloc_module_t *mod,
   *       int op,
   *       buffer_handle_t buffer,
   *       void (*free_callback)(void *),
   *       void **priv);
   */
  GRALLOC_MODULE_PERFORM_GET_IMPORTER_PRIVATE      = 0xffeeff02U,
};

enum {
    HAL_PIXEL_FORMAT_YCrCb_NV12         = 0x15, // YUY2
    HAL_PIXEL_FORMAT_YCrCb_NV12_10      = 0x17, // YUY2_10bit
    HAL_PIXEL_FORMAT_YCbCr_422_SP_10    = 0x18, //
    HAL_PIXEL_FORMAT_YCrCb_420_SP_10    = 0x19, //
};

struct gralloc_drm_t;
struct gralloc_drm_bo_t;

#define maxLayerNameLength     100
typedef struct rk_ashmem_t
{
    int32_t alreadyStereo;
    int32_t displayStereo;
    char LayerName[maxLayerNameLength + 1];
} rk_ashmem_t;

/**
 * 创建一个 gralloc_drm_device 实例.
 */
struct gralloc_drm_t *gralloc_drm_create(void);
void gralloc_drm_destroy(struct gralloc_drm_t *drm);

int gralloc_drm_get_fd(struct gralloc_drm_t *drm);

/**
 * 获取指定 hal_pixel_format 的 bytes_per_pixel.
 */
static inline int gralloc_drm_get_bpp(int format)
{
	int bpp;

	switch (format) {
#if PLATFORM_SDK_VERSION >= 26
	case HAL_PIXEL_FORMAT_RGBA_FP16:
		bpp = 8;
		break;
#endif
	case HAL_PIXEL_FORMAT_RGBA_8888:
	case HAL_PIXEL_FORMAT_RGBX_8888:
	case HAL_PIXEL_FORMAT_BGRA_8888:
#if PLATFORM_SDK_VERSION >= 26
	case HAL_PIXEL_FORMAT_RGBA_1010102:
#endif
		bpp = 4;
		break;
	case HAL_PIXEL_FORMAT_RGB_888:
		bpp = 3;
		break;
	case HAL_PIXEL_FORMAT_RGB_565:
	case HAL_PIXEL_FORMAT_YCbCr_422_I:
		bpp = 2;
		break;
	/* planar; only Y is considered */
	case HAL_PIXEL_FORMAT_YCrCb_NV12:
	case HAL_PIXEL_FORMAT_YV12:
	case HAL_PIXEL_FORMAT_YCbCr_422_SP:
	case HAL_PIXEL_FORMAT_YCrCb_420_SP:
	case HAL_PIXEL_FORMAT_YCbCr_420_888:
#if RK_DRM_GRALLOC
	case HAL_PIXEL_FORMAT_YCrCb_NV12_10:
	case MALI_GRALLOC_FORMAT_INTERNAL_P010:
#endif
	case HAL_PIXEL_FORMAT_BLOB:
		bpp = 1;
		break;
	default:
		bpp = 0;
		break;
	}

	return bpp;
}

static inline void gralloc_drm_align_geometry(int format, int *width, int *height)
{
	int align_w = 1, align_h = 1, extra_height_div = 0;

	switch (format) {
	case HAL_PIXEL_FORMAT_YV12:
		align_w = 32;
		align_h = 2;
		extra_height_div = 2;
		break;
	case HAL_PIXEL_FORMAT_YCbCr_422_SP:
		align_w = 2;
		extra_height_div = 1;
		break;
	case HAL_PIXEL_FORMAT_YCrCb_420_SP:
	case HAL_PIXEL_FORMAT_YCbCr_420_888:
		align_w = 2;
		align_h = 2;
		extra_height_div = 2;
		break;
	case HAL_PIXEL_FORMAT_YCbCr_422_I:
		align_w = 2;
		break;
	}

	*width = ALIGN(*width, align_w);
	*height = ALIGN(*height, align_h);

	if (extra_height_div)
		*height += *height / extra_height_div;
}

int gralloc_drm_handle_register(buffer_handle_t handle, struct gralloc_drm_t *drm);
int gralloc_drm_handle_unregister(buffer_handle_t handle);

struct gralloc_drm_bo_t *gralloc_drm_bo_create(struct gralloc_drm_t *drm, int width, int height, int format, int usage);
void gralloc_drm_bo_decref(struct gralloc_drm_bo_t *bo);

struct gralloc_drm_bo_t *gralloc_drm_bo_from_handle(buffer_handle_t handle);
int gralloc_drm_free_bo_from_handle(buffer_handle_t handle);
buffer_handle_t gralloc_drm_bo_get_handle(struct gralloc_drm_bo_t *bo, int *stride);
int gralloc_drm_get_gem_handle(buffer_handle_t handle);
void gralloc_drm_resolve_format(buffer_handle_t _handle, uint32_t *pitches, uint32_t *offsets, uint32_t *handles);
unsigned int planes_for_format(struct gralloc_drm_t *drm, int hal_format);

int gralloc_drm_bo_lock(struct gralloc_drm_bo_t *bo, int x, int y, int w, int h, int enable_write, void **addr);
void gralloc_drm_bo_unlock(struct gralloc_drm_bo_t *bo);

#ifdef USE_HWC2
int gralloc_drm_handle_get_rk_ashmem(buffer_handle_t _handle, struct rk_ashmem_t* rk_ashmem);
int gralloc_drm_handle_set_rk_ashmem(buffer_handle_t _handle, struct rk_ashmem_t* rk_ashmem);
#endif

int gralloc_drm_handle_get_phy_addr(buffer_handle_t _handle, uint32_t *phy_addr);

int gralloc_drm_handle_get_prime_fd(buffer_handle_t _handle, int *fd);

int gralloc_drm_handle_get_attributes(buffer_handle_t _handle, void *attrs);

int gralloc_drm_handle_get_internal_format(buffer_handle_t _handle, uint64_t *internal_format);
int gralloc_drm_handle_get_width(buffer_handle_t _handle, int *widht);
int gralloc_drm_handle_get_height(buffer_handle_t _handle, int *height);
int gralloc_drm_handle_get_stride(buffer_handle_t _handle, int *stride);
int gralloc_drm_handle_get_byte_stride(buffer_handle_t _handle, int *byte_stride);
int gralloc_drm_handle_get_format(buffer_handle_t _handle, int *format);
int gralloc_drm_handle_get_size(buffer_handle_t _handle, int *size);
int gralloc_drm_handle_get_usage(buffer_handle_t _handle, int *usage);

#ifdef __cplusplus
}
#endif
#endif /* _GRALLOC_DRM_H_ */
