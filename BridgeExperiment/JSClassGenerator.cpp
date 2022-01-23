//
//  JSClassGenerator.cpp
//  BridgeExperiment
//
//  Created by Volodymyr Boichentsov on 05/01/2022.
//

#include "JSClassGenerator.hpp"
#include "JSBridge.hpp"
#include <string>
#include <regex>

using namespace jsbridge;
namespace jsbridge {

void registerBaseJS() {
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
    
    function _inheritsLoose(subClass, superClass) {
      subClass.prototype = Object.create(superClass.prototype);
      subClass.prototype.constructor = subClass;
      _setPrototypeOf(subClass, superClass);
    }
    function _setPrototypeOf(o, p) {
      _setPrototypeOf = Object.setPrototypeOf || function _setPrototypeOf(o, p) {
        o.__proto__ = p; return o;
      }; return _setPrototypeOf(o, p);
    }
    function _assertThisInitialized(self) {
      if (self === void 0) {
        throw new ReferenceError("this hasn't been initialised - super() hasn't been called");
      }
      return self;
    }
    
    var JSBinding = function () {
      function JSBinding(ptrObject) {
        this.ptr = ptrObject;
      }
      JSBinding.getTemporaryObjectId = function getTemporaryObjectId() {
        JSBinding._tmpPtrID++;
        return JSBinding._tmpPtrID;
      };
      return JSBinding;
    }();
    JSBinding._tmpPtrID = 0; )";
    
    // TODO: replace BridgeTS with some module name
//    class_start = std::regex_replace(class_start, std::regex("TemplateJSBinding"), classname);
    
    JSBridge::eval(baseClass.c_str());
}

void generateJavaScriptClassDeclaration(const std::string& classname, const std::unordered_map<std::string, std::string>& funcs) {
    
    std::string class_start = R"(var TemplateJSBinding = function(_JSBinding) {
_inheritsLoose(TemplateJSBinding, _JSBinding);
function TemplateJSBinding(ptrObject) {
    if (!ptrObject) {
        ptrObject = Number(BridgeTS.getInstance().sync('{ "class" : "TemplateJSBinding", "function" : "constructor" }'));
    }
    var _this = _JSBinding.call(this, ptrObject) || this;
    return _assertThisInitialized(_this);
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
    for (auto& [key, value] : funcs) {
        ss << value << "\n";
    }
    
    // class end
    ss << "return " << classname << "; }(JSBinding);" <<  classname << ".promises = new Map();";
    
    jsbridge::JSBridge::eval(ss.str().c_str());
}

std::string generateJavaScriptFunction(const std::string& classid, const FunctionDescriptor desc) {
    
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
    
    if (!desc.is_void) {
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
    
    if (!desc.is_void && !desc.is_sync) {
        ss << ", callback : \""<< classid <<" ._callback\", cid : callid })); return p;";
    } else {
        ss << "}));";
    }
    
    ss << "};";
    
    return ss.str();
}

};
