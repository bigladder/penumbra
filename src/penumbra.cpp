/* Copyright (c) 2017 Big Ladder Software LLC. All rights reserved.
* See the LICENSE file for additional terms and conditions. */

// Standard
#include <memory>
#include <iostream>

// Penumbra
#include <penumbra/penumbra.h>
#include <penumbra-private.h>

namespace Pumbra{

// TODO also catch exceptions

Penumbra::Penumbra()
{
  penumbra = std::unique_ptr<PenumbraPrivate>(new PenumbraPrivate);
}

Penumbra::~Penumbra(){}

unsigned Penumbra::addSurface(const Surface& surface)
{
  penumbra->addSurface(surface);
  return penumbra->surfaceCounter++;
}

int Penumbra::setModel()
{
  const int vertexSize = 3;
  unsigned nextStartingIndex = 0;
  for (auto& surface : penumbra->surfaces ) {
    TessData tess = surface.tessellate();
    penumbra->surfaceBuffers.push_back({nextStartingIndex/vertexSize,tess.numVerts/vertexSize});
    for (unsigned i = 0; i < tess.numVerts; ++i) {
      penumbra->model.push_back(tess.vertices[i]);
    }
    nextStartingIndex += tess.numVerts;
  }

  penumbra->context.setModel(penumbra->model);
  return PN_SUCCESS;
}

int Penumbra::setSunPosition(
  const float azm, // in radians, clockwise, north = 0
  const float alt  // in radians, horizon = 0, vertical = pi/2
)
{
  penumbra->sun.setView(azm, alt);
  return PN_SUCCESS;
}

float Penumbra::calculatePSSF(unsigned surfaceIndex)
{
  penumbra->context.setScene(
    penumbra->surfaceBuffers[surfaceIndex].first,
    penumbra->surfaceBuffers[surfaceIndex].second,
    penumbra->sun.getView()
  );

  return penumbra->context.calculatePSSF(
    penumbra->surfaceBuffers[surfaceIndex].first,
    penumbra->surfaceBuffers[surfaceIndex].second
  );
}

int Penumbra::renderScene(unsigned surfaceIndex)
{

  penumbra->context.setScene(
    penumbra->surfaceBuffers[surfaceIndex].first,
    penumbra->surfaceBuffers[surfaceIndex].second,
    penumbra->sun.getView()
  );
  penumbra->context.showRendering(
    penumbra->surfaceBuffers[surfaceIndex].first,
    penumbra->surfaceBuffers[surfaceIndex].second
  );
  return PN_SUCCESS;
}

void Penumbra::setMessageCallback(
  PenumbraCallbackFunction callBackFunction,
  void* contextPtr
)
{
  penumbra->callBackFunction = callBackFunction;
  penumbra->messageCallbackContextPtr = contextPtr;
}

PenumbraPrivate::PenumbraPrivate() :
  surfaceCounter(0)
{}

PenumbraPrivate::~PenumbraPrivate(){}

void PenumbraPrivate::addSurface(const Surface& surface)
{
  surfaces.push_back(*surface.surface);
}

void PenumbraPrivate::sayMessage(
  const int messageType,
  const std::string message
) const
{
  if (callBackFunction != NULL) {
    (*callBackFunction)(messageType, message, messageCallbackContextPtr);
  } else {
    if (messageType == MSG_ERR) {
      std::cerr << "Error: " << message << std::endl;
      exit(EXIT_FAILURE);
    } else if (messageType == MSG_WARN) {
      std::cerr << "Warning: " << message << std::endl;
    } else /*if (messageType == MSG_INFO)*/ {
      std::cout << "Note: " << message << std::endl;
    }
  }
}

}
