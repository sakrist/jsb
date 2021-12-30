//
//  JSBridge.hpp
//  BridgeExperiment
//
//  Created by Volodymyr Boichentsov on 25/12/2021.
//

#ifndef JSBridge_hpp
#define JSBridge_hpp

#pragma once

#include <stdio.h>
#include <iostream>
#include <sstream>
#include <unordered_map>
#include <functional>

using namespace std::placeholders;

class JSBinding;

struct JSInvokeMessage {
    uintptr_t object;
    std::string function;
};

static inline void JSBindingInvokerFunction(JSBinding* object, const JSInvokeMessage& message);

class JSBridge {
public:
    static JSBridge& getInstance() {
        static JSBridge instance;
        return instance;
    }
    
    JSBridge(const JSBridge&) = delete;
    void operator=(const JSBridge&) = delete;
    
    void recive(const JSInvokeMessage& message) const {
        auto objIt = _register.find(message.object);
        if (objIt != _register.end()) {
            JSBindingInvokerFunction(objIt->second, message);
        }
    }
    
private:
    friend class JSBinding;
    JSBridge() {};
    std::unordered_map<uintptr_t, JSBinding*> _register;
};


class JSBridgeOperator {
public:
    
    virtual void send(const char* message) const = 0;

    virtual void recive(const char* message) const {
        // TODO: subclass and implement here JSInvokeMessage
//        JSInvokeMessage me = { json["object"], json["op"] };
//        JSBridge::getInstance().recive(me);
    }
};

/// not used ----
template<typename T>
struct Deducer;

template<typename ReturnType, typename ClassType, typename... Args>
struct Deducer<ReturnType(ClassType::*)(Args...)> {
    using type = ReturnType(Args...);
    using rtype = ReturnType;
    using classtype = ClassType;
    static constexpr size_t n_args = sizeof...(Args);
    
    template <size_t i>
    struct arg {
        using type = typename std::tuple_element<i, std::tuple<Args...>>::type;
    };
};
/// -----


struct FunctionInvokerBase {
    virtual void invoke(const JSInvokeMessage& message) const {};
};

template<typename T>
struct FunctionInvoker;

template<typename R, typename... Args>
struct FunctionInvoker<std::function<R(Args...)>> : public FunctionInvokerBase {
    FunctionInvoker(const std::string& n, std::function<R(Args...)> && func) : name(n), function(func) {
    }
    std::string name;
    std::function<R(Args...)>  function;
    
    void invoke(const JSInvokeMessage& message) const override {
        
        if constexpr (sizeof...(Args) == 0) {
            auto value = function();
        }
        if constexpr (sizeof...(Args) == 1) {
            function(1);
        }
        if constexpr (!std::is_same_v<void, R>) {

        }
    };
};

template<typename R, typename T, typename U, typename... Args>
std::function<R(Args...)> bindFunction(R (T::*f)(Args...), U p) {
    return [p,f](Args... args)->R { return (p->*f)(args...); };
};

class JSBinding {
    
public:
    JSBinding() {
        auto ptrKey = reinterpret_cast<uintptr_t>(this);
        JSBridge::getInstance()._register[ptrKey] = this;
    }
    
    ~JSBinding() {
        auto ptrKey = reinterpret_cast<uintptr_t>(this);
        JSBridge::getInstance()._register.erase(ptrKey);
        for (auto& [k, v] : _invokers) {
            delete v;
        }
        _invokers.clear();
    }
    
    template <typename Callable, typename ClassType>
    static inline void registerFunction(const std::string& name, Callable callable, ClassType* obj) {
        auto function = bindFunction(callable, obj);
        obj->_invokers[name] = new FunctionInvoker<decltype(function)>(name, std::move(function));
    }
    
    void invoke(const JSInvokeMessage& message) const {
        auto it = _invokers.find(message.function);
        if (it != _invokers.end()) {
            auto& invoker = it->second;
            invoker->invoke(message);
        } else {
            
            std::stringstream ss("failed to find ", std::ios_base::app |std::ios_base::out);
            ss << message.function << std::endl;
            throw std::runtime_error(ss.str());
            // TODO: do someting more
            printf("failed to find %s \n", message.function.c_str());
        }
    }

protected:
    
    std::unordered_map<std::string, FunctionInvokerBase* > _invokers;
    
    friend class JSBridge;
};

static inline void JSBindingInvokerFunction(JSBinding* object, const JSInvokeMessage& message) {
    object->invoke(message);
}



#endif /* JSBridge_hpp */
