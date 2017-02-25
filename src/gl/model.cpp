/* Copyright (c) 2017 Big Ladder Software LLC. All rights reserved.
* See the LICENSE file for additional terms and conditions. */

// Penumbra
#include <gl/model.h>

namespace Pumbra {
GLModel::~GLModel() {
  glDeleteVertexArrays(1, &vao);
  glDeleteBuffers(1, &vbo);
}

void GLModel::setVertices(const std::vector<float>& vertices) {

  vertexArray = vertices;
  numVerts = vertices.size();
  // Set up vertex array object
  glGenVertexArrays(1, &vao);
  glBindVertexArray(vao);

  // Set up array buffer to store vertex information
  glGenBuffers(1, &vbo);
  glBindBuffer(GL_ARRAY_BUFFER, vbo);
  glBufferData(GL_ARRAY_BUFFER, sizeof(float)*vertices.size(), &vertices[0], GL_STATIC_DRAW);

  // Set drawing pointers for current vertex buffer
  glEnableVertexAttribArray(0);
  glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(float) * 3, (void*)0);

}

void GLModel::draw(GLint first, GLsizei count) {
  glBindVertexArray(vao);
  glDrawArrays(GL_TRIANGLES, first, count);
}

}
