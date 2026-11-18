/* Copyright (c) 2017 Big Ladder Software LLC. All rights reserved.
 * See the LICENSE file for additional terms and conditions. */

#define _USE_MATH_DEFINES
#include <cmath>

#include <cfenv>
#if defined(_MSC_VER)
#include <float.h>
#endif

#include "gtest/gtest.h"

#include <penumbra/penumbra.h>

// Float definitions for PI from math header
constexpr float m_pi_f = static_cast<float>(M_PI);
constexpr float m_pi_2_f = static_cast<float>(M_PI_2);
constexpr float m_pi_4_f = static_cast<float>(M_PI_4);

float calculate_surface_exposure(float azimuth, float altitude) {

  // Due to the one-sided nature of shaded surfaces, we will revert
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

  if (!Penumbra::Penumbra::is_valid_context()) {
    GTEST_SKIP() << invalid_context_string << std::endl;
  }

  Penumbra::Penumbra penumbra;

  // Loop azimuth (with zero altitude).
  for (float azimuth = 0.0f; azimuth <= 2 * m_pi_f; azimuth += m_pi_4_f) {
    penumbra.set_sun_position(azimuth, 0.0f);
    float check_azimuth = penumbra.get_sun_azimuth();
    EXPECT_FLOAT_EQ(azimuth, check_azimuth);
  }
}

TEST(PenumbraTest, check_altitude) {
  if (!Penumbra::Penumbra::is_valid_context()) {
    GTEST_SKIP() << invalid_context_string << std::endl;
  }
  Penumbra::Penumbra penumbra;

  // Loop altitude around the axis (with zero azimuth).
  for (float altitude = 0.0f; altitude <= 2 * m_pi_f; altitude += m_pi_4_f) {
    penumbra.set_sun_position(0.0f, altitude);
    float check_altitude = penumbra.get_sun_altitude();
    EXPECT_FLOAT_EQ(altitude, check_altitude);
  }
}

TEST(PenumbraTest, azimuth) {
  if (!Penumbra::Penumbra::is_valid_context()) {
    GTEST_SKIP() << invalid_context_string << std::endl;
  }
  Penumbra::Surface wall({0.f, 0.f, 0.f, 1.f, 0.f, 0.f, 1.f, 0.f, 1.f, 0.f, 0.f, 1.f}, "Wall");
  Penumbra::Penumbra penumbra;
  unsigned int wall_id = penumbra.add_surface(wall);
  penumbra.set_model();
  // Loop azimuth around the surface (with zero altitude).
  for (float azimuth = 0.0f; azimuth <= 2 * m_pi_f; azimuth += m_pi_4_f) {
    penumbra.set_sun_position(azimuth, 0.0f);
    float wall_pssa = penumbra.calculate_pssa(wall_id);
    EXPECT_NEAR(wall_pssa, std::abs(cos(azimuth)), 0.01) << "azimuth evaluates to " << azimuth;
    // penumbra.render_scene(wall_id);
  }
}

