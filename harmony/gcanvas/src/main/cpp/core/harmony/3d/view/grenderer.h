//
// Created by yuantong on 2017/8/1.
//

#ifndef G_CANVAS_GRENDER_H
#define G_CANVAS_GRENDER_H

#include <EGL/egl.h>
#include <GLES2/gl2.h>

#include <ace/xcomponent/native_interface_xcomponent.h>
#include <pthread.h>
#include <semaphore.h>
#include <string.h>
#include "GCanvas.hpp"
#include "GcanvasWeexOhos.h"
#include "GCanvasManager.h"

using namespace gcanvas;
class GcanvasWeexOhos;

class GRenderer {
public:
    GRenderer(std::string key);

    void start();

    void stop();

    void destroy();

    virtual ~GRenderer();

    void setNativeWindow(OHNativeWindow *window);

    OHNativeWindow *getNativeWindow();

    bool initialize();

    void reInitialize();

    void signalUpGLthread();

    void setRefreshFlag(bool refresh);

    void requestCreateCanvas(std::string contextid);

    void requestViewportChanged();

    void waitResponse();

    void drawImageData(int tw, int th, std::string base64ImageData,
                       int sx, int sy, int sw, int sh, int dx, int dy, int dw, int dh);

    void bindTexture(OH_PixelmapNative *pixelmap, int id, int target, int level, int internalformat,
                     int format, int type);

    void surfaceDestroy();

    void contextExit();

    void surfaceExit();

    void setDevicePixelRatio(const float ratio);

public:
    pthread_t m_thread_id;
    pthread_mutex_t m_mutex;
    pthread_cond_t m_cond = PTHREAD_COND_INITIALIZER;
    OHNativeWindow *m_window;
    EGLDisplay m_egl_display;
    EGLSurface m_egl_surface;
    EGLContext m_egl_context;
    int m_context_type = 0;
    bool m_requestExit = false;
    bool m_requestSurfaceDestroy = false;
    bool m_refresh = false;
    bool m_createCanvas = false;
    GcanvasWeexOhos *mProxy = nullptr;
    sem_t m_SyncSem;
    sem_t m_dummy;
    std::string mContextId;
    int m_width = 0;
    int m_height = 0;
    bool m_started = false;
    bool m_viewportchanged = false;
    bool m_bindtexture = false;
    GColorRGBA mClearColor;
    bool needDisableImageSmoothing = false; // default mImageSmoothingEnabled is true in core/src/gcanvas/GCanvasState.h
    bool m_enableFboMsaa = false; // default mEnableFboMsaa is false in core/src/platform/Android/GCanvas2DContextAndroid.h
    float m_device_pixel_ratio = 1.0f;

    bool m_initialized = false;
    bool m_requestInitialize = false;

    bool m_sendEvent = false;

    static void *threadStartCallback(void *myself);

    void drawFrame();

    void renderLoop();

    bool sendEvent();

};


#endif //G_CANVAS_GRENDER_H
