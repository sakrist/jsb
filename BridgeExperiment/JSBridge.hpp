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
#include <type_traits>

using namespace std::placeholders;

#define JSBRIDGE_ALWAYS_INLINE __attribute__((always_inline))

namespace jsbridge {

template <typename T>
struct compatible_type {
    inline static constexpr bool value = (std::is_same_v<T, void>
                                          || std::is_same_v<T, bool>
                                          || std::is_same_v<T, char>
                                          || std::is_same_v<T, unsigned char>
                                          || std::is_same_v<T, short>
                                          || std::is_same_v<T, unsigned short>
                                          || std::is_same_v<T, int>
                                          || std::is_same_v<T, unsigned int>
                                          || std::is_same_v<T, long>
                                          || std::is_same_v<T, unsigned long>
                                          || std::is_same_v<T, float>
                                          || std::is_same_v<T, double>
                                          || std::is_same_v<T, uintptr_t>
                                          || std::is_pointer_v<T>
                                          );
};

template<typename T>
inline constexpr bool compatible_type_v = compatible_type<T>::value;

template <typename T>
struct remove_all : std::remove_cv<std::remove_reference_t<std::remove_pointer_t<T>>> { };
template <class _Tp>
using remove_all_t = typename remove_all<_Tp>::type;


// TODO: I don't like this

template <typename T, std::enable_if_t<std::is_same_v<T, int&&>, int> = 0>
JSBRIDGE_ALWAYS_INLINE auto arg_converter(int val) {
    return std::forward<T>(val);
};

template <typename T, std::enable_if_t<std::is_same_v<T, double&&>, int> = 0>
JSBRIDGE_ALWAYS_INLINE auto arg_converter(double val) {
    return std::forward<T>(val);
};
template <typename T, std::enable_if_t<std::is_same_v<T, float&&>, int> = 0>
JSBRIDGE_ALWAYS_INLINE auto arg_converter(float val) {
    return std::forward<T>(val);
};

template <typename T>
auto arg_converter(T&& val) {
    return std::forward<T>(val);
};

template <typename T>
auto arg_converter(T val) {
    return std::forward<T>(val);
};

template <typename T, std::enable_if_t<std::is_pointer_v<T>, int> = 0>
JSBRIDGE_ALWAYS_INLINE T arg_converter(uintptr_t val) {
    return reinterpret_cast<T>(val);
};


//class JSClassGenerator {
//    void 
//};

class JSBridgeCommunicator {
public:
    
    virtual ~JSBridgeCommunicator() = default;
    
    // sending message to JS from C++
    virtual void eval(const char* js_code, std::optional<std::function<void(const char*)>>&& completion = std::nullopt) const = 0;
    
    // JS calls C++ code
    virtual void recive(const char* message, std::optional<std::function<void(const char*)>>&& completion = std::nullopt) const = 0;
};


class JSBridge;
using JSArg = std::variant<bool, char, short, int, long, uintptr_t, float, double>;

struct JSInvokeMessage {
    uintptr_t object;
    std::string class_id;
    std::string function;
    
    // for async
    std::string callback;
    std::string callback_id;
    
    // sync
    std::function<void(const char*)> completion{nullptr};
    
    int args_count{ 0 };
    JSArg args[3];
};

struct FunctionInvokerBase {
    virtual void invoke(const JSInvokeMessage& message) {};
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
    
    
    static void eval(const char* js_code, std::optional<std::function<void(const char*)>>&& completion = std::nullopt) {
        getInstance()._communicator->eval(js_code, std::forward<decltype(completion)>(completion));
    }
    
    // JS calls C++ code
    static void recive(const char* message, std::optional<std::function<void(const char*)>>&& completion = std::nullopt) {
        getInstance()._communicator->recive(message, std::forward<decltype(completion)>(completion));
    }
    
    template <typename T, typename... Args>
    static void registerCommunicator(Args&&... args) {
        static_assert(std::is_base_of<JSBridgeCommunicator, T>::value, "Type must be derived from JSBridgeCommunicator");
        if(getInstance()._communicator) {
            assert(0);
        }
        getInstance()._communicator = std::make_unique<T>(std::forward<Args>(args)...);
    }
    
    void recive(JSInvokeMessage& message) const noexcept(false) {
        if (message.class_id.empty()) {
            _recive(invokers, message);
        } else {
            auto classIt = classes.find(message.class_id);
            if (classIt != classes.end()) {
                _recive(classIt->second->_invokers, message);
            } else {
                std::stringstream ss("Failed to find class id ", std::ios_base::app |std::ios_base::out);
                ss << message.class_id << std::endl;
                printf("%s\n", ss.str().c_str());
                throw std::logic_error(ss.str());
            }
        }
    }
    
    // Module functions registered from global scope
    InvokersMap invokers;
    
    // registered classes
    ClassesMap classes;
    
private:
    friend class base_class_;
    
    JSBridge() {};
    
