/* Copyright (c) 2017 Big Ladder Software and Chip Barnaby. All rights reserved.
 * See the LICENSE file for additional terms and conditions. */

// Standard
#include <iostream>

// Penumbra
#include <penumbra/penumbra.h>
#include <fakestate.h>

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

int run_main() {

    fakestate state;

  Pumbra::Polygon wallVerts = {0.f, 0.f, 0.f, 1.f, 0.f, 0.f, 1.f, 0.f, 1.f, 0.f, 0.f, 1.f};

  Pumbra::Polygon windowVerts = {0.25f, 0.f, 0.25f, 0.75f, 0.f, 0.25f,
                                 0.75f, 0.f, 0.5f,  0.25f, 0.f, 0.5f};

  Pumbra::Polygon awningVerts = {0.25f, 0.f,   0.5f, 0.75f, 0.f,   0.5f,
                                 0.75f, -0.5f, 0.5f, 0.25f, -0.5f, 0.5f};

  Pumbra::Surface wall(wallVerts);
  wall.addHole(windowVerts);

  Pumbra::Surface window(windowVerts);
  Pumbra::Surface awning(awningVerts);

  unsigned wallId = state.pumbra->addSurface(wall);
  unsigned windowId = state.pumbra->addSurface(window);
  unsigned awningId = state.pumbra->addSurface(awning);

  state.pumbra->setModel();
  state.pumbra->setSunPosition(2.50f, 0.3f);
  // state.pumbra.setSunPosition(3.14f, 0.0f);
  state.pumbra->renderScene(wallId);
  float wallPSSA = state.pumbra->calculatePSSA(wallId);

  std::cout << "Wall PSSA: " << wallPSSA << std::endl;

  state.pumbra->renderScene(windowId);
  float windowPSSA = state.pumbra->calculatePSSA(windowId);

  std::cout << "Window PSSA: " << windowPSSA << std::endl;

  state.pumbra->clearModel();

  Pumbra::Polygon finVerts = {0.75f, -0.25f, 0.5f,  0.75f, -0.25f, 0.25f,
                              0.75f, 0.0f,   0.25f, 0.75f, 0.0f,   0.5f};

  Pumbra::Surface fin(finVerts);

  windowId = state.pumbra->addSurface(window);
  awningId = state.pumbra->addSurface(awning);
  /*unsigned finID = */ state.pumbra->addSurface(fin);

  state.pumbra->setModel();

  state.pumbra->renderScene(windowId);
  windowPSSA = state.pumbra->calculatePSSA(windowId);

  std::cout << "Window PSSA with fin: " << windowPSSA << std::endl;

  return 0;
}

int main(void)
{
    return run_main();
}