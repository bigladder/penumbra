/* Copyright (c) 2017 Big Ladder Software LLC. All rights reserved.
 * See the LICENSE file for additional terms and conditions. */

// Penumbra
#include <gl/program.h>
#include <gl/shader.h>

namespace Pumbra {

GLProgram::GLProgram(const char *vertexSource, const char *fragmentSource,
                     const std::shared_ptr<Courierr::Courierr> &logger) {
  program = glCreateProgram();
  GLShader vertex(GL_VERTEX_SHADER, vertexSource, logger);
  glAttachShader(program, vertex.getInt());
  if (fragmentSource) {
    GLShader fragment(GL_FRAGMENT_SHADER, fragmentSource, logger);
    glAttachShader(program, fragment.getInt());
  }
  glLinkProgram(program);
}

GLProgram::~GLProgram() = default;

GLuint GLProgram::getInt() const {
  return program;
}

} // namespace Pumbra
