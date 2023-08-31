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

const std::string invalid_context_string = "A valid context could not be created. Test skipped.";

TEST(PenumbraTest, check_azimuth) {

  if (!Pumbra::Penumbra::is_valid_context()) {
    GTEST_SKIP() << invalid_context_string << std::endl;
  }

  Pumbra::Penumbra pumbra;

  // Loop azimuth (with zero altitude).
  for (float azimuth = 0.0f; azimuth <= 2 * m_pi_f; azimuth += m_pi_4_f) {
    pumbra.set_sun_position(azimuth, 0.0f);
    float check_azimuth = pumbra.get_sun_azimuth();
    EXPECT_FLOAT_EQ(azimuth, check_azimuth);
  }
}

TEST(PenumbraTest, check_altitude) {
  if (!Pumbra::Penumbra::is_valid_context()) {
    GTEST_SKIP() << invalid_context_string << std::endl;
  }
  Pumbra::Penumbra pumbra;

  // Loop altitude around the axis (with zero azimuth).
  for (float altitude = 0.0f; altitude <= 2 * m_pi_f; altitude += m_pi_4_f) {
    pumbra.set_sun_position(0.0f, altitude);
    float check_altitude = pumbra.get_sun_altitude();
    EXPECT_FLOAT_EQ(altitude, check_altitude);
  }
}

TEST(PenumbraTest, azimuth) {
  if (!Pumbra::Penumbra::is_valid_context()) {
    GTEST_SKIP() << invalid_context_string << std::endl;
  }
  Pumbra::Surface wall({0.f, 0.f, 0.f, 1.f, 0.f, 0.f, 1.f, 0.f, 1.f, 0.f, 0.f, 1.f}, "Wall");
  Pumbra::Penumbra pumbra;
  unsigned wall_id = pumbra.add_surface(wall);
  pumbra.set_model();
  // Loop azimuth around the surface (with zero altitude).
  for (float azimuth = 0.0f; azimuth <= 2 * m_pi_f; azimuth += m_pi_4_f) {
    pumbra.set_sun_position(azimuth, 0.0f);
    float wall_pssa = pumbra.calculate_pssa(wall_id);
    EXPECT_NEAR(wall_pssa, std::abs(cos(azimuth)), 0.01) << "azimuth evaluates to " << azimuth;
    // penumbra.render_scene(wall_id);
  }
}

