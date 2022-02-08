//
//  jsb
//  Created by Volodymyr Boichentsov on 05/01/2022.
//

#pragma once

#include <stdio.h>
#include <string>
#include <sstream>
#include <unordered_map>


namespace jsb {

struct FunctionInvokerBase;
using InvokersMap = std::unordered_map<std::string, std::unique_ptr<FunctionInvokerBase>>;

struct FunctionDescriptor {
    std::string name;
    const char* signature;
    bool is_static{ false };
    bool is_sync{ true };
};

// JavaScript code generator
class CodeGenerator {
public:
    static void registerBase();

    static void classDeclaration(const std::string& classname, const InvokersMap& invokers);

    static std::string function(const std::string& classname, const FunctionDescriptor& desc);
};

}
