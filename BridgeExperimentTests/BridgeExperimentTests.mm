//
//  BridgeExperimentTests.m
//  BridgeExperimentTests
//
//  Created by Volodymyr Boichentsov on 18/12/2021.
//

#import <XCTest/XCTest.h>

#include "json.hpp"
using json = nlohmann::json;

#include "JSBridge.hpp"
#include "TestJSBinding.hpp"

using namespace jsbridge;


class TestSimple {
public:
    void setNumber(int i) {
        number = i;
    }
    int getNumber() {
        return number;
    }
    int number = 0;
};

JSBridge_BINDINGS(TestSimple_module) {
    class_<TestSimple>("TestSimple")
    .function("setNumber", &TestSimple::setNumber)
    .function("getNumber", &TestSimple::getNumber);
}

void Test_simple_set_and_get() {
    auto test = std::make_shared<TestSimple>();
    uintptr_t ptrObject = reinterpret_cast<uintptr_t>(test.get());
    
    JSInvokeMessage message1;
    message1.object = ptrObject;
    message1.class_id = "TestSimple";
    message1.function = "setNumber";
    message1.args[0] = 2;
    JSBridge::getInstance().recive(message1);
    
    JSInvokeMessage message2;
    message2.object = ptrObject;
    message2.class_id = "TestSimple";
    message2.function = "getNumber";
    message2.completion = [=](const char* result){ 
        
        json obj = json::parse(result);
        auto r = obj["r"].get<int>();
        assert(test->number == r);
    };
    JSBridge::getInstance().recive(message2);
}


// More cases for compilation
class TestCompilation {
public:
    void setNumber(int i) {
        number = i;
    }
    int getNumber() {
        return number;
    }
    void setFNumber(float i) {
        fnumber = i;
    }
    float getFNumber() {
        return fnumber;
    }
    float fnumber = 0;
    int number = 0;
    
    static void runTest() {}
    
    static double getDoubleMax() { return DBL_MAX; }
    static int getIntMax() { return INT_MAX; }
};


void Test_when_function_not_from_class() {
    // TODO: should not compile
    class_<TestCompilation>("TestCompilation2")
    .function("setNumber", &TestSimple::setNumber)
    .function("getNumber", &TestSimple::getNumber);
}

void Test_static_functions_compile() {
    class_<TestCompilation>("TestCompilation")
    .function("setNumber", &TestCompilation::setNumber)
    .function("getNumber", &TestCompilation::getNumber)
    .class_function("runTest", &TestCompilation::runTest)
    .class_function("getDoubleMax", &TestCompilation::getDoubleMax)
    .class_function("getIntMax", &TestCompilation::getIntMax);
}

void Test_redefinition() {
    try {
        class_<TestCompilation>("TestCompilation");
        assert(0);
    } catch (const std::logic_error& error) {
        printf("%s", error.what());
    }
}



@interface BridgeExperimentTests : XCTestCase
@end
@implementation BridgeExperimentTests
- (void)testExample {
    // This is an example of a functional test case.
    // Use XCTAssert and related functions to verify your tests produce the correct results.
    
    Test_simple_set_and_get();
    
    Test_when_function_not_from_class();
    
    Test_static_functions_compile();
    
    Test_redefinition();
}
@end