TEST(PenumbraTest, interior) {
  if (!Pumbra::Penumbra::is_valid_context()) {
    GTEST_SKIP() << invalid_context_string << std::endl;
  }
  float const r_W = 2., r_D = 1., r_H = 1.; // Overall dimensions
  float const w_W = 1., w_H = .5;           // Window dimensions

  Pumbra::Polygon window_polygon = {
      -w_W / 2.f, r_D / 2.f, -w_H / 2.f, w_W / 2.f,  r_D / 2.f, -w_H / 2.f,
      w_W / 2.f,  r_D / 2.f, w_H / 2.f,  -w_W / 2.f, r_D / 2.f, w_H / 2.f,
  };

  Pumbra::Surface front_wall({
      -r_W / 2.f,
      r_D / 2.f,
      -r_H / 2.f,
      r_W / 2.f,
      r_D / 2.f,
      -r_H / 2.f,
      r_W / 2.f,
      r_D / 2.f,
      r_H / 2.f,
      -r_W / 2.f,
      r_D / 2.f,
      r_H / 2.f,
  });
  front_wall.add_hole(window_polygon);

  Pumbra::Surface window(window_polygon);
  Pumbra::Surface back_wall({
      r_W / 2.f,
      -r_D / 2.f,
      -r_H / 2.f,
      -r_W / 2.f,
      -r_D / 2.f,
      -r_H / 2.f,
      -r_W / 2.f,
      -r_D / 2.f,
      r_H / 2.f,
      r_W / 2.f,
      -r_D / 2.f,
      r_H / 2.f,
  });
  Pumbra::Surface roof({
      -r_W / 2.f,
      r_D / 2.f,
      r_H / 2.f,
      r_W / 2.f,
      r_D / 2.f,
      r_H / 2.f,
      r_W / 2.f,
      -r_D / 2.f,
      r_H / 2.f,
      -r_W / 2.f,
      -r_D / 2.f,
      r_H / 2.f,
  });
  Pumbra::Surface floor({
      -r_W / 2.f,
      r_D / 2.f,
      -r_H / 2.f,
      r_W / 2.f,
      r_D / 2.f,
      -r_H / 2.f,
      r_W / 2.f,
      -r_D / 2.f,
      -r_H / 2.f,
      -r_W / 2.f,
      -r_D / 2.f,
      -r_H / 2.f,
  });
  Pumbra::Surface left_wall({
      -r_W / 2.f,
      r_D / 2.f,
      -r_H / 2.f,
      -r_W / 2.f,
      r_D / 2.f,
      r_H / 2.f,
      -r_W / 2.f,
      -r_D / 2.f,
      r_H / 2.f,
      -r_W / 2.f,
      -r_D / 2.f,
      -r_H / 2.f,
  });
  Pumbra::Surface right_wall({
      r_W / 2.f,
      r_D / 2.f,
      -r_H / 2.f,
      r_W / 2.f,
      r_D / 2.f,
      r_H / 2.f,
      r_W / 2.f,
      -r_D / 2.f,
      r_H / 2.f,
      r_W / 2.f,
      -r_D / 2.f,
      -r_H / 2.f,
  });

  Pumbra::Penumbra penumbra;

  unsigned front_wall_id = penumbra.add_surface(front_wall);
  unsigned window_id = penumbra.add_surface(window);
  unsigned back_wall_id = penumbra.add_surface(back_wall);
  [[maybe_unused]] unsigned roof_id = penumbra.add_surface(roof);
  unsigned floor_id = penumbra.add_surface(floor);
  unsigned left_wall_id = penumbra.add_surface(left_wall);
  unsigned right_wall_id = penumbra.add_surface(right_wall);

  penumbra.set_model();
  penumbra.set_sun_position(0.0f, 0.f);
  EXPECT_NEAR(penumbra.calculate_pssa(front_wall_id), r_W * r_H - w_W * w_H, 0.01);
  // penumbra.render_scene(front_wall_id);

  EXPECT_NEAR(penumbra.calculate_interior_pssas({window_id}, {back_wall_id})[back_wall_id],
              w_W * w_H, 0.01);
  // penumbra.render_interior_scene({window_id},{back_wall_id});

  penumbra.set_sun_position(3.1415f, 0.f);
  EXPECT_NEAR(penumbra.calculate_pssa(front_wall_id), 0.f,
              0.01); // front_wall should be blocked by back wall.
  // penumbra.render_scene(front_wall_id);

  penumbra.set_sun_position(0.5f, 0.5f);
  std::unordered_map<unsigned, float> interior_pssas = penumbra.calculate_interior_pssas(
      {window_id}, {back_wall_id, floor_id, left_wall_id, right_wall_id});
  EXPECT_GT(interior_pssas[back_wall_id], 0.f);
  EXPECT_GT(interior_pssas[floor_id], 0.f);
  EXPECT_GT(interior_pssas[left_wall_id], 0.f);
  EXPECT_EQ(interior_pssas[right_wall_id], 0.f);
  // penumbra.render_interior_scene({window_id},{back_wall_id, floor_id, left_wall_id,
  // right_wall_id});
}

