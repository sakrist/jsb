//
//  jsb
//  Created by Volodymyr Boichentsov on 05/01/2022.
//

#include "CodeGenerator.hpp"
#include "Bridge.hpp"
#include <string>
#include <regex>

namespace jsb {

void CodeGenerator::registerBase() {
    const std::string baseClass = R"(
    var BridgeTS = function () {
      function BridgeTS() {}
      BridgeTS.getInstance = function getInstance() {
        if (!BridgeTS.instance) {
          BridgeTS.instance = new BridgeTS();
        }
        return BridgeTS.instance;
      };
      var _proto = BridgeTS.prototype;
      _proto.async = function async(message) {
        window.webkit.messageHandlers.BridgeTS.postMessage(message);
      };

      _proto.sync = function sync(message) {
        try {
          return prompt(message);
        } catch (error) {
          console.log('The native context does not exist yet' + error);
        }
        return undefined;
      };

      BridgeTS.generateCallID = function generateCallID(v) {
        return (v ? v : "0") + "_" + Date.now() + "_" + Math.floor(Math.random() * 10000);
      };
      BridgeTS._callback = function _callback(key, value) {
        var resolve = BridgeTS.promises.get(key);
        BridgeTS.promises["delete"](key);
        if (resolve) {
          resolve(value);
        }
      };
      return BridgeTS;
    }();
    BridgeTS.promises = new Map();
    )";
    
    // TODO: replace BridgeTS with some module name
//    class_start = std::regex_replace(class_start, std::regex("TemplateJSBinding"), classname);
    
    jsb::Bridge::eval(baseClass.c_str());
}

void CodeGenerator::classDeclaration(const std::string& classname, const std::vector<FunctionDescriptor>& funcs) {
    
    std::string class_start = R"(var TemplateJSBinding = function() {
function TemplateJSBinding(ptrObject) {
    if (!ptrObject) {
        this.ptr = Number(BridgeTS.getInstance().sync('{ "class" : "TemplateJSBinding", "function" : "constructor" }'));
    }
}
TemplateJSBinding._callback = function _callback(key, value) {
    var resolve = TemplateJSBinding.promises.get(key);
    TemplateJSBinding.promises["delete"](key);
    if (resolve) { resolve(value); }
};
var _proto = TemplateJSBinding.prototype;
)";
    
    class_start = std::regex_replace(class_start, std::regex("TemplateJSBinding"), classname);
    std::stringstream ss(class_start, std::ios_base::app |std::ios_base::out);
    
    // function declaraions
    for (const auto& value : funcs) {
        ss << classFunction(value) << "\n";
    }
    
    // class end
    ss << "return " << classname << "; }();" <<  classname << ".promises = new Map();";
    
    jsb::Bridge::eval(ss.str().c_str());
}

std::string CodeGenerator::classFunction(const FunctionDescriptor& desc) {
    
    std::stringstream ss("", std::ios_base::app |std::ios_base::out);
    if (desc.is_static) {
        ss << desc.classid << ".";
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
    
    if (!desc.is_void) {
        if (desc.is_sync) {
            ss << "return ";
        } else {
            ss << "var callid = BridgeTS.generateCallID(this.ptr);"
            "var p = new Promise(function(resolve){";
            ss << desc.classid <<".promises.set(callid,resolve);});";
        }
    }
    ss << ((desc.is_sync) ? "BridgeTS.getInstance().sync(JSON.stringify({ class : \"" : 
                    "BridgeTS.getInstance().async(JSON.stringify({ class : \"") << desc.classid << "\", function : \"" << desc.name << "\"";
    
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
    
    if (!desc.is_void && !desc.is_sync) {
        ss << ", callback : \""<< desc.classid <<" ._callback\", cid : callid })); return p;";
    } else {
        ss << "}));";
    }
    
    ss << "};";
    
    return ss.str();
}

};
