//
//  BridgeExperimentTests.m
//  BridgeExperimentTests
//
//  Created by Volodymyr Boichentsov on 18/12/2021.
//

#import <XCTest/XCTest.h>

#include "gtest/gtest.h"



@interface BridgeExperimentTests : XCTestCase
@end
@implementation BridgeExperimentTests

- (void)tests {
    // This is an example of a functional test case.
    // Use XCTAssert and related functions to verify your tests produce the correct results.
    
    int argc = 1;
    char* argv[argc + 1];
    argv[0] = "app";
    testing::InitGoogleTest(&argc, argv);
    int r = RUN_ALL_TESTS();
    XCTAssert(r == 0, @"Some tests are failed!");
}
@end
