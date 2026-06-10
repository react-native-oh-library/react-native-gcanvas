//
// Created on 2026/5/6.
//
// Node APIs are not fully supported. To solve the compilation error of the interface cannot be found,
// please include "napi/native_api.h".

#pragma once

namespace rnoh {

class IReactCacheCmd {
public:
    virtual ~IReactCacheCmd() = default;
    virtual void execute() = 0;
};



} // namespace rnoh
