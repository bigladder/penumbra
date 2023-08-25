/* Copyright (c) 2017 Big Ladder Software and Chip Barnaby. All rights reserved.
 * See the LICENSE file for additional terms and conditions. */

// Standard
#include <iostream>

// Penumbra
#include <penumbra/penumbra.h>

int main() {

  Pumbra::Surface wall({0.f, 0.f, 0.f, 1.f, 0.f, 0.f, 1.f, 0.f, 1.f, 0.f, 0.f, 1.f}, "Wall");

  Pumbra::Polygon window_vertices = {0.25f, 0.f, 0.25f, 0.75f, 0.f, 0.25f,
                                     0.75f, 0.f, 0.5f,  0.25f, 0.f, 0.5f};
  Pumbra::Surface window(window_vertices, "Window");
  wall.addHole(window_vertices);

  Pumbra::Surface awning(
      {0.25f, 0.f, 0.5f, 0.75f, 0.f, 0.5f, 0.75f, -0.5f, 0.5f, 0.25f, -0.5f, 0.5f}, "Awning");

  Pumbra::Penumbra::isValidContext();

  std::shared_ptr<Pumbra::PenumbraLogger> logger = std::make_shared<Pumbra::PenumbraLogger>();
  Pumbra::Penumbra pumbra(512u, logger);

  unsigned wallId = pumbra.addSurface(wall);
  unsigned windowId = pumbra.addSurface(window);
  [[maybe_unused]] auto awningId = pumbra.addSurface(awning);

  pumbra.setModel();
  pumbra.setSunPosition(2.50f, 0.3f);
  // pumbra.setSunPosition(3.14f, 0.0f);
  pumbra.renderScene(wallId);
  float wallPSSA = pumbra.calculatePSSA(wallId);

  logger->info(fmt::format("Wall PSSA: {}", wallPSSA));

  pumbra.renderScene(windowId);
  float windowPSSA = pumbra.calculatePSSA(windowId);

  logger->info(fmt::format("Window PSSA: {}", windowPSSA));

  pumbra.clearModel();

  Pumbra::Surface fin(
      {0.75f, -0.25f, 0.5f, 0.75f, -0.25f, 0.25f, 0.75f, 0.0f, 0.25f, 0.75f, 0.0f, 0.5f}, "Fin");

  windowId = pumbra.addSurface(window);
  /* awningId = */ pumbra.addSurface(awning);
  [[maybe_unused]] auto finID = pumbra.addSurface(fin);

  pumbra.setModel();

  pumbra.renderScene(windowId);
  windowPSSA = pumbra.calculatePSSA(windowId);

  logger->info(fmt::format("Window PSSA with fin: {}", windowPSSA));

  return 0;
}
