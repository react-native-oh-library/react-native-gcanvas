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
#include <list>
#include <multimedia/image_framework/image/image_common.h>
#include <multimedia/image_framework/image/image_source_native.h>
#include <multimedia/image_framework/image/pixelmap_native.h>
#include <string>
#include <random>
#include <iomanip>
#include <sstream>
#include "core/src/support/Log.h"
#include "libbase64.h"
#include <multimedia/image_framework/image/image_packer_native.h>
#include "core/src/RNCGCanvasBridge.h"
#include <dirent.h>
#include <sys/stat.h>
#include <algorithm>
#include <cctype>
#include <set>
#include <vector>

namespace rnoh {

/**
 * Generate an application unique ID
 */
inline std::string GenerateRandomUUID() {
    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_int_distribution<uint64_t> dist;

    uint64_t ab = dist(gen);
    uint64_t cd = dist(gen);

    ab = (ab & 0xFFFFFFFFFFFF0FFFULL) | 0x0000000000004000ULL;
    cd = (cd & 0x3FFFFFFFFFFFFFFFULL) | 0x8000000000000000ULL;
    std::stringstream ss;
    ss << std::hex << std::setfill('0') << std::setw(8) << (ab >> 32) << "-" << std::setw(4) << ((ab >> 16) & 0xFFFF)
       << "-" << std::setw(4) << (ab & 0xFFFF) << "-" << std::setw(4) << ((cd >> 48) & 0xFFFF) << "-" << std::setw(12)
       << (cd & 0xFFFFFFFFFFFFULL);
    return ss.str();
}

inline bool startsWith(const std::string &str, const std::string &prefix) {
    return str.size() >= prefix.size() && str.compare(0, prefix.size(), prefix) == 0;
}


inline Image_Size GetPixelMapSize(OH_PixelmapNative *data) {
    Image_Size imageSize = {0, 0};
    if (!data) {
        return imageSize;
    }
    OH_Pixelmap_ImageInfo *imageInfo = nullptr;
    Image_ErrorCode errorCode = OH_PixelmapImageInfo_Create(&imageInfo);
    if (errorCode != IMAGE_SUCCESS) {
        LOG_E("GetImageSize Create ImageInfo Failed: %d", errorCode);
        return imageSize;
    }
    errorCode = OH_PixelmapNative_GetImageInfo(data, imageInfo);
    if (errorCode != IMAGE_SUCCESS) {
        LOG_E("GetImageSize Get ImageInfo Failed: %d", errorCode);
        OH_PixelmapImageInfo_Release(imageInfo);
        return imageSize;
    }

    OH_PixelmapImageInfo_GetWidth(imageInfo, &imageSize.width);
    OH_PixelmapImageInfo_GetHeight(imageInfo, &imageSize.height);
    OH_PixelmapImageInfo_Release(imageInfo);
    return imageSize;
}

inline int GetPixelMapFormat(OH_PixelmapNative *data) {
    int32_t pixelFormat = 0;
    if (!data) {
        return pixelFormat;
    }
    OH_Pixelmap_ImageInfo *imageInfo = nullptr;
    Image_ErrorCode errorCode = OH_PixelmapImageInfo_Create(&imageInfo);
    if (errorCode != IMAGE_SUCCESS) {
        LOG_E("GetPixelMapFormat Create ImageInfo Failed: %d", errorCode);
        return pixelFormat;
    }
    errorCode = OH_PixelmapNative_GetImageInfo(data, imageInfo);
    if (errorCode != IMAGE_SUCCESS) {
        LOG_E("GetPixelMapFormat Get ImageInfo Failed: %d", errorCode);
        OH_PixelmapImageInfo_Release(imageInfo);
        return pixelFormat;
    }
    OH_PixelmapImageInfo_GetPixelFormat(imageInfo, &pixelFormat);
    OH_PixelmapImageInfo_Release(imageInfo);
    return pixelFormat;
}

inline std::vector<uint8_t> decode_base64(const std::string &base64Str) {
    size_t pos = base64Str.find(";base64,");
    if (pos == std::string::npos) {
        return {};
    }

    const int offset = 8;
    int length = base64Str.size() - pos - offset;

    std::vector<uint8_t> buffer(length * 3 / 4 + 1);
    size_t outLength = buffer.size();
    const int plainCodec = 1 << 3;
    base64_decode(base64Str.c_str() + pos + offset, length, reinterpret_cast<char *>(buffer.data()), &outLength,
                  plainCodec);

    buffer.resize(outLength);
    return buffer;
}

inline OH_PixelmapNative *createPixelmap_encodedData(uint8_t *data, size_t dataSize) {
    if (!data) {
        LOG_E("createPixelmap_encodedData uint8_t data is null");
        return nullptr;
    }
    OH_ImageSourceNative *imageSource = nullptr;
    Image_ErrorCode err = OH_ImageSourceNative_CreateFromData(data, dataSize, &imageSource);
    if (err != IMAGE_SUCCESS || imageSource == nullptr) {
        LOG_E("createPixelmap_encodedData get imageSource failed code:%d", err);
        return nullptr;
    }
    OH_PixelmapNative *pixelmap = nullptr;
    err = OH_ImageSourceNative_CreatePixelmap(imageSource, nullptr, &pixelmap);

    OH_ImageSourceNative_Release(imageSource);

    if (err != IMAGE_SUCCESS || pixelmap == nullptr) {
        LOG_E("createPixelmap_encodedData get PixelMap failed code:%d", err);
        return nullptr;
    }

    return pixelmap;
}


inline std::string decodeCode_pixelmap(OH_PixelmapNative *data, std::string mimeType, int quality) {
    LOG_D("decodeCode_pixelmap start");
    OH_ImagePackerNative *packer = nullptr;
    Image_ErrorCode errCode = OH_ImagePackerNative_Create(&packer);
    if (errCode != IMAGE_SUCCESS) {
        LOG_E("decodeCode_pixelmap create packer failed code:%d", errCode);
        return "";
    }

    auto size = GetPixelMapSize(data);
    int channel = 4;
    std::string dataUrl = "data:image/png;base64,";
    if (strcmp(mimeType.c_str(), "image/jpeg") == 0) {
        dataUrl = "data:image/jpeg;base64,";
    }
    OH_PackingOptions *options = nullptr;
    errCode = OH_PackingOptions_Create(&options);
    if (errCode != IMAGE_SUCCESS) {
        LOG_E("decodeCode_pixelmap create options failed code:%d", errCode);
        OH_ImagePackerNative_Release(packer);
        return "";
    }
    size_t bufferSize = size.width * size.height * channel + 4096;
    uint8_t *outBuffer = (uint8_t *)malloc(bufferSize);
    Image_String mimeType1 = {.data = (char *)mimeType.c_str(), .size = mimeType.length()};
    OH_PackingOptions_SetMimeType(options, &mimeType1);
    OH_PackingOptions_SetQuality(options, quality);
    errCode = OH_ImagePackerNative_PackToDataFromPixelmap(packer, options, data, outBuffer, &bufferSize);
    if (errCode != IMAGE_SUCCESS) {
        free(outBuffer);
        OH_PackingOptions_Release(options);
        OH_ImagePackerNative_Release(packer);
        LOG_E("decodeCode_pixelmap OH_ImagePackerNative_PackToDataFromPixelmap failed code:%d", errCode);
        return "";
    }
    OH_PackingOptions_Release(options);
    OH_ImagePackerNative_Release(packer);

    size_t base64Len = (bufferSize + 2) / 3 * 4 + 1; // +1 保留结尾符，虽然函数不会自动写 '\0'
    char *base64Out = (char *)malloc(base64Len);
    size_t actualBase64Len = base64Len;
    base64_encode(reinterpret_cast<const char *>(outBuffer), bufferSize, base64Out, &actualBase64Len, 0);

    dataUrl.append(base64Out, actualBase64Len);
    free(outBuffer);
    free(base64Out);
    LOG_D("decodeCode_pixelmap success");
    return dataUrl;
}

inline std::string gcanvas_to_lower_font_name(const std::string &value) {
    std::string result = value;
    std::transform(result.begin(), result.end(), result.begin(), [](unsigned char c) {
        return static_cast<char>(std::tolower(c));
    });
    return result;
}

inline std::string gcanvas_replace_font_separator_to_space(const std::string &value) {
    std::string result = value;
    std::replace(result.begin(), result.end(), '_', ' ');
    std::replace(result.begin(), result.end(), '-', ' ');
    return result;
}

inline bool gcanvas_is_font_file(const std::string &fileName) {
    std::string lowerName = gcanvas_to_lower_font_name(fileName);
    return lowerName.size() > 4 &&
           (lowerName.rfind(".ttf") == lowerName.size() - 4 ||
            lowerName.rfind(".otf") == lowerName.size() - 4 ||
            lowerName.rfind(".ttc") == lowerName.size() - 4);
}

inline std::string gcanvas_remove_font_ext(const std::string &fileName) {
    size_t pos = fileName.find_last_of('.');
    if (pos == std::string::npos) {
        return fileName;
    }
    return fileName.substr(0, pos);
}

inline bool gcanvas_file_exists(const std::string &path) {
    struct stat buffer {};
    return stat(path.c_str(), &buffer) == 0 && S_ISREG(buffer.st_mode);
}

inline void gcanvas_insert_font_family(const std::string &fontName, const std::string &fontFile) {
    if (fontName.empty() || fontFile.empty()) {
        return;
    }

    if (glHasFontFamily(fontName)) {
        return;
    }

    glInsertFontFamily(fontName, fontFile);
}

inline void gcanvas_insert_font_family_with_alias(const std::string &fontName, const std::string &fontFile) {
    if (fontName.empty() || fontFile.empty()) {
        return;
    }

    std::string lowerName = gcanvas_to_lower_font_name(fontName);
    std::string spaceName = gcanvas_replace_font_separator_to_space(fontName);
    std::string lowerSpaceName = gcanvas_to_lower_font_name(spaceName);

    gcanvas_insert_font_family(fontName, fontFile);
    gcanvas_insert_font_family(lowerName, fontFile);

    if (spaceName != fontName) {
        gcanvas_insert_font_family(spaceName, fontFile);
    }

    if (lowerSpaceName != lowerName && lowerSpaceName != spaceName) {
        gcanvas_insert_font_family(lowerSpaceName, fontFile);
    }
}

inline std::string gcanvas_find_default_system_font_file(const std::string &systemFontLocation) {
    std::vector<std::string> candidates = {
        "HarmonyOS_Sans.ttf"
    };
    for (auto &fileName : candidates) {
        if (gcanvas_file_exists(systemFontLocation + fileName)) {
            return fileName;
        }
    }
    return "";
}

inline void init_gcanvas_system_fonts() {
    static bool initialized = false;
    if (initialized) {
        return;
    }
    initialized = true;

    const std::string systemFontLocation = "/system/fonts/";

    glSetSystemFontLocation(systemFontLocation);

    std::string defaultFontFile = gcanvas_find_default_system_font_file(systemFontLocation);
    if (!defaultFontFile.empty()) {
        glSetDefaultFontFile(defaultFontFile);

        gcanvas_insert_font_family_with_alias("sans-serif", defaultFontFile);
        gcanvas_insert_font_family_with_alias("sans", defaultFontFile);
        gcanvas_insert_font_family_with_alias("default", defaultFontFile);
    }

    DIR *dir = opendir(systemFontLocation.c_str());
    if (dir == nullptr) {
        LOG_E("init_gcanvas_system_fonts open system font dir failed:%s", systemFontLocation.c_str());
        return;
    }

    struct dirent *entry = nullptr;
    while ((entry = readdir(dir)) != nullptr) {
        std::string fontFile = entry->d_name;
        if (!gcanvas_is_font_file(fontFile)) {
            continue;
        }

        std::string fontName = gcanvas_remove_font_ext(fontFile);
        gcanvas_insert_font_family_with_alias(fontName, fontFile);
    }

    closedir(dir);
}

inline std::vector<std::string> get_gcanvas_font_names() {
    init_gcanvas_system_fonts();

    std::vector<std::string> result;
    std::set<std::string> exists;

    auto fontNames = glGetFontNames();
    for (auto &fontName : fontNames) {
        if (!fontName.empty() && exists.insert(fontName).second) {
            result.push_back(fontName);
        }
    }

    return result;
}

inline void set_gcanvas_extra_font_location(const std::string &fontLocation) {
    if (fontLocation.empty()) {
        return;
    }

    std::string location = fontLocation;
    if (location.back() != '/') {
        location += "/";
    }

    glSetExtraFontLocation(location);

    LOG_D("set_gcanvas_extra_font_location:%s", location.c_str());
}

inline void add_gcanvas_font_family(const std::string &fontName, const std::string &fontFile) {
    if (fontName.empty() || fontFile.empty()) {
        return;
    }

    init_gcanvas_system_fonts();

    gcanvas_insert_font_family_with_alias(fontName, fontFile);

    LOG_D("add_gcanvas_font_family fontName:%s fontFile:%s", fontName.c_str(), fontFile.c_str());
}

}