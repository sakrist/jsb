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
    }
    
    ~TestJSBinding() {}
    
    int getNumber() {
        return i;
    }
    
    void setNumber2(int newi, int newi2) {
        i = newi + newi2;
    }
    
    void setNumber(int newi) {
        i = newi;
    }
    
    static int randomNumber(int modifier) {
        return arc4random() % modifier;
    }
    
    int i = 0;
};

#endif /* TestJSBinding_hpp */
