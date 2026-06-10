#ifndef OHOS_GCANVAS_PROXY_H
#define OHOS_GCANVAS_PROXY_H

#include "GCanvasWeex.hpp"


using namespace gcanvas;

class GRenderer;

class GcanvasWeexOhos : public GCanvasWeex {
public:
    GcanvasWeexOhos(std::string componentid, GRenderer *context);

    ~GcanvasWeexOhos();

    void signalUpGLthread();

    void setRefreshFlag(bool refresh);


public:
    GRenderer *mRender;
};


#endif //OHOS_GCANVAS_PROXY_H