    JSBRIDGE_ALWAYS_INLINE void _recive(const InvokersMap& invs, const JSInvokeMessage& message) const noexcept(false) {
        auto invokerIt = invs.find(message.function);
        if (invokerIt != invs.end()) {
            invokerIt->second->invoke(message);
        } else {
            std::stringstream ss("Failed to find function ", std::ios_base::app |std::ios_base::out);
            ss << message.function << std::endl;
            printf("%s\n", ss.str().c_str());
            throw std::logic_error(ss.str());
        }
    }
    
    std::unique_ptr<JSBridgeCommunicator> _communicator;
};


template<typename R>
JSBRIDGE_ALWAYS_INLINE static void functionReturn(const JSInvokeMessage& message, R&& value) {
 //   assert(!message.callback.empty() && !message.callback_id.empty());
    
    if (message.completion) {
        std::stringstream ss(message.callback, std::ios_base::app |std::ios_base::out);
        ss << "{\"r\":\"";
        if constexpr (std::is_pointer_v<R>) {
            ss << reinterpret_cast<uintptr_t>(value);
        } else {
            ss << value;
        }
        ss << "\"}";
        
        message.completion(ss.str().c_str());
    } else {
        // async approach
        std::stringstream ss(message.callback, std::ios_base::app |std::ios_base::out);
        ss << "(\"" << message.callback_id << "\", ";
        if constexpr (std::is_pointer_v<R>) {
            ss << reinterpret_cast<uintptr_t>(value);
        } else {
            ss << value;
        }
        ss << ");";
        
        JSBridge::eval(ss.str().c_str());
    }
}


template<typename T>
struct FunctionInvoker;

template<typename ReturnType, typename ClassType, typename... Args>
struct FunctionInvoker<ReturnType (ClassType::*)(Args...) const noexcept> : public FunctionInvokerBase {
    
    FunctionInvoker(ReturnType (ClassType::*f)(Args...) const noexcept) : _f(f){
        static_assert(compatible_type_v<std::remove_cv_t<ReturnType>>, "incompatible return type");
    }
      
    template<std::size_t... S>
    JSBRIDGE_ALWAYS_INLINE ReturnType _invoke(ClassType* object_, std::index_sequence<S...>, const JSArg *args) {
        
        // Here we are executing function from object
        return (object_->*_f)
        (arg_converter<std::tuple_element_t<S, std::tuple<Args...>>>(std::get<
                                  typename std::conditional<std::is_pointer_v<std::tuple_element_t<S, std::tuple<Args...>>>,
                                         uintptr_t,
                                         remove_all_t<std::tuple_element_t<S, std::tuple<Args...>>>>::type
                                 >(args[S]))...);
    }
      
    void invoke(const JSInvokeMessage& message) override {
        assert(message.object);
        
        ClassType* object_ = reinterpret_cast<ClassType*>(message.object);
        auto sequence = std::index_sequence_for<Args...>{};
        if constexpr (std::is_same_v<ReturnType, void>) {
            _invoke(object_, sequence, message.args);
        } else {
            functionReturn(message, std::forward<ReturnType>(_invoke(object_, sequence, message.args)));
        }
    };
      
private:
    ReturnType (ClassType::*_f)(Args...) const noexcept;
};

template<typename ReturnType, typename ClassType, typename... Args>
struct FunctionInvoker<ReturnType (ClassType::*)(Args...)> : public FunctionInvokerBase {
    
    FunctionInvoker(ReturnType (ClassType::*f)(Args...) ) : _f(f){
        static_assert(compatible_type_v<std::remove_cv_t<ReturnType>>, "incompatible return type");
    }
      
    template<std::size_t... S>
    JSBRIDGE_ALWAYS_INLINE ReturnType _invoke(ClassType* object_, std::index_sequence<S...>, const JSArg *args) {
        
        // Here we are executing function from object
        return (object_->*_f)
        (arg_converter<std::tuple_element_t<S, std::tuple<Args...>>>(std::get<
                                  typename std::conditional<std::is_pointer_v<std::tuple_element_t<S, std::tuple<Args...>>>,
                                         uintptr_t,
                                         remove_all_t<std::tuple_element_t<S, std::tuple<Args...>>>>::type
                                 >(args[S]))...);
    }
      
    void invoke(const JSInvokeMessage& message) override {
        assert(message.object);
        
        ClassType* object_ = reinterpret_cast<ClassType*>(message.object);
        auto sequence = std::index_sequence_for<Args...>{};
        if constexpr (std::is_same_v<ReturnType, void>) {
            _invoke(object_, sequence, message.args);
        } else {
            functionReturn(message, std::forward<ReturnType>(_invoke(object_, sequence, message.args)));
        }
    };
      
private:
    ReturnType (ClassType::*_f)(Args...);
};


template<typename ReturnType, typename... Args>
struct FunctionInvoker<ReturnType (*)(Args...)> : public FunctionInvokerBase {
    
    FunctionInvoker(ReturnType (*f)(Args...)) : _f(f)  {
        static_assert(compatible_type_v<std::remove_cv_t<ReturnType>>, "incompatible return type");
    }
    
