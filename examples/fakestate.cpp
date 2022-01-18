#include <fakestate.h>
#include <iostream>
#include <GLFW/glfw3.h>

void errorCallback(const int messageType, const std::string &message, void * /*contextPtr*/
) {
  if (messageType == Pumbra::MSG_INFO) {
    std::cout << "  NOTE: " << message << std::endl;
  } else if (messageType == Pumbra::MSG_WARN) {
    std::cout << "  WARNING: " << message << std::endl;
  } else if (messageType == Pumbra::MSG_ERR) {
    std::cout << "  ERROR: " << message << std::endl;
    exit(EXIT_FAILURE);
  }
}

fakestate::fakestate() {
    if (Pumbra::Penumbra::isValidContext()) {
        pumbra = std::make_unique<Pumbra::Penumbra>(errorCallback);
    }
}
