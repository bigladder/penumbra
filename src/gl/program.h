/* Copyright (c) 2017 Big Ladder Software LLC. All rights reserved.
 * See the LICENSE file for additional terms and conditions. */

#ifndef PROGRAM_H_
#define PROGRAM_H_

// Vendor
#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <courierr/courierr.h>

namespace Pumbra {

class GLProgram {
public:
  GLProgram(const char *vertexSource, const char *fragmentSource, Courierr::Courierr *logger);
  ~GLProgram();
  [[nodiscard]] GLuint getInt() const;

private:
  GLuint program;
};

} // namespace Pumbra

#endif // PROGRAM_H_
