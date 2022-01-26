//
//  class__Tests.cpp
//  BridgeExperimentTests
//
//  Created by Volodymyr Boichentsov on 04/01/2022.
//

#include "gtest/gtest.h"
#include "Bridge.hpp"



float add(int a, int b) {
    return a + b;
}

TEST(FunctionTests, simple) {
    
    jsb::function("add", &add);
    
}
