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

template <typename T>
T operator|(T a, T b) {
  return static_cast<T>(static_cast<std::underlying_type_t<T>>(a) |
                        static_cast<std::underlying_type_t<T>>(b));
}

struct FunctionInvokerBase;
using InvokersMap = std::unordered_map<std::string, std::unique_ptr<FunctionInvokerBase>>;

struct FunctionDescriptor {
    
    enum class Configuration : unsigned int {
        None = 0,
        Sync = 1 << 1,
        Static = 1 << 2,
        Class = 1 << 3
    };

    
    std::string name;
    const char* signature;
    Configuration config = Configuration::Sync;
};

// JavaScript code generator
class CodeGenerator {
public:
    static void registerBase(const std::string& moduleName);

    static void classDeclaration(const std::string& moduleName, 
      const std::string& className, 
      const InvokersMap& invokers);

    static std::string function(const std::string& moduleName, 
      const std::string& className, 
      const FunctionDescriptor& desc);
};

}
