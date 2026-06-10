//
// Created on 2026/5/11.
//
// Node APIs are not fully supported. To solve the compilation error of the interface cannot be found,
// please include "napi/native_api.h".

#include "GCanvas2dContext.h"
#include "CharacterSet.h"

void GCanvasContext::DrawTextWithLength(const char *text, int strLength, float x, float y, bool isStroke,
                                        float maxWidth) {
    if (strLength == 0) {
        strLength = static_cast<int>(strlen(text));
    }

    const GCompositeOperation old_op = mCurrentState->mGlobalCompositeOp;
    DoSetGlobalCompositeOperation(COMPOSITE_OP_SOURCE_OVER, COMPOSITE_OP_SOURCE_OVER);

    // scaleWidth
    float scaleWidth = 1.0;
    if (fabs(maxWidth - SHRT_MAX) > 1) {
        // 对maxwidth进行判断，避免默认值导致的每次measure操作
        float measureWidth = MeasureTextWidth(text);
        if (measureWidth > maxWidth) {
            scaleWidth = maxWidth / measureWidth;
        }
    }

    Utf8ToUCS4 *lbData = new Utf8ToUCS4(text, strLength);
    FillText(lbData->ucs4, lbData->ucs4len, x, y, isStroke, scaleWidth);

    delete lbData;
    DoSetGlobalCompositeOperation(old_op, old_op);
}

void GCanvasContext::DoSetGlobalCompositeOperation(GCompositeOperation op, GCompositeOperation alphaOp) {
    if (mCurrentState->mGlobalCompositeOp == op) {
        return;
    }
    SendVertexBufferToGPU();

    GBlendOperationFuncs funcs = GCompositeOperationFuncs(op);

    GBlendOperationFuncs alphaFuncs = GCompositeOperationFuncs(alphaOp);

    glBlendFuncSeparate(funcs.source, funcs.destination, alphaFuncs.source, alphaFuncs.destination);

    mCurrentState->mGlobalCompositeOp = op;
}
