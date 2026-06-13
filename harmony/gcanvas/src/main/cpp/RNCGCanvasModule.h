//
// Created on 2026/3/20.
//
// Node APIs are not fully supported. To solve the compilation error of the interface cannot be found,
// please include "napi/native_api.h".

#ifndef HARMONY_RNCGCANVASMODULE_H
#define HARMONY_RNCGCANVASMODULE_H
#include "RNOH/ArkTSTurboModule.h"
#include "RNCGCanvasInstance.h"
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

private:
    void CallTexImage2DToRender(OH_PixelmapNative *pixelmap,std::string refId, int target, int level, 
        int internalformat, int format, int type, int xoffset,int yoffset,bool isSub);
    
private:
    std::shared_ptr<RNCGCanvasInstance> GetInstance(const std::string &componentId);
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
    std::shared_ptr<RNCGCanvasInstance> instance{nullptr};
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
