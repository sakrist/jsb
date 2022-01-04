//
//  BridgeExperimentTests.m
//  BridgeExperimentTests
//
//  Created by Volodymyr Boichentsov on 18/12/2021.
//

#import <XCTest/XCTest.h>

#include "Tests.hpp"



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
