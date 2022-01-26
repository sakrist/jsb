//
//  TestCodeGenerator.cpp
//  BridgeExperimentTests
//
//  Created by Volodymyr Boichentsov on 08/01/2022.
//

#include <stdio.h>
#include "gtest/gtest.h"
#include "CodeGenerator.hpp"

TEST(CodeGenerator, FunctionGen_void_no_args) {
    std::string expected = R"(_proto.calculate = function calculate(){BridgeTS.getInstance().sync(JSON.stringify({ class : "TestClass", function : "calculate", object : this.ptr}));};)";
    
    jsb::FunctionDescriptor desc = {"TestClass", "calculate", true, 0, false, true};    
    auto result = jsb::CodeGenerator::classFunction(desc);
    ASSERT_EQ(expected, result);
}

TEST(CodeGenerator, FunctionGen_void_args1) {
    std::string expected = R"(_proto.setNumber = function setNumber(v0){BridgeTS.getInstance().sync(JSON.stringify({ class : "TestClass", function : "setNumber", object : this.ptr, args : [v0]}));};)";
    
    jsb::FunctionDescriptor desc = {"TestClass", "setNumber", true, 1, false, true};    
    auto result = jsb::CodeGenerator::classFunction(desc);
    ASSERT_EQ(expected, result);
}

TEST(CodeGenerator, FunctionGen_void_args2) {
    std::string expected = R"(_proto.setNumber = function setNumber(v0,v1){BridgeTS.getInstance().sync(JSON.stringify({ class : "TestClass", function : "setNumber", object : this.ptr, args : [v0,v1]}));};)";
    
    jsb::FunctionDescriptor desc = {"TestClass", "setNumber", true, 2, false, true};    
    auto result = jsb::CodeGenerator::classFunction(desc);
    ASSERT_EQ(expected, result);
}

TEST(CodeGenerator, FunctionGen_int_args2) {
    std::string expected = R"(_proto.setNumber = function setNumber(v0,v1){return BridgeTS.getInstance().sync(JSON.stringify({ class : "TestClass", function : "setNumber", object : this.ptr, args : [v0,v1]}));};)";
    
    jsb::FunctionDescriptor desc = {"TestClass", "setNumber", false, 2, false, true};    
    auto result = jsb::CodeGenerator::classFunction(desc);
    ASSERT_EQ(expected, result);
}

TEST(CodeGenerator, FunctionGen_int_args0) {
    std::string expected = R"(_proto.getNumber = function getNumber(){return BridgeTS.getInstance().sync(JSON.stringify({ class : "TestClass", function : "getNumber", object : this.ptr}));};)";
    
    jsb::FunctionDescriptor desc = {"TestClass", "getNumber", false, 0, false, true};    
    auto result = jsb::CodeGenerator::classFunction(desc);
    ASSERT_EQ(expected, result);
}

TEST(CodeGenerator, FunctionGen_int_args0_static) {
    std::string expected = R"(TestClass.random = function random(){return BridgeTS.getInstance().sync(JSON.stringify({ class : "TestClass", function : "random"}));};)";
    
    jsb::FunctionDescriptor desc = {"TestClass", "random", false, 0, true, true};    
    auto result = jsb::CodeGenerator::classFunction(desc);
    ASSERT_EQ(expected, result);
}

TEST(CodeGenerator, FunctionGen_int_args1_static) {
    std::string expected = R"(TestClass.random = function random(v0){return BridgeTS.getInstance().sync(JSON.stringify({ class : "TestClass", function : "random", args : [v0]}));};)";
    
    jsb::FunctionDescriptor desc = {"TestClass", "random", false, 1, true, true};    
    auto result = jsb::CodeGenerator::classFunction(desc);
    ASSERT_EQ(expected, result);
}

TEST(CodeGenerator, FunctionGen_void_args0_async_static) {
    std::string expected = R"(TestClass.calculateNumber = function calculateNumber(){BridgeTS.getInstance().async(JSON.stringify({ class : "TestClass", function : "calculateNumber"}));};)";
    
    jsb::FunctionDescriptor desc = {"TestClass", "calculateNumber", true, 0, true, false};    
    auto result = jsb::CodeGenerator::classFunction(desc);
    ASSERT_EQ(expected, result);
}

TEST(CodeGenerator, FunctionGen_void_args1_async) {
    std::string expected = R"(_proto.setNumber = function setNumber(v0){BridgeTS.getInstance().async(JSON.stringify({ class : "TestClass", function : "setNumber", object : this.ptr, args : [v0]}));};)";
    
    jsb::FunctionDescriptor desc = {"TestClass", "setNumber", true, 1, false, false};    
    auto result = jsb::CodeGenerator::classFunction(desc);
    ASSERT_EQ(expected, result);
}

TEST(CodeGenerator, FunctionGen_int_args2_async) {
    std::string expected = R"(_proto.calculateNumber = function calculateNumber(v0,v1){var callid = BridgeTS.generateCallID(this.ptr);var p = new Promise(function(resolve){TestClass.promises.set(callid,resolve);});BridgeTS.getInstance().async(JSON.stringify({ class : "TestClass", function : "calculateNumber", object : this.ptr, args : [v0,v1], callback : "TestClass ._callback", cid : callid })); return p;};)";
    
    jsb::FunctionDescriptor desc = {"TestClass", "calculateNumber", false, 2, false, false};    
    auto result = jsb::CodeGenerator::classFunction(desc);
    ASSERT_EQ(expected, result);
}

TEST(CodeGenerator, FunctionGen_int_args0_async_static) {
    std::string expected = R"(TestClass.calculateNumber = function calculateNumber(){var callid = BridgeTS.generateCallID(this.ptr);var p = new Promise(function(resolve){TestClass.promises.set(callid,resolve);});BridgeTS.getInstance().async(JSON.stringify({ class : "TestClass", function : "calculateNumber", callback : "TestClass ._callback", cid : callid })); return p;};)";
    
    jsb::FunctionDescriptor desc = {"TestClass", "calculateNumber", false, 0, true, false};    
    auto result = jsb::CodeGenerator::classFunction(desc);
    ASSERT_EQ(expected, result);
}
