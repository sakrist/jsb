//
//  MessageDescriptor.hpp
//  BridgeExperiment
//
//  Created by Volodymyr Boichentsov on 28/01/2023.
//

#ifndef JSMessageDescriptor_hpp
#define JSMessageDescriptor_hpp

#include <stdio.h>
#include "val.hpp"
#include "Helpers.hpp"
#include "Bridge.hpp"

namespace jsb {


struct MessageDescriptor {
    uintptr_t object;
    std::string class_id;
    std::string function;
    
    // for async
    std::string callback;
    std::string callback_id;
    
    // sync
    std::function<void(const char*)> completion{ nullptr };
    
    val args[5];
};


}

#endif /* JSMessageDescriptor_hpp */
