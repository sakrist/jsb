//
//  class__Tests.cpp
//  BridgeExperimentTests
//
//  Created by Volodymyr Boichentsov on 04/01/2022.
//

#include "gtest/gtest.h"
#include "json.hpp"
using json = nlohmann::json;

#include "JSBridge.hpp"

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

TEST(class_Tests, simple_set_and_get) {
    class_<TestSimple>("TestSimple")
    .function("setNumber", &TestSimple::setNumber)
    .function("getNumber", &TestSimple::getNumber);
    
    auto test = std::make_shared<TestSimple>();
    uintptr_t ptrObject = reinterpret_cast<uintptr_t>(test.get());
    
    JSMessageDescriptor message1;
    message1.object = ptrObject;
    message1.class_id = "TestSimple";
    message1.function = "setNumber";
    message1.args[0] = 2;
    JSBridge::getInstance().recive(message1);
    
    JSMessageDescriptor message2;
    message2.object = ptrObject;
    message2.class_id = "TestSimple";
    message2.function = "getNumber";
    message2.completion = [=](const char* result){ 
        
        json obj = json::parse(result);
        auto r = obj.get<int>();
        ASSERT_TRUE(test->number == r);
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
    char * getCharPtr() {
        return string;
    }
    
    std::string str0() {
        return str;
    }
    std::string& str1() {
        return str;
    }
    std::string* str2() {
        return &str;
    }
    
    std::string str {"hello"};
    float fnumber = 0;
    int number = 0;
    char string[10] = "test\0";
    static void runTest() {}
    
    static double getDoubleMax() { return std::numeric_limits<double>::max(); }
    static int getIntMax() { return std::numeric_limits<int>::max(); }
};


TEST(class_Tests, when_function_not_from_class) {
    // TODO: should not compile
    class_<TestCompilation>("TestCompilation0")
    .function("setNumber", &TestSimple::setNumber)
    .function("getNumber", &TestSimple::getNumber);
}

TEST(class_Tests, static_functions_compile) {
    class_<TestCompilation>("TestCompilation1")
    .function("setNumber", &TestCompilation::setNumber)
    .function("getNumber", &TestCompilation::getNumber)
    .class_function("runTest", &TestCompilation::runTest)
    .class_function("getDoubleMax", &TestCompilation::getDoubleMax)
    .class_function("getIntMax", &TestCompilation::getIntMax);
}

TEST(class_Tests, redefinition) {
    try {
        class_<TestCompilation>("TestCompilation1");
        ASSERT_TRUE(false);
    } catch (const std::logic_error& error) {
        printf("%s", error.what());
        ASSERT_TRUE(true);
    }
}


TEST(class_Tests, char_pointer) {
    class_<TestCompilation>("TestCompilation")
    .function("getCharPtr", &TestCompilation::getCharPtr);
}


TEST(class_Tests, string) {
    class_<TestCompilation>("TestCompilation3")
    .function("str0", &TestCompilation::str0)
//    .function("str1", &TestCompilation::str1) // TODO: must be compilable
    .function("str2", &TestCompilation::str2)
    ;
}


class SpeedTest {
public:    
    void fromjs(long time) {
        std::cout << "got milliseconds from js: "
                  << time << '\n';
        
        const auto p1 = std::chrono::system_clock::now();
     
        std::cout << "milliseconds since epoch: "
                  << std::chrono::duration_cast<std::chrono::milliseconds>(
                       p1.time_since_epoch()).count() << '\n';
    }
};

TEST(class_Tests, speedtest) {
    jsbridge::class_<SpeedTest>("SpeedTest")
    .constructor<>()
    .function("fromjs", &SpeedTest::fromjs)
    .registerJS();

    std::string code = "var st = new SpeedTest(); st.fromjs(Date.now());";
//    JSBridge::eval(code.c_str());
}


TEST(JSBridge_eval, simple) {
    std::string code = "(function foo() {  })()";
    JSBridge::eval(code.c_str());
}

TEST(JSBridge_eval, return_text_no_block) {
    std::string code = "(function foo() { return \"text space\"; })()";
    JSBridge::eval(code.c_str());
}

TEST(JSBridge_eval, return_text_with_block) {
    std::string code = "(function foo() { return \"text space\"; })()";
    JSBridge::eval(code.c_str(), [](const char* msg){
        ASSERT_TRUE(strcmp(msg, "text space") == 0);
    });
}


TEST(JSBridge_eval, return_json) {
    std::string code = "(function foo() { return \"{ \\\"msg\\\": 4567 }\"; })()";
    JSBridge::eval(code.c_str(), [](const char* msg){
        json obj = json::parse(msg);
        int val = obj["msg"].get<int>(); 
        ASSERT_EQ(val, 4567);
    });
}

TEST(JSBridge_eval, async) {
    bool hit = false;
    std::string code = "(function foo() { return \"1\"; })()";
    JSBridge::eval(code.c_str(), [&hit](const char* msg){
        hit = true;
    });
    ASSERT_FALSE(hit);
}


//TEST(JSBridge_eval, return_object) {
//    bool hit = false;
//    std::string code = "(function foo() { return new Object(); })()";
//    JSBridge::eval(code.c_str(), [&hit](const char* msg){
//        hit = true;
//    });
//    
//}
