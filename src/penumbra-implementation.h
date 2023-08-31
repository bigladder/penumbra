/* Copyright (c) 2017 Big Ladder Software LLC. All rights reserved.
 * See the LICENSE file for additional terms and conditions. */

#ifndef PENUMBRA_IMPLEMENTATION_H_
#define PENUMBRA_IMPLEMENTATION_H_

// Standard
#include <memory>

// vendor
#include <courierr/courierr.h>

// Penumbra
#include <penumbra/penumbra.h>
#include <penumbra/surface.h>
#include <surface-implementation.h>
#include <gl/context.h>

namespace Pumbra {

class PenumbraImplementation {

public:
  PenumbraImplementation(int size, const std::shared_ptr<Courierr::Courierr> &logger);
  ~PenumbraImplementation() = default;

public:
  void add_surface(const Surface &surface);
  Context context;
  Sun sun;
  std::vector<float> model;
  std::vector<SurfaceImplementation> surfaces;
  std::shared_ptr<Courierr::Courierr> logger;
  [[nodiscard]] bool check_surface(unsigned index) const;
};

} // namespace Pumbra
#endif // PENUMBRA_IMPLEMENTATION_H_
