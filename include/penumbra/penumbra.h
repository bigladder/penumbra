/* Copyright (c) 2017 Big Ladder Software LLC. All rights reserved.
 * See the LICENSE file for additional terms and conditions. */

#ifndef PENUMBRA_H_
#define PENUMBRA_H_

// Standard
#include <memory>
#include <string>
#include <vector>
#include <array>
#include <unordered_map>

// Penumbra
#include <penumbra/surface.h>
#include <penumbra/logging.h>

namespace Pumbra {

enum class VendorType { unknown, nvidia, amd, intel, vmware, mesa };

class PenumbraPrivate;

class Penumbra {
public:
  explicit Penumbra(unsigned int size = 512u, const std::shared_ptr<Courierr::Courierr> &logger =
                                                  std::make_shared<PenumbraLogger>());

  explicit Penumbra(const std::shared_ptr<Courierr::Courierr> &logger);

  ~Penumbra();

public:
  static bool is_valid_context();
  unsigned add_surface(const Surface &surface);
  void set_model();
  void clear_model();
  void set_sun_position(float azimuth, // in radians, clockwise, north = 0
                      float altitude  // in radians, horizon = 0, vertical = pi/2
  );
  float get_sun_azimuth();
  float get_sun_altitude();
  void submit_pssa(unsigned surface_index);
  void submit_pssa(const std::vector<unsigned> &surface_indices);
  void submit_pssa();
  unsigned get_number_of_surfaces();
  float fetch_pssa(unsigned surface_index);
  std::vector<float> fetch_pssa(const std::vector<unsigned> &surface_indices);
  std::vector<float> fetch_pssa();
  float calculate_pssa(unsigned surface_index);
  std::vector<float> calculate_pssa(const std::vector<unsigned> &surface_indices);
  std::vector<float> calculate_pssa();
  std::unordered_map<unsigned, float>
  calculate_interior_pssas(const std::vector<unsigned> &transparent_surface_indices,
                         const std::vector<unsigned> &interior_surface_indices);
  void render_scene(unsigned surface_index); // Primarily for debug purposes
  void
  render_interior_scene(std::vector<unsigned> transparent_surface_indices,
      std::vector<unsigned> interior_surface_indices); // Primarily for debug purposes
  VendorType get_vendor_name();
  std::shared_ptr<Courierr::Courierr> get_logger();

private:
  std::unique_ptr<PenumbraPrivate> penumbra;
};

} // namespace Pumbra

#endif /* PENUMBRA_H_ */
