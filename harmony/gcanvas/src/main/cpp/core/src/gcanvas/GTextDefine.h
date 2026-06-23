/**
 * Created by G-Canvas Open Source Team.
 * Copyright (c) 2017, Alibaba, Inc. All rights reserved.
 *
 * This source code is licensed under the Apache Licence 2.0.
 * For the full copyright and license information, please view
 * the LICENSE file in the root directory of this source tree.
 */
#ifndef GCANVAS_GTEXTDEFINE_H
#define GCANVAS_GTEXTDEFINE_H

typedef enum {
    GTEXT_BASELINE_ALPHABETIC = 0, // for Western fonts: Default. The text
                                   // baseline is the normal alphabetic baseline
    GTEXT_BASELINE_MIDDLE,  // The text baseline is the middle of the em square
    GTEXT_BASELINE_TOP,     // The text baseline is the top of the em square
    GTEXT_BASELINE_HANGING, // for Indian fonts : The text baseline is the
                            // hanging baseline
    GGTEXT_BASELINE_BOTTOM,  // The text baseline is the bottom of the bounding
                            // box
    GTEXT_BASELINE_IDEOGRAPHIC // for CJK fonts: The text baseline is the
                               // ideographic baseline
} GTextBaseline;

typedef enum {
    GTEXT_ALIGN_START = 0, // Default. The text baseline is the normal alphabetic baseline
    GTEXT_ALIGN_END,    // The text ends at the specified position
    GTEXT_ALIGN_LEFT,   // The text starts at the specified position
    GTEXT_ALIGN_CENTER, // The center of the text is placed at the specified
                        // position
    GTEXT_ALIGN_RIGHT   // The text ends at the specified position
} GTextAlign;

#endif /* GCANVAS_GTEXTDEFINE_H  */