TEST(PenumbraTest, calculate_pssa_multiple_surfaces) {
  if (!Pumbra::Penumbra::is_valid_context()) {
    GTEST_SKIP() << invalid_context_string << std::endl;
  }
  // create a cube
  float const r_W = 1.f, r_D = 1.f, r_H = 1.f; // Overall dimensions

  // TODO: put in fixture
  Pumbra::Surface front_wall({
      -r_W / 2.f,
      r_D / 2.f,
      -r_H / 2.f,
      r_W / 2.f,
      r_D / 2.f,
      -r_H / 2.f,
      r_W / 2.f,
      r_D / 2.f,
      r_H / 2.f,
      -r_W / 2.f,
      r_D / 2.f,
      r_H / 2.f,
  });
  Pumbra::Surface back_wall({
      r_W / 2.f,
      -r_D / 2.f,
      -r_H / 2.f,
      -r_W / 2.f,
      -r_D / 2.f,
      -r_H / 2.f,
      -r_W / 2.f,
      -r_D / 2.f,
      r_H / 2.f,
      r_W / 2.f,
      -r_D / 2.f,
      r_H / 2.f,
  });
  Pumbra::Surface roof({
      -r_W / 2.f,
      r_D / 2.f,
      r_H / 2.f,
      r_W / 2.f,
      r_D / 2.f,
      r_H / 2.f,
      r_W / 2.f,
      -r_D / 2.f,
      r_H / 2.f,
      -r_W / 2.f,
      -r_D / 2.f,
      r_H / 2.f,
  });
  Pumbra::Surface floor({
      -r_W / 2.f,
      r_D / 2.f,
      -r_H / 2.f,
      r_W / 2.f,
      r_D / 2.f,
      -r_H / 2.f,
      r_W / 2.f,
      -r_D / 2.f,
      -r_H / 2.f,
      -r_W / 2.f,
      -r_D / 2.f,
      -r_H / 2.f,
  });
  Pumbra::Surface left_wall({
      -r_W / 2.f,
      r_D / 2.f,
      -r_H / 2.f,
      -r_W / 2.f,
      r_D / 2.f,
      r_H / 2.f,
      -r_W / 2.f,
      -r_D / 2.f,
      r_H / 2.f,
      -r_W / 2.f,
      -r_D / 2.f,
      -r_H / 2.f,
  });
  Pumbra::Surface right_wall({
      r_W / 2.f,
      r_D / 2.f,
      -r_H / 2.f,
      r_W / 2.f,
      r_D / 2.f,
      r_H / 2.f,
      r_W / 2.f,
      -r_D / 2.f,
      r_H / 2.f,
      r_W / 2.f,
      -r_D / 2.f,
      -r_H / 2.f,
  });
  Pumbra::Penumbra penumbra;

  const unsigned int front_wall_id = penumbra.add_surface(front_wall);
  const unsigned int back_wall_id = penumbra.add_surface(back_wall);
  const unsigned int roof_id = penumbra.add_surface(roof);
  const unsigned int floor_id = penumbra.add_surface(floor);
  const unsigned int left_wall_id = penumbra.add_surface(left_wall);
  const unsigned int right_wall_id = penumbra.add_surface(right_wall);

  const std::vector<unsigned> test_cube{front_wall_id, back_wall_id,   roof_id,
                                        floor_id,      left_wall_id, right_wall_id};

  penumbra.set_model();

  penumbra.set_sun_position(0.0f, 0.0f);
  std::vector<float> results = penumbra.calculate_pssa(test_cube);

  float m_pi_3_4 = m_pi_4_f + m_pi_2_f;

  const std::vector<std::pair<float, float>> angular_test_data{
      {0.0f, 0.0f},      // front_wall full shade
      {m_pi_4_f, 0.0f},  // front_wall half shade, right_wall_id half shade
      {m_pi_2_f, 0.0f},  // right_wall full shade  !!
      {m_pi_3_4, 0.0f},  // back_wall half shade, right_wall half shade
      {-m_pi_2_f, 0.0f}, // left_wall full shade  !!
      {m_pi_f, 0.0f},    // back_wall full shade
      {0.0f, m_pi_2_f},  // roof full shade  !!
      {0.0f, -m_pi_2_f}, // floor full shade !!
  };

  for (auto const &sun_position : angular_test_data) {
    penumbra.set_sun_position(sun_position.first, sun_position.second);
    results = penumbra.calculate_pssa();
    float azimuth, altitude;
    for (auto side : test_cube) {
      switch (side) {
      case 0: // front_wall_id
        azimuth = sun_position.first;
        altitude = sun_position.second;
        break;
      case 1: // back_wall_id
        azimuth = sun_position.first + m_pi_f;
        altitude = sun_position.second;
        break;
      case 2: // roof_id
        azimuth = sun_position.first;
        altitude = sun_position.second - m_pi_2_f;
        break;
      case 3: // floor_id
        azimuth = sun_position.first;
        altitude = sun_position.second + m_pi_2_f;
        break;
      case 4: // left_wall_id
        azimuth = sun_position.first + m_pi_2_f;
        altitude = sun_position.second;
        break;
      case 5: // right_wall_id
        azimuth = sun_position.first - m_pi_2_f;
        altitude = sun_position.second;
        break;
      default:
        FAIL() << "Side not found.";
      }

      float expectedResults = calculate_surface_exposure(azimuth, altitude);

      // penumbra.render_scene(side);

      EXPECT_NEAR(results[side], expectedResults, 0.01);
    }
  }
}

