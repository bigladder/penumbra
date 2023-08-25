/* Copyright (c) 2017 Big Ladder Software LLC. All rights reserved.
 * See the LICENSE file for additional terms and conditions. */

// Penumbra
#include <sun.h>

namespace Pumbra {

Sun::Sun() : azimuth(0.f), altitude(0.f) {}

void Sun::setAzimuth(float azm) {
  azimuth = azm;
}

void Sun::setAltitude(float alt) {
  altitude = alt;
}

void Sun::setView(float azm, float alt) {
  setAzimuth(azm);
  setAltitude(alt);
  setView();
}

void Sun::setView() {

  float cosAlt = std::cos(altitude);

  vec3 eye = {cosAlt * std::sin(azimuth), cosAlt * std::cos(azimuth), std::sin(altitude)};
  vec3 center = {0.f, 0.f, 0.f};
  vec3 up = {0.f, 0.f, 1.f};

  mat4x4_look_at(view, eye, center, up);
}

mat4x4_ptr Sun::getView() {
  return view;
}

float Sun::getAzimuth() {
  return azimuth;
}

float Sun::getAltitude() {
  return altitude;
}
} // namespace Pumbra
