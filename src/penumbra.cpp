/* Copyright (c) 2017 Big Ladder Software LLC. All rights reserved.
 * See the LICENSE file for additional terms and conditions. */

// Standard
#include <memory>
#include <iostream>

#ifndef NDEBUG
#ifdef __unix__
#include <cfenv>
#endif
#endif

// Penumbra
#include <penumbra/penumbra.h>
#include <penumbra-implementation.h>

namespace Pumbra {

Penumbra::Penumbra(unsigned int size, const std::shared_ptr<Courierr::Courierr> &logger)
    : penumbra(std::make_unique<PenumbraImplementation>(static_cast<int>(size), logger)) {}

Penumbra::Penumbra(const std::shared_ptr<Courierr::Courierr> &logger)
    : penumbra(std::make_unique<PenumbraImplementation>(512, logger)) {}

Penumbra::~Penumbra() = default;

bool Penumbra::is_valid_context() {
  bool invalid(false);
  if (!glfwInit()) {
    invalid = true;
  }

  glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 2);
  glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 1);
  glfwWindowHint(GLFW_VISIBLE, GL_FALSE);
#ifndef NDEBUG
#ifdef __unix__
  // Temporarily Disable floating point exceptions
  fedisableexcept(FE_DIVBYZERO | FE_INVALID | FE_OVERFLOW);
#endif
#endif
  GLFWwindow *window = glfwCreateWindow(1, 1, "Penumbra", nullptr, nullptr);
#ifndef NDEBUG
#ifdef __unix__
  feenableexcept(FE_DIVBYZERO | FE_INVALID | FE_OVERFLOW);
#endif
#endif
  glfwMakeContextCurrent(window);
  invalid |= !window;
  glfwDestroyWindow(window);
  return !invalid;
}

VendorType Penumbra::get_vendor_name() {
  VendorType vendor_type;
  auto vendor_name = Context::vendorName();
  if (vendor_name == "NVIDIA") {
    vendor_type = VendorType::nvidia;
  } else if (vendor_name == "AMD" || vendor_name == "ATI" ||
             vendor_name == "Advanced Micro Devices" || vendor_name == "ATI Technologies Inc.") {
    vendor_type = VendorType::amd;
  } else if (vendor_name == "Intel" || vendor_name == "INTEL" || vendor_name == "Intel Inc.") {
    vendor_type = VendorType::intel;
  } else if (vendor_name == "VMware, Inc.") {
    vendor_type = VendorType::vmware;
  } else if (vendor_name == "Mesa" || vendor_name == "Mesa/X.org") {
    vendor_type = VendorType::mesa;
  } else {
    throw PenumbraException(
        fmt::format("Failed to find GPU or vendor name ({}) is not in list.", vendor_name),
        *(penumbra->logger));
  }
  return vendor_type;
}

unsigned Penumbra::add_surface(const Surface &surface) {
  penumbra->add_surface(surface);
  return static_cast<unsigned int>(penumbra->surfaces.size()) - 1u;
}

unsigned Penumbra::get_number_of_surfaces() {
  return static_cast<unsigned int>(penumbra->surfaces.size());
}

void Penumbra::set_model() {
  if (!penumbra->surfaces.empty()) {

    // Tessellate each surface into triangles
    std::vector<SurfaceBuffer> surface_buffers;
    unsigned next_starting_index = 0;
    unsigned surface_index = 0;
    for (auto &surface : penumbra->surfaces) {
      TessData tess = surface.tessellate();
      surface_buffers.emplace_back(next_starting_index / TessData::vertex_size,
                                   tess.number_of_vertices / TessData::vertex_size, surface_index);
      for (unsigned i = 0; i < tess.number_of_vertices; ++i) {
        penumbra->model.push_back(tess.vertices[i]);
      }
      next_starting_index += tess.number_of_vertices;
      ++surface_index;
    }
    penumbra->context.set_model(penumbra->model, surface_buffers);
  } else {
    penumbra->logger->warning("No surfaces added to Penumbra before calling set_model().");
  }
}

void Penumbra::clear_model() {
  penumbra->surfaces.clear();
  penumbra->model.clear();
  penumbra->context.clear_model();
}

void Penumbra::set_sun_position(const float azimuth, // in radians, clockwise, north = 0
                                const float altitude // in radians, horizon = 0, vertical = pi/2
) {
  penumbra->sun.set_view(azimuth, altitude);
}

float Penumbra::get_sun_azimuth() {
  return penumbra->sun.get_azimuth();
}

float Penumbra::get_sun_altitude() {
  return penumbra->sun.get_altitude();
}

void Penumbra::submit_pssa(unsigned surface_index) {
  if (penumbra->check_surface(surface_index)) {
    penumbra->context.submit_pssa(surface_index, penumbra->sun.get_view());
  } else {
    throw PenumbraException(
        fmt::format("Surface index, {}, does not exist. Cannot calculate PSSA.", surface_index),
        *(penumbra->logger));
  }
}

void Penumbra::submit_pssa(const std::vector<unsigned> &surface_indices) {
  for (auto const surface_index : surface_indices) {
    if (!penumbra->check_surface(surface_index)) {
      throw PenumbraException(
          fmt::format("Surface index, {}, does not exist. Cannot calculate PSSA.", surface_index),
          *(penumbra->logger));
    }
  }
  penumbra->context.submit_pssa(surface_indices, penumbra->sun.get_view());
}