TEST(PenumbraTest, vendor_name) {
  if (!Pumbra::Penumbra::is_valid_context()) {
    GTEST_SKIP() << invalid_context_string << std::endl;
  }

  Pumbra::Penumbra penumbra;
  Pumbra::VendorType vendor_type = penumbra.get_vendor_name();

  switch (vendor_type) {
  case Pumbra::VendorType::unknown:
    SUCCEED();
    break;
  case Pumbra::VendorType::nvidia:
    SUCCEED();
    break;
  case Pumbra::VendorType::amd:
    SUCCEED();
    break;
  case Pumbra::VendorType::intel:
    SUCCEED();
    break;
  case Pumbra::VendorType::vmware:
    SUCCEED();
    break;
  case Pumbra::VendorType::mesa:
    SUCCEED();
    break;
  default:
    FAIL() << "Vendor Name not found.";
  }
}

TEST(PenumbraTest, side_count_check) {
  if (!Pumbra::Penumbra::is_valid_context()) {
    GTEST_SKIP() << invalid_context_string << std::endl;
  }

  Pumbra::Surface wall({0.f, 0.f, 0.f, 1.f, 0.f, 0.f, 1.f, 0.f, 1.f, 0.f, 0.f, 1.f});
  Pumbra::Penumbra penumbra;

  penumbra.add_surface(wall);
  penumbra.set_model();

  EXPECT_EQ(penumbra.get_number_of_surfaces(), 1u);

  penumbra.clear_model();

  Pumbra::Surface front_wall({-.5f, .5f, -.5f, .5f, .5f, -.5f, .5f, .5f, .5f, -.5f, .5f, .5f});
  Pumbra::Surface back_wall({.5f, -.5f, -.5f, -.5f, -.5f, -.5f, -.5f, -.5f, .5f, .5f, -.5f, .5f});
  Pumbra::Surface roof({-.5f, .5f, .5f, .5f, .5f, .5f, .5f, -.5f, .5f, -.5f, -.5f, .5f});

  penumbra.add_surface(front_wall);
  penumbra.add_surface(back_wall);
  penumbra.add_surface(roof);

  penumbra.set_model();

  EXPECT_EQ(penumbra.get_number_of_surfaces(), 3u);
}

TEST(PenumbraTest, bad_surface_input_errors) {
  if (!Pumbra::Penumbra::is_valid_context()) {
    GTEST_SKIP() << invalid_context_string << std::endl;
  }

  const std::vector<unsigned> bad_test_cube{5, 6, 7, 8, 9, 10};
  Pumbra::Surface front_wall({-.5f, .5f, -.5f, .5f, .5f, -.5f, .5f, .5f, .5f, -.5f, .5f, .5f});
  Pumbra::Surface back_wall({.5f, -.5f, -.5f, -.5f, -.5f, -.5f, -.5f, -.5f, .5f, .5f, -.5f, .5f});
  Pumbra::Surface roof({-.5f, .5f, .5f, .5f, .5f, .5f, .5f, -.5f, .5f, -.5f, -.5f, .5f});

  Pumbra::Penumbra penumbra;

  penumbra.add_surface(front_wall);
  penumbra.add_surface(back_wall);
  penumbra.add_surface(roof);

  penumbra.set_model();

  EXPECT_THROW(penumbra.calculate_pssa(bad_test_cube), Pumbra::PenumbraException);

  EXPECT_THROW(penumbra.render_scene(11), Pumbra::PenumbraException);

  EXPECT_THROW(penumbra.fetch_pssa(bad_test_cube), Pumbra::PenumbraException);
}

int main(int argc, char **argv) {
  ::testing::InitGoogleTest(&argc, argv);

  return RUN_ALL_TESTS();
}
