//
//  ViewController.m
//  BridgeExperiment
//
//  Created by Volodymyr Boichentsov on 18/12/2021.
//

#import "ViewController.h"
#import <WebKit/WebKit.h>

#include "JSBridge.hpp"
#include "WKJSBridgeOperator.hpp"
#include "TestJSBinding.hpp"

//template <typename T>
//struct classtest {
//    classtest(T v) {
//        value = v;
//    };
//    T value;
//};
//    using invoker = classtest<
//        typename std::conditional<std::is_same<int, float>::value,
//                                  int,
//                                  float>::type>;
//
//    invoker t(1);
    


@implementation ViewController {
    std::shared_ptr<WKJSBridgeOperator> _bridgeOperator;
    
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
    
    _bridgeOperator = std::make_shared<WKJSBridgeOperator>(view);
    jsbridge::JSBridge::getInstance().bridgeOperator = _bridgeOperator;
    
    [self performSelector:@selector(registerTestObject) withObject:nil afterDelay:1];
    
    // Do any additional setup after loading the view.
}

- (void)userContentController:(WKUserContentController *)userContentController didReceiveScriptMessage:(WKScriptMessage *)message {
    if ([message.name isEqualToString:@"BridgeTS"]) {
        NSString *messageStr = message.body;
        _bridgeOperator->recive(messageStr.UTF8String, nullptr);
    }
}

- (void)webView:(WKWebView *)webView runJavaScriptTextInputPanelWithPrompt:(NSString *)prompt defaultText:(NSString *)defaultText initiatedByFrame:(WKFrameInfo *)frame completionHandler:(void (^)(NSString * _Nullable))completionHandler {
     const char* data = [prompt cStringUsingEncoding:NSUTF8StringEncoding];
    _bridgeOperator->recive(data, [=](const char* r){
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
    
    _bridgeOperator->send(ss.str().c_str());
}

float add(int a, int b) {
    return a + b;
}

inline std::vector<int> *vecIntFromIntPointer(uintptr_t vec) {
  return reinterpret_cast<std::vector<int> *>(vec);
}


JSBridge_BINDINGS(my_module) {
    jsbridge::function("add", &add);
    
    jsbridge::class_<TestJSBinding>("TestJSBinding")
        .constructor<>()
        .function("setNumber", &TestJSBinding::setNumber)
        .function("getNumber", &TestJSBinding::getNumber)
        .function("setNumber2", &TestJSBinding::setNumber2)
        .function("voidPtr", &TestJSBinding::voidPtr)
    
        .function("copyFrom", &TestJSBinding::copyFrom)
    
//        .function("voidPtr", &TestJSBinding::voidPtr)
//        .function("voidPtr", &TestJSBinding::voidPtr)
//        .function("voidPtr", &TestJSBinding::voidPtr)
        .class_function("randomNumber", &TestJSBinding::randomNumber);
    
    
//    jsbridge::register_vector<int>("VectorInt").constructor(&vecIntFromIntPointer);
}



@end
