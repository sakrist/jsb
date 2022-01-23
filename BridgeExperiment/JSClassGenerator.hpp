//
//  JSClassGenerator.hpp
//  BridgeExperiment
//
//  Created by Volodymyr Boichentsov on 05/01/2022.
//

#pragma once

#include <stdio.h>
#include <string>
#include <sstream>
#include <unordered_map>

#define JSBRIDGE_ALWAYS_INLINE __attribute__((always_inline))

namespace jsbridge {

void registerBaseJS();

void generateJavaScriptClassDeclaration(const std::string& classname, const std::unordered_map<std::string, std::string>& funcs);

struct FunctionDescriptor {
    std::string name;
    bool is_void{ false }; // TODO: replace with signature, but for now it's just bool for return type. true - void, false - something
    int args_count{ 0 };
    bool is_static{ false };
    bool is_sync{ true };
};

std::string generateJavaScriptFunction(const std::string& classid, const FunctionDescriptor desc);

}


