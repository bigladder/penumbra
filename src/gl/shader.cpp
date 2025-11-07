/* Copyright (c) 2017 Big Ladder Software LLC. All rights reserved.
 * See the LICENSE file for additional terms and conditions. */

// Standard
#include <iostream>

// Penumbra
#include <penumbra/logging.h>
#include "shader.h"
#include <format>

namespace Penumbra {
GLShader::GLShader(GLenum type, const char *source, Courier::Courier *logger_in)
    : logger(logger_in) {
  GLint shader_ok;
  GLsizei log_length;
  char info_log[8192];

  shader = glCreateShader(type);
  if (shader != 0) {
    glShaderSource(shader, 1, (const GLchar **)&source, nullptr);
    glCompileShader(shader);
    glGetShaderiv(shader, GL_COMPILE_STATUS, &shader_ok);
    if (shader_ok != GL_TRUE) {
      glGetShaderInfoLog(shader, 8192, &log_length, info_log);
      glDeleteShader(shader);
      shader = 0;
      std::string shader_type_string = (type == GL_FRAGMENT_SHADER) ? "fragment" : "vertex";
      logger->send_info(std::format("OpenGL {} shader: {}", shader_type_string, info_log));
      logger->send_error(std::format("Unable to compile {} shader.", shader_type_string));
    }
  }
}

GLuint GLShader::get() const {
  return shader;
}

} // namespace Penumbra
