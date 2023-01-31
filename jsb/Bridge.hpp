//
//  jsb
//  Created by Volodymyr Boichentsov on 25/12/2021.
//

#pragma once

#include <stdio.h>
#include <iostream>
#include <sstream>
#include <unordered_map>
#include <functional>
#include <variant>
#include <string>
#include <vector>
#include <type_traits>
#include "CodeGenerator.hpp"
#include "MessageDescriptor.hpp"
#include "Helpers.hpp"


using namespace std::placeholders;


namespace jsb {


class JSBridgeCommunicator {
public:
    
    virtual ~JSBridgeCommunicator() = default;
        
    // sending message to JS from C++
    virtual void eval(const char* js_code, std::function<void(const char*)>&& completion) const = 0;
    
    // JS calls C++ code
    virtual void recive(const char* json, std::function<void(const char*)>&& completion) const = 0;
};



// MARK: - FunctionInvokerBase
enum class InvokerType {
    Function,
    ClassFunction,
    ClassFunctionConst
};

struct FunctionInvokerBase {
    FunctionInvokerBase(FunctionDescriptor aDescriptor) : descriptor(aDescriptor) {}
    virtual void invoke(const MessageDescriptor& message) {};
    virtual InvokerType getType() { return InvokerType::Function; };
    const FunctionDescriptor descriptor;
};
using InvokersMap = std::unordered_map<std::string, std::unique_ptr<FunctionInvokerBase>>;

// MARK: -

class Bridge;
class base_class_ {
public:
    virtual ~base_class_() = default;
protected:
    friend class Bridge;
    virtual void registerJS(const std::string& moduleName) = 0;
    InvokersMap _invokers;
};
using ClassesMap = std::unordered_map<std::string, std::unique_ptr<base_class_>>;


class Bridge {
public:
    static Bridge& getInstance();
    
    Bridge(const Bridge&) = delete;
    void operator=(const Bridge&) = delete;
    
    
    static void eval(const char* js_code, std::function<void(const char*)>&& completion = nullptr) {
        getInstance()._communicator->eval(js_code, std::forward<decltype(completion)>(completion));
    }
    
    // JS calls C++ code
    static void recive(const char* message, std::function<void(const char*)>&& completion = nullptr) {
        getInstance()._communicator->recive(message, std::forward<decltype(completion)>(completion));
    }
    
    void setModuleName(std::string& name) {
        _moduleName = name;
    }
    
    template <typename T, typename... Args>
    static void registerCommunicator(Args&&... args) {
        static_assert(std::is_base_of<JSBridgeCommunicator, T>::value, "Type must be derived from JSBridgeCommunicator");
        if(_instance->_communicator) {
            assert(0);
        }
        _instance->_communicator = std::make_unique<T>(std::forward<Args>(args)...);
        
        const auto& moduleName = _instance->_moduleName;
        CodeGenerator::registerBase(moduleName);
        for(const auto& [key, cls] : _instance->classes) {
            cls->registerJS(moduleName);
        }
        
        for(const auto& [key, inv] : _instance->invokers) {
            auto jscode = CodeGenerator::function(moduleName, moduleName, inv->descriptor);
            _instance->eval(jscode.c_str());
        }
    }
    
