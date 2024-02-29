//
//  jsb
//  Created by Volodymyr Boichentsov on 05/01/2022.
//

#include "CodeGenerator.hpp"
#include "Bridge.hpp"
#include <string>
#include <regex>


template <typename T>
bool operator&(T a, T b) {
  return (static_cast<std::underlying_type_t<T>>(a) &
                        static_cast<std::underlying_type_t<T>>(b)) > 0;
}

namespace jsb {

void CodeGenerator::registerBase(const std::string& moduleName) {
    std::string moduleCode = R"(
    var JSBModule = function () {
      function JSBModule() {}
      JSBModule.getInstance = function getInstance() {
        if (!JSBModule.instance) {
          JSBModule.instance = new JSBModule();
        }
        return JSBModule.instance;
      };
      var _proto = JSBModule.prototype;
      _proto.async = function async(message) {
        window.webkit.messageHandlers.JSBModule.postMessage(message);
      };

      _proto.sync = function sync(message) {
        try {
          return prompt(message);
        } catch (error) {
          console.log('The native context does not exist yet' + error);
        }
        return undefined;
      };

      JSBModule.generateCallID = function generateCallID(v) {
        return (v ? v : "0") + "_" + Date.now() + "_" + Math.floor(Math.random() * 10000);
      };
      JSBModule._callback = function _callback(key, value) {
        var resolve = JSBModule.promises.get(key);
        JSBModule.promises["delete"](key);
        if (resolve) {
          resolve(value);
        }
      };
      return JSBModule;
    }();
    JSBModule.promises = new Map();
    )";
    
    moduleCode = std::regex_replace(moduleCode, std::regex("JSBModule"), moduleName);
    
    jsb::Bridge::eval(moduleCode.c_str());
}

void CodeGenerator::classDeclaration(const std::string& moduleName, 
 const std::string& className, 
 const InvokersMap& invokers) {
    
    std::string class_start = R"(var TemplateJSBinding = function() {
function TemplateJSBinding(ptrObject) {
    if (!ptrObject) {
        this.ptr = Number(JSBModule.getInstance().sync('{ "class" : "TemplateJSBinding", "function" : "ctor" }'));
    } else {
        this.ptr = Number(ptrObject);
    }
}
TemplateJSBinding._callback = function __callback__(key, value) {
    var resolve = TemplateJSBinding.promises.get(key);
    TemplateJSBinding.promises["delete"](key);
    if (resolve) { resolve(value); }
};
var _proto = TemplateJSBinding.prototype;

_proto.delete = function _dtor() {
JSBModule.getInstance().sync(JSON.stringify({ class : "TemplateJSBinding", function : "dtor", object : this.ptr, args : [this.ptr]}));};
)";
    
    class_start = std::regex_replace(class_start, std::regex("TemplateJSBinding"), className);
    class_start = std::regex_replace(class_start, std::regex("JSBModule"), moduleName);
    std::stringstream ss(class_start, std::ios_base::app |std::ios_base::out);
    
    // function declaraions
    for (const auto& [key, value] : invokers) {
        ss << function(moduleName, className, value->descriptor);
    }
    
    // class end
    ss << "return " << className << "; }();" <<  className << ".promises = new Map();";
    
    jsb::Bridge::eval(ss.str().c_str());
}

std::string CodeGenerator::function(const std::string& moduleName, 
  const std::string& className, 
  const FunctionDescriptor& desc) {
    
    if (desc.name.empty()) {
        return "";
    }
    
    bool return_void = (desc.signature[0] == 'v');
    size_t args_count = strlen(desc.signature)-1;
    
    std::stringstream ss("", std::ios_base::app |std::ios_base::out);
    if (desc.config & FunctionDescriptor::Configuration::Static) {
        ss << className << ".";
    } else {
        ss << "_proto.";
    }
    ss << desc.name << " = function _" << desc.name << "(";
    
    
    int i = (desc.config & FunctionDescriptor::Configuration::Class) ? 1 : 0;

    for(; i < args_count; i++) {
        ss << "v" << i;
        if (i != args_count-1) {
            ss << ",";
        }
    }
    ss << "){\n";
    
    if (!return_void) {
        if (desc.config & FunctionDescriptor::Configuration::Sync) {
            ss << "var result = ";
        } else {
            ss << "var callid = " << moduleName << ".generateCallID(this.ptr);"
            "var p = new Promise(function(resolve){";
            ss << className <<".promises.set(callid,resolve);});";
        }
    }
    
    ss << moduleName;
    ss << ((desc.config & FunctionDescriptor::Configuration::Sync) ? ".getInstance().sync(JSON.stringify({ class : \"" : ".getInstance().async(JSON.stringify({ class : \"");
    ss << ((className == moduleName) ? "" : className) << "\", function : \"" << desc.name << "\"";
    
    if (!(desc.config & FunctionDescriptor::Configuration::Static)) {
        ss << ", object : this.ptr";
    }

    if (args_count > 0) {
        ss << ", args : [" ;
        
        int i = 0;
        if ((desc.config & FunctionDescriptor::Configuration::Class)) {
            i = 1;
            ss << "this.ptr, " ;
        }
        for(; i < args_count; i++) {
            if (desc.signature[i+1] == 'p') {
                ss << "v" << i << ".ptr";
            } else {
                ss << "v" << i;
            }
            if (i != args_count-1) {
                ss << ",";
            }
        }
        ss << "]";
    }
    
    if (!return_void && !(desc.config & FunctionDescriptor::Configuration::Sync)) {
        ss << ", callback : \""<< className <<" ._callback\", cid : callid })); return p;";
    } else {
        ss << "}));\n";
        if (!return_void) {
            
            if (!desc.returnType.empty()) {
                ss << "result = new " << desc.returnType << "(result);\n";
            }
            
            ss << "return result;\n";
        }
    }
    
    ss << "};\n";
    
    return ss.str();
}

};
