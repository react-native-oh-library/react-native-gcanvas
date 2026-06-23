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

#ifndef HARMONY_RNCGCANVASMODULE_H
#define HARMONY_RNCGCANVASMODULE_H
#include "RNOH/ArkTSTurboModule.h"
#include "RNCGCanvasInstance.h"
#include "cmd/IReactCacheCmd.h"

namespace rnoh {
class JSI_EXPORT RNCGCanvasModule : public std::enable_shared_from_this<RNCGCanvasModule>, public ArkTSTurboModule {
public:
    RNCGCanvasModule(const ArkTSTurboModule::Context ctx, const std::string name);
    ~RNCGCanvasModule() override;
    void Enable(std::string componentId);
    void Disable(std::string componentId);
    void ResetGlViewport(std::string componentId);
    std::string Render(std::string componentId, std::string cmd, int type);
    void SetContextType(int type, std::string componentId);
    void SetDevicePixelRatio(std::string componentId, double ratio);
    void LoadImage(const std::string url, const int imageId, std::string componentId,
                   std::unique_ptr<std::optional<facebook::jsi::Value>> callbackFunc, facebook::jsi::Runtime &rt,
                   bool isPreLoad);
    std::string ToDataURL(std::string componentId, std::string mimeType, float quality);
    void DrawCanvas2Canvas(const std::string canvasId, int tw, int th, const std::string srcCanvasId, int sx, int sy,
                           int sw, int sh, int dx, int dy, int dw, int dh);
    void TexImage2D(std::string refId, int target, int level, int internalformat, int format, int type,
                    std::string path);
    void TexSubImage2D(const std::string refId, int target, int level, int xoffset, int yoffset, int format, int type,
                       const std::string path);
    void ResetComponent(std::string componentId);

private:
    void CallTexImage2DToRender(OH_PixelmapNative *pixelmap,std::string refId, int target, int level, 
        int internalformat, int format, int type, int xoffset,int yoffset,bool isSub);
    void ExecuteCachedCommands(const std::string& componentId, RNCGCanvasNode* node);
    std::shared_ptr<RNCGCanvasNode> GetNodeFromCache(const std::string& componentId);
    
private:
    std::shared_ptr<RNCGCanvasInstance> GetInstance(const std::string &componentId);
    std::unordered_map<std::string, std::shared_ptr<rnoh::RNCGCanvasNode>> m_cacheNodeMap;
    std::unordered_map<std::string, std::vector<IReactCacheCmd*>> m_cacheCmdList; 
};


/**
 * js回调
 */
struct PreLoadImageCallbackContext {
    std::unique_ptr<std::optional<facebook::jsi::Value>> callback;
    TaskExecutor::Shared taskExecutor;
    facebook::jsi::Runtime *rt;
    int imageId;
    std::string url;
    bool isPreload{false};
    std::shared_ptr<rnoh::RNCGCanvasNode> nodePtr{nullptr};
};

/**
 * TexImage2D Pack
 */
struct TexImageTempPack {
    std::string refId;
    int target;
    int level;
    int internalformat;
    int format;
    int type;
    int xoffset;
    int yoffset;
    std::shared_ptr<RNCGCanvasModule> outerThis{nullptr};
};
} // namespace rnoh


#endif // HARMONY_RNCGCANVASMODULE_H
