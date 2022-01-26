
## Project Description

Project is inspired by Emscripten's embind and later by Boost.Python.

## Targets Capabilities

Essential:

- [x] bind a C++ class 
- [x] bind regular functions of a C++ class 
- [x] bind const functions of a C++ class 
- [ ] bind perfect forwarding functions of a C++ class ?? 
- [x] bind static functions of a C++ class
- [x] bind simple constructor of a C++ class
- [ ] bind constructor with arguments of a C++ class
- [ ] bind property members of a C++ class
- [x] bind global functions
- [ ] read JS function's arguments and pass to C++
- [x] generate funcitons JavaScript code based on bindings
- [ ] execute generated TypeScript/JavaScript code at init
- [ ] bind external constructors for a C++ class 
- [ ] bind enums
- [ ] bind vectors
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



