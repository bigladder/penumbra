/* Copyright (c) 2017 Big Ladder Software LLC. All rights reserved.
 * See the LICENSE file for additional terms and conditions. */

#define _USE_MATH_DEFINES
#include <cmath>

#include "gtest/gtest.h"

#include <penumbra/penumbra.h>

// Float definitions for PI from math header
constexpr float m_pi_f = static_cast<float>(M_PI);
constexpr float m_pi_2_f = static_cast<float>(M_PI_2);
constexpr float m_pi_4_f = static_cast<float>(M_PI_4);

float calculate_surface_exposure(float azimuth, float altitude) {

  // Due to the one sided nature of shaded surfaces, we will revert
  // negative values to zero, as this indicates that the opposite side
  // of the surface is shaded. This shading should be disregarded.
  auto incident_azimuth = cos(azimuth);
  if (incident_azimuth < 0) {
    incident_azimuth = 0;
  }
  auto incident_altitude = cos(altitude);
  if (incident_altitude < 0) {
    incident_altitude = 0;
  }
  return incident_azimuth * incident_altitude;
}

const std::string invalid_context_str = "A valid context could not be created. Test skipped.";

TEST(PenumbraTest, check_azimuth) {

  if (!Pumbra::Penumbra::isValidContext()) {
    GTEST_SKIP() << invalid_context_str << std::endl;
  }

  Pumbra::Penumbra pumbra;

  // Loop azimuth (with zero altitude).
  for (float azm = 0.0f; azm <= 2 * m_pi_f; azm += m_pi_4_f) {
    pumbra.setSunPosition(azm, 0.0f);
    float check_azimuth = pumbra.getSunAzimuth();
    EXPECT_FLOAT_EQ(azm, check_azimuth);
  }
}

TEST(PenumbraTest, check_altitude) {
  if (!Pumbra::Penumbra::isValidContext()) {
    GTEST_SKIP() << invalid_context_str << std::endl;
  }
  Pumbra::Penumbra pumbra;

  // Loop altitude around the axis (with zero azimuth).
  for (float alt = 0.0f; alt <= 2 * m_pi_f; alt += m_pi_4_f) {
    pumbra.setSunPosition(0.0f, alt);
    float check_altitude = pumbra.getSunAltitude();
    EXPECT_FLOAT_EQ(alt, check_altitude);
  }
}

TEST(PenumbraTest, azimuth) {
  if (!Pumbra::Penumbra::isValidContext()) {
    GTEST_SKIP() << invalid_context_str << std::endl;
  }
  Pumbra::Polygon wallVerts = {0.f, 0.f, 0.f, 1.f, 0.f, 0.f, 1.f, 0.f, 1.f, 0.f, 0.f, 1.f};
  Pumbra::Surface wall(wallVerts);
  Pumbra::Penumbra pumbra;
  unsigned wallId = pumbra.addSurface(wall);
  pumbra.setModel();
  // Loop azimuth around the surface (with zero altitude).
  for (float azm = 0.0f; azm <= 2 * m_pi_f; azm += m_pi_4_f) {
    pumbra.setSunPosition(azm, 0.0f);
    float wallPSSA = pumbra.calculatePSSA(wallId);
    EXPECT_NEAR(wallPSSA, std::abs(cos(azm)), 0.01) << "azm evaluates to " << azm;
    // pumbra.renderScene(wallId);
  }
}

