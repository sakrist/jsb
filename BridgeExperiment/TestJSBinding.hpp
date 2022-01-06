//
//  TestJSBinding.hpp
//  BridgeExperiment
//
//  Created by Volodymyr Boichentsov on 29/12/2021.
//

#ifndef TestJSBinding_hpp
#define TestJSBinding_hpp

#include <stdio.h>
#include "JSBridge.hpp"


class TestJSBinding  {
public:
    
    TestJSBinding() {
//        i = num;
        printf("constructor TestJSBinding\n");
    }
    
    ~TestJSBinding() {}
    
    const int getNumber() {
        return i;
    }
    
    void setNumber2(int newi, int newi2) {
        i = newi + newi2;
    }
    
    void setNumber(int newi) {
        i = newi;
    }
    void setNumberf(float newi) {
        i = newi;
    }
    void setNumberd(double newi) {
        i = newi;
    }
    
    void forward(int&& newi) {
        i = newi;
    }
    void forwardf(float&& newi) {
        i = newi;
    }
    void forwardd(double&& newi) {
        i = newi;
    }
    
    void copyFrom(TestJSBinding* obj) {
        i = obj->i;
    }
    
    static int randomNumber(int modifier) {
        return arc4random() % modifier;
    }
    
    void* voidPtr() {
        return &i;
    }
    
//    std::string string() {
//        return "string";
//    }
    
    void const_func() const {
        printf("const_func\n");
    }
    
    int i = 0;
};

#endif /* TestJSBinding_hpp */
