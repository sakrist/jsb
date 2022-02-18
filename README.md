
## Project Description

The idea of the project is to create an ability to control  C++ objects from JavaScript code. 
Similar to what Emscripten provides to developers in their solution but with a limited feature set. 

Project is inspired by Emscripten's embind and later by Boost.Python.

## Targets Capabilities

Essential:

- [x] bind a class 
- [x] bind regular functions of a class 
- [x] bind const functions of a class 
- [x] bind static functions of a class
- [x] bind simple constructor of a class
- [x] bind global functions
- [x] generate funcitons JavaScript code based on bindings
- [x] delete function for C++ class
- [ ] val as wrapper around JS object - **in progress**
- [ ] read JS function's arguments and pass to C++ function - aka EM_JS

- [ ] implement typescript interafaces generator.

- [ ] bind perfect forwarding functions of a class ?? 
- [ ] bind constructor with arguments of a class
- [ ] bind property members of a class

- [x] execute generated JavaScript code at registerCommunicator 
- [ ] bind enums
- [ ] bind vectors - **in progress**
- [ ] [value types](https://emscripten.org/docs/porting/connecting_cpp_and_javascript/embind.html#value-types)
- [ ] policy - raw pointer
- [ ] smart pointers
- [ ] [advanced class concepts](https://emscripten.org/docs/porting/connecting_cpp_and_javascript/embind.html#advanced-class-concepts)
- [ ] [Type conversion](https://emscripten.org/docs/porting/connecting_cpp_and_javascript/embind.html#built-in-type-conversions)
- [ ] [Do performance testing and compare](https://emscripten.org/docs/porting/connecting_cpp_and_javascript/embind.html#performance)

- [ ] EM_JS and EM_ASYNC_JS
- [ ] simple EM_ASM

Non-essential might be out of scope:

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



