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

//- (void)test1
//{
//    XCTestExpectation *expectation = [self expectationWithDescription:@"Handler called"];
//
//    [self waitForExpectationsWithTimeout:1.0 handler:nil];
//}

- (void)setUp {
    [super setUp];

    NSString *description = [NSString stringWithFormat:@"%s%d", __FUNCTION__, __LINE__];
    XCTestExpectation *exp = [self expectationWithDescription:description];
    [self performSelector:@selector(full:) withObject:exp afterDelay:1];
    
    // Wait for the async request to complete
    [self waitForExpectationsWithTimeout:3 handler: nil];
}

- (void)full:(XCTestExpectation *) exp {
    [exp fulfill];
}

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
