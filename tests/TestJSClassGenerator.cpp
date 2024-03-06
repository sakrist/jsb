//
//  TestCodeGenerator.cpp
//  BridgeExperimentTests
//
//  Created by Volodymyr Boichentsov on 08/01/2022.
//

#include <stdio.h>
#include "gtest/gtest.h"
#include "CodeGenerator.hpp"

using namespace jsb;

TEST(CodeGenerator, FunctionGen_void_no_args) {
    std::string expected = R"(_proto.calculate = function _calculate(){jsbc.getInstance().sync(JSON.stringify({ class : "TestClass", function : "calculate", object : this.ptr}));};)";
    
    jsb::FunctionDescriptor desc = {"calculate", "v", FunctionDescriptor::Configuration::Sync };
    auto result = jsb::CodeGenerator::function("Module", "jsbc", "TestClass", desc);
    ASSERT_EQ(expected, result);
}

TEST(CodeGenerator, FunctionGen_void_args1) {
    std::string expected = R"(_proto.setNumber = function _setNumber(v0){jsbc.getInstance().sync(JSON.stringify({ class : "TestClass", function : "setNumber", object : this.ptr, args : [v0]}));};)";
    
    jsb::FunctionDescriptor desc = {"setNumber", "vi", FunctionDescriptor::Configuration::Sync};
    auto result = jsb::CodeGenerator::function("Module", "jsbc", "TestClass", desc);
    ASSERT_EQ(expected, result);
}

TEST(CodeGenerator, FunctionGen_void_args2) {
    std::string expected = R"(_proto.setNumber = function _setNumber(v0,v1){jsbc.getInstance().sync(JSON.stringify({ class : "TestClass", function : "setNumber", object : this.ptr, args : [v0,v1]}));};)";
    
    jsb::FunctionDescriptor desc = {"setNumber", "vii", FunctionDescriptor::Configuration::Sync};
    auto result = jsb::CodeGenerator::function("Module", "jsbc", "TestClass", desc);
    ASSERT_EQ(expected, result);
}

TEST(CodeGenerator, FunctionGen_int_args2) {
    std::string expected = R"(_proto.setNumber = function _setNumber(v0,v1){let r = jsbc.getInstance().sync(JSON.stringify({ class : "TestClass", function : "setNumber", object : this.ptr, args : [v0,v1]}));return r;};)";
    
    jsb::FunctionDescriptor desc = {"setNumber", "iii", FunctionDescriptor::Configuration::Sync};
    auto result = jsb::CodeGenerator::function("Module", "jsbc", "TestClass", desc);
    ASSERT_EQ(expected, result);
}

TEST(CodeGenerator, FunctionGen_int_args0) {
    std::string expected = R"(_proto.getNumber = function _getNumber(){let r = jsbc.getInstance().sync(JSON.stringify({ class : "TestClass", function : "getNumber", object : this.ptr}));return r;};)";
    
    jsb::FunctionDescriptor desc = {"getNumber", "i", FunctionDescriptor::Configuration::Sync};
    auto result = jsb::CodeGenerator::function("Module", "jsbc", "TestClass", desc);
    ASSERT_EQ(expected, result);
}

TEST(CodeGenerator, FunctionGen_int_args0_static) {
    std::string expected = R"(TestClass.random = function _random(){let r = jsbc.getInstance().sync(JSON.stringify({ class : "TestClass", function : "random"}));return r;};)";
    
    jsb::FunctionDescriptor desc = {"random", "i", FunctionDescriptor::Configuration::Sync | FunctionDescriptor::Configuration::Static};
    auto result = jsb::CodeGenerator::function("Module", "jsbc", "TestClass", desc);
    ASSERT_EQ(expected, result);
}

TEST(CodeGenerator, FunctionGen_int_args1_static) {
    std::string expected = R"(TestClass.random = function _random(v0){let r = jsbc.getInstance().sync(JSON.stringify({ class : "TestClass", function : "random", args : [v0]}));return r;};)";
    
    jsb::FunctionDescriptor desc = {"random", "ii", FunctionDescriptor::Configuration::Sync | FunctionDescriptor::Configuration::Static};
    auto result = jsb::CodeGenerator::function("Module", "jsbc", "TestClass", desc);
    ASSERT_EQ(expected, result);
}

TEST(CodeGenerator, FunctionGen_void_args0_async_static) {
    std::string expected = R"(TestClass.calculateNumber = function _calculateNumber(){jsbc.getInstance().async(JSON.stringify({ class : "TestClass", function : "calculateNumber"}));};)";
    
    jsb::FunctionDescriptor desc = {"calculateNumber", "v", FunctionDescriptor::Configuration::Static};
    auto result = jsb::CodeGenerator::function("Module", "jsbc", "TestClass", desc);
    ASSERT_EQ(expected, result);
}

TEST(CodeGenerator, FunctionGen_void_args1_async) {
    std::string expected = R"(_proto.setNumber = function _setNumber(v0){jsbc.getInstance().async(JSON.stringify({ class : "TestClass", function : "setNumber", object : this.ptr, args : [v0]}));};)";
    
    jsb::FunctionDescriptor desc = {"setNumber", "vi", FunctionDescriptor::Configuration::None};
    auto result = jsb::CodeGenerator::function("Module", "jsbc", "TestClass", desc);
    ASSERT_EQ(expected, result);
}

TEST(CodeGenerator, FunctionGen_int_args2_async) {
    std::string expected = R"(_proto.calculateNumber = function _calculateNumber(v0,v1){let callid = jsbc.generateCallID(this.ptr);let p = new Promise(function(resolve){TestClass.promises.set(callid,resolve);});jsbc.getInstance().async(JSON.stringify({ class : "TestClass", function : "calculateNumber", object : this.ptr, args : [v0,v1], callback : "TestClass._callback", cid : callid })); return p;};)";
    
    jsb::FunctionDescriptor desc = {"calculateNumber", "iii", FunctionDescriptor::Configuration::None};
    auto result = jsb::CodeGenerator::function("Module", "jsbc", "TestClass", desc);
    ASSERT_EQ(expected, result);
}

TEST(CodeGenerator, FunctionGen_int_args0_async_static) {
    std::string expected = R"(TestClass.calculateNumber = function _calculateNumber(){let callid = jsbc.generateCallID(this.ptr);let p = new Promise(function(resolve){TestClass.promises.set(callid,resolve);});jsbc.getInstance().async(JSON.stringify({ class : "TestClass", function : "calculateNumber", callback : "TestClass._callback", cid : callid })); return p;};)";
    
    jsb::FunctionDescriptor desc = {"calculateNumber", "i", FunctionDescriptor::Configuration::Static};    
    auto result = jsb::CodeGenerator::function("Module", "jsbc", "TestClass", desc);
    ASSERT_EQ(expected, result);
}
