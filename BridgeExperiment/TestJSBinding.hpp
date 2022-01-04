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
    
    void copyFrom(TestJSBinding* obj) {
        i = obj->i;
    }
    
    static int randomNumber(int modifier) {
        return arc4random() % modifier;
    }
    
    void* voidPtr() {
        return &i;
    }
    
    int i = 0;
};

#endif /* TestJSBinding_hpp */
