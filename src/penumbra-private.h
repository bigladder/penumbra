/* Copyright (c) 2017 Big Ladder Software LLC. All rights reserved.
* See the LICENSE file for additional terms and conditions. */

#ifndef PENUMBRA_PRIVATE_H_
#define PENUMBRA_PRIVATE_H_

// Standard
#include <memory>

// Penumbra
#include <penumbra/penumbra.h>
#include <penumbra/surface.h>
#include <surface-private.h>
#include <gl/context.h>

namespace Pumbra {

class PenumbraPrivate {

public:
  PenumbraPrivate();
  ~PenumbraPrivate();
  void addSurface(const Surface& surface);

public:
  Context context;
  Sun sun;
  PenumbraCallbackFunction callBackFunction;
  void* messageCallbackContextPtr;
  std::vector<float> model;
  unsigned surfaceCounter;
  std::vector<SurfacePrivate> surfaces;
  std::vector<std::pair<unsigned, unsigned>> surfaceBuffers; // first = Starting index, second = number of coordinates

private:
  void sayMessage(
    const int messageType,
    const std::string message
  ) const;

};


}
#endif // PENUMBRA_PRIVATE_H_
