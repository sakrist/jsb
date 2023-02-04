//
//  MessageDescriptor.hpp
//  BridgeExperiment
//
//  Created by Volodymyr Boichentsov on 28/01/2023.
//

#ifndef JSMessageDescriptor_hpp
#define JSMessageDescriptor_hpp

#pragma once

#include <stdio.h>
#include "Helpers.hpp"
#include "Bridge.hpp"

#include "json.hpp"

namespace jsb {


class Argument {
public:
    Argument() {}
    Argument(const nlohmann::json& object) : _object(object) {
        assert(!object.is_null());
    }
    
    template<typename T>
    explicit Argument(T&& value) {
        _object = nlohmann::basic_json(std::forward<T>(value));
    }
    
    template <typename T>
    T get() const {
        return _object.get<T>();
    }
    
    template <typename T>
    T get() {
        return _object.get<T>();
    }
    
    std::string get() {
        return _object.get<std::string>();
    }
    
    template <typename T, std::enable_if_t<std::is_pointer_v<T>, int> = 0>
    uintptr_t get() {
        return _object.get<uintptr_t>();
    }
    
    
private:
    nlohmann::json _object;
//     handle;
};

struct MessageDescriptor {
    uintptr_t object;
    std::string class_id;
    std::string function;
    
    // for async
    std::string callback;
    std::string callback_id;
    
    // sync
    std::function<void(const char*)> completion{ nullptr };
    
    Argument args[5];
};


}

#endif /* JSMessageDescriptor_hpp */
