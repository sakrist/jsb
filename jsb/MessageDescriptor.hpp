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


template <typename T>
T convertStringToArithmetic(const std::string& str) {
    std::istringstream iss(str);
    T result;
    
    if (!(iss >> result)) {
        throw std::invalid_argument("Invalid conversion from string to arithmetic type");
    }
    
    return result;
}

class Argument {
public:
    Argument() {}
    Argument(const nlohmann::json& object) : _object(object) {
//        assert(!object.is_null());
    }
    
    template<typename T>
    explicit Argument(T&& value) {
        _object = nlohmann::basic_json(std::forward<T>(value));
    }
    
    template <typename T>
    T get() const {
        if (_object.is_string() && std::is_arithmetic_v<T>) {
            auto o = _object.get<std::string>();
            return convertStringToArithmetic<T>(o);
        }
        if (_object.is_null()) {
            return 0;
        }
        return _object.get<T>();
    }
    
    std::string get() {
        return _object.get<std::string>();
    }
    
    template <typename T, std::enable_if_t<is_shared_v<T>, int> = 0>
    uintptr_t get() {
        return _object.get<uintptr_t>();
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
