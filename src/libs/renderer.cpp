#include <renderer.h>
#include <shaderUtils.h>
#include <string>
#include <iostream>

Renderer::Renderer(sgct::Engine *gEngine) {
  this->gEngine = gEngine;
};

/**
 * Adds a Renderable to this renderer and wraps it in a RenderConfig from the specified parameters.
 * Each renderable added will make one draw call when sent to render(). Renderables can be added
 * multiple times with different configs.
 * @param  renderable Renderable to add
 * @param  mode       Draw mode (e.g. GL_TRIANGLES or GL_LINES)
 * @param  vert       Vertex shader to use
 * @param  frag       Fragment shader to use
 * @param  spherical  (optional) Use spherical or cartesian coordinates. Default false
 * @return            index/ID of the created RenderConfig (i.e. position in renderConfigs vector)
 */
int Renderer::addRenderable(
  Renderable *renderable, GLenum mode, std::string vert, std::string frag, bool spherical) {
  RenderConfig rc(renderable, mode, vert, frag, spherical);

  rc.id = renderConfigs.size();
  init(rc);
  renderConfigs.push_back(rc);
  return rc.id;
};

/**
 * Initializes buffers, vertex arrays and shaders for a Renderable
 * @param renderConfig config to initialize from
 */
void Renderer::init(RenderConfig &renderConfig) {
  // generate VAO
  glGenVertexArrays(1, &(renderConfig.VAO));
  glBindVertexArray(renderConfig.VAO);

  // generate buffers
  glGenBuffers(1, &(renderConfig.VBO));
  glGenBuffers(1, &(renderConfig.IBO));

  loadToGPU(renderConfig);

  // init shaders
  std::string vertShader = shaderUtils::pathToShader(renderConfig.vertShader);
  std::string fragShader = shaderUtils::pathToShader(renderConfig.fragShader);
  sgct::ShaderManager::instance()->addShaderProgram(std::to_string(renderConfig.id), vertShader, fragShader);
  sgct::ShaderManager::instance()->bindShaderProgram(std::to_string(renderConfig.id));

  // Uniform locations
  renderConfig.matrixLocation = sgct::ShaderManager::instance()->getShaderProgram(std::to_string(renderConfig.id)).getUniformLocation("MVP");
  renderConfig.textureLocation = sgct::ShaderManager::instance()->getShaderProgram(std::to_string(renderConfig.id)).getUniformLocation("fboTex");
  renderConfig.fboTexSizeLocation = sgct::ShaderManager::instance()->getShaderProgram(std::to_string(renderConfig.id)).getUniformLocation("fboTexSize");

  sgct::ShaderManager::instance()->unBindShaderProgram();
};

/**
 * Uploads vertex + element data to the GPU
 * @param renderConfig config to upload data from
 */
void Renderer::loadToGPU(RenderConfig &renderConfig) {
  // Determine spherical or cartesian style vertices
  int vertexDim = renderConfig.sphericalCoords ? 2 : 3;

  Renderable *renderable = renderConfig.renderable;
  const GLfloat* vertexData = vertexDim == 2 ?
    renderable->getSphericalVertexData():
    renderable->getCartesianVertexData();
  const GLuint* elementData = renderable->getElementData();

  // Upload vertices
  glBindBuffer(GL_ARRAY_BUFFER, renderConfig.VBO);
  glBufferData(
    GL_ARRAY_BUFFER,
    renderable->getVertexCount()*sizeof(GLfloat)*vertexDim,
    vertexData,
    GL_STATIC_DRAW
  );

  // Use previous buffer as vertex buffer
  glEnableVertexAttribArray(0);
  glVertexAttribPointer(0, vertexDim, GL_FLOAT, GL_FALSE, 0, 0);

  // Upload elements (e.g. triangles or line segments)
  glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, renderConfig.IBO);
  glBufferData(
    GL_ELEMENT_ARRAY_BUFFER,
    renderable->getElementCount()*sizeof(GLuint)*renderable->getVertsPerElement(),
    elementData,
    GL_STATIC_DRAW
  );

  // Unbind
  glBindVertexArray(0);
  glBindBuffer(GL_ARRAY_BUFFER, 0);
  glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
};

