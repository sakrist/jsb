//
//  BridgeExperimentTests.m
//  BridgeExperimentTests
//
//  Created by Volodymyr Boichentsov on 18/12/2021.
//

#import <XCTest/XCTest.h>
#include "JSBridge.hpp"
#include "TestJSBinding.hpp"

@interface BridgeExperimentTests : XCTestCase

@end

@implementation BridgeExperimentTests

- (void)setUp {
    // Put setup code here. This method is called before the invocation of each test method in the class.
}

- (void)tearDown {
    // Put teardown code here. This method is called after the invocation of each test method in the class.
}

- (void)testExample {
    // This is an example of a functional test case.
    // Use XCTAssert and related functions to verify your tests produce the correct results.
//    char *message = "{ \"object\" : \"0x324234324\" , \"op\" : \"getNumber\" }";
    
    TestJSBinding *test = new TestJSBinding();
    
    JSInvokeMessage message1 = { reinterpret_cast<uintptr_t>(test), "setNumber" };
    JSBridge::getInstance().recive(message1);
    
    JSInvokeMessage message2 = { reinterpret_cast<uintptr_t>(test), "getNumber" };
    JSBridge::getInstance().recive(message2);
    
    delete test;
}


//- (void)testExample {
//    // This is an example of a functional test case.
//    // Use XCTAssert and related functions to verify your tests produce the correct results.
//
//    JSBridge::getInstance().recive("{ \"0x324234324\" : { \"op\" : \"getNumber\" }  }");
//}


- (void)testPerformanceExample {
    // This is an example of a performance test case.
    [self measureBlock:^{
        // Put the code you want to measure the time of here.
    }];
}

@end
