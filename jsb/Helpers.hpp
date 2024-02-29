//
//  Helpers.hpp
//  BridgeExperiment
//
//  Created by Volodymyr Boichentsov on 28/01/2023.
//

#pragma once

#ifndef Helpers_h
#define Helpers_h


#define JSB_ALWAYS_INLINE __attribute__((always_inline))

// copy of EMSCRIPTEN_BINDINGS/BOOST_PYTHON_MODULE for compatibility reason.
#define JSBridge_BINDINGS(name)                                         \
    static struct JSBridgeBindingInitializer_##name {                   \
          JSBridgeBindingInitializer_##name();                          \
    } JSBridgeBindingInitializer_##name##_instance;                     \
JSBridgeBindingInitializer_##name::JSBridgeBindingInitializer_##name()


namespace jsb {

template <typename T>
struct is_shared : std::false_type {};
template <typename T>
struct is_shared<std::shared_ptr<T>> : std::true_type {};
template<typename T>
inline constexpr bool is_shared_v = is_shared<T>::value;

template <typename T>
struct is_vector : std::false_type {};
template <typename T, typename Alloc>
struct is_vector<std::vector<T, Alloc>> : std::true_type {};
template<typename T>
inline constexpr bool is_vector_v = is_vector<T>::value;

template <typename T>
struct shared_ptr_type;
template <typename T>
struct shared_ptr_type<std::shared_ptr<T>> { using type = T; };
template <typename T>
using shared_ptr_type_t = typename shared_ptr_type<T>::type;

template <typename T>
struct vector_element_type { using type = T; };
template <typename U, typename Alloc>
struct vector_element_type<std::vector<U, Alloc>> { using type = U; };
template <typename T>
using vector_element_type_t = typename vector_element_type<T>::type;

template <typename T>
struct is_compatible_type {
    inline static constexpr bool value = (std::is_arithmetic_v<T>
                                          || std::is_same_v<T, void>
                                          || std::is_same_v<T, uintptr_t>
                                          || std::is_same_v<T, std::string>
                                          || std::is_pointer_v<T>
                                          || is_shared_v<T>
                                          );
};

template<typename T>
inline constexpr bool is_compatible_type_v = is_compatible_type<T>::value;

#if __cplusplus < 202002L
template <typename T>
struct remove_cvref : std::remove_cv<std::remove_reference_t<T>> { };
template <class _Tp>
using remove_cvref_t = typename remove_cvref<_Tp>::type;
#endif

template <typename _Tp>
struct is_string : std::is_same<std::string, remove_cvref_t<_Tp>> { };
template<typename T>
inline constexpr bool is_string_v = is_string<T>::value;


template <typename T, std::enable_if_t<std::is_arithmetic_v<T>, int> = 0>
JSB_ALWAYS_INLINE auto arg_converter(T val) {
    return val;
};

template <typename T, std::enable_if_t<std::is_reference_v<T>, int> = 0>
JSB_ALWAYS_INLINE auto arg_converter(T& val) {
    return std::forward<T>(val);
};


template <typename T, std::enable_if_t<!std::is_arithmetic_v<T>, int> = 0>
JSB_ALWAYS_INLINE auto arg_converter(T&& val) {
    return std::forward<T>(val);
};

// convert uintptr_t arrived from JS to pointer
template <typename T, std::enable_if_t<std::is_pointer_v<T>, int> = 0>
JSB_ALWAYS_INLINE T arg_converter(uintptr_t val) {
    return reinterpret_cast<T>(val);
};

// convert uintptr_t arrived from JS to reference
template <typename T, std::enable_if_t<std::is_reference_v<T> && !std::is_arithmetic_v<remove_cvref_t<T>>, int> = 0>
JSB_ALWAYS_INLINE auto arg_converter(uintptr_t val) {
    using type = remove_cvref_t<T>;
    type* var = reinterpret_cast<type *>(val);
    return std::ref(*var);
};

// convert uintptr_t arrived from JS to std::shared_ptr
template <typename T, std::enable_if_t<is_shared_v<T>, int> = 0>
JSB_ALWAYS_INLINE T arg_converter(uintptr_t val) {
    using Element_Type = shared_ptr_type_t<T>;
    return T(reinterpret_cast<Element_Type*>(val));
};


template <typename T>
struct is_ref_or_pointer {
    inline static constexpr bool value = (std::is_reference_v<T> 
                                          || std::is_pointer_v<T>
                                          || is_shared_v<T>);
};
template<typename T>
inline constexpr bool is_ref_or_pointer_v = is_ref_or_pointer<T>::value;




template<typename T>
struct is_class_function : std::false_type {};
template<typename ReturnType, typename ClassType, typename... Args>
struct is_class_function<ReturnType (ClassType::*)(Args...)> : std::true_type {};
template<typename ReturnType, typename ClassType, typename... Args>
struct is_class_function<ReturnType (ClassType::*)(Args...) const> : std::true_type {};
template<typename ReturnType, typename ClassType, typename... Args>
struct is_class_function<ReturnType (ClassType::*)(Args...) volatile> : std::true_type {};
template<typename ReturnType, typename ClassType, typename... Args>
struct is_class_function<ReturnType (ClassType::*)(Args...) const volatile> : std::true_type {};
template<typename T>
inline constexpr bool is_class_function_v = is_class_function<T>::value;

template<typename T>
struct function_return {};
template<typename ReturnType, typename ClassType, typename... Args>
struct function_return<ReturnType (ClassType::*)(Args...)> { using type = ReturnType; };
template<typename ReturnType, typename ClassType, typename... Args>
struct function_return<ReturnType (ClassType::*)(Args...) const> { using type = ReturnType; };
template<typename ReturnType, typename ClassType, typename... Args>
struct function_return<ReturnType (ClassType::*)(Args...) volatile> { using type = ReturnType; };
template<typename ReturnType, typename ClassType, typename... Args>
struct function_return<ReturnType (ClassType::*)(Args...) const volatile> { using type = ReturnType; };
template<typename ReturnType, typename... Args>
struct function_return<ReturnType (*)(Args...)> { using type = ReturnType; };
template <class _Tp>
using function_return_t = typename function_return<_Tp>::type;


template <typename T>
struct extract_type {
    using type = std::remove_cv_t<std::remove_reference_t<std::remove_pointer_t<T>>>;
};

// Specialization for shared_ptr
template <typename T>
struct extract_type<std::shared_ptr<T>> { using type = typename extract_type<T>::type; };

// Specialization for pointer
template <typename T>
struct extract_type<T*> { using type = typename extract_type<T>::type; };

// Specialization for reference
template <typename T>
struct extract_type<T&> { using type = typename extract_type<T>::type; };

template <typename _Tp>
using extract_type_t = typename extract_type<_Tp>::type;

}


#endif /* Helpers_h */
