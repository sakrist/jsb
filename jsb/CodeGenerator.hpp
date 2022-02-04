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

struct FunctionDescriptor {
    std::string classid;
    std::string name;
    bool is_void{ false }; // TODO: replace with signature, but for now it's just bool for return type. true - void, false - something
    int args_count{ 0 };
    bool is_static{ false };
    bool is_sync{ true };
};

// JavaScript code generator
class CodeGenerator {
public:
    static void registerBase();

    static void classDeclaration(const std::string& classname, const std::vector<FunctionDescriptor>& funcs);

    static std::string classFunction(const FunctionDescriptor& desc);
};

}
