/* Copyright (c) 2017 Big Ladder Software LLC. All rights reserved.
 * See the LICENSE file for additional terms and conditions. */

#ifndef SURFACE_H_
#define SURFACE_H_

// Standard
#include <vector>
#include <array>
#include <memory>
#include <string>

namespace Pumbra {

typedef std::vector<float> Polygon; // Implicit list of 3D coordinates

class SurfacePrivate;

class Surface {
  friend class PenumbraPrivate;

public:
  Surface();
  explicit Surface(const Polygon &polygon, const std::string &name = "");
  Surface(const Surface &surface);
  ~Surface();
  void add_hole(const Polygon &hole); // Defined in counter-clockwise order when facing front

private:
  std::shared_ptr<SurfacePrivate> surface;
};

} // namespace Pumbra
#endif // SURFACE_H_
