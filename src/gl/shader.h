/* Copyright (c) 2017 Big Ladder Software LLC. All rights reserved.
 * See the LICENSE file for additional terms and conditions. */

#ifndef SHADER_H_
#define SHADER_H_

// Vendor
#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <courierr/courierr.h>

namespace Pumbra {

class GLShader {
public:
  GLShader(GLenum type, const char *source, const std::shared_ptr<Courierr::Courierr> &logger);
  ~GLShader() = default;
  [[nodiscard]] GLuint getInt() const;

private:
  GLuint shader;
  std::shared_ptr<Courierr::Courierr> logger;
};

} // namespace Pumbra

#endif // SHADER_H_
