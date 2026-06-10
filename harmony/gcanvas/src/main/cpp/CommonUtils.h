//
// Created on 2026/5/7.
//
// Node APIs are not fully supported. To solve the compilation error of the interface cannot be found,
// please include "napi/native_api.h".
#include <multimedia/image_framework/image/image_common.h>
#include <multimedia/image_framework/image/image_source_native.h>
#include <multimedia/image_framework/image/pixelmap_native.h>
#include <native_drawing/drawing_types.h>
#include <native_drawing/drawing_font_mgr.h>
#include <string>
#include <random>
#include <iomanip>
#include <sstream>
#include "core/src/support/Log.h"
#include "libbase64.h"
#include <multimedia/image_framework/image/image_packer_native.h>

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


inline std::vector<std::string> get_sys_font_name() {
    std::vector<std::string> result;
    OH_Drawing_FontMgr *fontMgr = OH_Drawing_FontMgrCreate();
    int familyCount = OH_Drawing_FontMgrGetFamilyCount(fontMgr);
    for (int i = 0; i < familyCount; i++) {
        char *familyName = OH_Drawing_FontMgrGetFamilyName(fontMgr, i);
        if (familyName) {
            result.push_back(familyName);
            OH_Drawing_FontMgrDestroyFamilyName(familyName);
        }
    }

    OH_Drawing_FontMgrDestroy(fontMgr);
    return result;
}
}