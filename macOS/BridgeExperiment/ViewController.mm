//
//  ViewController.m
//  BridgeExperiment
//
//  Created by Volodymyr Boichentsov on 18/12/2021.
//

#import "ViewController.h"
#import <WebKit/WebKit.h>

#include "Bridge.hpp"
#include "WKJSBridgeCommunicator.hpp"
#include "TestJSBinding.hpp"
#include "CodeGenerator.hpp"

//#include "em_js.h"

// TODO: parse arguments and return correct result
//EM_JS(int, foo, (int x, int y), { return 2 * x + y; });

    
using namespace jsb;

@implementation ViewController {
    
    WKWebView *webview;
}

- (void)viewDidLoad {
    [super viewDidLoad];
    
    WKUserContentController *controller = [[WKUserContentController alloc] init];
    [controller addScriptMessageHandler:self name:@"JSBModule"];
    
    WKWebViewConfiguration *config = [[WKWebViewConfiguration alloc] init];
    [config.preferences setValue:@YES forKey:@"allowFileAccessFromFileURLs"];
    config.userContentController = controller;
    
    webview = [[WKWebView alloc] initWithFrame:self.view.frame configuration:config];
    webview.autoresizingMask = NSViewWidthSizable | NSViewHeightSizable;
    webview.UIDelegate = self;
    webview.navigationDelegate = self;
    [self.view addSubview:webview];

    
    NSURL *url = [[NSBundle mainBundle] URLForResource:@"index" withExtension:@"html"];
    NSURL *path = url.URLByDeletingLastPathComponent;
    [webview loadFileURL:url allowingReadAccessToURL:path];
    
    // Do any additional setup after loading the view.
}

- (void)webView:(WKWebView *)webView didFinishNavigation:(null_unspecified WKNavigation *)navigation {
    jsb::Bridge::registerCommunicator<WKJSBridgeCommunicator>(webview);
}

- (void)userContentController:(WKUserContentController *)userContentController didReceiveScriptMessage:(WKScriptMessage *)message {
    if ([message.name isEqualToString:@"JSBModule"]) {
        NSString *messageStr = message.body;
        Bridge::recive(messageStr.UTF8String, nullptr);
    }
}

- (void)webView:(WKWebView *)webView runJavaScriptTextInputPanelWithPrompt:(NSString *)prompt defaultText:(NSString *)defaultText initiatedByFrame:(WKFrameInfo *)frame completionHandler:(void (^)(NSString * _Nullable))completionHandler {
     const char* data = [prompt cStringUsingEncoding:NSUTF8StringEncoding];
    
    Bridge::recive(data, [&](const char* r) {
        NSString *ret = [[NSString alloc] initWithBytesNoCopy:(void*)r length:strlen(r) encoding:NSUTF8StringEncoding freeWhenDone:NO];

//        NSString *ret = [NSString stringWithCString:r encoding:NSUTF8StringEncoding]; // or
        completionHandler(ret);
    });
}

- (void)setRepresentedObject:(id)representedObject {
    [super setRepresentedObject:representedObject];

    // Update the view, if already loaded.
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

void print_string() {
    printf("print string \n");
}

int add_numbers(int a, int b) {
    
    bool hit = false;
    std::string code = "(function foo() { JSBModule.print_string(); return \"1\";})()";
    Bridge::eval(code.c_str(), [&hit](const char* msg){
        printf("print string 2 \n");
        hit = true;
    });
    
    return a+b;
}


JSBridge_BINDINGS(my_module) {
    
    jsb::register_vector<int>("VectorInt").constructor(&vecIntFromIntPointer);
    
//    jsb::register_map<std::string, int>("MapSi");
    
    jsb::register_vector<std::string>("VectorString");
    
    function("add_numbers", &add_numbers);
    function("print_string", &print_string);
    
    jsb::class_<TempClass>("TempClass").constructor<>();
    
    jsb::class_<TestJSBinding>("TestJSBinding")
    .constructor<>()
//    .smart_ptr<std::shared_ptr<TestJSBinding>>("TestJSBindingPtr")
    .function("setNumber", &TestJSBinding::setNumber)
    .function("setNumberf", &TestJSBinding::setNumberf)
    .function("setNumberd", &TestJSBinding::setNumberd)
    .function("getNumber", &TestJSBinding::getNumber)
    .function("const_func", &TestJSBinding::const_func)
    .function("setString", &TestJSBinding::setString)
    .function("setString2", &setString2)
    .function("getString2", &getString2)
    
//    .function("setNumber2", &TestJSBinding::setNumber2)
//    .function("voidPtr", &TestJSBinding::voidPtr)
    .function("copyFrom", &TestJSBinding::copyFrom)
//    .function("forward", &TestJSBinding::forward)
//    .function("forwardf", &TestJSBinding::forwardf)
//    .function("forwardd", &TestJSBinding::forwardd)
//    .function("string2", &TestJSBinding::string2)
//    .function("string", &TestJSBinding::string)

    
    
    
//        .function("voidPtr", &TestJSBinding::voidPtr)
//        .function("voidPtr", &TestJSBinding::voidPtr)
//        .function("voidPtr", &TestJSBinding::voidPtr)
        .class_function("randomNumber", &TestJSBinding::randomNumber);
    
}



@end
