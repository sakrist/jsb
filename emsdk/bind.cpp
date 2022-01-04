//
//  bind.cpp
//  BridgeExperiment
//
//  Created by Volodymyr Boichentsov on 22/12/2021.
//

#include "bind.hpp"
#include "TestBunny.hpp"

using namespace emscripten;

class dummy {
    void setInt(int i) {
        i;
    };
};

float lerp(float a, float b, float t) {
    return (1 - t) * a + t * b;
}

EMSCRIPTEN_BINDINGS(my_module) {
    function("lerp", &lerp);
    
    class_<dummy>("dummy")
        .function("setInt", &dummy::setInt);
}



namespace emscripten {
namespace internal {

void _embind_register_class(
    TYPEID classType,
    TYPEID pointerType,
    TYPEID constPointerType,
    TYPEID baseClassType,
    const char* getActualTypeSignature,
    GenericFunction getActualType,
    const char* upcastSignature,
    GenericFunction upcast,
    const char* downcastSignature,
    GenericFunction downcast,
    const char* className,
    const char* destructorSignature,
                            GenericFunction destructor) {
    
    
    
    printf("log \n");
}

typedef void (*FuncPointer)();

void _embind_register_class_function(
    TYPEID classType,
    const char* methodName,
    unsigned argCount,
    const TYPEID argTypes[],
    const char* invokerSignature,
    GenericFunction invoker,
    void* context,
    unsigned isPureVirtual) {
    
//    VoidIntFn * f = (VoidIntFn*)context;
    
    TestBunny * b = new TestBunny();
    
    
    auto inv = ((MethodInvoker<decltype(&TestBunny::setNumber), void, TestBunny *, int> *)&invoker)->invoke;
    
    (*inv)(&TestBunny::setNumber, b, 1);
//    (*inv)(b, 1);
    
//    FuncPointer f = context;
//
//    auto func = std::bind(f, b, 1);
//    func();
    printf("log \n");
}



}
}
