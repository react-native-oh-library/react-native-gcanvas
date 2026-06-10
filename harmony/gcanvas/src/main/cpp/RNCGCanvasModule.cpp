//
// Created on 2026/3/20.
//
// Node APIs are not fully supported. To solve the compilation error of the interface cannot be found,
// please include "napi/native_api.h".

#include "RNCGCanvasModule.h"
#include "RNOH/RNInstanceCAPI.h"
#include "core/src/support/Log.h"
#include "CommonUtils.h"
#include "imageknifec/imageknifec.h"


using namespace facebook;
using namespace react;

namespace rnoh {

jsi::Value __hostFunction_enable(facebook::jsi::Runtime &rt, react::TurboModule &turboModule,
                                 const facebook::jsi::Value *args, size_t count) {
    std::string componentId = "";
    if (count > 0) {
        const jsi::Value &arg = args[0];
        if (arg.isObject()) {
            jsi::Object obj = arg.getObject(rt);
            jsi::Array keys = obj.getPropertyNames(rt);
            size_t keyCount = keys.size(rt);
            for (int i = 0; i < keyCount; i++) {
                jsi::String keyStr = keys.getValueAtIndex(rt, i).getString(rt);
                std::string key = keyStr.utf8(rt);
                jsi::Value value = obj.getProperty(rt, keyStr);
                if (key == "componentId") {
                    componentId = value.getString(rt).utf8(rt);
                } else if (key == "config") {
                    jsi::Array arrayValue = value.getObject(rt).asArray(rt);
                    LOG(INFO) << "GCanvas __hostFunction_enable args size:" << arrayValue.size(rt);
                }
            }
        }
    }
    auto self = static_cast<RNCGCanvasModule *>(&turboModule);
    self->Enable(componentId);
    return facebook::jsi::Value::undefined();
}
jsi::Value __hostFunction_extendCallNative(facebook::jsi::Runtime &rt, react::TurboModule &turboModule,
                                           const facebook::jsi::Value *args, size_t count) {
    std::string resultValue = "";
    if (count > 0) {
        const jsi::Value &arg = args[0];
        std::string componentId = "";
        std::string cmdValue = "";
        int typeValue = 0;
        if (arg.isObject()) {
            jsi::Object obj = arg.getObject(rt);
            jsi::Array keys = obj.getPropertyNames(rt);
            size_t keyCount = keys.size(rt);
            for (int i = 0; i < keyCount; i++) {
                jsi::String keyStr = keys.getValueAtIndex(rt, i).getString(rt);
                std::string key = keyStr.utf8(rt);
                jsi::Value value = obj.getProperty(rt, keyStr);
                if (key == "contextId" && value.isString()) {
                    componentId = value.getString(rt).utf8(rt);
                } else if (key == "args") {
                    cmdValue = value.getString(rt).utf8(rt);
                } else if (key == "type") {
                    typeValue = value.getNumber();
                }
            }
        }
        LOG_D("GCanvas __hostFunction_extendCallNative componentId = %s, count=%d", componentId.c_str(), count);
        if (!componentId.empty()) {
            auto self = static_cast<RNCGCanvasModule *>(&turboModule);
            resultValue = self->Render(componentId, cmdValue, typeValue);
        }
    }
    jsi::Object obj(rt);
    obj.setProperty(rt, "result", jsi::String::createFromUtf8(rt, resultValue));
    return jsi::Value(rt, obj);
}
jsi::Value __hostFunction_setContextType(facebook::jsi::Runtime &rt, react::TurboModule &turboModule,
                                         const facebook::jsi::Value *args, size_t count) {
    if (count == 2) {
        int type = args[0].getNumber();
        std::string componentId = args[1].getString(rt).utf8(rt);
        auto self = static_cast<RNCGCanvasModule *>(&turboModule);
        self->SetContextType(type, componentId);
    }
    return facebook::jsi::Value::undefined();
}
jsi::Value __hostFunction_setDevicePixelRatio(facebook::jsi::Runtime &rt, react::TurboModule &turboModule,
                                              const facebook::jsi::Value *args, size_t count) {
    if (count == 2) {
        std::string componentId = args[0].getString(rt).utf8(rt);
        double ratio = args[1].getNumber();
        auto self = static_cast<RNCGCanvasModule *>(&turboModule);
        self->SetDevicePixelRatio(componentId, ratio);
    }
    return facebook::jsi::Value::undefined();
}
jsi::Value __hostFunction_render(facebook::jsi::Runtime &rt, react::TurboModule &turboModule,
                                 const facebook::jsi::Value *args, size_t count) {
    if (count == 3) {
        std::string componentId = args[0].getString(rt).utf8(rt);
        std::string cmd = args[1].getString(rt).utf8(rt);
        int type = args[2].getNumber();
        auto self = static_cast<RNCGCanvasModule *>(&turboModule);
        self->Render(componentId, cmd, type);
    }
    return facebook::jsi::Value::undefined();
}
jsi::Value __hostFunction_resetGlViewport(facebook::jsi::Runtime &rt, react::TurboModule &turboModule,
                                          const facebook::jsi::Value *args, size_t count) {
    if (count == 1) {
        std::string componentId = args[0].getString(rt).utf8(rt);
        auto self = static_cast<RNCGCanvasModule *>(&turboModule);
        self->ResetGlViewport(componentId);
    }
    return facebook::jsi::Value::undefined();
}
jsi::Value __hostFunction_disabled(facebook::jsi::Runtime &rt, react::TurboModule &turboModule,
                                   const facebook::jsi::Value *args, size_t count) {
    if (count >= 1 && args[0].isString()) {
        std::string componentId = args[0].getString(rt).utf8(rt);
        auto self = static_cast<RNCGCanvasModule *>(&turboModule);
        self->Disable(componentId);
    }
    return facebook::jsi::Value::undefined();
}

jsi::Value __hostFunction_preLoadImage(facebook::jsi::Runtime &rt, react::TurboModule &turboModule,
                                       const facebook::jsi::Value *args, size_t count) {
    std::string url = "";
    std::optional<jsi::Value> callbackFunc;
    int imageId = 0;
    if (count == 2) {
        const jsi::Value &arg = args[0];
        if (arg.isObject()) {
            jsi::Array arrayValue = arg.getObject(rt).asArray(rt);
            if (arrayValue.size(rt) == 2) {
                jsi::Value srcJsValue = arrayValue.getValueAtIndex(rt, 0);
                if (srcJsValue.isString()) {
                    url = srcJsValue.getString(rt).utf8(rt);
                }
                jsi::Value imageIdValue = arrayValue.getValueAtIndex(rt, 1);
                if (imageIdValue.isNumber()) {
                    imageId = imageIdValue.getNumber();
                }
            }
            LOG(INFO) << "GCanvas __hostFunction_preLoadImage url:" << url;
        }
        const jsi::Value &callback = args[1];
        if (callback.isObject() && callback.asObject(rt).isFunction(rt)) {
            callbackFunc = std::move(callback.asObject(rt).asFunction(rt));
        }
    }
    if (url.empty()) {
        if (callbackFunc.has_value()) {
            jsi::Function funcV = callbackFunc.value().asObject(rt).asFunction(rt);
            jsi::Object obj(rt);
            obj.setProperty(rt, "error", jsi::String::createFromUtf8(rt, "invalid input param. url is empty"));
            funcV.call(rt, obj);
        }
        return facebook::jsi::Value::undefined();
    }
    auto self = static_cast<RNCGCanvasModule *>(&turboModule);
    self->LoadImage(url, imageId, "", std::make_unique<std::optional<jsi::Value>>(std::move(callbackFunc)), rt, true);
    return facebook::jsi::Value::undefined();
}


jsi::Value __hostFunction_bindImageTexture(facebook::jsi::Runtime &rt, react::TurboModule &turboModule,
                                           const facebook::jsi::Value *args, size_t count) {
    std::string url = "";
    int imageId = 0;
    std::string componentId = "";
    std::optional<jsi::Value> callbackFunc;
    if (count == 3) {
        const jsi::Value &arg = args[0];
        if (arg.isObject()) {
            jsi::Array arrayValue = arg.getObject(rt).asArray(rt);
            if (arrayValue.size(rt) == 2) {
                jsi::Value srcJsValue = arrayValue.getValueAtIndex(rt, 0);
                if (srcJsValue.isString()) {
                    url = srcJsValue.getString(rt).utf8(rt);
                }
                jsi::Value imageIdValue = arrayValue.getValueAtIndex(rt, 1);
                if (imageIdValue.isNumber()) {
                    imageId = imageIdValue.getNumber();
                }
            }
        }
        const jsi::Value &jsComponentId = args[1];
        componentId = jsComponentId.getString(rt).utf8(rt);
        const jsi::Value &callback = args[2];
        if (callback.isObject() && callback.asObject(rt).isFunction(rt)) {
            callbackFunc = std::move(callback.asObject(rt).asFunction(rt));
        }
    }
    LOG(INFO) << "GCanvas __hostFunction_bindImageTexture url:" << url << ";imageId:" << imageId
              << ";componentId:" << componentId;
    if (url.empty() || componentId.empty()) {
        return facebook::jsi::Value::undefined();
    }
    auto self = static_cast<RNCGCanvasModule *>(&turboModule);
    self->LoadImage(url, imageId, componentId, std::make_unique<std::optional<jsi::Value>>(std::move(callbackFunc)), rt,
                    false);
    return facebook::jsi::Value::undefined();
}

jsi::Value __hostFunction_toDataURL(facebook::jsi::Runtime &rt, react::TurboModule &turboModule,
                                    const facebook::jsi::Value *args, size_t count) {
    if (count != 3) {
        return facebook::jsi::Value::undefined();
    }
    std::string componentId = args[0].getString(rt).utf8(rt);
    std::string mimeType = args[1].getString(rt).utf8(rt);
    float quality = args[2].getNumber();
    auto self = static_cast<RNCGCanvasModule *>(&turboModule);
    std::string result = self->ToDataURL(componentId, mimeType, quality);
    return facebook::jsi::String::createFromUtf8(rt, result);
}


jsi::Value __hostFunction_drawCanvas2Canvas(facebook::jsi::Runtime &rt, react::TurboModule &turboModule,
                                            const facebook::jsi::Value *args, size_t count) {
    std::string componentId = "";
    std::string srcComponentId = "";
    int tw = 0;
    int th = 0;
    int sx = 0;
    int sy = 0;
    int sw = 0;
    int sh = 0;
    int dx = 0;
    int dy = 0;
    int dw = 0;
    int dh = 0;


    if (count > 0) {
        const jsi::Value &arg = args[0];
        if (arg.isObject()) {
            jsi::Object obj = arg.getObject(rt);
            jsi::Array keys = obj.getPropertyNames(rt);
            size_t keyCount = keys.size(rt);
            for (int i = 0; i < keyCount; i++) {
                jsi::String keyStr = keys.getValueAtIndex(rt, i).getString(rt);
                std::string key = keyStr.utf8(rt);
                jsi::Value value = obj.getProperty(rt, keyStr);
                if (key == "dstComponentId" && value.isString()) {
                    componentId = value.getString(rt).utf8(rt);
                } else if (key == "srcComponentId") { // imageId
                    srcComponentId = value.getString(rt).utf8(rt);
                } else if (key == "tw") {
                    tw = value.getNumber();
                } else if (key == "th") {
                    th = value.getNumber();
                } else if (key == "sx") {
                    sx = value.getNumber();
                } else if (key == "sy") {
                    sy = value.getNumber();
                } else if (key == "sw") {
                    sw = value.getNumber();
                } else if (key == "sh") {
                    sh = value.getNumber();
                } else if (key == "dx") {
                    dx = value.getNumber();
                } else if (key == "dy") {
                    dy = value.getNumber();
                } else if (key == "dw") {
                    dw = value.getNumber();
                } else if (key == "dh") {
                    dh = value.getNumber();
                }
            }
        }
    }
    if (componentId.empty()) {
        auto self = static_cast<RNCGCanvasModule *>(&turboModule);
        self->DrawCanvas2Canvas(componentId, tw, th, srcComponentId, sx, sy, sw, sh, dx, dy, dw, dh);
    }
    return facebook::jsi::Value::undefined();
}

jsi::Value __hostFunction_setLogLevel(facebook::jsi::Runtime &rt, react::TurboModule &turboModule,
                                      const facebook::jsi::Value *args, size_t count) {
    if (count > 0) {
        const jsi::Value &arg = args[0];
        int level = arg.getNumber();
        switch (level) { // 0-debug;1-info;2-warn;3-error;
        case 0:
            SetLogLevel(LOG_LEVEL_DEBUG);
            break;
        case 1:
            SetLogLevel(LOG_LEVEL_INFO);
            break;
        case 2:
            SetLogLevel(LOG_LEVEL_WARN);
            break;
        case 3:
            SetLogLevel(LOG_LEVEL_ERROR);
            break;
        default:
            SetLogLevel(LOG_LEVEL_DEBUG);
            break;
        }
    }
    return facebook::jsi::Value::undefined();
}
jsi::Value __hostFunction_getFontNames(facebook::jsi::Runtime &rt, react::TurboModule &turboModule,
                                       const facebook::jsi::Value *args, size_t count) {

    auto fontArray = get_sys_font_name();
    jsi::Array resultArray = jsi::Array(rt, fontArray.size());
    for (int i = 0; i < fontArray.size(); i++) {
        resultArray.setValueAtIndex(rt, i, jsi::String::createFromUtf8(rt, fontArray[i]));
    }
    return resultArray;
}

jsi::Value __hostFunction_texImage2D(facebook::jsi::Runtime &rt, react::TurboModule &turboModule,
                                     const facebook::jsi::Value *args, size_t count) {
    if (count == 7) {
        std::string componentId = args[0].getString(rt).utf8(rt);
        int target = args[1].getNumber();
        int level = args[2].getNumber();
        int internalformat = args[3].getNumber();
        int format = args[4].getNumber();
        int type = args[5].getNumber();
        std::string src = args[6].getString(rt).utf8(rt);
        auto self = static_cast<RNCGCanvasModule *>(&turboModule);
        self->TexImage2D(componentId, target, level, internalformat, format, type, src);
    }
    return facebook::jsi::Value::undefined();
}
jsi::Value __hostFunction_texSubImage2D(facebook::jsi::Runtime &rt, react::TurboModule &turboModule,
                                        const facebook::jsi::Value *args, size_t count) {

    // final String refId, final int target, final int level,
    // final int xoffset, final int yoffset, final int format, final int type, String path
    if (count == 8) {
        std::string componentId = args[0].getString(rt).utf8(rt);
        int target = args[1].getNumber();
        int level = args[2].getNumber();
        int xoffset = args[3].getNumber();
        int yoffset = args[4].getNumber();
        int format = args[5].getNumber();
        int type = args[6].getNumber();
        std::string path = args[7].getString(rt).utf8(rt);
        auto self = static_cast<RNCGCanvasModule *>(&turboModule);
        self->TexSubImage2D(componentId, target, level, xoffset, yoffset, format, type, path);
    }
    return facebook::jsi::Value::undefined();
}
RNCGCanvasModule::RNCGCanvasModule(const ArkTSTurboModule::Context ctx, const std::string name)
    : ArkTSTurboModule(ctx, name) {
    methodMap_["enable"] = MethodMetadata{2, __hostFunction_enable};
    methodMap_["extendCallNative"] = MethodMetadata{3, __hostFunction_extendCallNative};
    methodMap_["setContextType"] = MethodMetadata{2, __hostFunction_setContextType};
    methodMap_["setDevicePixelRatio"] = MethodMetadata{2, __hostFunction_setDevicePixelRatio};
    methodMap_["render"] = MethodMetadata{3, __hostFunction_render};
    methodMap_["preLoadImage"] = MethodMetadata{2, __hostFunction_preLoadImage};
    methodMap_["bindImageTexture"] = MethodMetadata{3, __hostFunction_bindImageTexture};
    methodMap_["toDataURL"] = MethodMetadata{3, __hostFunction_toDataURL};
    methodMap_["resetGlViewport"] = MethodMetadata{1, __hostFunction_resetGlViewport};
    methodMap_["drawCanvas2Canvas"] = MethodMetadata{1, __hostFunction_drawCanvas2Canvas};
    methodMap_["setLogLevel"] = MethodMetadata{1, __hostFunction_setLogLevel};
    methodMap_["getFontNames"] = MethodMetadata{0, __hostFunction_getFontNames};
    methodMap_["texImage2D"] = MethodMetadata{7, __hostFunction_texImage2D};
    methodMap_["texSubImage2D"] = MethodMetadata{7, __hostFunction_texSubImage2D};

    methodMap_["disable"] = MethodMetadata{1, __hostFunction_disabled};
}

RNCGCanvasModule::~RNCGCanvasModule() {}

std::shared_ptr<RNCGCanvasInstance> RNCGCanvasModule::GetInstance(const std::string &componentId) {
    auto instance = m_ctx.instance.lock();
    if (instance == nullptr) {
        return nullptr;
    }
    auto instanceCAPI = std::dynamic_pointer_cast<RNInstanceCAPI>(instance);
    if (!instanceCAPI) {
        return nullptr;
    }
    auto componentInstance = instanceCAPI->findComponentInstanceByTag(std::stoi(componentId));
    if (!componentInstance) {
        LOG(ERROR) << "GCanvas Enable findComponentInstanceByTag failed";
        return nullptr;
    }
    auto canvasInstance = std::dynamic_pointer_cast<RNCGCanvasInstance>(componentInstance);
    if (!canvasInstance) {
        LOG(ERROR) << "GCanvas Enable get RNCGCanvasInstance failed:";
        return nullptr;
    }
    return canvasInstance;
}

void RNCGCanvasModule::Enable(std::string componentId) {
    auto canvasInstance = GetInstance(componentId);
    if (canvasInstance) {
        LOG_D("GCanvas enable");
    }
}
void RNCGCanvasModule::ResetGlViewport(std::string componentId) {
    auto canvasInstance = GetInstance(componentId);
    if (canvasInstance) {
        LOG_D("GCanvas ResetGlViewport");
        RNCGCanvasNode &node = static_cast<RNCGCanvasNode &>(canvasInstance->getLocalRootArkUINode());
        node.SurfaceChange();
    }
}
std::string RNCGCanvasModule::Render(std::string componentId, std::string cmd, int type) {
    auto canvasInstance = GetInstance(componentId);
    if (canvasInstance) {
        RNCGCanvasNode &node = static_cast<RNCGCanvasNode &>(canvasInstance->getLocalRootArkUINode());
        return node.Render(cmd, type);
    }
    return "";
}

void RNCGCanvasModule::SetContextType(int type, std::string componentId) {
    auto canvasInstance = GetInstance(componentId);
    if (canvasInstance) {
        RNCGCanvasNode &node = static_cast<RNCGCanvasNode &>(canvasInstance->getLocalRootArkUINode());
        node.SetDevicePixelRatio(node.GetDensity());
        node.SetWrapperContextType(type);
    }
}

void RNCGCanvasModule::SetDevicePixelRatio(std::string componentId, double ratio) {
    auto canvasInstance = GetInstance(componentId);
    if (canvasInstance) {
        RNCGCanvasNode &node = static_cast<RNCGCanvasNode &>(canvasInstance->getLocalRootArkUINode());
        node.SetDevicePixelRatio(ratio);
    }
}

void RNCGCanvasModule::LoadImage(const std::string url, const int imageId, std::string componentId,
                                 std::unique_ptr<std::optional<facebook::jsi::Value>> callbackFunc,
                                 facebook::jsi::Runtime &rt, bool isPreLoad) {
    if (startsWith(url, "data:image")) {
        std::vector<uint8_t> imageData = decode_base64(url);
        OH_PixelmapNative *pixelMap = createPixelmap_encodedData(imageData.data(), imageData.size());
        auto size = GetPixelMapSize(pixelMap);
        bool is_error = size.width == 0 && size.height == 0;
        if (callbackFunc && callbackFunc->has_value()) {
            auto &cbVal = callbackFunc->value();
            if (cbVal.isObject()) {
                auto obj = cbVal.asObject(rt);
                if (obj.isFunction(rt)) {
                    auto func = obj.asFunction(rt);
                    facebook::jsi::Object result(rt);
                    if (is_error) {
                        result.setProperty(rt, "error",
                                           facebook::jsi::String::createFromUtf8(
                                               rt, "request failed ImageknifecGetCacheImageByLoadSrc"));
                    } else {
                        result.setProperty(rt, "width", jsi::Value(static_cast<double>(size.width)));
                        result.setProperty(rt, "height", jsi::Value(static_cast<double>(size.height)));
                        result.setProperty(rt, "url", facebook::jsi::String::createFromUtf8(rt, url));
                        result.setProperty(rt, "id", jsi::Value(imageId));
                    }
                    func.call(rt, result);
                }
            }
        }
        if (!isPreLoad && !componentId.empty()) {
            RNCGCanvasNode &node = static_cast<RNCGCanvasNode &>(GetInstance(componentId)->getLocalRootArkUINode());
            node.BindTexture(std::move(pixelMap), imageId);
        }
        return;
    }
    auto ctx = new PreLoadImageCallbackContext;
    ctx->callback = std::move(callbackFunc);
    ctx->taskExecutor = this->m_ctx.taskExecutor;
    ctx->imageId = imageId;
    ctx->rt = &rt;
    ctx->url = url;
    ctx->isPreload = isPreLoad;
    if (!isPreLoad) {
        ctx->instance = GetInstance(componentId);
    }

    ImageknifecGetCacheImageCallbackObject callbackObject;
    callbackObject.usrCtx = ctx;
    callbackObject.callback = [](void *usrCtx, ImageknifecImageData *imageData) {
        std::unique_ptr<PreLoadImageCallbackContext> ctx(static_cast<PreLoadImageCallbackContext *>(usrCtx));
        OH_PixelmapNative *pixelmap = NULL;
        ImageknifecError errorCode = ImageknifecImageDataGetPixelmap(imageData, &pixelmap);
        LOG_D("gcanvas LoadImage callback errorcode:%d", errorCode);
        auto size = GetPixelMapSize(pixelmap);
        if (!ctx->callback || !ctx->callback->has_value()) {
            return;
        }
        if (!pixelmap) {
            return;
        }
        auto callbackPtr = ctx->callback.release();
        auto taskExecutor = ctx->taskExecutor;
        int imageId = ctx->imageId;
        std::string url = ctx->url;
        if (!ctx->isPreload && ctx->instance) {
            RNCGCanvasNode &node = static_cast<RNCGCanvasNode &>(ctx->instance->getLocalRootArkUINode());
            node.BindTexture(std::move(pixelmap), ctx->imageId);
            ImageknifecImageDataRelease(imageData);
            return;
        }

        taskExecutor->runTask(TaskThread::JS, [callbackPtr, size, url, imageId, errorCode, rtPtr = ctx->rt]() {
            if (rtPtr) {
                facebook::jsi::Runtime &jsRuntime = *rtPtr;
                std::unique_ptr<std::optional<facebook::jsi::Value>> cb(callbackPtr);
                if (cb && cb->has_value()) {
                    auto &cbVal = cb->value();
                    if (cbVal.isObject()) {
                        auto obj = cbVal.asObject(jsRuntime);
                        if (obj.isFunction(jsRuntime)) {
                            auto func = obj.asFunction(jsRuntime);
                            facebook::jsi::Object result(jsRuntime);
                            if (errorCode != IMAGE_KNIFE_SUCCESS) {
                                result.setProperty(
                                    jsRuntime, "error",
                                    facebook::jsi::String::createFromUtf8(jsRuntime, "load image failed"));
                            } else {
                                result.setProperty(jsRuntime, "width", jsi::Value(static_cast<double>(size.width)));
                                result.setProperty(jsRuntime, "height", jsi::Value(static_cast<double>(size.height)));
                                result.setProperty(jsRuntime, "url",
                                                   facebook::jsi::String::createFromUtf8(jsRuntime, url));
                                result.setProperty(jsRuntime, "id", jsi::Value(imageId));
                            }
                            func.call(jsRuntime, result);
                            LOG_D("__hostFunction_preLoadImage callback width:%d;height:%d", size.width, size.height);
                        }
                    }
                }
            }
        });
        ImageknifecImageDataRelease(imageData);
    };

    ImageknifecError requestError =
        ImageknifecGetCacheImageByLoadSrc(url.c_str(), &callbackObject, IMAGE_KNIFE_CACHE_STRATEGY_DEFAULT, NULL);
    LOG_D("gcanvas LoadImage ImageknifecGetCacheImageByLoadSrc error code:%d", requestError);
    if (requestError != IMAGE_KNIFE_SUCCESS) {
        delete ctx;

        if (callbackFunc && callbackFunc->has_value()) {
            auto &cbVal = callbackFunc->value();
            if (cbVal.isObject()) {
                auto obj = cbVal.asObject(rt);
                if (obj.isFunction(rt)) {
                    auto func = obj.asFunction(rt);
                    facebook::jsi::Object result(rt);
                    result.setProperty(
                        rt, "error",
                        facebook::jsi::String::createFromUtf8(rt, "request failed ImageknifecGetCacheImageByLoadSrc"));
                    func.call(rt, result);
                }
            }
        }
    } else {
        callbackFunc.release();
    }
}


void RNCGCanvasModule::TexImage2D(std::string refId, int target, int level, int internalformat, int format, int type,
                                  std::string url) {
    auto ctx = new TexImageTempPack;
    ctx->refId = refId;
    ctx->target = target;
    ctx->level = level;
    ctx->internalformat = internalformat;
    ctx->format = format;
    ctx->type = type;
    ctx->outerThis = shared_from_this();

    ImageknifecGetCacheImageCallbackObject callbackObject;
    callbackObject.usrCtx = ctx;
    callbackObject.callback = [](void *usrCtx, ImageknifecImageData *imageData) {
        std::unique_ptr<TexImageTempPack> ctx(static_cast<TexImageTempPack *>(usrCtx));
        OH_PixelmapNative *pixelmap = NULL;
        ImageknifecError errorCode = ImageknifecImageDataGetPixelmap(imageData, &pixelmap);
        if (errorCode != IMAGE_KNIFE_SUCCESS) {
            LOG_D("gcanvas TexImage2D ImageknifecImageDataGetPixelmap error code:%d", errorCode);
            return;
        }
        auto size = GetPixelMapSize(pixelmap);
        int pixelFormat = GetPixelMapFormat(pixelmap);
        if (size.width == 0 || size.height == 0)
            return;
        int glInternalformat = ctx->internalformat;
        int glFormat = ctx->format;
        int bytesPerPixel = 4;
        switch (pixelFormat) {
        case 2: // RGB_565
            glInternalformat = 0x1907;
            glFormat = 0x1907;
            bytesPerPixel = 2;
            break;
        case 3:                        // RGBA_8888
        case 4:                        // BGRA_8888
            glInternalformat = 0x1908; // GL_RGBA
            glFormat = 0x1908;
            bytesPerPixel = 4;
            break;
        case 5:                        // RGB_888
            glInternalformat = 0x1907; // GL_RGB
            glFormat = 0x1907;
            bytesPerPixel = 3;
            break;
        default:
            LOG_E("GCanvas the bitmap format=%d not support.", pixelFormat);
            return;
        }

        size_t bufSize = (size_t)size.width * size.height * bytesPerPixel;
        std::vector<uint8_t> pixels(bufSize);
        Image_ErrorCode err = OH_PixelmapNative_ReadPixels(pixelmap, pixels.data(), &bufSize);
        if (err != IMAGE_SUCCESS)
            return;
        

        std::string pixelStr;
        pixelStr.reserve(bufSize * 4);
        for (size_t i = 0; i < bufSize; ++i) {
            pixelStr += std::to_string(pixels[i]);
            if (i + 1 < bufSize)
                pixelStr += ',';
        }

        size_t srcLen = pixelStr.size();
        size_t outLen = 0;
        char *outBuf = new char[4 * ((srcLen + 2) / 3) + 1];
        base64_encode(pixelStr.c_str(), srcLen, outBuf, &outLen, 0);
        std::string base64Str(outBuf, outLen);
        delete[] outBuf;

        int arrayType = 1;
        int border = 0;

        std::string cmd = "102,9," + std::to_string(ctx->target) + "," + std::to_string(ctx->level) + "," +
                          std::to_string(glInternalformat) + "," + std::to_string(size.width) + "," +
                          std::to_string(size.height) + "," + std::to_string(border) + "," + std::to_string(glFormat) +
                          "," + std::to_string(ctx->type) + "," + std::to_string(arrayType) + "," + base64Str;

        auto module = ctx->outerThis;
        if (module) {
            module->Render(ctx->refId, cmd, 0x60000000);
        }
    };
    ImageknifecError requestError =
        ImageknifecGetCacheImageByLoadSrc(url.c_str(), &callbackObject, IMAGE_KNIFE_CACHE_STRATEGY_MEMORY, NULL);
    LOG_D("GCanvas TexImage2D ImageknifecGetCacheImageByLoadSrc error code:%d", requestError);
    if (requestError != IMAGE_KNIFE_SUCCESS) {
        delete ctx;
    }
}

void RNCGCanvasModule::TexSubImage2D(const std::string refId, int target, int level, int xoffset, int yoffset,
                                     int format, int type, const std::string url) {

    auto ctx = new TexImageTempPack;
    ctx->refId = refId;
    ctx->target = target;
    ctx->level = level;
    ctx->xoffset = xoffset;
    ctx->yoffset = yoffset;
    ctx->format = format;
    ctx->type = type;
    ctx->outerThis = shared_from_this();

    ImageknifecGetCacheImageCallbackObject callbackObject;
    callbackObject.usrCtx = ctx;
    callbackObject.callback = [](void *usrCtx, ImageknifecImageData *imageData) {
        std::unique_ptr<TexImageTempPack> ctx(static_cast<TexImageTempPack *>(usrCtx));
        OH_PixelmapNative *pixelmap = NULL;
        ImageknifecError errorCode = ImageknifecImageDataGetPixelmap(imageData, &pixelmap);
        if (errorCode != IMAGE_KNIFE_SUCCESS) {
            LOG_D("GCanvas TexImage2D ImageknifecImageDataGetPixelmap error code:%d", errorCode);
            return;
        }
        auto size = GetPixelMapSize(pixelmap);
        int pixelFormat = GetPixelMapFormat(pixelmap);
        if (size.width == 0 || size.height == 0)
            return;
        int glInternalformat = ctx->internalformat;
        int glFormat = ctx->format;
        int bytesPerPixel = 4;
        switch (pixelFormat) {
        case 2: // RGB_565
            glInternalformat = 0x1907;
            glFormat = 0x1907;
            bytesPerPixel = 2;
            break;
        case 3:                        // RGBA_8888
        case 4:                        // BGRA_8888
            glInternalformat = 0x1908; // GL_RGBA
            glFormat = 0x1908;
            bytesPerPixel = 4;
            break;
        case 5:                        // RGB_888
            glInternalformat = 0x1907; // GL_RGB
            glFormat = 0x1907;
            bytesPerPixel = 3;
            break;
        default:
            LOG_E("gcanvas the bitmap format=%d not support.", pixelFormat);
            return;
        }

        size_t bufSize = (size_t)size.width * size.height * bytesPerPixel;
        std::vector<uint8_t> pixels(bufSize);
        Image_ErrorCode err = OH_PixelmapNative_ReadPixels(pixelmap, pixels.data(), &bufSize);
        if (err != IMAGE_SUCCESS)
            return;
        
        std::string pixelStr;
        pixelStr.reserve(bufSize * 4);
        for (size_t i = 0; i < bufSize; ++i) {
            pixelStr += std::to_string(pixels[i]);
            if (i + 1 < bufSize)
                pixelStr += ',';
        }

        size_t srcLen = pixelStr.size();
        size_t outLen = 0;
        char *outBuf = new char[4 * ((srcLen + 2) / 3) + 1];
        base64_encode(pixelStr.c_str(), srcLen, outBuf, &outLen, 0);
        std::string base64Str(outBuf, outLen);
        delete[] outBuf;

        int arrayType = 1;
        std::string cmd = "105,9," + std::to_string(ctx->target) + "," + std::to_string(ctx->level) + "," +
                          std::to_string(ctx->xoffset) + "," + std::to_string(ctx->yoffset) + "," +
                          std::to_string(size.width) + "," + std::to_string(size.height) + "," +
                          std::to_string(ctx->format) + "," + std::to_string(ctx->type) + "," +
                          std::to_string(arrayType) + "," + base64Str;

        auto module = ctx->outerThis;
        if (module) {
            module->Render(ctx->refId, cmd, 0x60000000);
        }
    };
    ImageknifecError requestError =
        ImageknifecGetCacheImageByLoadSrc(url.c_str(), &callbackObject, IMAGE_KNIFE_CACHE_STRATEGY_MEMORY, NULL);
    LOG_D("GCanvas TexImage2D ImageknifecGetCacheImageByLoadSrc error code:%d", requestError);
    if (requestError != IMAGE_KNIFE_SUCCESS) {
        delete ctx;
    }
}

std::string RNCGCanvasModule::ToDataURL(std::string componentId, std::string mimeType, float quality) {
    auto canvasInstance = GetInstance(componentId);
    if (canvasInstance) {
        RNCGCanvasNode &node = static_cast<RNCGCanvasNode &>(canvasInstance->getLocalRootArkUINode());
        OH_PixelmapNative *pixelMap = node.GetNodePixelMap();
        if (pixelMap) {
            std::string result = decodeCode_pixelmap(pixelMap, mimeType, static_cast<int>(quality));
            OH_PixelmapNative_Release(pixelMap);
            return result;
        }
    }
    return "";
}


void RNCGCanvasModule::DrawCanvas2Canvas(const std::string componentId, int tw, int th, const std::string srcCanvasId,
                                         int sx, int sy, int sw, int sh, int dx, int dy, int dw, int dh) {

    std::string srcKey = "";
    auto srcCanvasInstance = GetInstance(srcCanvasId);
    if (srcCanvasInstance) {
        RNCGCanvasNode &node = static_cast<RNCGCanvasNode &>(srcCanvasInstance->getLocalRootArkUINode());
        srcKey = node.GetKey();
    }

    auto canvasInstance = GetInstance(componentId);
    if (canvasInstance && !srcKey.empty()) {
        RNCGCanvasNode &node = static_cast<RNCGCanvasNode &>(canvasInstance->getLocalRootArkUINode());
        node.DrawCanvas2Canvas(tw, th, srcKey, sx, sy, sw, sh, dx, dy, dw, dh);
    }
}


void RNCGCanvasModule::Disable(std::string componentId) {
    m_ctx.taskExecutor->runTask(TaskThread::MAIN, [weakSelf = weak_from_this(), componentId] {
        auto self = weakSelf.lock();
        if (!self)
            return;
        auto canvasInstance = self->GetInstance(componentId);
        if (!canvasInstance) {
            LOG_E("GCanvas Disable can not find canvas with id ===> %s", componentId.c_str());
            return;
        }
//        RNCGCanvasNode &node = static_cast<RNCGCanvasNode &>(canvasInstance->getLocalRootArkUINode());
//         node.SetDevicePixelRatio(ratio);
//        canvasInstance->Disable(componentId);
    });
}
}; // namespace rnoh