    void recive(MessageDescriptor& message) const noexcept(false) {
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
    ClassesMap classes;
    
private:
    Bridge() {};
    friend class base_class_;
    
    inline static Bridge* _instance{ nullptr };
    static std::mutex _mutex;
    
    std::string _moduleName{ "JSBModule" };
    
    JSB_ALWAYS_INLINE void _recive(const InvokersMap& invs, const MessageDescriptor& message) const noexcept(false) {
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

JSB_ALWAYS_INLINE inline static void functionVoid(const MessageDescriptor& message) {
    if (message.completion) {
        message.completion("");
    }
}

template<typename R>
JSB_ALWAYS_INLINE static void functionReturn(const MessageDescriptor& message, R&& value) {
    
    if (message.completion) {
        std::stringstream ss(message.callback, std::ios_base::app |std::ios_base::out);
        
        if constexpr (std::is_pointer_v<R>) {
            ss << reinterpret_cast<uintptr_t>(value);
        } else if constexpr (std::is_same_v<R, std::string>) {
            ss << value;
        } else {
            ss << value;
        }
#if DEBUG
        std::cout << "return: " << ss.str() << std::endl;
#endif
        
        message.completion(ss.str().c_str());
    } else {
        assert(!message.callback.empty() && !message.callback_id.empty());
        
        // async approach
        std::stringstream ss(message.callback, std::ios_base::app |std::ios_base::out);
        ss << "(\"" << message.callback_id << "\", ";
        if constexpr (std::is_pointer_v<R>) {
            ss << reinterpret_cast<uintptr_t>(value);
        } else {
            ss << value;
        }
        ss << ");";
        
        Bridge::eval(ss.str().c_str());
    }
}

// MARK: - FunctionInvoker

// use uintptr_t for ref or pointers
template <typename _Tp>
using convert_if_object_t = typename std::conditional<is_ref_or_pointer_v<_Tp>, uintptr_t, remove_cvref_t<_Tp>>::type;

// use std::string or outcome of convert_if_object_t
template <typename _Tp>
using convert_t = typename std::conditional<is_string_v<_Tp>, remove_cvref_t<_Tp>, convert_if_object_t<_Tp>>::type;


template<typename T>
struct FunctionInvoker;

template<typename ReturnType, typename ClassType, typename... Args>
struct FunctionInvoker<ReturnType (ClassType::*)(Args...)> : public FunctionInvokerBase {
    
    FunctionInvoker(ReturnType (ClassType::*f)(Args...), FunctionDescriptor aDescriptor) : 
      FunctionInvokerBase(aDescriptor), _f(f) {
        static_assert(compatible_type_v<remove_cvref_t<ReturnType>>, "incompatible return type");
    }
    
    template<std::size_t... S>
    JSB_ALWAYS_INLINE ReturnType _invoke(ClassType* object_, std::index_sequence<S...>, const val* args) {
        
        using tupleArgs = std::tuple<Args...>;
        
            // determine the type to use for argument conversion
          // convert each argument using the arg_converter and put result in a tuple
        // pass the converted arguments to the function and invoke it
        return (object_->*_f)
        (arg_converter<std::tuple_element_t<S, tupleArgs>>(
                                args[S].get<convert_t<std::tuple_element_t<S, tupleArgs>>>()
                                                           )...);
    }
      
    void invoke(const MessageDescriptor& message) override {
        assert(message.object);
        
        
        ClassType* object_ = reinterpret_cast<ClassType*>(message.object);
                
        auto sequence = std::index_sequence_for<Args...>{};
        if constexpr (std::is_same_v<ReturnType, void>) {
            _invoke(object_, sequence, message.args);
            functionVoid(message);
        } else {
            functionReturn(message, std::forward<ReturnType>(_invoke(object_, sequence, message.args)));
        }
    };

    InvokerType getType() override {
        return InvokerType::ClassFunction;
    };
    
private:
    ReturnType (ClassType::*_f)(Args...);
};

// same as above but with const
template<typename ReturnType, typename ClassType, typename... Args>
struct FunctionInvoker<ReturnType (ClassType::*)(Args...) const> : public FunctionInvokerBase {
    
    FunctionInvoker(ReturnType (ClassType::*f)(Args...) const, FunctionDescriptor aDescriptor) :  
      FunctionInvokerBase(aDescriptor), _f(f) {
        static_assert(compatible_type_v<remove_cvref_t<ReturnType>>, "incompatible return type");
    }
      
    template<std::size_t... S>
    JSB_ALWAYS_INLINE ReturnType _invoke(ClassType* object_, std::index_sequence<S...>, const val* args) {
        
        using tupleArgs = std::tuple<Args...>;
        
            // determine the type to use for argument conversion
          // convert each argument using the arg_converter and put result in a tuple
        // pass the converted arguments to the function and invoke it
        return (object_->*_f)
        (arg_converter<std::tuple_element_t<S, tupleArgs>>(
                                args[S].get<convert_t<std::tuple_element_t<S, tupleArgs>>>()
                                                           )...);
    }
      
    void invoke(const MessageDescriptor& message) override {
        assert(message.object);
        
        ClassType* object_ = reinterpret_cast<ClassType*>(message.object);
        auto sequence = std::index_sequence_for<Args...>{};
        if constexpr (std::is_same_v<ReturnType, void>) {
            _invoke(object_, sequence, message.args);
            functionVoid(message);
        } else {
            functionReturn(message, std::forward<ReturnType>(_invoke(object_, sequence, message.args)));
        }
    };

    InvokerType getType() override {
        return InvokerType::ClassFunctionConst;
    };
    
private:
    ReturnType (ClassType::*_f)(Args...) const;
};


template<typename ReturnType, typename... Args>
struct FunctionInvoker<ReturnType (*)(Args...)> : public FunctionInvokerBase {
    
    FunctionInvoker(ReturnType (*f)(Args...), FunctionDescriptor aDescriptor) : 
      FunctionInvokerBase(aDescriptor), _f(f) {
        static_assert(compatible_type_v<remove_cvref_t<ReturnType>>, "incompatible return type");
    }
    
    template<std::size_t... S>
    JSB_ALWAYS_INLINE ReturnType _invoke(std::index_sequence<S...>, const val* args) {
        
        // create a tuple of argument types
        using tupleArgs = std::tuple<Args...>;
        
        
            // determine the type to use for argument conversion
          // convert each argument using the arg_converter and put result in a tuple
        // pass the converted arguments to the function and invoke it
        return (*_f)
        (arg_converter<std::tuple_element_t<S, tupleArgs>>(
                                args[S].get<convert_t<std::tuple_element_t<S, tupleArgs>>>()
                                                           )...);
    }
      
    void invoke(const MessageDescriptor& message) override {
        auto sequence = std::index_sequence_for<Args...>{};
        if constexpr (std::is_same_v<ReturnType, void>) {
            _invoke(sequence, message.args);
            functionVoid(message);
        } else {
            functionReturn(message, std::forward<ReturnType>(_invoke(sequence, message.args)));
        }
    };

private:
    ReturnType (*_f)(Args...);
};


template<typename T>
using return_t = typename FunctionInvoker<T>::rtype;

template<typename T>
inline constexpr int args_count_v = FunctionInvoker<T>::args_count;



// MARK: - SignatureCode, SignatureString

namespace internal {

template<typename T>
struct SignatureCode {};


template<>
struct SignatureCode<uintptr_t> {
    static constexpr char get() {
        return 'p';
    }
};

template<>
struct SignatureCode<int> {
    static constexpr char get() {
        return 'i';
    }
};

template<>
struct SignatureCode<void> {
    static constexpr char get() {
        return 'v';
    }
};

template<>
struct SignatureCode<float> {
    static constexpr char get() {
        return 'f';
    }
};

template<>
struct SignatureCode<double> {
    static constexpr char get() {
        return 'd';
    }
};

template<typename... Args>
const char* getGenericSignature() {
    static constexpr char signature[] = { SignatureCode<Args>::get()..., 0 };
    return signature;
}

template<typename T> struct SignatureTranslator { 
    using type = typename std::conditional<std::is_pointer_v<T>, uintptr_t, int>::type; };
template<> struct SignatureTranslator<void> { using type = void; };
template<> struct SignatureTranslator<float> { using type = float; };
template<> struct SignatureTranslator<double> { using type = double; };

template<typename... Args>
JSB_ALWAYS_INLINE const char* getSpecificSignature() {
    return getGenericSignature<typename SignatureTranslator<Args>::type...>();
}

template<typename Return, typename... Args>
JSB_ALWAYS_INLINE const char* getSignature(Return (*)(Args...)) {
    return getSpecificSignature<Return, Args...>();
}

template<typename Return, typename Class, typename... Args>
JSB_ALWAYS_INLINE const char* getSignature(Return (Class::*)(Args...) const) {
    return getSpecificSignature<Return, Args...>();
}

template<typename Return, typename Class, typename... Args>
JSB_ALWAYS_INLINE const char* getSignature(Return (Class::*)(Args...)) {
    return getSpecificSignature<Return, Args...>();
}

} // end namespace internal


// MARK: - Class

template<typename ClassType, typename... Args>
JSB_ALWAYS_INLINE ClassType* operator_new(Args&&... args) {
    return new ClassType(std::forward<Args>(args)...);
}

template<typename ClassType>
void raw_destructor(ClassType* ptr) {
    delete ptr;
}

template<typename ClassType>
class class_ : public base_class_ {
    
public:
    explicit class_(const char* name) noexcept(false) {
        _name = name;
        auto it = Bridge::getInstance().classes.find(_name);
        if (it != Bridge::getInstance().classes.end()) {
            std::stringstream ss("Class with \"", std::ios_base::app |std::ios_base::out);
            ss << _name << "\" name already bind!" << std::endl;
            printf("%s\n", ss.str().c_str());
            throw std::logic_error(ss.str());            
        }
        _this = new class_<ClassType>();
        _this->_name = _name;
        Bridge::getInstance().classes[_name] = std::unique_ptr<class_<ClassType>>(_this);
        
        _private("dtor", &raw_destructor<ClassType>);
    }
    
    template <typename T>
    JSB_ALWAYS_INLINE class_& smart_ptr(const char* name) {
        // TODO: impl
        assert(0);
        return *this;
    }
    
    template <typename Callable>
    JSB_ALWAYS_INLINE class_& function(const char* fname, Callable callable) {
        
        FunctionDescriptor::Configuration config = FunctionDescriptor::Configuration::Sync;
        if constexpr (!is_class_function<Callable>::value) {
            config = FunctionDescriptor::Configuration::Sync | FunctionDescriptor::Configuration::Class;
        }
        auto function = new FunctionInvoker<Callable>(callable, {fname, internal::getSignature(callable), config});
        _this->_addFunction(fname, std::unique_ptr<FunctionInvokerBase>(function));
        
        return *this;
    }
    
    template <typename Callable>
    JSB_ALWAYS_INLINE class_& class_function(const char* fname, Callable callable) {
        
        auto config = FunctionDescriptor::Configuration::Static | FunctionDescriptor::Configuration::Sync;
        
        auto function = new FunctionInvoker<Callable>(callable, {fname, internal::getSignature(callable), config});
        _this->_addFunction(fname, std::unique_ptr<FunctionInvokerBase>(function));
        return *this;
    }
    
    template<typename... ConstructorArgs, typename... Policies>
    JSB_ALWAYS_INLINE class_& constructor(Policies... policies) {
        return _private("ctor", &operator_new<ClassType, ConstructorArgs...>, policies...);
    }
    
    template<typename Callable, typename... Policies>
    JSB_ALWAYS_INLINE class_& constructor(Callable callable, Policies... policies) {
        return _private("ctorc", callable, policies...);
    }
    
    void registerJS(const std::string& moduleName) override {
        if (_this) { _this->registerJS(moduleName); return; }
        CodeGenerator::classDeclaration(moduleName, _name, _invokers);
    }
    
private:
    class_() = default;
    
    template<typename Callable, typename... Policies>
    JSB_ALWAYS_INLINE class_& _private(const std::string& fname, Callable callable, Policies...) {
        _this->_addFunction(fname, std::unique_ptr<FunctionInvokerBase>(new FunctionInvoker<Callable>(callable, {})));
        return *this;
    }
    
    JSB_ALWAYS_INLINE void _addFunction(const std::string& name, std::unique_ptr<FunctionInvokerBase>&& func) {
        auto it = _invokers.find(name);
        if (it != _invokers.end()) {
            printf("Error: overriding key: %s\n", name.c_str());
            assert(0);
        }
        _invokers[name] = std::move(func);
    }
    
    class_<ClassType>* _this{ nullptr };
    std::string _name;

};



template<typename Callable, typename... Policies>
void function(const char* fname, Callable callable, Policies...) {
    auto config = FunctionDescriptor::Configuration::Static | FunctionDescriptor::Configuration::Sync;
    auto function = new FunctionInvoker<Callable>(callable, {fname, internal::getSignature(callable), config});
    Bridge::getInstance().invokers.insert( { fname, std::unique_ptr<FunctionInvokerBase>(function) });
}





// copy from emscripten

namespace internal {



template<typename VectorType>
struct VectorAccess {
    
    static typename VectorType::value_type get(
        const VectorType& v,
        typename VectorType::size_type index
    ) {
        if (index >= v.size() || index < 0) {
            throw std::out_of_range("index out of range");
        }
        return v[index];
    }

    static bool set(
        VectorType& v,
        typename VectorType::size_type index,
        const typename VectorType::value_type& value
    ) {
        if (index >= v.size() || index < 0) {
            return false;
        }
        v[index] = value;
        return true;
    }
};

} // end namespace internal



template<typename T>
class_<std::vector<T>>& register_vector(const char* name) {
    typedef std::vector<T> VecType;

    void (VecType::*push_back)(const T&) = &VecType::push_back;
    void (VecType::*resize)(const size_t, const T&) = &VecType::resize;
    size_t (VecType::*size)() const = &VecType::size;
    return class_<std::vector<T>>(name)
        .template constructor<>()
        .function("push_back", push_back)
        .function("resize", resize)
        .function("size", size)
        .function("get", &internal::VectorAccess<VecType>::get)
        .function("set", &internal::VectorAccess<VecType>::set)
    ;
}

/*
namespace internal {

template<typename MapType>
struct MapAccess {
    static typename MapType::mapped_type get(
        const MapType& m,
        const typename MapType::key_type& k
    ) {
        auto i = m.find(k);
        if (i == m.end()) {
            throw std::out_of_range("index out of range");
        } else {
            return i->second;
        }
    }

    static void set(
        MapType& m,
        const typename MapType::key_type& k,
        const typename MapType::mapped_type& v
    ) {
        m[k] = v;
    }

    static std::vector<typename MapType::key_type> keys(
        const MapType& m
    ) {
      std::vector<typename MapType::key_type> keys;
      keys.reserve(m.size());
      for (const auto& pair : m) {
        keys.push_back(pair.first);
      }
      return keys;
    }
};

} // end namespace internal


 // TODO: need to fix
template<typename K, typename V>
class_<std::map<K, V>> register_map(const char* name) {
    typedef std::map<K,V> MapType;

    size_t (MapType::*size)() const = &MapType::size;
    return class_<MapType>(name)
        .template constructor<>()
        .function("size", size)
        .function("get", internal::MapAccess<MapType>::get)
        .function("set", internal::MapAccess<MapType>::set)
    // TODO: need to fix
//        .function("keys", internal::MapAccess<MapType>::keys)
        ;
}
 */

} // namespace jsb


