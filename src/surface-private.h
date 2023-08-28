/* Copyright (c) 2017 Big Ladder Software LLC. All rights reserved.
 * See the LICENSE file for additional terms and conditions. */

#ifndef SURFACE_PRIVATE_H_
#define SURFACE_PRIVATE_H_

// Vendor
#include <tesselator.h>
#include <courierr/courierr.h>

// Standard
#include <vector>
#include <array>

namespace Pumbra {

struct TessData {
  TessData(const float *array, unsigned numVerts);
  std::vector<float> vertices;
  unsigned numVerts;
  static const int polySize{3};   // making triangles
  static const int vertexSize{3}; // i.e., 3D
};

class SurfacePrivate {
public:
  SurfacePrivate() = default;
  explicit SurfacePrivate(Polygon polygon);
  TessData tessellate();
  Polygon polygon;
  std::vector<Polygon> holes;
  std::shared_ptr<Courierr::Courierr> logger;
  std::string name;

private:
  std::shared_ptr<float> vertPtr;
};

} // namespace Pumbra
#endif // SURFACE_PRIVATE_H_
