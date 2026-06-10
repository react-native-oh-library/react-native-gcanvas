/**
 * Created by G-Canvas Open Source Team.
 * Copyright (c) 2017, Alibaba, Inc. All rights reserved.
 *
 * This source code is licensed under the Apache Licence 2.0.
 * For the full copyright and license information, please view
 * the LICENSE file in the root directory of this source tree.
 */

#ifndef GCANVAS_GCanvasHarmony_H
#define GCANVAS_GCanvasHarmony_H


#include "GCanvas.hpp"
#include "GCanvas2DContextHarmony.h"


class GCanvasHarmony : public gcanvas::GCanvas {


public:


    API_EXPORT GCanvasHarmony(std::string contextId, const GCanvasConfig& config, GCanvasHooks *hooks = nullptr);


    void CreateContext() override;


    API_EXPORT GCanvas2DContextHarmony* GetCanvasContextHarmony();


};


#endif //GCANVAS_GCanvasHarmony_H

