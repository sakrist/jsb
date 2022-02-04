//
//  jsb
//  Created by Volodymyr Boichentsov on 25/12/2021.
//

#include "Bridge.hpp"


namespace jsb {

Bridge& Bridge::getInstance() {
    if (!_instance) {
        _instance = new Bridge();
    }
    return *_instance;
}

}
