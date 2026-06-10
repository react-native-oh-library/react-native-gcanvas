//
// Created on 2026/5/7.
//
// Node APIs are not fully supported. To solve the compilation error of the interface cannot be found,
// please include "napi/native_api.h".

#pragma once
#ifndef OHOS
#define OHOS 1
#endif
#include "stdint.h"
#include <string>
#include <ace/xcomponent/native_interface_xcomponent.h>
#include "gmanager.h"


namespace rnoh {

void gCanvasSurfaceCreated(std::string canvasId, OHNativeWindow *window, uint64_t width, uint64_t height,
                           std::string clearColor) {
    LOG_D("SurfaceCreated canvasId=%s,width=%d,height=%d，clearColor=%s\n", canvasId.c_str(), width, height,
          clearColor.c_str());
    GRenderer *render = GManager::getSingleton()->findRenderer(canvasId);
    if (!render) {
        render = GManager::getSingleton()->newRenderer(canvasId);
    }
    if (render->m_width != width || render->m_height != height) {
        render->setNativeWindow(window);
        render->m_width = width;
        render->m_height = height;
        render->m_enableFboMsaa = false;
        if (!render->m_started) {
            if (!clearColor.empty()) {
                render->mClearColor = StrValueToColorRGBA(clearColor.c_str());
                LOG_D("parse color r:%f, g:%f, b:%f, a:%f", render->mClearColor.rgba.r, render->mClearColor.rgba.g,
                      render->mClearColor.rgba.b, render->mClearColor.rgba.a);
            }
            render->start();
            render->requestViewportChanged();
        } else {
            render->requestViewportChanged();
        }

        if (render->mProxy) {
            render->mProxy->initWebglExt();
        }
    }
}
void SurfaceChange(OHNativeWindow *window, uint64_t width, uint64_t height) {}
void SurfaceDestroy() {}

void gCanvasSetContextType(std::string canvasId, int type) {
    LOG_D("gCanvasSetContextType canvasId=%s,type=%d\n", canvasId.c_str(), type);
    GRenderer *render = GManager::getSingleton()->findRenderer(canvasId);
    if (!render) {
        render = GManager::getSingleton()->newRenderer(canvasId);
    }
    render->m_context_type = type;

    render->requestCreateCanvas(canvasId);
    if (render->mProxy) {
        render->mProxy->initWebglExt();
    }
    LOG_D("SetContextType end");
}

void gCanvasSetDevicePixelRatio(std::string canvasId, double ratio) {
    LOG_D("gCanvasSetDevicePixelRatio canvasId=%s,ratio=%d\n", canvasId.c_str(), ratio);
    GRenderer *render = GManager::getSingleton()->findRenderer(canvasId);
    if (!render) {
        render = GManager::getSingleton()->newRenderer(canvasId);
    }
    if (render) {
        render->setDevicePixelRatio((const float)ratio);
    }
}

void gCanvasSetHiQuality(std::string canvasId, bool isHiQuality) {
    LOG_D("gCanvasSetHiQuality canvasId=%s,isHiQuality=%d\n", canvasId.c_str(), isHiQuality);
    GCanvasManager *theManager = GCanvasManager::GetManager();
    GCanvasWeex *theCanvas = (GCanvasWeex *)theManager->GetCanvas(canvasId);
    if (theCanvas) {
        theCanvas->GetGCanvasContext()->SetHiQuality(isHiQuality);
    }
}
std::string gCanvasRender(std::string canvasId, std::string renderCommands, int type) {
    LOG_D("gCanvasRender canvasId=%s,renderCommands=%s,type=%d\n", canvasId.c_str(), renderCommands.c_str(), type);
    GCanvasManager *theManager = GCanvasManager::GetManager();
    GCanvasWeex *theCanvas = (GCanvasWeex *)theManager->GetCanvas(canvasId);
    if (theCanvas) {
        const char *result = theCanvas->CallNative(type, renderCommands);
        LOG_D("gcanvas_render result:%s:", result);
        return result == nullptr ? "" : result;
    }
    return "";
}
} // namespace rnoh

void gCanvasDestroy(std::string canvasId) {
    GRenderer *render = GManager::getSingleton()->findRenderer(canvasId);
    if (render) {
        render->surfaceDestroy();
    }
}

void gCanvasBindTexture(OH_PixelmapNative *pixelmap, std::string canvasId, int imageID) {
    if (canvasId.empty()) {
        return;
    }
    GRenderer *render = GManager::getSingleton()->findRenderer(canvasId);
    if (render != nullptr) {
        render->bindTexture(pixelmap, imageID, GL_TEXTURE_2D, 0, GL_RGBA, GL_RGBA, GL_UNSIGNED_BYTE);
    }
}

void gCanvasDrawImageData(const std::string contextId, int tw, int th, const std::string base64ImageData, int sx,
                          int sy, int sw, int sh, int dx, int dy, int dw, int dh) {
    if (contextId.empty()) {
        return;
    }
    GRenderer *render = GManager::getSingleton()->findRenderer(contextId);
    if (render != nullptr) {
         render->drawImageData(tw, th, base64ImageData, sx, sy, sw, sh, dx, dy, dw, dh);
    }
}

void gCanvasDrawCanvas2Canvas(const std::string contextId, int tw, int th, const std::string srcContextId, int sx,
                              int sy, int sw, int sh, int dx, int dy, int dw, int dh) {

    if (contextId.empty() || srcContextId.empty()) {
        return;
    }
    char renderCommands[16] = {0};
    sprintf(renderCommands, "R0,0,%d,%d;", tw, th);
    std::string base64ImageData = rnoh::gCanvasRender(srcContextId, renderCommands, 0x20000000);

    gCanvasDrawImageData(contextId, tw, th, base64ImageData, sx, sy, sw, sh, dx, dy, dw, dh);
}