    template<std::size_t... S>
    JSBRIDGE_ALWAYS_INLINE ReturnType _invoke(std::index_sequence<S...>, const JSArg *args) {
        return (*_f)
        (arg_converter<std::tuple_element_t<S, std::tuple<Args...>>>(std::get<
                                     typename std::conditional<std::is_pointer_v<std::tuple_element_t<S, std::tuple<Args...>>>,
                                            uintptr_t,
                                            remove_all_t<std::tuple_element_t<S, std::tuple<Args...>>>>::type
                                    >(args[S]))...);
    }
      
    void invoke(const JSInvokeMessage& message) override {
        auto sequence = std::index_sequence_for<Args...>{};
        if constexpr (std::is_same_v<ReturnType, void>) {
            _invoke(sequence, message.args);
        } else {
            functionReturn(message, std::forward<ReturnType>(_invoke(sequence, message.args)));
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
    
    explicit class_(const char* name) noexcept(false) {
        _name = name;
        auto it = JSBridge::getInstance().classes.find(_name);
        if (it != JSBridge::getInstance().classes.end()) {
            std::stringstream ss("Class with ", std::ios_base::app |std::ios_base::out);
            ss << _name << " already bind" << std::endl;
            printf("%s\n", ss.str().c_str());
            throw std::logic_error(ss.str());            
        }
    }
    
    ~class_() {
        // some what it is hack
        class_<ClassType>* _class_ = new class_<ClassType>(_name.c_str());
        std::swap(_class_->_invokers, _invokers);
        JSBridge::getInstance().classes.insert({ _name , std::unique_ptr<base_class_>( _class_ ) });
    }
    
    template <typename Callable>
    JSBRIDGE_ALWAYS_INLINE class_& function(const char* name, Callable callable) {
        _addFunction( { name, std::unique_ptr<FunctionInvokerBase>(new FunctionInvoker<Callable>(callable)) });
        return *this;
    }
    
    template <typename Callable>
    JSBRIDGE_ALWAYS_INLINE class_& class_function(const char* name, Callable callable) {
        _addFunction( { name, std::unique_ptr<FunctionInvokerBase>(new FunctionInvoker<Callable>(callable)) });
        return *this;
    }
    
    template<typename... ConstructorArgs, typename... Policies>
    JSBRIDGE_ALWAYS_INLINE class_& constructor(Policies... policies) {
        return constructor(&operator_new<ClassType, ConstructorArgs...>, policies...);
    }
    
    template<typename Callable, typename... Policies>
    JSBRIDGE_ALWAYS_INLINE class_& constructor(Callable callable, Policies...) {
        _addFunction( { "constructor", std::unique_ptr<FunctionInvokerBase>(new FunctionInvoker<Callable>(callable)) });
        return *this;
    }
    
    
private:
    
    JSBRIDGE_ALWAYS_INLINE void _addFunction(std::pair<std::string, std::unique_ptr<FunctionInvokerBase>>&& pair) {
        auto it = _invokers.find(pair.first);
        if (it != _invokers.end()) {
            printf("Error: overriding key: %s\n", pair.first.c_str());
            assert(0);
        }
        _invokers.insert(std::move(pair));
    }
    
    void _generateJS() {
        
    }
    
    std::string _name;

};



template<typename Callable, typename... Policies>
void function(const char* name, Callable callable, Policies...) {
    JSBridge::getInstance().invokers.insert( { name, std::unique_ptr<FunctionInvokerBase>(new FunctionInvoker<Callable>(callable)) });
}





// copy from emscripten

namespace internal {



template<typename VectorType>
struct VectorAccess {
    // TODO: impl
//    static val get(
//        const VectorType& v,
//        typename VectorType::size_type index
//    ) {
//        if (index < v.size()) {
//            return val(v[index]);
//        } else {
//            return val::undefined();
//        }
//    }

    static bool set(
        VectorType& v,
        typename VectorType::size_type index,
        const typename VectorType::value_type& value
    ) {
        v[index] = value;
        return true;
    }
};

} // end namespace internal



template<typename T>
class_<std::vector<T>> register_vector(const char* name) {
    typedef std::vector<T> VecType;

    void (VecType::*push_back)(const T&) = &VecType::push_back;
    void (VecType::*resize)(const size_t, const T&) = &VecType::resize;
    size_t (VecType::*size)() const = &VecType::size;
    return class_<std::vector<T>>(name)
        .template constructor<>()
        .function("push_back", push_back)
        .function("resize", resize)
        .function("size", size)
    // TODO: implement get set for vector
//        .function("get", &internal::VectorAccess<VecType>::get)
        .function("set", &internal::VectorAccess<VecType>::set);
}


} // namespace jsbridge

// copy of EMSCRIPTEN_BINDINGS/BOOST_PYTHON_MODULE for compatibility reason.
#define JSBridge_BINDINGS(name)                                         \
    static struct JSBridgeBindingInitializer_##name {                   \
          JSBridgeBindingInitializer_##name();                          \
    } JSBridgeBindingInitializer_##name##_instance;                     \
JSBridgeBindingInitializer_##name::JSBridgeBindingInitializer_##name()


#endif /* JSBridge_hpp */
