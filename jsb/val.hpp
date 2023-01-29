//
//  jsb
//  Created by Volodymyr Boichentsov on 25/12/2021.
//

#pragma once
#include "json.hpp"


namespace jsb {

class val {
public:
    val() {}
    val(const nlohmann::json& object) : _object(object) { }
    
    template<typename T>
    explicit val(T&& value) {
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

}
