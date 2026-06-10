/**
 * Created by G-Canvas Open Source Team.
 * Copyright (c) 2017, Alibaba, Inc. All rights reserved.
 *
 * This source code is licensed under the Apache Licence 2.0.
 * For the full copyright and license information, please view
 * the LICENSE file in the root directory of this source tree.
 */

#include "GCanvasHarmony.h"
#include "GCanvas2DContextHarmony.h"
#include "GCanvas.hpp"


GCanvasHarmony::GCanvasHarmony(std::string contextId, const GCanvasConfig& config, GCanvasHooks *hooks) :
    GCanvas(contextId, config, hooks)
{
    CreateContext();
}


void GCanvasHarmony::CreateContext()
{
    mCanvasContext = new GCanvas2DContextHarmony(0, 0, mConfig);
    mCanvasContext->mContextId = this->mContextId;
//#ifndef GCANVAS_WEEX
//    if (mConfig.contextType == 0) {
//        ResetStateStack();
//        InitializeGLShader();
//    }
//#endif
}


GCanvas2DContextHarmony* GCanvasHarmony::GetCanvasContextHarmony()
{
    return (GCanvas2DContextHarmony*)mCanvasContext;
}




