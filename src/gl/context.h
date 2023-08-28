/* Copyright (c) 2017 Big Ladder Software LLC. All rights reserved.
 * See the LICENSE file for additional terms and conditions. */

#ifndef CONTEXT_H_
#define CONTEXT_H_

// Standard
#include <vector>
#include <array>
#include <limits>
#include <unordered_map>
#include <memory>
#include <string>

// Vendor
#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <courierr/courierr.h>

// Penumbra
#include <sun.h>
#include <gl/model.h>
#include <gl/shader.h>
#include <gl/program.h>

#define MAX_FLOAT std::numeric_limits<float>::max()

namespace Pumbra {

class Context {

public:
  Context(GLint size, const std::shared_ptr<Courierr::Courierr> &logger);
  ~Context();
  void showRendering(unsigned surfaceIndex, mat4x4 sunView);
  void setModel(const std::vector<float> &vertices,
                const std::vector<SurfaceBuffer> &surfaceBuffers);
  float setScene(mat4x4 sunView, const SurfaceBuffer *surfaceBuffer = nullptr, bool clipFar = true);
  /*
  void bufferedQuery(const unsigned surfaceIndex);
  void bufferedQuery(const SurfaceBuffer &surfaceBuffer);
  */
  void submitPSSA(unsigned surfaceIndex, mat4x4 sunView);
  void submitPSSA(const std::vector<unsigned> &surfaceIndices, mat4x4 sunView);
  void submitPSSA(mat4x4 sunView);
  float calculatePSSA(unsigned surfaceIndex);
  std::vector<float> calculatePSSA(const std::vector<unsigned> &surfaceIndices);
  std::vector<float> calculatePSSA();
  std::unordered_map<unsigned, float>
  calculateInteriorPSSAs(const std::vector<unsigned> &hiddenSurfaces,
                         const std::vector<unsigned> &interiorSurfaces, mat4x4 sunView);
  void showInteriorRendering(const std::vector<unsigned> &hiddenSurfaceIndices,
                             unsigned interiorSurfaceIndex, mat4x4 sunView);
  void clearModel();
  static std::string vendorName();

private:
  GLFWwindow *window;
  GLuint fbo{}, rbo{};
  static const char *renderVertexShaderSource;
  static const char *renderFragmentShaderSource;
  static const char *calculationVertexShaderSource;
  GLint size;
  GLModel model;
  std::unique_ptr<GLProgram> renderProgram;
  std::unique_ptr<GLProgram> calcProgram;
  bool modelSet;
  float modelBox[8][4] = {};
  mat4x4 projection = {}, view = {}, mvp = {};
  mat4x4 cameraView = {};
  GLint mvpLocation{}, vColLocation{};
  bool isWireFrame;
  bool isCameraMode;
  float left{0}, right{0}, bottom{0}, top{0}, near_{0}, far_{0};
  float viewScale;
  double prevPosX, prevPosY;
  float cameraRotAngleX, cameraRotAngleY;
  bool lbutton_down;
  bool isRenderMode;
  std::vector<GLuint> queries;
  std::vector<float> pixelAreas;
  std::vector<GLint> pixelCounts;
  std::vector<int> indexBuffer;
  /*
  int currentBufferIndex{0};
  int bufferSize{16};
  */
  std::shared_ptr<Courierr::Courierr> logger;

  void drawModel();
  void drawExcept(const std::vector<SurfaceBuffer> &hiddenSurfaces);
  void setMVP();
  void setCameraMVP();
  void calcCameraView();
  void toggleWireFrame();
  void toggleCameraMode();
  void initOffScreenMode();
  void initRenderMode();
};

} // namespace Pumbra

#endif // CONTEXT_H_
