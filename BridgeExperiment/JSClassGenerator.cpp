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

void jsbridge::generateJavaScriptClassDeclaration(const std::string& classname, const std::unordered_map<std::string, std::string>& funcs) {
    
    std::string class_start = R"(
var TemplateJSBinding = /*#__PURE__*/function (_JSBinding) {
    _inheritsLoose(TemplateJSBinding, _JSBinding);

    function TemplateJSBinding(ptrObject) {
        var _this;

        if (!ptrObject) {
            ptrObject = BridgeTS.getInstance().sync('{ "class" : "TemplateJSBinding", "function" : "constructor" }');
            _this = _JSBinding.call(this, ptrObject) || this;
        } else {
            _this = _JSBinding.call(this, ptrObject) || this;
        }

        return _assertThisInitialized(_this);
    }
    
    TemplateJSBinding._callback = function _callback(key, value) {
        var resolve = TemplateJSBinding.promises.get(key);
        TemplateJSBinding.promises["delete"](key);

        if (resolve) {
            resolve(value);
        }
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
    ss << "return " << classname << "; }(JSBinding);" <<  classname << ".promises = /*#__PURE__*/new Map();";
    
    jsbridge::JSBridge::eval(ss.str().c_str(),[](const char*){});
}
