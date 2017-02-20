/* Copyright (c) 2017 Big Ladder Software and Charles S. Barnaby. All rights reserved.
* See the LICENSE file for additional terms and conditions. */

#include <penumbra/context.h>

namespace Pumbra {

// TODO: remove color or make color uniform (see explicit uniform location)
// https://www.khronos.org/opengl/wiki/Uniform_(GLSL)
// May require regeneration of GLAD

// TODO: Separate into more objects:
// - Shader program
// - Camera
// - Results? (a container for the surface and its properties)

const char* Context::vertexShaderSource =
R"src(
  #version 150
  uniform mat4 MVP;
  in vec3 vCol;
  in vec3 vPos;
  out vec3 color;
  void main()
  {
    gl_Position = MVP * vec4(vPos, 1.0);
    color = vCol;
  }
)src";

const char* Context::fragmentShaderSource =
R"src(
  #version 150
  in vec3 color;
  out vec4 outColor;
  void main()
  {
    outColor = vec4(color, 1.0);
  }
)src";

GLModel::~GLModel() {
  glDeleteVertexArrays(1, &vao);
  glDeleteBuffers(1, &vbo);
}

void GLModel::setVertices(const std::vector<std::array<float, 3>>& vertices, const std::array<float, 3>& color) {

  numVerts = vertices.size();
  std::vector<float> cVertices;
  cVertices.reserve(numVerts * 6);

  for (std::size_t i = 0; i < numVerts; i++) {
    cVertices.push_back(vertices[i][0]);
    cVertices.push_back(vertices[i][1]);
    cVertices.push_back(vertices[i][2]);
    cVertices.push_back(color[0]);
    cVertices.push_back(color[1]);
    cVertices.push_back(color[2]);
  }

  // Set up vertex array object
  glGenVertexArrays(1, &vao);
  glBindVertexArray(vao);

  // Set up array buffer to store vertex information
  glGenBuffers(1, &vbo);
  glBindBuffer(GL_ARRAY_BUFFER, vbo);
  glBufferData(GL_ARRAY_BUFFER, sizeof(float)*cVertices.size(), &cVertices[0], GL_STATIC_DRAW);

  // Set drawing pointers for current vertex buffer
  glEnableVertexAttribArray(0);
  glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE,
    sizeof(float) * 6, (void*)0);
  glEnableVertexAttribArray(1);
  glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE,
    sizeof(float) * 6, (void*)(sizeof(float) * 3));
}

void GLModel::draw() {
  glBindVertexArray(vao);
  glDrawArrays(GL_TRIANGLES, 0, numVerts);
}

Context::Context(std::size_t size) :
  size(size)
{
  glfwSetErrorCallback([](int error, const char* description){fprintf(stderr, "Error (%d): %s\n", error, description);});

  if (!glfwInit()) {throw;}

  glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
  glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 2);
  glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
  glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GLFW_TRUE);
  glfwWindowHint(GLFW_VISIBLE, GL_FALSE);
  window = glfwCreateWindow(1, 1, "Penumbra", NULL, NULL);
  glfwMakeContextCurrent(window);
  if (!window) {
    glfwTerminate();
    throw;
  }

  // OpenGL extension loader
  gladLoadGLLoader((GLADloadproc) glfwGetProcAddress);
  glfwSwapInterval(1);

  glEnable(GL_DEPTH_TEST);

  GLint shader_ok;
  GLsizei log_length;
  char info_log[8192];


  // Vertex shader
  vertexShader = glCreateShader(GL_VERTEX_SHADER);
  glShaderSource(vertexShader, 1, &vertexShaderSource, NULL);
  glCompileShader(vertexShader);
  glGetShaderiv(vertexShader, GL_COMPILE_STATUS, &shader_ok);

  if (shader_ok != GL_TRUE)
  {
      fprintf(stderr, "ERROR: Failed to compile vertex_shader shader.\n");
      glGetShaderInfoLog(vertexShader, 8192, &log_length,info_log);
      fprintf(stderr, "ERROR: \n%s\n\n", info_log);
      glDeleteShader(vertexShader);
  }


  // Fragment shader
  fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
  glShaderSource(fragmentShader, 1, &fragmentShaderSource, NULL);
  glCompileShader(fragmentShader);
  glGetShaderiv(fragmentShader, GL_COMPILE_STATUS, &shader_ok);

  if (shader_ok != GL_TRUE)
  {
      fprintf(stderr, "ERROR: Failed to compile vertex_shader shader.\n");
      glGetShaderInfoLog(fragmentShader, 8192, &log_length,info_log);
      fprintf(stderr, "ERROR: \n%s\n\n", info_log);
      glDeleteShader(fragmentShader);
  }


  // Shader program
  program = glCreateProgram();
  glAttachShader(program, vertexShader);
  glAttachShader(program, fragmentShader);
  glLinkProgram(program);
  glUseProgram(program);


  mvpLocation = glGetUniformLocation(program, "MVP");

  glBindAttribLocation(program, 0, "vPos");
  glBindAttribLocation(program, 1, "vCol");
}

Context::~Context(){
  glDeleteFramebuffers(1, &fbo);
  glDeleteRenderbuffers(1, &rbo);
  glfwTerminate();
}

