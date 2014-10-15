#include <renderer.h>
#include <shaderUtils.h>
#include <string>
#include <iostream>

Renderer::Renderer(sgct::Engine *gEngine) {
  this->gEngine = gEngine;
};

int Renderer::addRenderable(
  Renderable *renderable, GLenum mode, std::string vert, std::string frag, bool spherical) {
  RenderConfig rc(renderable, mode, vert, frag, spherical);

  init(rc);
  renderConfigs.push_back(rc);
  return rc.id;
};

void Renderer::init(RenderConfig &renderConfig) {
  // generate VAO
  glGenVertexArrays(1, &(renderConfig.VAO));
  glBindVertexArray(renderConfig.VAO);

  // generate buffers
  glGenBuffers(1, &(renderConfig.VBO));
  glGenBuffers(1, &(renderConfig.IBO));

  loadToGPU(renderConfig);

  // init shaders
  renderConfig.id = renderConfigs.size();

  std::string vertShader = shaderUtils::pathToShader(renderConfig.vertShader);
  std::string fragShader = shaderUtils::pathToShader(renderConfig.fragShader);
  sgct::ShaderManager::instance()->addShaderProgram(std::to_string(renderConfig.id), vertShader, fragShader);
  sgct::ShaderManager::instance()->bindShaderProgram(std::to_string(renderConfig.id));
  renderConfig.Matrix_Loc = sgct::ShaderManager::instance()->getShaderProgram(std::to_string(renderConfig.id)).getUniformLocation("MVP");

  sgct::ShaderManager::instance()->unBindShaderProgram();
};

void Renderer::loadToGPU(RenderConfig &renderConfig) {
  int vertexDim = renderConfig.sphericalCoords ? 2 : 3;

  Renderable *renderable = renderConfig.renderable;
  const GLfloat* vertexData = vertexDim == 2 ?
    renderable->getSphericalVertexData():
    renderable->getCartesianVertexData();
  const GLuint* elementData = renderable->getElementData();

  glBindBuffer(GL_ARRAY_BUFFER, renderConfig.VBO);
  glBufferData(
    GL_ARRAY_BUFFER,
    renderable->getVertexCount()*sizeof(GLfloat)*vertexDim,
    vertexData,
    GL_STATIC_DRAW
  );

  glEnableVertexAttribArray(0);
  glVertexAttribPointer(0, vertexDim, GL_FLOAT, GL_FALSE, 0, 0);

  glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, renderConfig.IBO);
  glBufferData(
    GL_ELEMENT_ARRAY_BUFFER,
    renderable->getElementCount()*sizeof(GLuint)*renderable->getVertsPerElement(),
    elementData,
    GL_STATIC_DRAW
  );

  glBindVertexArray(0); //unbind
  glBindBuffer(GL_ARRAY_BUFFER, 0);
  glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
};

void Renderer::render(int configId) {
  RenderConfig renderConfig = renderConfigs.at(configId);
  Renderable *renderable = renderConfig.renderable;

  if (renderable->update) {
    loadToGPU(renderConfig);
    renderable->update = false;
  }

  rot += 0.04f;
  glm::mat4 rot_stat = glm::rotate(glm::mat4(1.0f), -60.0f, glm::vec3(1.0f, 0.0f, 0.0f));
  glm::mat4 rot_mat = glm::rotate(glm::mat4(1.0f), rot, glm::vec3(0.0f, 0.0f, 1.0f));
  glm::mat4 scene_mat = glm::translate(glm::mat4(1.0f), glm::vec3(0.0, -1.0, -25.0))*rot_stat*rot_mat;
  glm::mat4 MVP = gEngine->getActiveModelViewProjectionMatrix() * scene_mat;

  sgct::ShaderManager::instance()->bindShaderProgram(std::to_string(renderConfig.id));

  glUniformMatrix4fv(renderConfig.Matrix_Loc, 1, GL_FALSE, &MVP[0][0]);

  glBindVertexArray(renderConfig.VAO);
  glEnableVertexAttribArray(0);
  glBindBuffer(GL_ARRAY_BUFFER, renderConfig.VBO);
  glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, renderConfig.IBO);
  glDrawElements(
    renderConfig.mode,
    renderable->getElementCount()*renderable->getVertsPerElement(),
    GL_UNSIGNED_INT,
    0
  );
  glDisableVertexAttribArray(0);

  //unbind
  glBindVertexArray(0);
  sgct::ShaderManager::instance()->unBindShaderProgram();
};

void Renderer::renderAll() {
  for (int i = 0; i < renderConfigs.size(); ++i) {
    render(i);
  }
};