void Penumbra::submit_pssa() {
  penumbra->context.submit_pssa(penumbra->sun.get_view());
}

float Penumbra::fetch_pssa(unsigned surface_index) {
  if (penumbra->check_surface(surface_index)) {
    return penumbra->context.calculate_pssa(surface_index);
  } else {
    throw PenumbraException(
        fmt::format("Surface index, {}, does not exist. Cannot calculate PSSA.", surface_index),
        *(penumbra->logger));
  }
}

std::vector<float> Penumbra::fetch_pssa(const std::vector<unsigned> &surface_indices) {
  for (auto const surface_index : surface_indices) {
    if (!penumbra->check_surface(surface_index)) {
      throw PenumbraException(
          fmt::format("Surface index, {}, does not exist. Cannot calculate PSSA.", surface_index),
          *(penumbra->logger));
    }
  }
  return penumbra->context.calculate_pssa(surface_indices);
}

std::vector<float> Penumbra::fetch_pssa() {
  return penumbra->context.calculate_pssa();
}

float Penumbra::calculate_pssa(unsigned surface_index) {
  submit_pssa(surface_index);
  return fetch_pssa(surface_index);
}

std::vector<float> Penumbra::calculate_pssa(const std::vector<unsigned> &surface_indices) {
  submit_pssa(surface_indices);
  return fetch_pssa(surface_indices);
}

std::vector<float> Penumbra::calculate_pssa() {
  submit_pssa();
  return fetch_pssa();
}

std::unordered_map<unsigned, float>
Penumbra::calculate_interior_pssas(const std::vector<unsigned> &transparent_surface_indices,
                                   const std::vector<unsigned> &interior_surface_indices) {
  std::unordered_map<unsigned, float> pssas;
  if (!transparent_surface_indices.empty()) {
    if (penumbra->check_surface(transparent_surface_indices[0])) {
      for (auto const transparent_surface_index : transparent_surface_indices) {
        if (!penumbra->check_surface(transparent_surface_index)) {
          throw PenumbraException(
              fmt::format("Transparent surface index, {}, does not exist. Cannot calculate PSSA.",
                          transparent_surface_index),
              *(penumbra->logger));
        }
      }
      for (auto const interior_surface_index : interior_surface_indices) {
        if (!penumbra->check_surface(interior_surface_index)) {
          throw PenumbraException(
              fmt::format("Interior surface index, {}, does not exist. Cannot calculate PSSA.",
                          interior_surface_index),
              *(penumbra->logger));
        }
      }
      pssas = penumbra->context.calculate_interior_pssas(
          transparent_surface_indices, interior_surface_indices, penumbra->sun.get_view());
    } else {
      throw PenumbraException(
          fmt::format("Transparent surface index, {}, does not exist. Cannot calculate PSSA.",
                      transparent_surface_indices[0]),
          *(penumbra->logger));
    }
  } else {
    throw PenumbraException(
        "Cannot calculate interior PSSAs without defining at least one transparent surface index.",
        *(penumbra->logger));
  }
  return pssas;
}

void Penumbra::render_scene(unsigned surface_index) {
  if (penumbra->check_surface(surface_index)) {
    penumbra->context.show_rendering(surface_index, penumbra->sun.get_view());
  } else {
    throw PenumbraException(
        fmt::format("Surface index, {}, does not exist. Cannot render scene.", surface_index),
        *(penumbra->logger));
  }
}

void Penumbra::render_interior_scene(std::vector<unsigned> transparent_surface_indices,
                                     std::vector<unsigned> interior_surface_indices) {
  if (!transparent_surface_indices.empty()) {
    if (penumbra->check_surface(transparent_surface_indices[0])) {
      for (auto &transparent_surface_index : transparent_surface_indices) {
        if (!penumbra->check_surface(transparent_surface_index)) {
          throw PenumbraException(
              fmt::format("Transparent surface index, {}, does not exist. Cannot render scene.",
                          transparent_surface_index),
              *(penumbra->logger));
        }
      }
      for (auto &interior_surface_index : interior_surface_indices) {
        if (penumbra->check_surface(interior_surface_index)) {
          penumbra->context.showInteriorRendering(transparent_surface_indices,
                                                  interior_surface_index, penumbra->sun.get_view());
        } else {
          throw PenumbraException(
              fmt::format("Interior surface index, {}, does not exist. Cannot render scene.",
                          interior_surface_index),
              *(penumbra->logger));
        }
      }
    } else {
      throw PenumbraException(
          fmt::format("Transparent surface index, {}, does not exist. Cannot render scene.",
                      transparent_surface_indices[0]),
          *(penumbra->logger));
    }
  } else {
    throw PenumbraException("Cannot render interior scene without defining at least one "
                            "transparent surface index.",
                            *(penumbra->logger));
  }
}
std::shared_ptr<Courierr::Courierr> Penumbra::get_logger() {
  return penumbra->logger;
}

PenumbraImplementation::PenumbraImplementation(int size,
                                               const std::shared_ptr<Courierr::Courierr> &logger_in)
    : context(size, logger_in.get()), logger(logger_in) {}

void PenumbraImplementation::add_surface(const Surface &surface) {
  surface.surface->logger = logger;
  if (surface.surface->name.empty()) {
    surface.surface->name = fmt::format("Surface {}", surfaces.size());
  }
  surfaces.push_back(*surface.surface);
}

bool PenumbraImplementation::check_surface(const unsigned index) const {
  return index < surfaces.size();
}

} // namespace Pumbra
