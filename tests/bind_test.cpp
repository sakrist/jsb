#include <emscripten.h>
#include <emscripten/bind.h>

#include "TestJSBinding.hpp"

using namespace emscripten;

using TestJSBindingPtr = std::shared_ptr<TestJSBinding>;

EMSCRIPTEN_BINDINGS(my_module) {
        
    register_vector<TestJSBinding>("VectorTmp");

    class_<TestJSBinding>("TestJSBinding")
    .smart_ptr<TestJSBindingPtr>("TestJSBindingPtr")
    .constructor<>()
    .function("setNumber", &TestJSBinding::setNumber)
    .function("setNumberf", &TestJSBinding::setNumberf)
    .function("setNumberd", &TestJSBinding::setNumberd)
    .function("getNumber", &TestJSBinding::getNumber)
    .function("const_func", &TestJSBinding::const_func)
    .function("setString", &TestJSBinding::setString)
    .function("setString2", &setString2)
    .function("getString2", &getString2);
    // .function("copyFrom", &TestJSBinding::copyFrom);

}

EM_JS(void, testFunc, (), {
        var vec = new TestJSBinding();
        // vec.push(new TestJSBinding());
        // console.log("size " + vec.size());
    });

int main() {

    testFunc();
    

    printf("hello \n");
    return 0;
}