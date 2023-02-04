
## Project Description

The idea of the project is to create the ability to control C++ objects from JavaScript code, similar to what Emscripten provides to developers but with a limited feature set due to memory access limitations. 
The project is inspired by Emscripten's embind and later by Boost.Python.

## Samples

- [x] macOS - look in macOS directory
- [ ] iOS 
- [ ] Android 
- [ ] Windows


#### macOS
Look into `ViewController.mm`  to see on how to register Bridge communicator.

Example on how to bind C++ code:
```cpp
class TestJSBinding  {
public:
    TestJSBinding() { }
    
    int getNumber() {
        return i;
    }
    void setNumber(int newi) {
        i = newi;
    }
    int i;
}


jsb::class_<TestJSBinding>("TestJSBinding")
.constructor<>()
.function("setNumber", &TestJSBinding::setNumber)
.function("getNumber", &TestJSBinding::getNumber);
```

In javascript:
```javascript
var object = new TestJSBinding();
object.setNumber(10);
var value2 = object.getNumber();
object.delete();
```

## Features list

Essential:

- [x] bind a class 
- [x] bind regular functions of a class 
- [x] bind const functions of a class 
- [x] bind static functions of a class
- [x] bind simple constructor of a class
- [x] bind global functions
- [x] generate funcitons JavaScript code based on bindings
- [x] delete function for a C++ class
- [x] execute generated JavaScript code at registerCommunicator
- [ ] val as wrapper around JS object - **in progress** - not possible
- [ ] EM_JS - read JS function's arguments and pass to a C++ function
- [ ] bind perfect forwarding functions of a class ?? 
- [ ] bind constructor with arguments of a class
- [ ] bind property members of a class
- [ ] implement typescript interafaces generator.
- [ ] bind enums
- [x] bind vectors

Non-essential 

- [ ] [value types](https://emscripten.org/docs/porting/connecting_cpp_and_javascript/embind.html#value-types)
- [ ] smart pointers
- [ ] [advanced class concepts](https://emscripten.org/docs/porting/connecting_cpp_and_javascript/embind.html#advanced-class-concepts)
- [ ] [Type conversion](https://emscripten.org/docs/porting/connecting_cpp_and_javascript/embind.html#built-in-type-conversions)
- [ ] [Do performance testing and compare](https://emscripten.org/docs/porting/connecting_cpp_and_javascript/embind.html#performance)

- [ ] EM_ASYNC_JS
- [ ] simple EM_ASM


Non-essential -  out of scope:

- [ ] Deriving from C++ classes in JavaScript
    - [ ] [Abstract methods](https://emscripten.org/docs/porting/connecting_cpp_and_javascript/embind.html#abstract-methods)
    - [ ] [Non-abstract virtual methods](https://emscripten.org/docs/porting/connecting_cpp_and_javascript/embind.html#non-abstract-virtual-methods)
    - [ ] [Base classes](https://emscripten.org/docs/porting/connecting_cpp_and_javascript/embind.html#non-abstract-virtual-methods)
    - [ ] [Automatic downcasting](https://emscripten.org/docs/porting/connecting_cpp_and_javascript/embind.html#non-abstract-virtual-methods)
    - [ ] [Overloaded functions](https://emscripten.org/docs/porting/connecting_cpp_and_javascript/embind.html#overloaded-functions)
    
- [ ] advanced EM_ASM from em_asm.h

More for reference:
https://emscripten.org/docs/porting/connecting_cpp_and_javascript/embind.html

## Configuration



