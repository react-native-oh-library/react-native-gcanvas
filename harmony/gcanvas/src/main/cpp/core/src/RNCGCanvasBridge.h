/*
 * Copyright (C) 2026 Huawei Device Co., Ltd.
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#pragma once
#ifndef OHOS
#define OHOS 1
#endif
#include "stdint.h"
#include <string>
#include <ace/xcomponent/native_interface_xcomponent.h>
#include "gmanager.h"
#include "GSystemFontInformation.h"
#include <vector>
#include <list>

namespace rnoh {

inline void gCanvasSurfaceCreated(std::string canvasId, OHNativeWindow *window, uint64_t width, uint64_t height,
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
inline void SurfaceChange(OHNativeWindow *window, uint64_t width, uint64_t height) {}
inline void SurfaceDestroy() {}

inline void gCanvasSetContextType(std::string canvasId, int type) {
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

inline void gCanvasSetDevicePixelRatio(std::string canvasId, double ratio) {
    LOG_D("gCanvasSetDevicePixelRatio canvasId=%s,ratio=%d\n", canvasId.c_str(), ratio);
    GRenderer *render = GManager::getSingleton()->findRenderer(canvasId);
    if (!render) {
        render = GManager::getSingleton()->newRenderer(canvasId);
    }
    if (render) {
        render->setDevicePixelRatio((const float)ratio);
    }
}

inline void gCanvasSetHiQuality(std::string canvasId, bool isHiQuality) {
    LOG_D("gCanvasSetHiQuality canvasId=%s,isHiQuality=%d\n", canvasId.c_str(), isHiQuality);
    GCanvasManager *theManager = GCanvasManager::GetManager();
    GCanvasWeex *theCanvas = (GCanvasWeex *)theManager->GetCanvas(canvasId);
    if (theCanvas) {
        theCanvas->GetGCanvasContext()->SetHiQuality(isHiQuality);
    }
}
inline std::string gCanvasRender(std::string canvasId, std::string renderCommands, int type) {
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


inline void gCanvasDestroy(std::string canvasId) {
    GRenderer *render = GManager::getSingleton()->findRenderer(canvasId);
    if (render) {
        render->surfaceDestroy();
    }
}

inline void gCanvasBindTexture(OH_PixelmapNative *pixelmap, std::string canvasId, int imageID) {
    if (canvasId.empty()) {
        return;
    }
    GRenderer *render = GManager::getSingleton()->findRenderer(canvasId);
    if (render != nullptr) {
        render->bindTexture(pixelmap, imageID, GL_TEXTURE_2D, 0, GL_RGBA, GL_RGBA, GL_UNSIGNED_BYTE);
    }
}

inline void gCanvasDrawImageData(const std::string contextId, int tw, int th, const std::string base64ImageData, int sx,
                          int sy, int sw, int sh, int dx, int dy, int dw, int dh) {
    if (contextId.empty()) {
        return;
    }
    GRenderer *render = GManager::getSingleton()->findRenderer(contextId);
    if (render != nullptr) {
        render->drawImageData(tw, th, base64ImageData, sx, sy, sw, sh, dx, dy, dw, dh);
    }
}

inline void gCanvasDrawCanvas2Canvas(const std::string contextId, int tw, int th, const std::string srcContextId, int sx,
                              int sy, int sw, int sh, int dx, int dy, int dw, int dh) {

    if (contextId.empty() || srcContextId.empty()) {
        return;
    }
    char renderCommands[16] = {0};
    sprintf(renderCommands, "R0,0,%d,%d;", tw, th);
    std::string base64ImageData = rnoh::gCanvasRender(srcContextId, renderCommands, 0x20000000);

    gCanvasDrawImageData(contextId, tw, th, base64ImageData, sx, sy, sw, sh, dx, dy, dw, dh);
}

inline void glResetComponent(const std::string componentId) {
    GCanvasManager *theManager = GCanvasManager::GetManager();
    GCanvasWeex *theCanvas = (GCanvasWeex *)theManager->GetCanvas(componentId);
    if (theCanvas) {
        theCanvas->clearCmdQueue();
        LOG_D("GCanvas ResetComponent clearCmdQueue done");
    }
}

inline void glSetExtraFontLocation(std::string path){
    SystemFontInformation::GetSystemFontInformation()
    ->SetExtraFontLocation(path.c_str());
}

inline void glInsertFontFamily(const char *fontName, std::list<const char *> &fontFileList){
    SystemFontInformation::GetSystemFontInformation()
    ->InsertFontFamily(fontName,fontFileList);
}

inline void glSetSystemFontLocation(std::string path) {
    SystemFontInformation::GetSystemFontInformation()
        ->SetSystemFontLocation(path.c_str());
}

inline void glSetDefaultFontFile(std::string fontFile) {
    SystemFontInformation::GetSystemFontInformation()
        ->SetDefaultFontFile(fontFile.c_str());
}

inline bool glHasFontFamily(std::string fontName) {
    return SystemFontInformation::GetSystemFontInformation()
        ->FindFontFamily(fontName.c_str()) != nullptr;
}

inline void glInsertFontFamily(std::string fontName, std::string fontFile) {
    if (fontName.empty() || fontFile.empty()) {
        return;
    }

    std::list<const char *> fontFileList;
    fontFileList.push_back(fontFile.c_str());

    SystemFontInformation::GetSystemFontInformation()
        ->InsertFontFamily(fontName.c_str(), fontFileList);
}

inline std::vector<std::string> glGetFontNames() {
    std::vector<std::string> result;

    auto fontFamilies = SystemFontInformation::GetSystemFontInformation()
        ->getFontFamilies();

    for (auto it = fontFamilies->begin(); it != fontFamilies->end(); ++it) {
        if ((*it).first != nullptr) {
            result.push_back((*it).first);
        }
    }

    return result;
}
} // namespace rnoh
