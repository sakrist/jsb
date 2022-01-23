//
//  JSBridge.cpp
//  BridgeExperiment
//
//  Created by Volodymyr Boichentsov on 25/12/2021.
//

#include "JSBridge.hpp"


namespace jsbridge {

JSBridge& JSBridge::getInstance() {
    static JSBridge instance;
    return instance;
}

}
