/* Copyright (c) 2017 Big Ladder Software LLC. All rights reserved.
* See the LICENSE file for additional terms and conditions. */

// Penumbra
#include <gl/model.h>

#ifdef __APPLE__
#define glGenVertexArraysX glGenVertexArraysAPPLE
#define glBindVertexArrayX glBindVertexArrayAPPLE
#define glDeleteVertexArraysX glDeleteVertexArraysAPPLE
#else
#define glGenVertexArraysX glGenVertexArrays
#define glBindVertexArrayX glBindVertexArray
#define glDeleteVertexArraysX glDeleteVertexArrays
#endif

namespace Pumbra {

SurfaceBuffer::SurfaceBuffer(GLint begin, GLint count) :
  begin(begin),
  count(count)
{}

GLModel::~GLModel() {
  clearModel();
}

void GLModel::clearModel() {
  glDeleteVertexArraysX(1, &vao);
  glDeleteBuffers(1, &vbo);
}

void GLModel::setVertices(const std::vector<float>& vertices) {

  vertexArray = vertices;
  numVerts = vertices.size();
  // Set up vertex array object
  glGenVertexArraysX(1, &vao);
  glBindVertexArrayX(vao);

  // Set up array buffer to store vertex information
  glGenBuffers(1, &vbo);
  glBindBuffer(GL_ARRAY_BUFFER, vbo);
  glBufferData(GL_ARRAY_BUFFER, sizeof(float)*vertices.size(), &vertices[0], GL_STATIC_DRAW);

  // Set drawing pointers for current vertex buffer
  glEnableVertexAttribArray(0);
  glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(float) * 3, (void*)0);

}

void GLModel::drawSurface(SurfaceBuffer surfaceBuffer) {
  glDrawArrays(GL_TRIANGLES, surfaceBuffer.begin, surfaceBuffer.count);
}

void GLModel::drawAll() {
  glDrawArrays(GL_TRIANGLES, 0, numVerts/vertexSize);
}

/*void GLModel::drawExcept(GLint first, GLsizei count) {
  glDrawArrays(GL_TRIANGLES, 0, first - 1);
  glDrawArrays(GL_TRIANGLES, first + count, numVerts/vertexSize);
}*/

}
