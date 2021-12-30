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

class TestJSBinding : public JSBinding {
public:
    
    TestJSBinding() : JSBinding() {
        registerFunction("getNumber", &TestJSBinding::getNumber, this);
        registerFunction("setNumber", &TestJSBinding::setNumber, this);
    }
    
    ~TestJSBinding() {}
    
    int getNumber() {
        i++;
        return i;
    }
    
    void setNumber(int newi) {
        i = newi;
    }
    
    int i = 0;
};

#endif /* TestJSBinding_hpp */
