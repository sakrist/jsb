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

void CodeGenerator::registerBase(const std::string& moduleName,
                                 const std::string& comName) {
    std::string moduleCode = R"(
    window.#Module# = {};
    var #_jsbc_# = function () {
      function #_jsbc_#() {}
        #_jsbc_#.getInstance = function getInstance() {
        if (!#_jsbc_#.instance) {
            #_jsbc_#.instance = new #_jsbc_#();
        }
        return #_jsbc_#.instance;
      };
      let _proto = #_jsbc_#.prototype;
      _proto.async = function async(message) {
        window.webkit.messageHandlers.#_jsbc_#.postMessage(message);
      };

      _proto.sync = function sync(message) {
        try {
          return prompt(message);
        } catch (error) {
          console.log('The native context does not exist yet' + error);
        }
        return undefined;
      };

      #_jsbc_#.generateCallID = function generateCallID(v) {
        return (v ? v : "0") + "_" + Date.now() + "_" + Math.floor(Math.random() * 10000);
      };
      #_jsbc_#._callback = function _callback(key, value) {
        let resolve = #_jsbc_#.promises.get(key);
        #_jsbc_#.promises["delete"](key);
        if (resolve) {
          resolve(value);
        }
      };
      return #_jsbc_#;
    }();
    #_jsbc_#.promises = new Map();
    )";
    
    moduleCode = std::regex_replace(moduleCode, std::regex("#Module#"), moduleName);
    moduleCode = std::regex_replace(moduleCode, std::regex("#_jsbc_#"), comName);
    
    jsb::Bridge::eval(moduleCode.c_str());
}

void CodeGenerator::classDeclaration(const std::string& moduleName, 
 const std::string& comName,
 const std::string& className,
 const InvokersMap& invokers) {
    
    std::string class_start = R"(window.#Module#.#TemplateClass# = function() {
function #TemplateClass#(ptrObject) {
    if (!ptrObject) {
        this.ptr = Number(#_jsbc_#.getInstance().sync('{ "class" : "#TemplateClass#", "function" : "ctor" }'));
    } else {
        this.ptr = Number(ptrObject);
    }
}
#TemplateClass#._callback = function __callback__(key, value) {
    let resolve = window.#Module#.#TemplateClass#.promises.get(key);
    window.#Module#.#TemplateClass#.promises["delete"](key);
    if (resolve) { resolve(value); }
};
let _proto = #TemplateClass#.prototype;

_proto.delete = function _dtor() {
    #_jsbc_#.getInstance().sync(JSON.stringify({ class : "#TemplateClass#", function : "dtor", object : this.ptr, args : [this.ptr]}));};
)";
    
    class_start = std::regex_replace(class_start, std::regex("#TemplateClass#"), className);
    class_start = std::regex_replace(class_start, std::regex("#Module#"), moduleName);
    class_start = std::regex_replace(class_start, std::regex("#_jsbc_#"), comName);
    std::stringstream ss(class_start, std::ios_base::app |std::ios_base::out);
    
    // function declaraions
    for (const auto& [key, value] : invokers) {
        ss << function(moduleName, comName, className, value->descriptor);
        ss << "\n";
    }
    
    // class end
    ss << "return " << className << "; }();\nwindow."<< moduleName <<"." <<  className << ".promises = new Map();";
    
    jsb::Bridge::eval(ss.str().c_str());
}

std::string CodeGenerator::function(const std::string& moduleName,
  const std::string& comName,
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
    ss << "){";
    
    if (!return_void) {
        if (desc.config & FunctionDescriptor::Configuration::Sync) {
            ss << "let r = ";
        } else {
            ss << "let callid = " << comName << ".generateCallID(this.ptr);"
            "let p = new Promise(function(resolve){";
            ss << className <<".promises.set(callid,resolve);});";
        }
    }
    
    ss << comName;
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
                ss << "(v"<<i<<" ? v"<<i<<".ptr : null)";
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
        ss << ", callback : \""<< className <<"._callback\", cid : callid })); return p;";
    } else {
        ss << "}));";
        if (!return_void) {
            
            if (!desc.returnType.empty()) {
                ss << "r = new window."<< moduleName <<"." << desc.returnType << "(r);";
            }
            
            ss << "return r;";
        }
    }
    
    ss << "};";
    
    return ss.str();
}

};
