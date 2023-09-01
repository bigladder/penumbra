/* Copyright (c) 2017 Big Ladder Software LLC. All rights reserved.
 * See the LICENSE file for additional terms and conditions. */

#ifndef SUN_H_
#define SUN_H_

// Standard
#include <cmath>
#include <linmath.h>

// Vendor
#include <courierr/courierr.h>

namespace Penumbra {

typedef float (*mat4x4_ptr)[4];

class Sun {
public:
  Sun();

  // Member functions
public:
  mat4x4_ptr get_view();
  void set_view(float azimuth, float altitude);
  // void set_view(mat4x4 view);
  [[nodiscard]] float get_azimuth() const;
  [[nodiscard]] float get_altitude() const;

private:
  void set_view();
  void set_azimuth(float azimuth);
  void set_altitude(float altitude);

  // Data Members
private:
  mat4x4 view = {};
  float azimuth, altitude;
};

} // namespace Penumbra

#endif // SUN_H_
