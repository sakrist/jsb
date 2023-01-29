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

TEST(IsRefOrPointerTest, Reference) {
    int x = 0;
    int& y = x;
    EXPECT_TRUE(jsb::is_ref_or_pointer_v<decltype(y)>);
    EXPECT_TRUE(jsb::is_ref_or_pointer<decltype(y)>::value);
}

TEST(IsRefOrPointerTest, Pointer) {
    int x = 0;
    int* y = &x;
    EXPECT_TRUE(jsb::is_ref_or_pointer_v<decltype(y)>);
    EXPECT_TRUE(jsb::is_ref_or_pointer<decltype(y)>::value);
}

TEST(IsRefOrPointerTest, Value) {
    int x = 0;
    EXPECT_FALSE(jsb::is_ref_or_pointer_v<decltype(x)>);
    EXPECT_FALSE(jsb::is_ref_or_pointer<decltype(x)>::value);
}
