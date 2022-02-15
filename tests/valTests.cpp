//
//  val_Tests.cpp
//  BridgeExperimentTests
//
//  Created by Volodymyr Boichentsov on 04/01/2022.
//

#include "gtest/gtest.h"
#include "Bridge.hpp"


TEST(valTests, simple) {
    
    auto v1 = jsb::val(1);
    ASSERT_EQ(v1.get<int>(), 1);
    
    auto v2 = jsb::val(2.0);
    ASSERT_EQ(v2.get<double>(), 2.0);
    
    auto v3 = jsb::val(2.0f);
    ASSERT_EQ(v3.get<float>(), 2.0f);
    
    auto v4 = jsb::val(true);
    ASSERT_EQ(v4.get<bool>(), true);
    
    std::string testString = "# test string @";
    auto v5 = jsb::val(testString);
    
    ASSERT_EQ(v5.get<std::string>(), testString);
}
