/* Copyright (c) 2017 Big Ladder Software LLC. All rights reserved.
 * See the LICENSE file for additional terms and conditions. */

// Standard
#include <vector>
#include <array>

// Penumbra
#include <penumbra/surface.h>
#include <surface-private.h>
#include <penumbra/logging.h>

namespace Pumbra {

/*
void* stdAlloc(void* userData, unsigned size)
{
        int* allocated = ( int*)userData;
        TESS_NOTUSED(userData);
        *allocated += (int)size;
        return malloc(size);
}

void stdFree(void* userData, void* ptr)
{
        TESS_NOTUSED(userData);
        free(ptr);
}
*/

TessData::TessData(const float *array, unsigned numVerts) : numVerts(numVerts) {
  vertices.insert(vertices.end(), (const float *)array, (const float *)array + numVerts);
}

Surface::Surface() {
  surface = std::make_shared<SurfacePrivate>();
}

Surface::Surface(const Polygon &polygon, const std::string &name_in) {
  surface = std::make_shared<SurfacePrivate>(polygon);
  surface->name = name_in;
}

Surface::Surface(const Surface &surface_in) {
  surface = surface_in.surface;
}

Surface::~Surface() = default;

void Surface::addHole(const Polygon &hole) {
  surface->holes.push_back(hole);
}

SurfacePrivate::SurfacePrivate(Polygon polygon) : polygon(std::move(polygon)) {}

TessData SurfacePrivate::tessellate() {
  TESStesselator *tess = tessNewTess(nullptr);

  if (!tess) {
    throw PenumbraException(fmt::format("Unable to create tessellator for surface, \"{}\".", name),
                            *logger);
  }

  // Add primary polygon
  tessAddContour(tess, TessData::polySize, &polygon[0], sizeof(float) * TessData::vertexSize,
                 (int)polygon.size() / TessData::vertexSize);

  // Add holes
  for (auto &hole : holes) {
    tessAddContour(tess, TessData::polySize, &hole[0], sizeof(float) * TessData::vertexSize,
                   (int)hole.size() / TessData::vertexSize);
  }

  if (!tessTesselate(tess, TESS_WINDING_ODD, TESS_POLYGONS, TessData::polySize,
                     TessData::vertexSize, nullptr)) {
    throw PenumbraException(fmt::format("Unable to tessellate surface, \"{}\".", name), *logger);
  }

  // For now convert to glDrawArrays() style of vertices, sometime may change to glDrawElements
  // (with element buffers)
  std::vector<float> vertexArray;
  const TESSreal *verts = tessGetVertices(tess);
  const int nelems = tessGetElementCount(tess);
  const TESSindex *elems = tessGetElements(tess);
  for (int i = 0; i < nelems * TessData::polySize; ++i) {
    const int vert = *(elems + i);
    for (int j = 0; j < TessData::vertexSize; ++j) {
      vertexArray.push_back(verts[vert * TessData::vertexSize + j]);
    }
  }

  TessData data(&vertexArray[0], static_cast<unsigned int>(vertexArray.size()));

  tessDeleteTess(tess);

  return data;
}

} // namespace Pumbra
