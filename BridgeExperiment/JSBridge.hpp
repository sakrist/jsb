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
#include <variant>

using namespace std::placeholders;

#define JSBRIDGE_ALWAYS_INLINE __attribute__((always_inline))

namespace jsbridge {


class JSBridgeOperator {
public:
    virtual void send(const char* message) const = 0;
    virtual void recive(const char* message) const = 0;
};


class JSBridge;
using JSArg = std::variant<char, short, int, long, uintptr_t, float, double>;

struct JSInvokeMessage {
    uintptr_t object;
    std::string class_id;
    std::string function;
    std::string callback;
    std::string callback_id;
    
    int args_count{ 0 };
    JSArg args[3];
};

struct FunctionInvokerBase {
    virtual void invoke(const JSInvokeMessage& message) const {};
};
using InvokersMap = std::unordered_map<std::string, std::unique_ptr<FunctionInvokerBase>>;

class base_class_ {
protected:
    InvokersMap _invokers;
    friend class JSBridge;
};
using ClassesMap =  std::unordered_map<std::string, std::unique_ptr<base_class_>>;


class JSBridge {
public:
    static JSBridge& getInstance() {
        static JSBridge instance;
        return instance;
    }
    
    JSBridge(const JSBridge&) = delete;
    void operator=(const JSBridge&) = delete;
    
    void recive(const JSInvokeMessage& message) const noexcept(false) {
        if (message.object == 0) {
            _recive(invokers, message);
        } else {
            auto objIt = classes.find(message.class_id);
            if (objIt != classes.end()) {
                _recive(objIt->second->_invokers, message);
            } else {
                std::stringstream ss("Failed to find class id ", std::ios_base::app |std::ios_base::out);
                ss << message.class_id << std::endl;
                throw std::runtime_error(ss.str());
            }
        }
    }
    
    std::shared_ptr<JSBridgeOperator> bridgeOperator;
    
    // Module invokers
    InvokersMap invokers;
    
    ClassesMap classes;
    
private:
    
    JSBRIDGE_ALWAYS_INLINE void _recive(const InvokersMap& invs, const JSInvokeMessage& message) const noexcept(false) {
        auto invokerIt = invs.find(message.function);
        if (invokerIt != invs.end()) {
            invokerIt->second->invoke(message);
        } else {
            std::stringstream ss("Failed to find function ", std::ios_base::app |std::ios_base::out);
            ss << message.function << std::endl;
            throw std::runtime_error(ss.str());
        }
    }
    friend class base_class_;
    
    JSBridge() {};
};


template<typename R>
JSBRIDGE_ALWAYS_INLINE static void functionReturn(const JSInvokeMessage& message, R&& value) {
    if (!message.callback.empty() && !message.callback_id.empty()) {
        std::stringstream ss(message.callback, std::ios_base::app |std::ios_base::out);
        ss << "(\"" << message.callback_id << "\", " << value << ");";
        JSBridge::getInstance().bridgeOperator->send(ss.str().c_str());
    }
}


template<typename T>
struct Convert {
    auto operator()(int) -> T {
        return T{};
    }
};

template<typename T>
struct FunctionInvoker;

// TODO: implement const FunctionInvoker

template<typename ReturnType, typename ClassType, typename... Args>
struct FunctionInvoker<ReturnType (ClassType::*)(Args...)>
  : public FunctionInvokerBase {
    
    FunctionInvoker(ReturnType (ClassType::*f)(Args...) ) : _f(f){}
    
    ReturnType (ClassType::*_f)(Args...);
    
    template<std::size_t... S>
    JSBRIDGE_ALWAYS_INLINE ReturnType _invoke(ClassType* object_, std::index_sequence<S...>, const JSArg *args) const {
        
        // both S and Args expanded
//        return (object_->*_f)(Convert<Args>{}(std::get<std::tuple_element_t<S, std::tuple<Args...>>>(args[S]))...);
        return (object_->*_f)(std::get<std::tuple_element_t<S, std::tuple<Args...>>>(args[S])...);
    }
      
    void invoke(const JSInvokeMessage& message) const override {
        assert(message.object);
        
        ClassType* object_ = reinterpret_cast<ClassType*>(message.object);
        auto sequence = std::index_sequence_for<Args...>{};
        if constexpr (std::is_same_v<ReturnType, void>) {
            _invoke(object_, sequence, message.args);
        } else {
            functionReturn(message, _invoke(object_, sequence, message.args));
        }
    };
};


template<typename ReturnType, typename... Args>
struct FunctionInvoker<ReturnType (*)(Args...)> : public FunctionInvokerBase {
    
