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
#include <RNOH/arkui/ArkUINode.h>
#include <ace/xcomponent/native_interface_xcomponent.h>

#ifndef HARMONY_RNCGCANVASNODE_H
#define HARMONY_RNCGCANVASNODE_H

namespace rnoh {
class RNCGCanvasNode : public ArkUINode {
public:
    RNCGCanvasNode();
    ~RNCGCanvasNode() override;
    static void OnSurfaceCreated(OH_NativeXComponent *component, void *window);
    static void OnSurfaceChanged(OH_NativeXComponent *component, void *window);
    static void OnSurfaceDestroyed(OH_NativeXComponent *component, void *window);
    static void DispatchTouchEvent(OH_NativeXComponent *component, void *window);
    static RNCGCanvasNode *getNode(OH_NativeXComponent *component);
    bool IsReady();
    void SetCommonParam(std::string randomUUID, double pointScaleFactor);
    void SetWrapperContextType(int type);
    std::string Render(std::string renderCommands, int type);
    std::string GetKey();
    double GetDensity();
    void SurfaceChange();
    void SetDevicePixelRatio(double ratio);
    void BindTexture(OH_PixelmapNative *pixelmap, int imageID);
    void ResetComponent();
    OH_PixelmapNative *GetNodePixelMap();
    void SetWindowAndSize(OHNativeWindow *window, uint64_t width, uint64_t height);

    void DrawCanvas2Canvas(int tw, int th, std::string srcComponentId, int sx, int sy, int sw, int sh, int dx, int dy,
                           int dw, int dh);
    
    void destroy();

private:
    void setReady(bool ready) { isReady = ready; }

private:
    OH_NativeXComponent_Callback m_xComponentCallback_;
    OH_NativeXComponent *m_nativeXComponent{nullptr};
    OHNativeWindow *m_window{nullptr};
    uint64_t m_width{0};
    uint64_t m_height{0};
    static inline std::unordered_map<OH_NativeXComponent *, RNCGCanvasNode *> s_componentMap;
    static inline std::mutex s_Mutex;
    bool isReady{false};
    std::string m_randomUUID{""};
    double m_pointScaleFactor{1.0};
    std::string m_backGroundColor{"#00000000"};
    void *high_lib_handle = NULL;
    ArkUI_SnapshotOptions *(*createSnapshotOptionsMethod)();
    int32_t (*snapshotOptionsSetScaleMethod)(ArkUI_SnapshotOptions *snapshotOptions, float scale);
    int32_t (*getNodeSnapshotMethod)(ArkUI_NodeHandle node, ArkUI_SnapshotOptions *snapshotOptions,
                                     OH_PixelmapNative **pixelmap);
};
} // namespace rnoh


#endif // HARMONY_RNCGCANVASNODE_H
