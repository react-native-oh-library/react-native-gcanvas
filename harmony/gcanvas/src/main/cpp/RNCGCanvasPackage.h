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


#include "RNCGCanvasInstance.h"
#include "RNCGCanvasModule.h"
#include "generated/RNOH/generated/BaseReactNativeGcanvasPackage.h"

namespace rnoh {

class GCanvasPackageComponentInstanceFactoryDelegate : public ComponentInstanceFactoryDelegate {
public:
    using ComponentInstanceFactoryDelegate::ComponentInstanceFactoryDelegate;

    ComponentInstance::Shared create(ComponentInstance::Context ctx) override {
        if (ctx.componentName == "RNCGCanvasView") {
            return std::make_shared<rnoh::RNCGCanvasInstance>(std::move(ctx));
        }
        return nullptr;
    }
};

class RNCGCanvasTurboModuleFactoryDelegate : public TurboModuleFactoryDelegate {
public:
    SharedTurboModule createTurboModule(Context ctx, const std::string &name) const override {
        if (name == "GCanvasModule") {
             return std::make_shared<RNCGCanvasModule>(ctx, name);
        }
        return nullptr;
    }
};


class RNCGCanvasPackage : public BaseReactNativeGcanvasPackage {
    using Super = BaseReactNativeGcanvasPackage;
    using Super::Super;

    ComponentInstanceFactoryDelegate::Shared createComponentInstanceFactoryDelegate() override {
        return std::make_shared<GCanvasPackageComponentInstanceFactoryDelegate>();
    }
    std::unique_ptr<TurboModuleFactoryDelegate> createTurboModuleFactoryDelegate() override {
        return std::make_unique<RNCGCanvasTurboModuleFactoryDelegate>();
    }
};
} // namespace rnoh