    FunctionInvoker(ReturnType (*f)(Args...) ) : _f(f){}
    
    // TODO: functions with && , i.e. perfect forwarding does not work.
    template<std::size_t... S>
    JSBRIDGE_ALWAYS_INLINE ReturnType _invoke(std::index_sequence<S...>, const JSArg *args) const {
        return (*_f)(std::get< std::tuple_element_t<S, std::tuple<Args...>> >(args[S])...);
    }
      
    void invoke(const JSInvokeMessage& message) const override {
        auto sequence = std::index_sequence_for<Args...>{};
        if constexpr (std::is_same_v<ReturnType, void>) {
            _invoke(sequence, message.args);
        } else {
            functionReturn(message, _invoke(sequence, message.args));
        }
    };
      
private:
    ReturnType (*_f)(Args...);
};

template<typename ClassType, typename... Args>
JSBRIDGE_ALWAYS_INLINE ClassType* operator_new(Args&&... args) {
    return new ClassType(std::forward<Args>(args)...);
}

template<typename ClassType>
class class_ : public base_class_ {
    
public:
    class_() = delete;
    
    explicit class_(const char* name) {
        _name = name;
    }
    
    ~class_() {
        // some what it is hack
        class_<ClassType>* _class_ = new class_<ClassType>(_name.c_str());
        std::swap(_class_->_invokers, _invokers);
        JSBridge::getInstance().classes.insert({ _name , std::unique_ptr<base_class_>( _class_ ) });
    }
    
    template <typename Callable>
    JSBRIDGE_ALWAYS_INLINE class_& function(const char* name, Callable callable) {
        _invokers.insert( { name, std::unique_ptr<FunctionInvokerBase>(new FunctionInvoker<Callable>(callable)) });
        return *this;
    }
    
    template <typename Callable>
    JSBRIDGE_ALWAYS_INLINE class_& class_function(const char* name, Callable callable) {
        return function(name, callable);
    }
    
    template<typename... ConstructorArgs, typename... Policies>
    JSBRIDGE_ALWAYS_INLINE class_& constructor(Policies... policies) {
        return constructor(&operator_new<ClassType, ConstructorArgs...>, policies...);
    }
    
    template<typename Callable, typename... Policies>
    JSBRIDGE_ALWAYS_INLINE class_& constructor(Callable callable, Policies...) {
        _invokers.insert( { "constructor", std::unique_ptr<FunctionInvokerBase>(new FunctionInvoker<Callable>(callable)) });
        return *this;
    }
    
    
private:
    std::string _name;

};



template<typename Callable, typename... Policies>
void function(const char* name, Callable callable, Policies...) {
    JSBridge::getInstance().invokers.insert( { name, std::unique_ptr<FunctionInvokerBase>(new FunctionInvoker<Callable>(callable)) });
}

}

// copy of EMSCRIPTEN_BINDINGS for compatibility reason.
#define JSBridge_BINDINGS(name)                                         \
    static struct JSBridgeBindingInitializer_##name {                   \
          JSBridgeBindingInitializer_##name();                          \
    } JSBridgeBindingInitializer_##name##_instance;                     \
JSBridgeBindingInitializer_##name::JSBridgeBindingInitializer_##name()


#endif /* JSBridge_hpp */
