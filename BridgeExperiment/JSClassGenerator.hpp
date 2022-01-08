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
void generateJavaScriptClassDeclaration(const std::string& classname, const std::unordered_map<std::string, std::string>& funcs);

struct FunctionDescriptor {
    std::string name;
    int args_count{ 0 };
    bool is_static{ false };
    bool is_sync{ true };
};

template<typename R>
JSBRIDGE_ALWAYS_INLINE std::string generateJavaScriptFunction(const std::string& classid, const FunctionDescriptor desc) {
/* Examples
    // sync
    _proto.getNumber = function getNumber() {
        return BridgeTS.getInstance().sync(JSON.stringify({ class : "TestJSBinding", object : this.ptr, function : "getNumber" }));
    };
    _proto.setNumber = function setNumber(number) {
        BridgeTS.getInstance().sync(JSON.stringify({ class : "TestJSBinding", object : this.ptr, function : "setNumber", args : [ number ] }));
    };
    // async
    _proto.setNumber = function setNumber(index) {
        BridgeTS.getInstance().async(JSON.stringify({ class : "TestJSBinding", object : this.ptr, function : "setNumber", args : [ index ]}));
    };
    // async return promise
    _proto.getNumber3 = function getNumber3() {
        var callid = BridgeTS.generateCallID(this.ptr);
        var p = new Promise(function (resolve) {
            TestJSBinding.promises.set(callid, resolve);
        });
        BridgeTS.getInstance().async(JSON.stringify({ class : "TestJSBinding", object : this.ptr, function : "getNumber", callback : "TestJSBinding._callback", cid : callid }));
        return p;
    };
*/
    
    std::stringstream ss("", std::ios_base::app |std::ios_base::out);
    if (desc.is_static) {
        ss << classid << ".";
    } else {
        ss << "_proto.";
    }
    ss << desc.name << " = function " << desc.name << "(";
    
    for(int i = 0; i < desc.args_count; i++) {
        ss << "v" << i;
        if (i != desc.args_count-1) {
            ss << ",";
        }
    }
    ss << "){";
    
    if constexpr (!std::is_same_v<R, void>) {
        if (desc.is_sync) {
            ss << "return ";
        } else {
            ss << "var callid = BridgeTS.generateCallID(this.ptr);"
            "var p = new Promise(function(resolve){";
            ss << classid <<".promises.set(callid,resolve);});";
        }
        
    }
    ss << ((desc.is_sync) ? "BridgeTS.getInstance().sync(JSON.stringify({ class : \"" : 
                    "BridgeTS.getInstance().async(JSON.stringify({ class : \"") << classid << "\", function : \"" << desc.name << "\"";
    
    if (!desc.is_static) {
        ss << ", object : this.ptr";
    }

    if (desc.args_count > 0) {
        ss << ", args : [" ;
        for(int i = 0; i < desc.args_count; i++) {
            ss << "v" << i;
            if (i != desc.args_count-1) {
                ss << ",";
            }
        }
        ss << "]";
    }
    
    if constexpr (!std::is_same_v<R, void>) {
        if (!desc.is_sync) {
            ss << ", callback : \""<< classid <<" ._callback\", cid : callid })); return p;";
        } else {
            ss << "}));";
        }
    } else {
        ss << "}));";
    }
    
    ss << "};";
    
    return ss.str();
}

}


