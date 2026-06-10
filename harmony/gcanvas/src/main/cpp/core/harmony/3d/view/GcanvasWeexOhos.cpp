#include "GcanvasWeexOhos.h"
#include "grenderer.h"

using namespace gcanvas;

GcanvasWeexOhos::GcanvasWeexOhos(std::string componentid, GRenderer *context) :
        GCanvasWeex(componentid, GCanvasConfig({false, true})), mRender(context) {

}

GcanvasWeexOhos::~GcanvasWeexOhos() {
    mRender = nullptr;
}

/**
 * GCanvasWeexAndroid和GRenderer形成环状依赖,
 */
void GcanvasWeexOhos::signalUpGLthread() {
    if (mRender != nullptr) {
        mRender->signalUpGLthread();
    }
}

void GcanvasWeexOhos::setRefreshFlag(bool refresh) {
    if (mRender != nullptr) {
        mRender->setRefreshFlag(refresh);
    }
}