void Context::setModel(const std::vector<std::array<float, 3>>& vertices) {

  // set model vertices
  model.setVertices(vertices, {{ 0.5f, 0.5f, 0.5f }});

  float bLeft = FLT_MAX, bBottom = FLT_MAX, bFront = FLT_MAX;
  float bRight = -FLT_MAX, bTop = -FLT_MAX, bBack = -FLT_MAX;


  // calculate bounding box
  for (auto a : vertices) {
    bLeft = std::min(a[0], bLeft);
    bRight = std::max(a[0], bRight);
    bFront = std::min(a[1], bFront);
    bBack = std::max(a[1], bBack);
    bBottom = std::min(a[2], bBottom);
    bTop = std::max(a[2], bTop);
  }

  float tempBox[8][4] =
  {
    { bLeft, bFront, bBottom, 0.0 },
    { bLeft, bFront, bTop, 0.0 },
    { bLeft, bBack, bBottom, 0.0 },
    { bLeft, bBack, bTop, 0.0 },
    { bRight, bFront, bBottom, 0.0 },
    { bRight, bFront, bTop, 0.0 },
    { bRight, bBack, bBottom, 0.0 },
    { bRight, bBack, bTop, 0.0 }
  };

  for (std::size_t i = 0; i < 8; i++) {
    for (std::size_t j = 0; j < 4; j++) {
      modelBox[i][j] = tempBox[i][j];
    }
  }

}

void Context::setScene(const std::vector<std::array<float, 3>>& vertices, mat4x4 sunView) {

  for (std::size_t i = 0; i < 4; i++) {
    for (std::size_t j = 0; j < 4; j++) {
      view[j][i] = sunView[j][i];
    }
  }

  // set surface vertices
  surface.setVertices(vertices, {{ 1.f, 1.f, 1.f }});

  // calculate clipping planes in rendered coorinates
  float left(FLT_MAX);
  float right(-FLT_MAX);
  float bottom(FLT_MAX);
  float top(-FLT_MAX);
  float near_(-FLT_MAX);
  float far_(FLT_MAX);

  for (auto a : vertices) {
    vec4 point = { a[0], a[1], a[2], 0 };
    vec4 trans;
    mat4x4_mul_vec4(trans, view, point);
    left = std::min(trans[0], left);
    right = std::max(trans[0], right);
    bottom = std::min(trans[1], bottom);
    top = std::max(trans[1], top);
    //near_ = min(trans[2], near_);
    far_ = std::min(trans[2], far_);
  }

  // Use model box to determine near clipping plane
  for (std::size_t i = 0; i < 8; i++) {
    vec4 trans;
    mat4x4_mul_vec4(trans, view, modelBox[i]);
    near_ = std::max(trans[2], near_);
  }

  // account for camera position
  near_ -= 1.f;
  far_ -= 1.f;

  // Grow horizontal extents of view by one pixel on each side
  float deltaX = (right - left)/size;
  left -= deltaX;
  right += deltaX;

  // Grow vertical extents of view by one pixel on each side
  float deltaY = (top - bottom)/size;
  bottom -= deltaY;
  top += deltaY;

  // calculate pixel area (A[i]*cos(theta) for each pixel of the surface)
  // multiplies by the number of pixels to get projected sunlit surface fraction
  pixelArea = (right - left)*(top - bottom) / (size*size);

  // set projection matrix
  mat4x4_ortho(projection, left, right, bottom, top, -near_, -far_);
  setMVP();
}


void Context::showRendering()
{
  glfwSetWindowSize(window, size, size);
  glfwShowWindow(window);

  while (!glfwWindowShouldClose(window))
  {
    drawSurfaces();
    glfwSwapBuffers(window);
    glfwPollEvents();
  }

  glfwWindowHint(GLFW_VISIBLE, GL_FALSE);
  glfwHideWindow(window);
}

void Context::drawSurfaces()
{
  glViewport(0, 0, size, size);
  glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
  glDepthFunc(GL_LESS);
  model.draw();
  glDepthFunc(GL_EQUAL);
  surface.draw();
}

void Context::setMVP()
{
  mat4x4_mul(mvp, projection, view);
  glUniformMatrix4fv(mvpLocation, 1, GL_FALSE, (const GLfloat*)mvp);
}

std::size_t Context::countPixels() {
  glGenFramebuffers(1, &fbo);
  glGenRenderbuffers(1, &rbo);
  glBindFramebuffer(GL_FRAMEBUFFER, fbo);

  glBindRenderbuffer(GL_RENDERBUFFER, rbo);
  glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT24, size, size);
  glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, rbo);

  GLenum status = glCheckFramebufferStatus(GL_FRAMEBUFFER);
  if (status != GL_FRAMEBUFFER_COMPLETE) {
    throw;
  }

  glGenQueries(1, &query);
  glColorMask(GL_FALSE, GL_FALSE, GL_FALSE, GL_FALSE);

  glViewport(0, 0, size, size);
  glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
  glDepthFunc(GL_LESS);
  model.draw();
  glDepthFunc(GL_EQUAL);
  glBeginQuery(GL_SAMPLES_PASSED, query);
  surface.draw();
  glEndQuery(GL_SAMPLES_PASSED);

  // reset to default framebuffer
  glBindFramebuffer(GL_FRAMEBUFFER, 0);

  // wait until the result is available
  GLint ready(0);
  while (!ready) {
    glGetQueryObjectiv(query, GL_QUERY_RESULT_AVAILABLE, &ready);
  }

  // retrieve result
  GLint pixelCount;
  glGetQueryObjectiv(query, GL_QUERY_RESULT, &pixelCount);

  return pixelCount;

}

}
