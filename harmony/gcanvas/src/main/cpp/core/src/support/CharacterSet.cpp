/**
 * Created by G-Canvas Open Source Team.
 * Copyright (c) 2017, Alibaba, Inc. All rights reserved.
 *
 * This source code is licensed under the Apache Licence 2.0.
 * For the full copyright and license information, please view
 * the LICENSE file in the root directory of this source tree.
 */
#include "CharacterSet.h"
#include "Log.h"
//TODO
//#include <unicode/utf.h>


/**
 * 行为说明：
 *   - 正确解码时，ch 写入完整码点（>0），i 前进相应的 UTF-8 字节数。
 *   - 遇到非法字节或过早结束，ch 写入 0xFFFD，i 前进 1 个字节（跳过错误字节）。
 *   - 字节长度 n 应 >= 0；若 n < 0 则视为字符串以 '\0' 结尾，此处未处理该情况（请确保 n >= 0）。
 */

inline void U8_NEXT_impl(const char* s, int& i, int n, unsigned int& ch) {
    if (n >= 0 && i >= n) {
        ch = 0xFFFFFFFF; // 表示结束（原 ICU 的 U8_NEXT 行为类似）
        return;
    }

    unsigned char c = static_cast<unsigned char>(s[i]);

    // ASCII 快速路径
    if (c < 0x80) {
        ch = c;
        ++i;
        return;
    }

    // 确定序列长度与首字节掩码
    int len;
    unsigned int codepoint;

    if (c >= 0xC2 && c <= 0xDF) {
        len = 2;
        codepoint = c & 0x1F;
    } else if (c >= 0xE0 && c <= 0xEF) {
        len = 3;
        codepoint = c & 0x0F;
    } else if (c >= 0xF0 && c <= 0xF4) {
        len = 4;
        codepoint = c & 0x07;
    } else {
        // 非法起始字节（包括 0x80-0xC1、0xF5-0xFF）
        ch = 0xFFFD;
        ++i;
        return;
    }

    // 检查后续字节是否足够且格式正确
    for (int j = 1; j < len; ++j) {
        if (i + j >= n) {
            // 字符串过早结束
            ch = 0xFFFD;
            i += j; // 跳过已检查的部分，通常 i 会停留在末尾
            return;
        }
        unsigned char next = static_cast<unsigned char>(s[i + j]);
        if ((next & 0xC0) != 0x80) {
            // 后续字节格式错误
            ch = 0xFFFD;
            i += j; // 跳过当前错误字节组
            return;
        }
        codepoint = (codepoint << 6) | (next & 0x3F);
    }

    // 检查过度长编码（overlong）和代理对（surrogate）
    if ((len == 2 && codepoint < 0x80) ||
        (len == 3 && codepoint < 0x800) ||
        (len == 4 && codepoint < 0x10000) ||
        (codepoint >= 0xD800 && codepoint <= 0xDFFF) ||
        (codepoint > 0x10FFFF)) {
        ch = 0xFFFD;
        i += len;
        return;
    }

    ch = codepoint;
    i += len;
}

#define U8_NEXT(s, i, n, ch) U8_NEXT_impl(s, i, n, ch)

Utf8ToUCS4::Utf8ToUCS4(const Utf8ToUCS4 &utf8)
    : utf8(0), utf8len(0), ucs4(0), ucs4len(0)
{
}

Utf8ToUCS4::Utf8ToUCS4(const char *utf8In, int utf8lenIn)
    : utf8(utf8In), utf8len(utf8lenIn), ucs4(0), ucs4len(0)
{
    getUcs4();
}

Utf8ToUCS4::~Utf8ToUCS4()
{
    if (ucs4) delete[] ucs4;
}

void Utf8ToUCS4::getUcs4()
{
    ucs4 = new unsigned int[utf8len + 1];

    int offset = 0;
    unsigned ch;

    int i = 0;
    while (i < utf8len) {
        // TODO
        U8_NEXT(utf8, i, utf8len, ch);
        if (ch >= 0) {
            // LOG_D("[getCodePoint][%x]", ch);
            ucs4[offset++] = ch;
        }
    }

    ucs4len = offset;
}
