#include "glImpl.h"

gfx_state* g_state = NULL;

#define NUM_SCREENS (2)

gfx_device *g_screens[NUM_SCREENS] = {nullptr, nullptr};

extern "C" {

EGLAPI EGLDisplay EGLAPIENTRY eglGetDisplay(EGLNativeDisplayType display_id) {
  if (display_id >= NUM_SCREENS)
    return EGL_NO_DISPLAY;

  return g_screens + display_id;
}

EGLAPI EGLBoolean EGLAPIENTRY eglInitialize(EGLDisplay dpy, EGLint *major,
                                            EGLint *minor) {
  const EGLNativeDisplayType display_id = static_cast<EGLNativeDisplayType>(reinterpret_cast<char *>(dpy) - reinterpret_cast<char *>(g_screens));
  if (display_id >= NUM_SCREENS) {
#ifndef DISABLE_ERRORS
    setErrorEGL(EGL_BAD_DISPLAY);
#endif
    return EGL_FALSE;
  }

  if (major)  *major = 1;
  if (minor)  *minor = 4;

  if (g_screens[display_id]) {
    return EGL_TRUE;
  }

  g_screens[display_id] = new gfx_device_3ds(new gfx_state, display_id == GFX_TOP ? 400 : 320, 240);

  return EGL_TRUE;
}

EGLAPI EGLBoolean EGLAPIENTRY eglChooseConfig(EGLDisplay dpy, const EGLint *attrib_list,
                           EGLConfig *configs, EGLint config_size, EGLint *num_config) {
  const EGLNativeDisplayType display_id = static_cast<EGLNativeDisplayType>(reinterpret_cast<char *>(dpy) - reinterpret_cast<char *>(g_screens));
  if (display_id >= NUM_SCREENS) {
#ifndef DISABLE_ERRORS
    setErrorEGL(EGL_BAD_DISPLAY);
#endif
    return EGL_FALSE;
  }

  int alpha_size = 0, blue_size = 0, green_size = 0, red_size = 0,
      depth_size = 0, stencil_size = 0;
  for (; EGL_NONE != *attrib_list; attrib_list += 2) {
    switch (attrib_list[0]) {
    case EGL_ALPHA_SIZE:    alpha_size    = attrib_list[1]; break;
    case EGL_BLUE_SIZE:     blue_size     = attrib_list[1]; break;
    case EGL_GREEN_SIZE:    green_size    = attrib_list[1]; break;
    case EGL_RED_SIZE:      red_size      = attrib_list[1]; break;
    case EGL_DEPTH_SIZE:    depth_size    = attrib_list[1]; break;
    case EGL_STENCIL_SIZE:  stencil_size  = attrib_list[1]; break;
#ifndef DISABLE_ERRORS
    case EGL_BUFFER_SIZE:
    case EGL_CONFIG_CAVEAT:
    case EGL_CONFIG_ID:
    case EGL_LEVEL:
    case EGL_MAX_PBUFFER_HEIGHT:
    case EGL_MAX_PBUFFER_PIXELS:
    case EGL_MAX_PBUFFER_WIDTH:
    case EGL_NATIVE_RENDERABLE:
    case EGL_NATIVE_VISUAL_ID:
    case EGL_NATIVE_VISUAL_TYPE:
    case EGL_SAMPLES:
    case EGL_SAMPLE_BUFFERS:
    case EGL_SURFACE_TYPE:
    case EGL_TRANSPARENT_TYPE:
    case EGL_TRANSPARENT_BLUE_VALUE:
    case EGL_TRANSPARENT_GREEN_VALUE:
    case EGL_TRANSPARENT_RED_VALUE:
    case EGL_BIND_TO_TEXTURE_RGB:
    case EGL_BIND_TO_TEXTURE_RGBA:
    case EGL_MIN_SWAP_INTERVAL:
    case EGL_MAX_SWAP_INTERVAL:
      break;
    default:
      setErrorEGL(EGL_BAD_ATTRIBUTE);
      return EGL_FALSE;
#endif
    }
  }

  *num_config = 0;
  if (stencil_size != EGL_DONT_CARE && stencil_size > 8) {
    return EGL_TRUE;
  }
  if (depth_size != EGL_DONT_CARE && depth_size > 24) {
    return EGL_TRUE;
  }
  if (alpha_size != EGL_DONT_CARE && alpha_size > 0) {
    return EGL_TRUE;
  }
  if (blue_size != EGL_DONT_CARE && blue_size > 0) {
    return EGL_TRUE;
  }
  if (green_size != EGL_DONT_CARE && green_size > 0) {
    return EGL_TRUE;
  }
  if (red_size != EGL_DONT_CARE && red_size > 0) {
    return EGL_TRUE;
  }
  if (config_size != 0) {
    *configs = dpy;
    *num_config = 1;
  }

  return EGL_TRUE;
}

EGLAPI EGLContext EGLAPIENTRY eglCreateContext(EGLDisplay dpy, EGLConfig config, EGLContext share_context, const EGLint *attrib_list) {
  const EGLNativeDisplayType display_id = static_cast<EGLNativeDisplayType>(reinterpret_cast<char *>(dpy) - reinterpret_cast<char *>(g_screens));
  if (display_id >= NUM_SCREENS) {
#ifndef DISABLE_ERRORS
    setErrorEGL(EGL_BAD_DISPLAY);
#endif
    return EGL_FALSE;
  }

  return dpy;
}

EGLAPI EGLSurface EGLAPIENTRY eglCreateWindowSurface(EGLDisplay dpy, EGLConfig config, NativeWindowType window, const EGLint *attrib_list) {
  const EGLNativeDisplayType display_id = static_cast<EGLNativeDisplayType>(reinterpret_cast<char *>(dpy) - reinterpret_cast<char *>(g_screens));
  if (display_id >= NUM_SCREENS) {
#ifndef DISABLE_ERRORS
    setErrorEGL(EGL_BAD_DISPLAY);
#endif
    return EGL_FALSE;
  }

  return dpy;
}

EGLAPI EGLBoolean EGLAPIENTRY eglSwapBuffers(EGLDisplay dpy,
                                             EGLSurface surface) {
  const EGLNativeDisplayType display_id = static_cast<EGLNativeDisplayType>(reinterpret_cast<char *>(dpy) - reinterpret_cast<char *>(g_screens));
  if (display_id >= NUM_SCREENS) {
#ifndef DISABLE_ERRORS
    setErrorEGL(EGL_BAD_DISPLAY);
#endif
    return EGL_FALSE;
  }

  gfx_device *device = g_screens[display_id];
  if (device == nullptr) {
#ifndef DISABLE_ERRORS
    setErrorEGL(EGL_NOT_INITIALIZED);
#endif
    return EGL_FALSE;
  }

  reinterpret_cast<gfx_device_3ds *>(device)->flush();
  return EGL_TRUE;
}

EGLAPI EGLBoolean EGLAPIENTRY eglMakeCurrent (EGLDisplay dpy, EGLSurface draw, EGLSurface read, EGLContext ctx) {
  const EGLNativeDisplayType display_id = static_cast<EGLNativeDisplayType>(reinterpret_cast<char *>(dpy) - reinterpret_cast<char *>(g_screens));
  if (display_id >= NUM_SCREENS) {
#ifndef DISABLE_ERRORS
    setErrorEGL(EGL_BAD_DISPLAY);
#endif
    return EGL_FALSE;
  }

  g_state = g_screens[display_id]->g_state;
  return EGL_TRUE;
}

} // extern "C"
