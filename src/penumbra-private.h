/* Copyright (c) 2017 Big Ladder Software LLC. All rights reserved.
 * See the LICENSE file for additional terms and conditions. */

#ifndef PENUMBRA_PRIVATE_H_
#define PENUMBRA_PRIVATE_H_

// Standard
#include <memory>

// vendor
#include <courierr/courierr.h>

// Penumbra
#include <penumbra/penumbra.h>
#include <penumbra/surface.h>
#include <surface-private.h>
#include <gl/context.h>

namespace Pumbra {

class PenumbraPrivate {

public:
  PenumbraPrivate(int size, const std::shared_ptr<Courierr::Courierr> &logger);
  ~PenumbraPrivate() = default;

public:
  void add_surface(const Surface &surface);
  Context context;
  Sun sun;
  std::vector<float> model;
  std::vector<SurfacePrivate> surfaces;
  std::shared_ptr<Courierr::Courierr> logger;
  [[nodiscard]] bool check_surface(unsigned index) const;
};

} // namespace Pumbra
#endif // PENUMBRA_PRIVATE_H_