TEST(PenumbraTest, interior) {
  if (!Pumbra::Penumbra::isValidContext()) {
    GTEST_SKIP() << invalid_context_str << std::endl;
  }
  float const r_W = 2., r_D = 1., r_H = 1.; // Overall dimensions
  float const w_W = 1., w_H = .5;           // Window dimensions

  Pumbra::Polygon wallFrontVerts = {
      -r_W / 2.f, r_D / 2.f, -r_H / 2.f, r_W / 2.f,  r_D / 2.f, -r_H / 2.f,
      r_W / 2.f,  r_D / 2.f, r_H / 2.f,  -r_W / 2.f, r_D / 2.f, r_H / 2.f,
  };

  Pumbra::Polygon wallFrontWindowVerts = {
      -w_W / 2.f, r_D / 2.f, -w_H / 2.f, w_W / 2.f,  r_D / 2.f, -w_H / 2.f,
      w_W / 2.f,  r_D / 2.f, w_H / 2.f,  -w_W / 2.f, r_D / 2.f, w_H / 2.f,
  };

  Pumbra::Polygon wallBackVerts = {
      r_W / 2.f,  -r_D / 2.f, -r_H / 2.f, -r_W / 2.f, -r_D / 2.f, -r_H / 2.f,
      -r_W / 2.f, -r_D / 2.f, r_H / 2.f,  r_W / 2.f,  -r_D / 2.f, r_H / 2.f,
  };

  /*	Pumbra::Polygon wallBackVerts =	//Can use this to check changing norm of back wall.
  {
            -r_W / 2.f, -r_D / 2.f, -r_H / 2.f,
            r_W / 2.f, -r_D / 2.f, -r_H / 2.f,
            r_W / 2.f, -r_D / 2.f, r_H / 2.f,
            -r_W / 2.f, -r_D / 2.f, r_H / 2.f,
          };
  */
  Pumbra::Polygon roofVerts = {
      -r_W / 2.f, r_D / 2.f,  r_H / 2.f, r_W / 2.f,  r_D / 2.f,  r_H / 2.f,
      r_W / 2.f,  -r_D / 2.f, r_H / 2.f, -r_W / 2.f, -r_D / 2.f, r_H / 2.f,
  };

  Pumbra::Polygon floorVerts = {
      -r_W / 2.f, r_D / 2.f,  -r_H / 2.f, r_W / 2.f,  r_D / 2.f,  -r_H / 2.f,
      r_W / 2.f,  -r_D / 2.f, -r_H / 2.f, -r_W / 2.f, -r_D / 2.f, -r_H / 2.f,
  };

  Pumbra::Polygon sideWallLeftVerts = {
      -r_W / 2.f, r_D / 2.f,  -r_H / 2.f, -r_W / 2.f, r_D / 2.f,  r_H / 2.f,
      -r_W / 2.f, -r_D / 2.f, r_H / 2.f,  -r_W / 2.f, -r_D / 2.f, -r_H / 2.f,
  };

  Pumbra::Polygon sideWallRightVerts = {
      r_W / 2.f, r_D / 2.f,  -r_H / 2.f, r_W / 2.f, r_D / 2.f,  r_H / 2.f,
      r_W / 2.f, -r_D / 2.f, r_H / 2.f,  r_W / 2.f, -r_D / 2.f, -r_H / 2.f,
  };

  Pumbra::Surface wallFront(wallFrontVerts);
  wallFront.addHole(wallFrontWindowVerts);

  Pumbra::Surface window(wallFrontWindowVerts);
  Pumbra::Surface wallBack(wallBackVerts);
  Pumbra::Surface roof(roofVerts);
  Pumbra::Surface floor(floorVerts);
  Pumbra::Surface sideWallLeft(sideWallLeftVerts);
  Pumbra::Surface sideWallRight(sideWallRightVerts);

  Pumbra::Penumbra pumbra2;

  unsigned wallFrontId = pumbra2.addSurface(wallFront);
  unsigned windowId = pumbra2.addSurface(window);
  unsigned wallBackId = pumbra2.addSurface(wallBack);
  /* unsigned roofId = */ pumbra2.addSurface(roof);
  unsigned floorId = pumbra2.addSurface(floor);
  unsigned sideWallLeftId = pumbra2.addSurface(sideWallLeft);
  unsigned sideWallRightId = pumbra2.addSurface(sideWallRight);

  pumbra2.setModel();
  pumbra2.setSunPosition(0.0f, 0.f);
  float wallPSSA2 = pumbra2.calculatePSSA(wallFrontId);
  EXPECT_NEAR(wallPSSA2, r_W * r_H - w_W * w_H, 0.01);
  // pumbra2.renderScene(wallFrontId);

  float backWallInteriorPSSA = pumbra2.calculateInteriorPSSAs({windowId}, {wallBackId})[wallBackId];
  EXPECT_NEAR(backWallInteriorPSSA, w_W * w_H, 0.01);
  // pumbra2.renderInteriorScene({windowId},{wallBackId});

  pumbra2.setSunPosition(3.1415f, 0.f);
  float wallPSSA3 = pumbra2.calculatePSSA(wallFrontId);
  EXPECT_NEAR(wallPSSA3, 0.f, 0.01); // WallFront should be blocked by back wall.
  // pumbra2.renderScene(wallFrontId);

  pumbra2.setSunPosition(0.5f, 0.5f);
  std::unordered_map<unsigned, float> intPSSAs = pumbra2.calculateInteriorPSSAs(
      {windowId}, {wallBackId, floorId, sideWallLeftId, sideWallRightId});
  backWallInteriorPSSA = intPSSAs[wallBackId];
  float floorInteriorPSSA = intPSSAs[floorId];
  float leftWallInteriorPSSA = intPSSAs[sideWallLeftId];
  float rightWallInteriorPSSA = intPSSAs[sideWallRightId];
  EXPECT_GT(backWallInteriorPSSA, 0.f);
  EXPECT_GT(floorInteriorPSSA, 0.f);
  EXPECT_GT(leftWallInteriorPSSA, 0.f);
  EXPECT_EQ(rightWallInteriorPSSA, 0.f);
  // pumbra2.renderInteriorScene({windowId},{wallBackId, floorId, sideWallLeftId, sideWallRightId});
}

