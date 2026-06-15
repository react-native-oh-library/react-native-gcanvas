//
// Created on 2026/4/2.
//
// Node APIs are not fully supported. To solve the compilation error of the interface cannot be found,
// please include "napi/native_api.h".

#include "RNCGCanvasNode.h"
#include "RNOH/arkui/NativeNodeApi.h"
#include "RNCGCanvasBridge.h"
#include "core/src/RNCGCanvasBridge.h"
#include "core/src/support/Log.h"
#include <native_buffer/native_buffer.h>
#include <native_window/external_window.h>
#include <dlfcn.h>

namespace rnoh {

void RNCGCanvasNode::OnSurfaceCreated(OH_NativeXComponent *component, void *window) {
    if (window && component) {
        uint64_t width = 0, height = 0;
        int status = OH_NativeXComponent_GetXComponentSize(component, window, &width, &height);
        if (status == OH_NATIVEXCOMPONENT_RESULT_SUCCESS) {
            auto node = getNode(component);
            if (node) {
                int code = OH_NativeWindow_NativeWindowHandleOpt(static_cast<OHNativeWindow *>(window), SET_FORMAT,
                                                                 NATIVEBUFFER_PIXEL_FMT_RGBA_8888);
                if (code == 0) {
                    static_cast<RNCGCanvasNode *>(node)->setReady(true);
                    node->SetWindowAndSize(static_cast<OHNativeWindow *>(window), width, height);
                    node->SurfaceChange();
                }
            }
        }
    }
}

void RNCGCanvasNode::OnSurfaceChanged(OH_NativeXComponent *component, void *window) {
    if (window && component) {
        uint64_t width = 0, height = 0;
        int status = OH_NativeXComponent_GetXComponentSize(component, window, &width, &height);
        if (status == OH_NATIVEXCOMPONENT_RESULT_SUCCESS) {
            auto node = getNode(component);
            if (node) {
                LOG_D("GCanvas OnSurfaceChanged");
                node->SetWindowAndSize(static_cast<OHNativeWindow *>(window), width, height);
            }
        }
    }
}

void RNCGCanvasNode::OnSurfaceDestroyed(OH_NativeXComponent *component, void *window) {
    auto node = getNode(component);
    if (node) {
        node->destroy();
    }
}

void RNCGCanvasNode::DispatchTouchEvent(OH_NativeXComponent *component, void *window) {
}

RNCGCanvasNode::RNCGCanvasNode()
    : ArkUINode(NativeNodeApi::getInstance()->createNode(ArkUI_NodeType::ARKUI_NODE_XCOMPONENT)) {

    m_xComponentCallback_.OnSurfaceCreated = OnSurfaceCreated;
    m_xComponentCallback_.OnSurfaceChanged = OnSurfaceChanged;
    m_xComponentCallback_.OnSurfaceDestroyed = OnSurfaceDestroyed;
    m_xComponentCallback_.DispatchTouchEvent = DispatchTouchEvent;
    ArkUI_NumberValue surfaceTypeValue[] = {{.u32 = ARKUI_XCOMPONENT_TYPE_SURFACE}};
    ArkUI_AttributeItem surfaceType = {surfaceTypeValue, sizeof(surfaceTypeValue) / sizeof(ArkUI_NumberValue)};
    NativeNodeApi::getInstance()->setAttribute(getArkUINodeHandle(), NODE_XCOMPONENT_TYPE, &surfaceType);

    m_nativeXComponent = OH_NativeXComponent_GetNativeXComponent(getArkUINodeHandle());
    OH_NativeXComponent_RegisterCallback(m_nativeXComponent, &m_xComponentCallback_);
    if (m_nativeXComponent) {
        std::lock_guard<std::mutex> lock(s_Mutex);
        s_componentMap[m_nativeXComponent] = this;
    }
    // 设置背景透明
    ArkUI_NumberValue preparedColorValue[] = {{.u32 = 0x00000000}};
    ArkUI_AttributeItem colorItem = {preparedColorValue, sizeof(preparedColorValue) / sizeof(ArkUI_NumberValue)};
    NativeNodeApi::getInstance()->setAttribute(m_nodeHandle, NODE_BACKGROUND_COLOR, &colorItem);
}

RNCGCanvasNode::~RNCGCanvasNode() { destroy(); }


RNCGCanvasNode *RNCGCanvasNode::getNode(OH_NativeXComponent *component) {
    if (component == nullptr) {
        return nullptr;
    }
    std::lock_guard<std::mutex> lock(s_Mutex);
    auto it = s_componentMap.find(component);
    if (it != s_componentMap.end()) {
        return it->second;
    }
    return nullptr;
}

void RNCGCanvasNode::SetCommonParam(std::string randomUUID, double pointScaleFactor) {
    m_randomUUID = randomUUID;
    m_pointScaleFactor = pointScaleFactor;
    LOG_D("gcanvas SetCommonParam m_randomUUID = %s;m_pointScaleFactor=%f", m_randomUUID.c_str(), m_pointScaleFactor);
}

bool RNCGCanvasNode::IsReady() { return isReady; }

void RNCGCanvasNode::SetWrapperContextType(int type) {
    // _2D-->0;  _3D--->1
    gCanvasSetContextType(GetKey(), type);
}

std::string RNCGCanvasNode::Render(std::string renderCommands, int type) {
    if (GetKey().empty()) {
        LOG_E("GCanvas m_randomUUID is empty");
        return "";
    }
    LOG(INFO) << "GCanvas Render renderCommands:" << renderCommands;
    return gCanvasRender(GetKey(), renderCommands, type);
}

void RNCGCanvasNode::SurfaceChange() {
    if (m_window) {
        gCanvasSurfaceCreated(GetKey(), m_window, m_width, m_height, m_backGroundColor);
    } else {
        LOG_E("GCanvas SurfaceChange OHNativeWindow is null");
    }
}

void RNCGCanvasNode::SetWindowAndSize(OHNativeWindow *window, uint64_t width, uint64_t height) {
    m_window = window;
    m_width = width;
    m_height = height;
}


void RNCGCanvasNode::SetDevicePixelRatio(double ratio) { gCanvasSetDevicePixelRatio(GetKey(), ratio); }

void RNCGCanvasNode::destroy() {
    if (!GetKey().empty()) {
        gCanvasDestroy(GetKey());
    }
    if (m_nativeXComponent != nullptr) {
        OH_NativeXComponent_RegisterCallback(m_nativeXComponent, nullptr);
        std::lock_guard<std::mutex> lock(s_Mutex);
        s_componentMap.erase(m_nativeXComponent);
        m_nativeXComponent = nullptr;
    }
    if (m_window) {
        OH_NativeWindow_DestroyNativeWindow(m_window);
        m_window = nullptr;
    }
    m_randomUUID = "";
    m_pointScaleFactor = 1.0;
}


void RNCGCanvasNode::BindTexture(OH_PixelmapNative *pixelmap, int imageID) {
    gCanvasBindTexture(pixelmap, GetKey(), imageID);
}

std::string RNCGCanvasNode::GetKey() {
    LOG_D("gcanvas GetKey m_randomUUID = %s;m_pointScaleFactor=%f", m_randomUUID.c_str(), m_pointScaleFactor);
    return m_randomUUID;
}

double RNCGCanvasNode::GetDensity() { return m_pointScaleFactor; }


OH_PixelmapNative *RNCGCanvasNode::GetNodePixelMap() {
    high_lib_handle = dlopen("libace_ndk.z.so", RTLD_LAZY);
    if (high_lib_handle == NULL) {
        LOG_D("[gcanvas RNCGCanvasNode] libace_ndk.z.so not existed");
        dlclose(high_lib_handle);
        return nullptr;
    }
    createSnapshotOptionsMethod =
        (ArkUI_SnapshotOptions * (*)()) dlsym(high_lib_handle, "OH_ArkUI_CreateSnapshotOptions");
    snapshotOptionsSetScaleMethod =
        (int32_t(*)(ArkUI_SnapshotOptions *, float))dlsym(high_lib_handle, "OH_ArkUI_SnapshotOptions_SetScale");
    getNodeSnapshotMethod = (int32_t(*)(ArkUI_NodeHandle, ArkUI_SnapshotOptions *, OH_PixelmapNative **))dlsym(
        high_lib_handle, "OH_ArkUI_GetNodeSnapshot");

    bool isCreateSnapshotOptionsMethodExisted = createSnapshotOptionsMethod != NULL;
    bool isSnapshotOptionsSetScaleMethodExisted = snapshotOptionsSetScaleMethod != NULL;
    bool isGetNodeSnapshotMethodExisted = getNodeSnapshotMethod != NULL;
    if (isCreateSnapshotOptionsMethodExisted && isSnapshotOptionsSetScaleMethodExisted &&
        isGetNodeSnapshotMethodExisted) {
        LOG_D("[gcanvas RNCGCanvasNode] method existed");
        OH_PixelmapNative *pixelMap;
        ArkUI_SnapshotOptions *options = createSnapshotOptionsMethod();
        snapshotOptionsSetScaleMethod(options, 1);
        int32_t code = getNodeSnapshotMethod(getArkUINodeHandle(), options, &pixelMap);
        if (code == ARKUI_ERROR_CODE_NO_ERROR) {
            dlclose(high_lib_handle);
            return pixelMap;
        }
    }
    dlclose(high_lib_handle);
    return nullptr;
}


void RNCGCanvasNode::DrawCanvas2Canvas(int tw, int th, std::string srcComponentId, int sx, int sy, int sw, int sh,
                                       int dx, int dy, int dw, int dh) {

    gCanvasDrawCanvas2Canvas(GetKey(), tw, th, srcComponentId, sx, sy, sw, sh, dx, dy, dw, dh);
}

void RNCGCanvasNode::ResetComponent(){
    glResetComponent(GetKey());
}
} // namespace rnoh