/**
 * Renders the specified RenderConfig to its FBO, matching the corresponding stitchStep
 * @param configId   renderConfig ID (i.e. position in renderConfigs vector)
 * @param stitchStep (optional) sgct stitch step. Default 0
 */
void Renderer::renderToFBO(int configId, int stitchStep) {
  RenderConfig &renderConfig = renderConfigs.at(configId);

  // Generate FBOs
  if (renderConfig.framebuffers.size() < stitchStep + 1) {
    int fboWidth, fboHeight;
    gEngine->getActiveViewportSize(fboWidth, fboHeight);
    renderConfig.framebuffers.push_back(new FBO(fboWidth, fboHeight));
  }

  // Unbind screen FBO, render to our FBO, then bind back to screen FBO.
  GLint screenFBO;
  glGetIntegerv(GL_FRAMEBUFFER_BINDING, &screenFBO);
  renderConfig.framebuffers.at(stitchStep)->activate();
  render(configId);
  glBindFramebuffer(GL_FRAMEBUFFER, screenFBO);
};

/**
 * Renders the specified RenderConfig to the currently bound FBO
 * @param configId        renderConfig ID (i.e. position in renderConfigs vector)
 * @param configWithFBOId (optional) ID of renderConfig with FBO to send to shader. Default -1
 * @param stitchStep      (optional) sgct stitch step. Default 0
 */
void Renderer::render(int configId, int configWithFBOId, int stitchStep) {
  RenderConfig renderConfig = renderConfigs.at(configId);
  Renderable *renderable = renderConfig.renderable;

  // Upload new vertices if needed
  if (renderable->update) {
    loadToGPU(renderConfig);
    renderable->update = false;
  }

  // Define MVP matrix
  rot += 0.04f;
  glm::mat4 rot_stat = glm::rotate(glm::mat4(1.0f), DOME_ROTATION, glm::vec3(1.0f, 0.0f, 0.0f));
  glm::mat4 rot_mat = glm::rotate(glm::mat4(1.0f), rot, glm::vec3(0.0f, 0.0f, 1.0f));
  glm::mat4 scene_mat = glm::translate(glm::mat4(1.0f), glm::vec3(0.0, -1.0, -25.0))*rot_stat*rot_mat;
  glm::mat4 MVP = gEngine->getActiveModelViewProjectionMatrix()*rot_stat;

  // Upload uniforms
  sgct::ShaderManager::instance()->bindShaderProgram(std::to_string(renderConfig.id));
  glUniformMatrix4fv(renderConfig.matrixLocation, 1, GL_FALSE, &MVP[0][0]);

  if (configWithFBOId > -1 && configWithFBOId < renderConfigs.size()) {
    int fboWidth, fboHeight;
    gEngine->getActiveViewportSize(fboWidth, fboHeight);
    glUniform2f(renderConfig.fboTexSizeLocation, fboWidth, fboHeight);

    RenderConfig renderConfigFBO = renderConfigs.at(configWithFBOId);
    RenderConfig renderConfig = renderConfigs.at(configId);

    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, *(renderConfigFBO.framebuffers.at(stitchStep)->getTexture()));

    glUniform1i(renderConfig.textureLocation, 0);
  }

  // Bind vertex + element/index buffers
  glBindVertexArray(renderConfig.VAO);
  glEnableVertexAttribArray(0);
  glBindBuffer(GL_ARRAY_BUFFER, renderConfig.VBO);
  glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, renderConfig.IBO);

  // Draw!
  glDrawElements(
    renderConfig.mode,
    renderable->getElementCount()*renderable->getVertsPerElement(),
    GL_UNSIGNED_INT,
    0
  );

  // Clean up + unbind
  glDisableVertexAttribArray(0);
  glBindVertexArray(0);
  sgct::ShaderManager::instance()->unBindShaderProgram();
};