TEST(PenumbraTest, calculatePSSA_multiple_surfaces) {
  if (!Pumbra::Penumbra::isValidContext()) {
    GTEST_SKIP() << invalid_context_str << std::endl;
  }
  // create a cube
  float const r_W = 1.f, r_D = 1.f, r_H = 1.f; // Overall dimensions

  Pumbra::Polygon wallFrontVerts = {
      -r_W / 2.f, r_D / 2.f, -r_H / 2.f, r_W / 2.f,  r_D / 2.f, -r_H / 2.f,
      r_W / 2.f,  r_D / 2.f, r_H / 2.f,  -r_W / 2.f, r_D / 2.f, r_H / 2.f,
  };

  Pumbra::Polygon wallBackVerts = {
      r_W / 2.f,  -r_D / 2.f, -r_H / 2.f, -r_W / 2.f, -r_D / 2.f, -r_H / 2.f,
      -r_W / 2.f, -r_D / 2.f, r_H / 2.f,  r_W / 2.f,  -r_D / 2.f, r_H / 2.f,
  };

  Pumbra::Polygon roofVerts = {
      -r_W / 2.f, r_D / 2.f,  r_H / 2.f, r_W / 2.f,  r_D / 2.f,  r_H / 2.f,
      r_W / 2.f,  -r_D / 2.f, r_H / 2.f, -r_W / 2.f, -r_D / 2.f, r_H / 2.f,
  };

  Pumbra::Polygon floorVerts = {
      -r_W / 2.f, r_D / 2.f,  -r_H / 2.f, r_W / 2.f,  r_D / 2.f,  -r_H / 2.f,
      r_W / 2.f,  -r_D / 2.f, -r_H / 2.f, -r_W / 2.f, -r_D / 2.f, -r_H / 2.f,
  };

  Pumbra::Polygon sideWallLeftVerts = {
      -r_W / 2.f, r_D / 2.f,  -r_H / 2.f, -r_W / 2.f, r_D / 2.f,  r_H / 2.f,
      -r_W / 2.f, -r_D / 2.f, r_H / 2.f,  -r_W / 2.f, -r_D / 2.f, -r_H / 2.f,
  };

  Pumbra::Polygon sideWallRightVerts = {
      r_W / 2.f, r_D / 2.f,  -r_H / 2.f, r_W / 2.f, r_D / 2.f,  r_H / 2.f,
      r_W / 2.f, -r_D / 2.f, r_H / 2.f,  r_W / 2.f, -r_D / 2.f, -r_H / 2.f,
  };

  Pumbra::Surface wallFront(wallFrontVerts);
  Pumbra::Surface wallBack(wallBackVerts);
  Pumbra::Surface roof(roofVerts);
  Pumbra::Surface floor(floorVerts);
  Pumbra::Surface sideWallLeft(sideWallLeftVerts);
  Pumbra::Surface sideWallRight(sideWallRightVerts);

  Pumbra::Penumbra pumbra;

  const unsigned wallFrontId = pumbra.addSurface(wallFront);
  const unsigned wallBackId = pumbra.addSurface(wallBack);
  const unsigned roofId = pumbra.addSurface(roof);
  const unsigned floorId = pumbra.addSurface(floor);
  const unsigned sideWallLeftId = pumbra.addSurface(sideWallLeft);
  const unsigned sideWallRightId = pumbra.addSurface(sideWallRight);

  const std::vector<unsigned> test_cube{wallFrontId, wallBackId,     roofId,
                                        floorId,     sideWallLeftId, sideWallRightId};

  pumbra.setModel();

  pumbra.setSunPosition(0.0f, 0.0f);
  std::vector<float> results = pumbra.calculatePSSA(test_cube);

  float m_pi_3_4 = m_pi_4_f + m_pi_2_f;

  const std::vector<std::pair<float, float>> angular_test_data{
      {0.0f, 0.0f},      // wallFront full shade
      {m_pi_4_f, 0.0f},  // wallFront half shade, sideWallRightId half shade
      {m_pi_2_f, 0.0f},  // sideWallRight full shade  !!
      {m_pi_3_4, 0.0f},  // wallBack half shade, sideWallRight half shade
      {-m_pi_2_f, 0.0f}, // sideWallLeft full shade  !!
      {m_pi_f, 0.0f},    // wallBack full shade
      {0.0f, m_pi_2_f},  // roof full shade  !!
      {0.0f, -m_pi_2_f}, // floor full shade !!
  };

  for (auto const &sunPosition : angular_test_data) {
    pumbra.setSunPosition(sunPosition.first, sunPosition.second);
    results = pumbra.calculatePSSA();
    float azimuth, altitude;
    for (auto side : test_cube) {
      switch (side) {
      case 0: // wallFrontId
        azimuth = sunPosition.first;
        altitude = sunPosition.second;
        break;
      case 1: // wallBackId
        azimuth = sunPosition.first + m_pi_f;
        altitude = sunPosition.second;
        break;
      case 2: // roofId
        azimuth = sunPosition.first;
        altitude = sunPosition.second - m_pi_2_f;
        break;
      case 3: // floorId
        azimuth = sunPosition.first;
        altitude = sunPosition.second + m_pi_2_f;
        break;
      case 4: // sideWallLeftId
        azimuth = sunPosition.first + m_pi_2_f;
        altitude = sunPosition.second;
        break;
      case 5: // sideWallRightId
        azimuth = sunPosition.first - m_pi_2_f;
        altitude = sunPosition.second;
        break;
      default:
        FAIL() << "Side not found.";
      }

      float expectedResults = calculate_surface_exposure(azimuth, altitude);

      // pumbra.renderScene(side);

      EXPECT_NEAR(results[side], expectedResults, 0.01);
    }
  }
}

