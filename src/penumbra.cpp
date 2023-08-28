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
#include <penumbra-private.h>

namespace Pumbra {

void penumbraTerminate() {
  glfwTerminate();
}

Penumbra::Penumbra(unsigned int size, const std::shared_ptr<Courierr::Courierr> &logger)
    : penumbra(std::make_unique<PenumbraPrivate>(static_cast<int>(size), logger)) {}

Penumbra::Penumbra(const std::shared_ptr<Courierr::Courierr> &logger)
    : penumbra(std::make_unique<PenumbraPrivate>(512, logger)) {}

Penumbra::~Penumbra() = default;

bool Penumbra::isValidContext() {
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

VendorName Penumbra::getVendorName() {
  VendorName vendorType;
  auto vendorName = Context::vendorName();
  if (vendorName == "NVIDIA") {
    vendorType = VendorName::nvidia;
  } else if (vendorName == "AMD" || vendorName == "ATI" || vendorName == "Advanced Micro Devices" ||
             vendorName == "ATI Technologies Inc.") {
    vendorType = VendorName::amd;
  } else if (vendorName == "Intel" || vendorName == "INTEL" || vendorName == "Intel Inc.") {
    vendorType = VendorName::intel;
  } else if (vendorName == "VMware, Inc.") {
    vendorType = VendorName::vmware;
  } else if (vendorName == "Mesa") {
    vendorType = VendorName::mesa;
  } else {
    throw PenumbraException(
        fmt::format("Failed to find GPU or vendor name ({}) is not in list.", vendorName),
        *(penumbra->logger));
  }
  return vendorType;
}

unsigned Penumbra::addSurface(const Surface &surface) {
  penumbra->addSurface(surface);
  return static_cast<unsigned int>(penumbra->surfaces.size()) - 1u;
}

unsigned Penumbra::getNumSurfaces() {
  return static_cast<unsigned int>(penumbra->surfaces.size());
}

void Penumbra::setModel() {
  if (!penumbra->surfaces.empty()) {

    // Tessellate each surface into triangles
    std::vector<SurfaceBuffer> surfaceBuffers;
    unsigned nextStartingIndex = 0;
    unsigned surfNum = 0;
    for (auto &surface : penumbra->surfaces) {
      TessData tess = surface.tessellate();
      surfaceBuffers.emplace_back(nextStartingIndex / TessData::vertexSize,
                                  tess.numVerts / TessData::vertexSize, surfNum);
      for (unsigned i = 0; i < tess.numVerts; ++i) {
        penumbra->model.push_back(tess.vertices[i]);
      }
      nextStartingIndex += tess.numVerts;
      ++surfNum;
    }
    penumbra->context.setModel(penumbra->model, surfaceBuffers);
  } else {
    penumbra->logger->warning("No surfaces added to Penumbra before calling setModel().");
  }
}

void Penumbra::clearModel() {
  penumbra->surfaces.clear();
  penumbra->model.clear();
  penumbra->context.clearModel();
}

void Penumbra::setSunPosition(const float azm, // in radians, clockwise, north = 0
                              const float alt  // in radians, horizon = 0, vertical = pi/2
) {
  penumbra->sun.setView(azm, alt);
}

float Penumbra::getSunAzimuth() {
  return penumbra->sun.getAzimuth();
}

float Penumbra::getSunAltitude() {
  return penumbra->sun.getAltitude();
}

void Penumbra::submitPSSA(unsigned surfaceIndex) {
  if (penumbra->checkSurface(surfaceIndex)) {
    penumbra->context.submitPSSA(surfaceIndex, penumbra->sun.getView());
  } else {
    throw PenumbraException(
        fmt::format("Surface index, {}, does not exist. Cannot calculate PSSA.", surfaceIndex),
        *(penumbra->logger));
  }
}

void Penumbra::submitPSSA(const std::vector<unsigned> &surfaceIndices) {
  for (auto const surfaceIndex : surfaceIndices) {
    if (!penumbra->checkSurface(surfaceIndex)) {
      throw PenumbraException(
          fmt::format("Surface index, {}, does not exist. Cannot calculate PSSA.", surfaceIndex),
          *(penumbra->logger));
    }
  }
  penumbra->context.submitPSSA(surfaceIndices, penumbra->sun.getView());
}

void Penumbra::submitPSSA() {
  penumbra->context.submitPSSA(penumbra->sun.getView());
}

float Penumbra::fetchPSSA(unsigned surfaceIndex) {
  if (penumbra->checkSurface(surfaceIndex)) {
    return penumbra->context.calculatePSSA(surfaceIndex);
  } else {
    throw PenumbraException(
        fmt::format("Surface index, {}, does not exist. Cannot calculate PSSA.", surfaceIndex),
        *(penumbra->logger));
  }
}

std::vector<float> Penumbra::fetchPSSA(const std::vector<unsigned> &surfaceIndices) {
  for (auto const surfaceIndex : surfaceIndices) {
    if (!penumbra->checkSurface(surfaceIndex)) {
      throw PenumbraException(
          fmt::format("Surface index, {}, does not exist. Cannot calculate PSSA.", surfaceIndex),
          *(penumbra->logger));
    }
  }
  return penumbra->context.calculatePSSA(surfaceIndices);
}

std::vector<float> Penumbra::fetchPSSA() {
  return penumbra->context.calculatePSSA();
}

float Penumbra::calculatePSSA(unsigned surfaceIndex) {
  submitPSSA(surfaceIndex);
  return fetchPSSA(surfaceIndex);
}

std::vector<float> Penumbra::calculatePSSA(const std::vector<unsigned> &surfaceIndices) {
  submitPSSA(surfaceIndices);
  return fetchPSSA(surfaceIndices);
}

std::vector<float> Penumbra::calculatePSSA() {
  submitPSSA();
  return fetchPSSA();
}

std::unordered_map<unsigned, float>
Penumbra::calculateInteriorPSSAs(const std::vector<unsigned> &transparentSurfaceIndices,
                                 const std::vector<unsigned> &interiorSurfaceIndices) {
  std::unordered_map<unsigned, float> pssas;
  if (!transparentSurfaceIndices.empty()) {
    if (penumbra->checkSurface(transparentSurfaceIndices[0])) {
      for (auto const transSurf : transparentSurfaceIndices) {
        if (!penumbra->checkSurface(transSurf)) {
          throw PenumbraException(
              fmt::format("Transparent surface index, {}, does not exist. Cannot calculate PSSA.",
                          transSurf),
              *(penumbra->logger));
        }
      }
      for (auto const intSurf : interiorSurfaceIndices) {
        if (!penumbra->checkSurface(intSurf)) {
          throw PenumbraException(
              fmt::format("Interior surface index, {}, does not exist. Cannot calculate PSSA.",
                          intSurf),
              *(penumbra->logger));
        }
      }
      pssas = penumbra->context.calculateInteriorPSSAs(
          transparentSurfaceIndices, interiorSurfaceIndices, penumbra->sun.getView());
    } else {
      throw PenumbraException(
          fmt::format("Transparent surface index, {}, does not exist. Cannot calculate PSSA.",
                      transparentSurfaceIndices[0]),
          *(penumbra->logger));
    }
  } else {
    throw PenumbraException(
        "Cannot calculate interior PSSAs without defining at least one transparent surface index.",
        *(penumbra->logger));
  }
  return pssas;
}

void Penumbra::renderScene(unsigned surfaceIndex) {
  if (penumbra->checkSurface(surfaceIndex)) {
    penumbra->context.showRendering(surfaceIndex, penumbra->sun.getView());
  } else {
    throw PenumbraException(
        fmt::format("Surface index, {}, does not exist. Cannot render scene.", surfaceIndex),
        *(penumbra->logger));
  }
}

void Penumbra::renderInteriorScene(std::vector<unsigned> transparentSurfaceIndices,
                                   std::vector<unsigned> interiorSurfaceIndices) {
  if (!transparentSurfaceIndices.empty()) {
    if (penumbra->checkSurface(transparentSurfaceIndices[0])) {
      for (auto &transSurf : transparentSurfaceIndices) {
        if (!penumbra->checkSurface(transSurf)) {
          throw PenumbraException(
              fmt::format("Transparent surface index, {}, does not exist. Cannot render scene.",
                          transSurf),
              *(penumbra->logger));
        }
      }
      for (auto &intSurf : interiorSurfaceIndices) {
        if (penumbra->checkSurface(intSurf)) {
          penumbra->context.showInteriorRendering(transparentSurfaceIndices, intSurf,
                                                  penumbra->sun.getView());
        } else {
          throw PenumbraException(
              fmt::format("Interior surface index, {}, does not exist. Cannot render scene.",
                          intSurf),
              *(penumbra->logger));
        }
      }
    } else {
      throw PenumbraException(
          fmt::format("Transparent surface index, {}, does not exist. Cannot render scene.",
                      transparentSurfaceIndices[0]),
          *(penumbra->logger));
    }
  } else {
    throw PenumbraException("Cannot render interior scene without defining at least one "
                            "transparent surface index.",
                            *(penumbra->logger));
  }
}

PenumbraPrivate::PenumbraPrivate(int size, const std::shared_ptr<Courierr::Courierr> &logger)
    : context(size, logger), logger(logger) {}

void PenumbraPrivate::addSurface(const Surface &surface) {
  surface.surface->logger = logger;
  if (surface.surface->name.empty()) {
    surface.surface->name = fmt::format("Surface {}", surfaces.size());
  }
  surfaces.push_back(*surface.surface);
}

bool PenumbraPrivate::checkSurface(const unsigned index) const {
  return index < surfaces.size();
}

} // namespace Pumbra
