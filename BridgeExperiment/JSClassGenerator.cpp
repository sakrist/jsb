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

void jsbridge::generateJavaScriptClassDeclaration(const std::string& classname) {
    
    std::string class_var = R"(
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

    var _proto = TestJSBinding.prototype;

    //##declaraions##

    return TemplateJSBinding;
}(JSBinding);
TemplateJSBinding.promises = /*#__PURE__*/new Map();)";
    
    class_var = std::regex_replace(class_var, std::regex("TemplateJSBinding"), classname);
    
    jsbridge::JSBridge::eval(class_var.c_str());
   
}