TEST(PenumbraTest, vendor_name) {
  if (!Pumbra::Penumbra::isValidContext()) {
    GTEST_SKIP() << invalid_context_str << std::endl;
  }

  Pumbra::Penumbra pumbra;
  Pumbra::VendorName vname = pumbra.getVendorName();

  switch (vname) {
  case Pumbra::VendorName::unknown:
    SUCCEED();
    break;
  case Pumbra::VendorName::nvidia:
    SUCCEED();
    break;
  case Pumbra::VendorName::amd:
    SUCCEED();
    break;
  case Pumbra::VendorName::intel:
    SUCCEED();
    break;
  case Pumbra::VendorName::vmware:
    SUCCEED();
    break;
  case Pumbra::VendorName::mesa:
    SUCCEED();
    break;
  default:
    FAIL() << "Vendor Name not found.";
  }
}

TEST(PenumbraTest, side_count_check) {
  if (!Pumbra::Penumbra::isValidContext()) {
    GTEST_SKIP() << invalid_context_str << std::endl;
  }

  Pumbra::Polygon wallVerts = {0.f, 0.f, 0.f, 1.f, 0.f, 0.f, 1.f, 0.f, 1.f, 0.f, 0.f, 1.f};
  Pumbra::Surface wall(wallVerts);
  Pumbra::Penumbra pumbra;

  pumbra.addSurface(wall);
  pumbra.setModel();

  EXPECT_EQ(pumbra.getNumSurfaces(), 1u);

  pumbra.clearModel();

  Pumbra::Polygon wallFrontVerts = {-.5f, .5f, -.5f, .5f, .5f, -.5f, .5f, .5f, .5f, -.5f, .5f, .5f};

  Pumbra::Polygon wallBackVerts = {.5f,  -.5f, -.5f, -.5f, -.5f, -.5f,
                                   -.5f, -.5f, .5f,  .5f,  -.5f, .5f};

  Pumbra::Polygon roofVerts = {-.5f, .5f, .5f, .5f, .5f, .5f, .5f, -.5f, .5f, -.5f, -.5f, .5f};

  Pumbra::Surface wallFront(wallFrontVerts);
  Pumbra::Surface wallBack(wallBackVerts);
  Pumbra::Surface roof(roofVerts);

  pumbra.addSurface(wallFront);
  pumbra.addSurface(wallBack);
  pumbra.addSurface(roof);

  pumbra.setModel();

  EXPECT_EQ(pumbra.getNumSurfaces(), 3u);
}

