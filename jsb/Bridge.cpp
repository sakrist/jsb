//
//  jsb
//  Created by Volodymyr Boichentsov on 25/12/2021.
//

#include "Bridge.hpp"


namespace jsb {

std::mutex Bridge::_mutex;

Bridge& Bridge::getInstance() {
    if (!_instance) {
        std::unique_lock<std::mutex> lock(_mutex);
        if (!_instance) {
            _instance = new (std::nothrow) Bridge();
        }
    }
    return *_instance;
}

}
