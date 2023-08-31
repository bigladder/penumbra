/* Copyright (c) 2017 Big Ladder Software and Chip Barnaby. All rights reserved.
 * See the LICENSE file for additional terms and conditions. */

// Standard
#include <iostream>

// Penumbra
#include <penumbra/penumbra.h>

class AwningLogger : public Penumbra::PenumbraLogger {
public:
  void info(const std::string_view message) override {
    write_message("Info", message);
  }
};

int main() {

  Penumbra::Surface wall({0.f, 0.f, 0.f, 1.f, 0.f, 0.f, 1.f, 0.f, 1.f, 0.f, 0.f, 1.f}, "Wall");

  Penumbra::Polygon window_vertices = {0.25f, 0.f, 0.25f, 0.75f, 0.f, 0.25f,
                                       0.75f, 0.f, 0.5f,  0.25f, 0.f, 0.5f};
  Penumbra::Surface window(window_vertices, "Window");
  wall.add_hole(window_vertices);

  Penumbra::Surface awning(
      {0.25f, 0.f, 0.5f, 0.75f, 0.f, 0.5f, 0.75f, -0.5f, 0.5f, 0.25f, -0.5f, 0.5f}, "Awning");

  Penumbra::Penumbra::is_valid_context();

  std::shared_ptr<AwningLogger> logger = std::make_shared<AwningLogger>();
  Penumbra::Penumbra penumbra(512u, logger);

  unsigned wall_id = penumbra.add_surface(wall);
  unsigned window_id = penumbra.add_surface(window);
  [[maybe_unused]] auto awning_id = penumbra.add_surface(awning);

  penumbra.set_model();
  penumbra.set_sun_position(2.50f, 0.3f);
  // penumbra.set_sun_position(3.14f, 0.0f);
  penumbra.render_scene(wall_id);

  logger->info(fmt::format("Wall PSSA: {}", penumbra.calculate_pssa(wall_id)));

  penumbra.render_scene(window_id);

  logger->info(fmt::format("Window PSSA: {}", penumbra.calculate_pssa(window_id)));

  penumbra.clear_model();

  Penumbra::Surface fin(
      {0.75f, -0.25f, 0.5f, 0.75f, -0.25f, 0.25f, 0.75f, 0.0f, 0.25f, 0.75f, 0.0f, 0.5f}, "Fin");

  window_id = penumbra.add_surface(window);
  penumbra.add_surface(awning);
  [[maybe_unused]] auto finID = penumbra.add_surface(fin);

  penumbra.set_model();

  penumbra.render_scene(window_id);

  logger->info(fmt::format("Window PSSA with fin: {}", penumbra.calculate_pssa(window_id)));

  return 0;
}