TEST(PenumbraTest, interior) {
  if (!Penumbra::Penumbra::is_valid_context()) {
    GTEST_SKIP() << invalid_context_string << std::endl;
  }
  float const r_W = 2., r_D = 1., r_H = 1.; // Overall dimensions
  float const w_W = 1., w_H = .5;           // Window dimensions

  Penumbra::Polygon window_polygon = {
      -w_W / 2.f, r_D / 2.f, -w_H / 2.f, w_W / 2.f,  r_D / 2.f, -w_H / 2.f,
      w_W / 2.f,  r_D / 2.f, w_H / 2.f,  -w_W / 2.f, r_D / 2.f, w_H / 2.f,
  };

  Penumbra::Surface front_wall({
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

  Penumbra::Surface window(window_polygon);
  Penumbra::Surface back_wall({
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
  Penumbra::Surface roof({
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
  Penumbra::Surface floor({
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
  Penumbra::Surface left_wall({
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
  Penumbra::Surface right_wall({
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

  Penumbra::Penumbra penumbra;

  unsigned int front_wall_id = penumbra.add_surface(front_wall);
  unsigned int window_id = penumbra.add_surface(window);
  unsigned int back_wall_id = penumbra.add_surface(back_wall);
  [[maybe_unused]] unsigned int roof_id = penumbra.add_surface(roof);
  unsigned int floor_id = penumbra.add_surface(floor);
  unsigned int left_wall_id = penumbra.add_surface(left_wall);
  unsigned int right_wall_id = penumbra.add_surface(right_wall);

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
  std::unordered_map<unsigned int, float> interior_pssas = penumbra.calculate_interior_pssas(
      {window_id}, {back_wall_id, floor_id, left_wall_id, right_wall_id});
  EXPECT_GT(interior_pssas[back_wall_id], 0.f);
  EXPECT_GT(interior_pssas[floor_id], 0.f);
  EXPECT_GT(interior_pssas[left_wall_id], 0.f);
  EXPECT_EQ(interior_pssas[right_wall_id], 0.f);
  // penumbra.render_interior_scene({window_id},{back_wall_id, floor_id, left_wall_id,
  // right_wall_id});
}

TEST(PenumbraTest, calculate_pssa_multiple_surfaces) {
  if (!Penumbra::Penumbra::is_valid_context()) {
    GTEST_SKIP() << invalid_context_string << std::endl;
  }
  // create a cube
  float const r_W = 1.f, r_D = 1.f, r_H = 1.f; // Overall dimensions

  // TODO: put in fixture
  Penumbra::Surface front_wall({
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
  Penumbra::Surface back_wall({
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
  Penumbra::Surface roof({
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
  Penumbra::Surface floor({
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
  Penumbra::Surface left_wall({
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
  Penumbra::Surface right_wall({
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
  Penumbra::Penumbra penumbra;

  const unsigned int front_wall_id = penumbra.add_surface(front_wall);
  const unsigned int back_wall_id = penumbra.add_surface(back_wall);
  const unsigned int roof_id = penumbra.add_surface(roof);
  const unsigned int floor_id = penumbra.add_surface(floor);
  const unsigned int left_wall_id = penumbra.add_surface(left_wall);
  const unsigned int right_wall_id = penumbra.add_surface(right_wall);

  const std::vector<unsigned int> test_cube{front_wall_id, back_wall_id, roof_id,
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
  if (!Penumbra::Penumbra::is_valid_context()) {
    GTEST_SKIP() << invalid_context_string << std::endl;
  }

  Penumbra::Penumbra penumbra;
  Penumbra::VendorType vendor_type = penumbra.get_vendor_name();

  switch (vendor_type) {
  case Penumbra::VendorType::unknown:
    SUCCEED();
    break;
  case Penumbra::VendorType::nvidia:
    SUCCEED();
    break;
  case Penumbra::VendorType::amd:
    SUCCEED();
    break;
  case Penumbra::VendorType::intel:
    SUCCEED();
    break;
  case Penumbra::VendorType::vmware:
    SUCCEED();
    break;
  case Penumbra::VendorType::mesa:
    SUCCEED();
    break;
  case Penumbra::VendorType::apple:
    SUCCEED();
    break;
  default:
    FAIL() << "Vendor Name not found.";
  }
}

TEST(PenumbraTest, side_count_check) {
  if (!Penumbra::Penumbra::is_valid_context()) {
    GTEST_SKIP() << invalid_context_string << std::endl;
  }

  Penumbra::Surface wall({0.f, 0.f, 0.f, 1.f, 0.f, 0.f, 1.f, 0.f, 1.f, 0.f, 0.f, 1.f});
  Penumbra::Penumbra penumbra;

  penumbra.add_surface(wall);
  penumbra.set_model();

  EXPECT_EQ(penumbra.get_number_of_surfaces(), 1u);

  penumbra.clear_model();

  Penumbra::Surface front_wall({-.5f, .5f, -.5f, .5f, .5f, -.5f, .5f, .5f, .5f, -.5f, .5f, .5f});
  Penumbra::Surface back_wall({.5f, -.5f, -.5f, -.5f, -.5f, -.5f, -.5f, -.5f, .5f, .5f, -.5f, .5f});
  Penumbra::Surface roof({-.5f, .5f, .5f, .5f, .5f, .5f, .5f, -.5f, .5f, -.5f, -.5f, .5f});

  penumbra.add_surface(front_wall);
  penumbra.add_surface(back_wall);
  penumbra.add_surface(roof);

  penumbra.set_model();

  EXPECT_EQ(penumbra.get_number_of_surfaces(), 3u);
}

TEST(PenumbraTest, bad_surface_input_errors) {
  if (!Penumbra::Penumbra::is_valid_context()) {
    GTEST_SKIP() << invalid_context_string << std::endl;
  }

  const std::vector<unsigned int> bad_test_cube{5, 6, 7, 8, 9, 10};
  Penumbra::Surface front_wall({-.5f, .5f, -.5f, .5f, .5f, -.5f, .5f, .5f, .5f, -.5f, .5f, .5f});
  Penumbra::Surface back_wall({.5f, -.5f, -.5f, -.5f, -.5f, -.5f, -.5f, -.5f, .5f, .5f, -.5f, .5f});
  Penumbra::Surface roof({-.5f, .5f, .5f, .5f, .5f, .5f, .5f, -.5f, .5f, -.5f, -.5f, .5f});

  Penumbra::Penumbra penumbra;

  penumbra.add_surface(front_wall);
  penumbra.add_surface(back_wall);
  penumbra.add_surface(roof);

  penumbra.set_model();

  EXPECT_THROW(penumbra.calculate_pssa(bad_test_cube), std::runtime_error);

  EXPECT_THROW(penumbra.render_scene(11), std::runtime_error);

  EXPECT_THROW(penumbra.retrieve_pssa(bad_test_cube), std::runtime_error);
}

// Enable divide-by-zero, invalid and overflow traps the way a host application (e.g., an
// EnergyPlus debug build) might. Returns false if trapping isn't supported on this platform.
static bool enable_floating_point_traps() {
#if defined(__GLIBC__)
  return feenableexcept(FE_DIVBYZERO | FE_INVALID | FE_OVERFLOW) != -1;
#elif defined(__APPLE__) && defined(__aarch64__)
  std::fenv_t environment;
  std::fegetenv(&environment);
  environment.__fpcr |= __fpcr_trap_divbyzero | __fpcr_trap_invalid | __fpcr_trap_overflow;
  return std::fesetenv(&environment) == 0;
#elif defined(__APPLE__) && defined(__x86_64__)
  std::fenv_t environment;
  std::fegetenv(&environment);
  constexpr unsigned short traps = FE_DIVBYZERO | FE_INVALID | FE_OVERFLOW;
  environment.__control &= static_cast<unsigned short>(~traps);
  environment.__mxcsr &= ~(static_cast<unsigned int>(traps) << 7);
  return std::fesetenv(&environment) == 0;
#elif defined(_MSC_VER)
  unsigned int control_word;
  _controlfp_s(&control_word, 0, 0);
  return _controlfp_s(&control_word, control_word & ~(_EM_ZERODIVIDE | _EM_INVALID | _EM_OVERFLOW),
                      _MCW_EM) == 0;
#else
  return false;
#endif
}

TEST(PenumbraTest, floating_point_traps_enabled_by_host) {
  std::fenv_t original_environment;
  std::fegetenv(&original_environment);
  if (!enable_floating_point_traps()) {
    GTEST_SKIP() << "Enabling floating point traps is not supported on this platform.";
  }
  std::fenv_t trapping_environment;
  std::fegetenv(&trapping_environment);

  // Any FP exception raised by GLFW, the windowing system or the GL driver would crash here
  // (SIGFPE on x86_64, SIGILL on arm64 macOS).
  if (Penumbra::Penumbra::is_valid_context()) {
    Penumbra::Surface wall({0.f, 0.f, 0.f, 1.f, 0.f, 0.f, 1.f, 0.f, 1.f, 0.f, 0.f, 1.f}, "Wall");
    {
      Penumbra::Penumbra penumbra;
      unsigned int wall_id = penumbra.add_surface(wall);
      penumbra.set_model();
      penumbra.set_sun_position(0.0f, 0.0f);
      EXPECT_NEAR(penumbra.calculate_pssa(wall_id), 1.0f, 0.01);
    }
  }

  // Penumbra must hand the host's environment (including its traps) back untouched.
  std::fenv_t final_environment;
  std::fegetenv(&final_environment);
  std::fesetenv(&original_environment);
#if defined(__APPLE__) && defined(__aarch64__)
  EXPECT_EQ(final_environment.__fpcr, trapping_environment.__fpcr);
#elif defined(__APPLE__) && defined(__x86_64__)
  EXPECT_EQ(final_environment.__control, trapping_environment.__control);
  EXPECT_EQ(final_environment.__mxcsr & 0xFF80u, trapping_environment.__mxcsr & 0xFF80u);
#elif defined(__GLIBC__)
  std::fesetenv(&final_environment);
  EXPECT_EQ(fegetexcept(), FE_DIVBYZERO | FE_INVALID | FE_OVERFLOW);
  std::fesetenv(&original_environment);
#endif
  (void)trapping_environment;
}

int main(int argc, char **argv) {
  ::testing::InitGoogleTest(&argc, argv);

  return RUN_ALL_TESTS();
}
