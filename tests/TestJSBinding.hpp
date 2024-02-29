//
//  TestJSBinding.hpp
//  BridgeExperiment
//
//  Created by Volodymyr Boichentsov on 29/12/2021.
//

#ifndef TestJSBinding_hpp
#define TestJSBinding_hpp

#include <stdio.h>
#include <string>



class TestJSBinding  {
public:
    
    TestJSBinding() {
//        i = num;
        printf("ctor TestJSBinding\n");
    }
    
    ~TestJSBinding() {
        printf("dtor TestJSBinding\n");
    }
    
    const int getNumber() {
        return i;
    }
    
    std::shared_ptr<TestJSBinding> getObject() {
        return std::make_shared<TestJSBinding>();
    }
    
    void setObject(std::shared_ptr<TestJSBinding> obj) {
        
    }
    
    void setNumber2(int newi, int newi2) {
        i = newi + newi2;
    }
    
    void setNumber(int newi) {
        i = newi;
    }
    void setNumberf(float newi) {
        i = newi;
    }
    void setNumberd(double newi) {
        i = newi;
    }
    void setString(std::string string) {
        _str = string;
    }
    
    void forward(int&& newi) {
        i = newi;
    }
    void forwardf(float&& newi) {
        i = newi;
    }
    void forwardd(double&& newi) {
        i = newi;
    }
    
    void copyFrom(TestJSBinding* obj) {
        i = obj->i;
    }
    
    static int randomNumber(int modifier) {
        return rand() % modifier;
    }
    
    void* voidPtr() {
        return &i;
    }
    
    std::string string() {
        
        std::string test_text = R"(The first web browser with a graphical user interface, Mosaic, was released in 1993. Accessible to non-technical people, it played a prominent role in the rapid growth of the nascent World Wide Web.[15] The lead developers of Mosaic then founded the Netscape corporation, which released a more polished browser, Netscape Navigator, in 1994. This quickly became the most-used.[16][17]
        "test \" s `§1  "
  During these formative years of the Web, web pages could only be static, lacking the capability for dynamic behavior after the page was loaded in the browser. There was a desire in the burgeoning web development scene to remove this limitation, so in 1995, Netscape decided to add a scripting language to Navigator. They pursued two routes to achieve this: collaborating with Sun Microsystems to embed the Java programming language, while also hiring Brendan Eich to embed the Scheme language.[6]

  Netscape management soon decided that the best option was for Eich to devise a new language, with syntax similar to Java and less like Scheme or other extant scripting languages.[5][6] Although the new language and its interpreter implementation were called LiveScript when first shipped as part of a Navigator beta in September 1995, the name was changed to JavaScript for the official release in December.[6][1][18]

  The choice of the JavaScript name has caused confusion, implying that it is directly related to Java. At the time, the dot-com boom had begun and Java was the hot new language, so Eich considered the JavaScript name a marketing ploy by Netscape)";
        
        return test_text;
    }
    
    char* string2() {
        return r;
    }
    
    void const_func() const {
        printf("const_func\n");
    }
    
    std::string _str;
    char r[10] = "string\0";
    int i = 0;
};


static void setString2(TestJSBinding& object, std::string string) {
    object._str = string;
}

static std::string getString2(TestJSBinding& object) {
    return object._str;
}

#endif /* TestJSBinding_hpp */
