//
//  class__Tests.cpp
//  BridgeExperimentTests
//
//  Created by Volodymyr Boichentsov on 04/01/2022.
//

#include "gtest/gtest.h"
#include "Bridge.hpp"
#include "TestJSBinding.hpp"

void testingFunc1(int i, TestJSBinding* obj, float f) {}


TEST(SignatureTests, const_func) {
    auto func = &TestJSBinding::const_func;
    auto signature = jsb::internal::getSignature(func);
    ASSERT_TRUE(strcmp(signature, "v") == 0);
}

TEST(SignatureTests, setNumber) {
    auto func = &TestJSBinding::setNumber;
    auto signature = jsb::internal::getSignature(func);
    ASSERT_TRUE(strcmp(signature, "vi") == 0);
}

TEST(SignatureTests, string2) {
    auto func = &TestJSBinding::string2;
    auto signature = jsb::internal::getSignature(func);
    ASSERT_TRUE(strcmp(signature, "p") == 0);
}

TEST(SignatureTests, copyFrom) {
    auto func = &TestJSBinding::copyFrom;
    auto signature = jsb::internal::getSignature(func);
    ASSERT_TRUE(strcmp(signature, "vp") == 0);
}

TEST(SignatureTests, setNumberd) {
    auto func = &TestJSBinding::setNumberd;
    auto signature = jsb::internal::getSignature(func);
    ASSERT_TRUE(strcmp(signature, "vd") == 0);
}

TEST(SignatureTests, setNumberf) {
    auto func = &TestJSBinding::setNumberf;
    auto signature = jsb::internal::getSignature(func);
    ASSERT_TRUE(strcmp(signature, "vf") == 0);
}

TEST(SignatureTests, testingFunc1_) {
    auto func = &testingFunc1;
    auto signature = jsb::internal::getSignature(func);
    ASSERT_TRUE(strcmp(signature, "vipf") == 0);
}
