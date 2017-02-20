/* Copyright (c) 2017 Big Ladder Software and Charles S. Barnaby. All rights reserved.
* See the LICENSE file for additional terms and conditions. */

#ifndef CONTEXT_H_
#define CONTEXT_H_

#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <penumbra/sun.h>
#include <vector>
#include <array>
#include <limits>

#define FLT_MAX std::numeric_limits<float>::max()

namespace Pumbra {

class GLModel {
public:
  ~GLModel();
  void setVertices(const std::vector<std::array<float, 3>>& vertices, const std::array<float, 3>& color);
  void draw();
private:
  GLuint vbo, vao;
  std::size_t numVerts;
};

class Context {

public:
  Context(std::size_t size=600);
  ~Context();
  void showRendering();
  void drawSurfaces();
  void setModel(const std::vector<std::array<float, 3>>& vertices);
  void setScene(const std::vector<std::array<float, 3>>& vertices, mat4x4 sunView);
  std::size_t countPixels();

private:
  GLFWwindow* window;
  GLuint query, fbo, rbo;
  static const char* vertexShaderSource;
  static const char* fragmentShaderSource;
  GLuint vertexShader, fragmentShader, program;
  GLint mvpLocation;
  std::size_t size;
  GLModel model, surface;
  float pixelArea;
  float modelBox[8][4];
  mat4x4 projection, view, mvp;

  void setMVP();


};

}

#endif // CONTEXT_H_
