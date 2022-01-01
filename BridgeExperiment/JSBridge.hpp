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
    std::unordered_map<std::string, std::unique_ptr<base_class_>> classes;
    
private:
    
    void _recive(const InvokersMap& invs, const JSInvokeMessage& message) const noexcept(false) {
        auto invokerIt = invs.find(message.function);
        if (invokerIt != invs.end()) {
            invokerIt->second->invoke(message);
        } else {
            std::stringstream ss("Failed to find function ", std::ios_base::app |std::ios_base::out);
            ss << message.function << std::endl;
            throw std::runtime_error(ss.str());
        }
    }
    
    JSBridge() {};
};


template<typename R>
static inline void functionReturn(const JSInvokeMessage& message, R&& value) {
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

template<typename R, typename ClassType, typename... Args>
struct FunctionInvoker<R (ClassType::*)(Args...)>
  : public FunctionInvokerBase {
    
    FunctionInvoker(R (ClassType::*f)(Args...) ) : _f(f){}
    
    R (ClassType::*_f)(Args...);
    
    template<std::size_t... S>
    inline R _invoke(ClassType* object_, std::index_sequence<S...>, const JSArg *args) const {
        
        // both S and Args expanded
//        return (object_->*_f)(Convert<Args>{}(std::get<std::tuple_element_t<S, std::tuple<Args...>>>(args[S]))...);
        return (object_->*_f)(std::get<std::tuple_element_t<S, std::tuple<Args...>>>(args[S])...);
    }
      
    void invoke(const JSInvokeMessage& message) const override {
        assert(message.object);
        
        ClassType* object_ = reinterpret_cast<ClassType*>(message.object);
        auto sequence = std::index_sequence_for<Args...>{};
        if constexpr (std::is_same_v<R, void>) {
            _invoke(object_, sequence, message.args);
        } else {
            functionReturn(message, _invoke(object_, sequence, message.args));
        }
    };
};


template<typename R, typename... Args>
struct FunctionInvoker<R (*)(Args...)>
  : public FunctionInvokerBase {
    
    FunctionInvoker(R (*f)(Args...) ) : _f(f){}
    
    R (*_f)(Args...);
    
    template<std::size_t... S>
    inline R _invoke(std::index_sequence<S...>, const JSArg *args) const {
        return (*_f)(std::get<std::tuple_element_t<S, std::tuple<Args...>>>(args[S])...);
    }
      
    void invoke(const JSInvokeMessage& message) const override {
        auto sequence = std::index_sequence_for<Args...>{};
        if constexpr (std::is_same_v<R, void>) {
            _invoke(sequence, message.args);
        } else {
            functionReturn(message, _invoke(sequence, message.args));
        }
    };
};

template<typename ClassType>
class class_ : public base_class_ {
    
public:
    class_() = delete;
    
    explicit class_(const char* name) {
        _name = name;
    }
    
    ~class_() {
        class_<ClassType>* _class_ = new class_<ClassType>(_name.c_str());
        std::swap(_class_->_invokers, _invokers);
        JSBridge::getInstance().classes.insert({ _name , std::unique_ptr<base_class_>( _class_ ) });
    }
    
    template <typename Callable>
    inline class_<ClassType>& function(const char* name, Callable callable) {
        _invokers.insert( { name, std::unique_ptr<FunctionInvokerBase>(new FunctionInvoker<Callable>(callable)) });
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