TEST(PenumbraTest, bad_surface_input_errors) {
  if (!Pumbra::Penumbra::isValidContext()) {
    GTEST_SKIP() << invalid_context_str << std::endl;
  }

  const std::vector<unsigned> bad_test_cube{5, 6, 7, 8, 9, 10};

  Pumbra::Polygon wallFrontVerts = {-.5f, .5f, -.5f, .5f, .5f, -.5f, .5f, .5f, .5f, -.5f, .5f, .5f};

  Pumbra::Polygon wallBackVerts = {.5f,  -.5f, -.5f, -.5f, -.5f, -.5f,
                                   -.5f, -.5f, .5f,  .5f,  -.5f, .5f};

  Pumbra::Polygon roofVerts = {-.5f, .5f, .5f, .5f, .5f, .5f, .5f, -.5f, .5f, -.5f, -.5f, .5f};

  Pumbra::Surface wallFront(wallFrontVerts);
  Pumbra::Surface wallBack(wallBackVerts);
  Pumbra::Surface roof(roofVerts);

  Pumbra::Penumbra pumbra;

  pumbra.addSurface(wallFront);
  pumbra.addSurface(wallBack);
  pumbra.addSurface(roof);

  pumbra.setModel();

  EXPECT_THROW(pumbra.calculatePSSA(bad_test_cube), Pumbra::PenumbraException);

  EXPECT_THROW(pumbra.renderScene(11), Pumbra::PenumbraException);

  EXPECT_THROW(pumbra.fetchPSSA(bad_test_cube), Pumbra::PenumbraException);
}

int main(int argc, char **argv) {
  ::testing::InitGoogleTest(&argc, argv);

  return RUN_ALL_TESTS();
}
