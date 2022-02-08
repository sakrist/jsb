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
    std::string expected = R"(_proto.calculate = function _calculate(){JSBModule.getInstance().sync(JSON.stringify({ class : "TestClass", function : "calculate", object : this.ptr}));};)";
    
    jsb::FunctionDescriptor desc = {"calculate", "v", false, true};    
    auto result = jsb::CodeGenerator::function("JSBModule", "TestClass", desc);
    ASSERT_EQ(expected, result);
}

TEST(CodeGenerator, FunctionGen_void_args1) {
    std::string expected = R"(_proto.setNumber = function _setNumber(v0){JSBModule.getInstance().sync(JSON.stringify({ class : "TestClass", function : "setNumber", object : this.ptr, args : [v0]}));};)";
    
    jsb::FunctionDescriptor desc = {"setNumber", "vi", false, true};    
    auto result = jsb::CodeGenerator::function("JSBModule", "TestClass", desc);
    ASSERT_EQ(expected, result);
}

TEST(CodeGenerator, FunctionGen_void_args2) {
    std::string expected = R"(_proto.setNumber = function _setNumber(v0,v1){JSBModule.getInstance().sync(JSON.stringify({ class : "TestClass", function : "setNumber", object : this.ptr, args : [v0,v1]}));};)";
    
    jsb::FunctionDescriptor desc = {"setNumber", "vii", false, true};    
    auto result = jsb::CodeGenerator::function("JSBModule", "TestClass", desc);
    ASSERT_EQ(expected, result);
}

TEST(CodeGenerator, FunctionGen_int_args2) {
    std::string expected = R"(_proto.setNumber = function _setNumber(v0,v1){return JSBModule.getInstance().sync(JSON.stringify({ class : "TestClass", function : "setNumber", object : this.ptr, args : [v0,v1]}));};)";
    
    jsb::FunctionDescriptor desc = {"setNumber", "iii", false, true};    
    auto result = jsb::CodeGenerator::function("JSBModule", "TestClass", desc);
    ASSERT_EQ(expected, result);
}

TEST(CodeGenerator, FunctionGen_int_args0) {
    std::string expected = R"(_proto.getNumber = function _getNumber(){return JSBModule.getInstance().sync(JSON.stringify({ class : "TestClass", function : "getNumber", object : this.ptr}));};)";
    
    jsb::FunctionDescriptor desc = {"getNumber", "i", false, true};    
    auto result = jsb::CodeGenerator::function("JSBModule", "TestClass", desc);
    ASSERT_EQ(expected, result);
}

TEST(CodeGenerator, FunctionGen_int_args0_static) {
    std::string expected = R"(TestClass.random = function _random(){return JSBModule.getInstance().sync(JSON.stringify({ class : "TestClass", function : "random"}));};)";
    
    jsb::FunctionDescriptor desc = {"random", "i", true, true};    
    auto result = jsb::CodeGenerator::function("JSBModule", "TestClass", desc);
    ASSERT_EQ(expected, result);
}

TEST(CodeGenerator, FunctionGen_int_args1_static) {
    std::string expected = R"(TestClass.random = function _random(v0){return JSBModule.getInstance().sync(JSON.stringify({ class : "TestClass", function : "random", args : [v0]}));};)";
    
    jsb::FunctionDescriptor desc = {"random", "ii", true, true};    
    auto result = jsb::CodeGenerator::function("JSBModule", "TestClass", desc);
    ASSERT_EQ(expected, result);
}

TEST(CodeGenerator, FunctionGen_void_args0_async_static) {
    std::string expected = R"(TestClass.calculateNumber = function _calculateNumber(){JSBModule.getInstance().async(JSON.stringify({ class : "TestClass", function : "calculateNumber"}));};)";
    
    jsb::FunctionDescriptor desc = {"calculateNumber", "v", true, false};    
    auto result = jsb::CodeGenerator::function("JSBModule", "TestClass", desc);
    ASSERT_EQ(expected, result);
}

TEST(CodeGenerator, FunctionGen_void_args1_async) {
    std::string expected = R"(_proto.setNumber = function _setNumber(v0){JSBModule.getInstance().async(JSON.stringify({ class : "TestClass", function : "setNumber", object : this.ptr, args : [v0]}));};)";
    
    jsb::FunctionDescriptor desc = {"setNumber", "vi", false, false};    
    auto result = jsb::CodeGenerator::function("JSBModule", "TestClass", desc);
    ASSERT_EQ(expected, result);
}

TEST(CodeGenerator, FunctionGen_int_args2_async) {
    std::string expected = R"(_proto.calculateNumber = function _calculateNumber(v0,v1){var callid = JSBModule.generateCallID(this.ptr);var p = new Promise(function(resolve){TestClass.promises.set(callid,resolve);});JSBModule.getInstance().async(JSON.stringify({ class : "TestClass", function : "calculateNumber", object : this.ptr, args : [v0,v1], callback : "TestClass ._callback", cid : callid })); return p;};)";
    
    jsb::FunctionDescriptor desc = {"calculateNumber", "iii", false, false};    
    auto result = jsb::CodeGenerator::function("JSBModule", "TestClass", desc);
    ASSERT_EQ(expected, result);
}

TEST(CodeGenerator, FunctionGen_int_args0_async_static) {
    std::string expected = R"(TestClass.calculateNumber = function _calculateNumber(){var callid = JSBModule.generateCallID(this.ptr);var p = new Promise(function(resolve){TestClass.promises.set(callid,resolve);});JSBModule.getInstance().async(JSON.stringify({ class : "TestClass", function : "calculateNumber", callback : "TestClass ._callback", cid : callid })); return p;};)";
    
    jsb::FunctionDescriptor desc = {"calculateNumber", "i", true, false};    
    auto result = jsb::CodeGenerator::function("JSBModule", "TestClass", desc);
    ASSERT_EQ(expected, result);
}
