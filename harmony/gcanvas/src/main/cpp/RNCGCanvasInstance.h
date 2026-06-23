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

#ifndef HARMONY_RNCGCANVASINSTANCE_H
#define HARMONY_RNCGCANVASINSTANCE_H

#include "generated/RNOH/generated/components/BaseRNCGCanvasViewComponentInstance.h"
#include "RNCGCanvasNode.h"
namespace rnoh {
class RNCGCanvasInstance : public BaseRNCGCanvasViewComponentInstance{
public:
    RNCGCanvasInstance(Context context);
    ~RNCGCanvasInstance();
    void onChildInserted(ComponentInstance::Shared const &childComponentInstance, std::size_t index) override;

    void onChildRemoved(ComponentInstance::Shared const &childComponentInstance) override;
    RNCGCanvasNode &getLocalRootArkUINode() override;

    void onPropsChanged(SharedConcreteProps const &props) override;

    
public:
    RNCGCanvasNode m_canvasNode;
};

} // namespace rnoh

#endif //HARMONY_RNCGCANVASINSTANCE_H
