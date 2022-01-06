//
//  ViewController.m
//  BridgeExperiment
//
//  Created by Volodymyr Boichentsov on 18/12/2021.
//

#import "ViewController.h"
#import <WebKit/WebKit.h>

#include "JSBridge.hpp"
#include "WKJSBridgeCommunicator.hpp"
#include "TestJSBinding.hpp"
#include "JSClassGenerator.hpp"

#include "em_js.h"

// TODO: parse arguments and return correct result
//EM_JS(int, foo, (int x, int y), { return 2 * x + y; });

    
using namespace jsbridge;

@implementation ViewController {
    std::shared_ptr<WKJSBridgeCommunicator> _bridgeOperator;
    
    std::shared_ptr<TestJSBinding> _test;
}

- (void)viewDidLoad {
    [super viewDidLoad];
    
    WKUserContentController *controller = [[WKUserContentController alloc] init];
    [controller addScriptMessageHandler:self name:@"BridgeTS"];
    
    WKWebViewConfiguration *config = [[WKWebViewConfiguration alloc] init];
    [config.preferences setValue:@YES forKey:@"allowFileAccessFromFileURLs"];
    config.userContentController = controller;
    
    WKWebView *view = [[WKWebView alloc] initWithFrame:self.view.frame configuration:config];
    view.autoresizingMask = NSViewWidthSizable | NSViewHeightSizable;
    view.UIDelegate = self;
    [self.view addSubview:view];

    
    NSURL *url = [[NSBundle mainBundle] URLForResource:@"index" withExtension:@"html"];
    NSURL *path = url.URLByDeletingLastPathComponent;
    [view loadFileURL:url allowingReadAccessToURL:path];
    
    
    jsbridge::JSBridge::registerCommunicator<WKJSBridgeCommunicator>(view);
    
    [self performSelector:@selector(registerTestObject) withObject:nil afterDelay:1];
    
//    int r = foo(1, 2);
    // Do any additional setup after loading the view.
}

- (void)userContentController:(WKUserContentController *)userContentController didReceiveScriptMessage:(WKScriptMessage *)message {
    if ([message.name isEqualToString:@"BridgeTS"]) {
        NSString *messageStr = message.body;
        JSBridge::recive(messageStr.UTF8String);
    }
}

- (void)webView:(WKWebView *)webView runJavaScriptTextInputPanelWithPrompt:(NSString *)prompt defaultText:(NSString *)defaultText initiatedByFrame:(WKFrameInfo *)frame completionHandler:(void (^)(NSString * _Nullable))completionHandler {
     const char* data = [prompt cStringUsingEncoding:NSUTF8StringEncoding];
    
    JSBridge::recive(data, [&](const char* r) {
        NSString *ret = [NSString stringWithCString:r encoding:NSUTF8StringEncoding];
        completionHandler(ret);
    });
}

- (void)setRepresentedObject:(id)representedObject {
    [super setRepresentedObject:representedObject];

    // Update the view, if already loaded.
}

- (void) registerTestObject {
    _test = std::make_shared<TestJSBinding>();
    auto ptr = reinterpret_cast<uintptr_t>(_test.get());
    
    std::stringstream ss("App.getTestJSBinding(", std::ios_base::app |std::ios_base::out);
    ss << ptr << ");";
    
    JSBridge::eval(ss.str().c_str());
    
    std::string code = "(function foo() { return \"{'msg': 4567 }\"; })()";
    JSBridge::eval(code.c_str()); 
    
    generateJavaScriptClassDeclaration("TempClass");
    JSBridge::eval("var obj = new TempClass();"); 
}

float add(int a, int b) {
    return a + b;
}

inline std::vector<int> *vecIntFromIntPointer(uintptr_t vec) {
  return reinterpret_cast<std::vector<int> *>(vec);
}

class TempClass {
public:
    TempClass() {
        printf("TempClass constructor");
    }
};


JSBridge_BINDINGS(my_module) {
    jsbridge::function("add", &add);
    
    jsbridge::class_<TempClass>("TempClass").constructor<>();
    
    jsbridge::class_<TestJSBinding>("TestJSBinding")
    .constructor<>()
    .function("setNumber", &TestJSBinding::setNumber)
    .function("setNumberf", &TestJSBinding::setNumberf)
    .function("setNumberd", &TestJSBinding::setNumberd)
    .function("getNumber", &TestJSBinding::getNumber)
    .function("setNumber2", &TestJSBinding::setNumber2)
    .function("voidPtr", &TestJSBinding::voidPtr)

    .function("copyFrom", &TestJSBinding::copyFrom)
    .function("forward", &TestJSBinding::forward)
    .function("forwardf", &TestJSBinding::forwardf)
    .function("forwardd", &TestJSBinding::forwardd)
//    .function("string", &TestJSBinding::string)
//    .function("const_func", &TestJSBinding::const_func)
    
    
    
//        .function("voidPtr", &TestJSBinding::voidPtr)
//        .function("voidPtr", &TestJSBinding::voidPtr)
//        .function("voidPtr", &TestJSBinding::voidPtr)
        .class_function("randomNumber", &TestJSBinding::randomNumber);
    
    
    class_<std::string>("string")
    .function("c_str", &std::string::c_str);
    
//    jsbridge::register_vector<int>("VectorInt").constructor(&vecIntFromIntPointer);
}



@end
