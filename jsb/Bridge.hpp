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

#include "val.hpp"

using namespace std::placeholders;

#define JSB_ALWAYS_INLINE __attribute__((always_inline))

// copy of EMSCRIPTEN_BINDINGS/BOOST_PYTHON_MODULE for compatibility reason.
#define JSBridge_BINDINGS(name)                                         \
    static struct JSBridgeBindingInitializer_##name {                   \
          JSBridgeBindingInitializer_##name();                          \
    } JSBridgeBindingInitializer_##name##_instance;                     \
JSBridgeBindingInitializer_##name::JSBridgeBindingInitializer_##name()

namespace jsb {

template <typename T>
struct compatible_type {
    inline static constexpr bool value = (std::is_arithmetic<T>::value
                                          || std::is_same_v<T, void>
                                          || std::is_same_v<T, uintptr_t>
                                          || std::is_same_v<T, std::string>
                                          || std::is_pointer_v<T>
                                          );
};

template<typename T>
inline constexpr bool compatible_type_v = compatible_type<T>::value;

#if __cplusplus < 202002L
template <typename T>
struct remove_cvref : std::remove_cv<std::remove_reference_t<T>> { };
//struct remove_cvref : std::remove_cv<std::remove_reference_t<std::remove_pointer_t<T>>> { };
template <class _Tp>
using remove_cvref_t = typename remove_cvref<_Tp>::type;
#endif



template <typename T, std::enable_if_t<std::is_arithmetic_v<T>, int> = 0>
JSB_ALWAYS_INLINE auto arg_converter(T val) {
    return val;
};

template <typename T, std::enable_if_t<!std::is_arithmetic_v<T>, int> = 0>
JSB_ALWAYS_INLINE auto arg_converter(T&& val) {
    return std::forward<T>(val);
};

template <typename T, std::enable_if_t<std::is_pointer_v<T>, int> = 0>
JSB_ALWAYS_INLINE T arg_converter(uintptr_t val) {
    return reinterpret_cast<T>(val);
};


//class CodeGenerator {
//    void 
//};

class JSBridgeCommunicator {
public:
    
    virtual ~JSBridgeCommunicator() = default;
        
    // sending message to JS from C++
    virtual void eval(const char* js_code, std::function<void(const char*)>&& completion) const = 0;
    
    // JS calls C++ code
    virtual void recive(const char* json, std::function<void(const char*)>&& completion) const = 0;
};


struct JSMessageDescriptor {
    uintptr_t object;
    std::string class_id;
    std::string function;
    
    // for async
    std::string callback;
    std::string callback_id;
    
    // sync
    std::function<void(const char*)> completion{ nullptr };
    
    val args[3];
};

struct FunctionInvokerBase {
    FunctionInvokerBase(FunctionDescriptor aDescriptor) : descriptor(aDescriptor) {}
    virtual void invoke(const JSMessageDescriptor& message) {};
    const FunctionDescriptor descriptor;
};
using InvokersMap = std::unordered_map<std::string, std::unique_ptr<FunctionInvokerBase>>;

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
    
    void recive(JSMessageDescriptor& message) const noexcept(false) {
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
    
    JSB_ALWAYS_INLINE void _recive(const InvokersMap& invs, const JSMessageDescriptor& message) const noexcept(false) {
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

JSB_ALWAYS_INLINE inline static void functionVoid(const JSMessageDescriptor& message) {
    if (message.completion) {
        message.completion("");
    }
}

template<typename R>
JSB_ALWAYS_INLINE static void functionReturn(const JSMessageDescriptor& message, R&& value) {
    
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
        
        // Here we are executing function from object
        return (object_->*_f)
        (arg_converter<std::tuple_element_t<S, std::tuple<Args...>>>(args[S].get<
                                  typename std::conditional<std::is_pointer_v<std::tuple_element_t<S, std::tuple<Args...>>>,
                                         uintptr_t,
                                         remove_cvref_t<std::tuple_element_t<S, std::tuple<Args...>>>>::type
                                 >())...);
    }
      
    void invoke(const JSMessageDescriptor& message) override {
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

private:
    ReturnType (ClassType::*_f)(Args...);
};

template<typename ReturnType, typename ClassType, typename... Args>
struct FunctionInvoker<ReturnType (ClassType::*)(Args...) const> : public FunctionInvokerBase {
    
    FunctionInvoker(ReturnType (ClassType::*f)(Args...) const, FunctionDescriptor aDescriptor) :  
      FunctionInvokerBase(aDescriptor), _f(f) {
        static_assert(compatible_type_v<remove_cvref_t<ReturnType>>, "incompatible return type");
    }
      
    template<std::size_t... S>
    JSB_ALWAYS_INLINE ReturnType _invoke(ClassType* object_, std::index_sequence<S...>, const val* args) {
        
        // Here we are executing function from object
        return (object_->*_f)
        (arg_converter<std::tuple_element_t<S, std::tuple<Args...>>>(args[S].get<
                                  typename std::conditional<std::is_pointer_v<std::tuple_element_t<S, std::tuple<Args...>>>,
                                         uintptr_t,
                                         remove_cvref_t<std::tuple_element_t<S, std::tuple<Args...>>>>::type
                                 >())...);
    }
      
    void invoke(const JSMessageDescriptor& message) override {
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
        return (*_f)
        (arg_converter<std::tuple_element_t<S, std::tuple<Args...>>>(args[S].get<
                                     typename std::conditional<std::is_pointer_v<std::tuple_element_t<S, std::tuple<Args...>>>,
                                            uintptr_t,
                                            remove_cvref_t<std::tuple_element_t<S, std::tuple<Args...>>>>::type
                                    >())...);
    }
      
    void invoke(const JSMessageDescriptor& message) override {
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

template<typename ClassType, typename... Args>
JSB_ALWAYS_INLINE ClassType* operator_new(Args&&... args) {
    return new ClassType(std::forward<Args>(args)...);
}

template<typename ClassType>
void raw_destructor(ClassType* ptr) {
    delete ptr;
}

template<typename T>
using return_t = typename FunctionInvoker<T>::rtype;

template<typename T>
inline constexpr int args_count_v = FunctionInvoker<T>::args_count;

////////////////////////////////////////////////////////////////////////////////
// SignatureCode, SignatureString
////////////////////////////////////////////////////////////////////////////////

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
    
    template <typename Callable>
    JSB_ALWAYS_INLINE class_& function(const char* fname, Callable callable) {
        auto function = new FunctionInvoker<Callable>(callable, {fname, internal::getSignature(callable)});
        _this->_addFunction(fname, std::unique_ptr<FunctionInvokerBase>(function));
        return *this;
    }
    
    template <typename Callable>
    JSB_ALWAYS_INLINE class_& class_function(const char* fname, Callable callable) {
        auto function = new FunctionInvoker<Callable>(callable, {fname, internal::getSignature(callable), true});
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
    auto function = new FunctionInvoker<Callable>(callable, {fname, internal::getSignature(callable), true});
    Bridge::getInstance().invokers.insert( { fname, std::unique_ptr<FunctionInvokerBase>(function) });
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
    
    static typename VectorType::value_type get(
        const VectorType& v,
        typename VectorType::size_type index
    ) {
        return v[index];
    }

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
//        .function("get", &internal::VectorAccess<VecType>::get)
//        .function("set", &internal::VectorAccess<VecType>::set)
    ;
}


} // namespace jsb


