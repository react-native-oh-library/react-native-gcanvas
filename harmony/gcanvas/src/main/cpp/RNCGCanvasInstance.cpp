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

#include "RNCGCanvasInstance.h"
#include "CommonUtils.h"
#include "RNOH/CppComponentInstance.h"
#include <glog/logging.h>
#include "imageknife.h"

namespace rnoh {

RNCGCanvasInstance::RNCGCanvasInstance(Context context) : BaseRNCGCanvasViewComponentInstance(std::move(context)) {
    ImageKnifePro::ImageKnife::GetInstance().Init();
    m_canvasNode.SetCommonParam(GenerateRandomUUID(), getLayoutMetrics().pointScaleFactor);
    m_canvasNode.SetWrapperContextType(0);
}

RNCGCanvasInstance::~RNCGCanvasInstance() {}

void RNCGCanvasInstance::onChildInserted(ComponentInstance::Shared const &childComponentInstance, std::size_t index) {
    CppComponentInstance::onChildInserted(childComponentInstance, index);
}

void RNCGCanvasInstance::onChildRemoved(ComponentInstance::Shared const &childComponentInstance) {
    CppComponentInstance::onChildRemoved(childComponentInstance);
};

RNCGCanvasNode &RNCGCanvasInstance::getLocalRootArkUINode() { return m_canvasNode; };


void RNCGCanvasInstance::onPropsChanged(SharedConcreteProps const &props) {
    CppComponentInstance::onPropsChanged(props);
};

} // namespace rnoh
