#include <renderer.h>
#include <shaderUtils.h>
#include <string>
#include <iostream>
#include <stdio.h>
#include <renderConfig.h>
#include "sgct/SGCTSettings.h"

Renderer::Renderer(sgct::Engine *gEngine) {
  this->gEngine = gEngine;
};

Renderer::~Renderer() {
  for (auto &pair : renderConfigs) {
    RenderConfig &rc = pair.second;
    // delete shaders
    sgct::ShaderManager::instance()->removeShaderProgram(std::to_string(rc.id));

    // delete FBOs
    for (auto fbo : rc.framebuffers) {
      delete fbo;
    }
  }
  renderConfigs.clear();
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
  Renderable *renderable, GLenum mode, std::string vert, std::string frag, bool spherical, bool useBiLinear) {
  RenderConfig rc(renderable, mode, vert, frag, spherical);

  rc.id = nextId++;
  init(rc);
  //std::cout << "add renderable with id = " << rc.id << std::endl;
  renderConfigs.emplace(rc.id, rc);
  return rc.id;
};

void Renderer::removeRenderable(int configId) {
  auto rcIt = renderConfigs.find(configId);
  if (rcIt == renderConfigs.end()) {
    //std::cout << "Could not find renderable" << std::endl;
    return;
  }

  RenderConfig &rc = rcIt->second;

  sgct::ShaderManager::instance()->removeShaderProgram(std::to_string(rc.id));

  // delete buffers + VAO
  rc.renderable->detach();

  resetFBO(configId);
  renderConfigs.erase(configId);
};

/**
 * Initializes buffers, vertex arrays and shaders for a Renderable
 * @param renderConfig config to initialize from
 */
void Renderer::init(RenderConfig &renderConfig) {
  // generate buffers + VAO
  renderConfig.renderable->attach();
  renderConfig.renderable->loadToGPU(renderConfig.sphericalCoords);

  // init shaders
  std::string vertShader = shaderUtils::pathToShader(renderConfig.vertShader);
  std::string fragShader = shaderUtils::pathToShader(renderConfig.fragShader);

  sgct::ShaderManager *sMan = sgct::ShaderManager::instance();
  sMan->addShaderProgram(std::to_string(renderConfig.id), vertShader, fragShader);

  // Uniform locations
  renderConfig.matrixLocation = sMan->getShaderProgram(std::to_string(renderConfig.id)).getUniformLocation("MVP");
  renderConfig.textureLocation = sMan->getShaderProgram(std::to_string(renderConfig.id)).getUniformLocation("fboTex");
  renderConfig.fboTexSizeLocation = sMan->getShaderProgram(std::to_string(renderConfig.id)).getUniformLocation("fboTexSize");
};

/**
 * Assign uniform to the specified RenderConfig
 * @param configId renderConfig ID (must have been added to this renderer)
 * @param uniform  any uniform available in uniformWrappers.h
 */
void Renderer::setUniform(int configId, AbstractUniform *uniform) {
  auto rcIt = renderConfigs.find(configId);
  if (rcIt == renderConfigs.end()) {
    //std::cout << "Trying to add uniform to non-existing RenderConfig" << std::endl;
    return;
  }
  RenderConfig &rc = rcIt->second;

  sgct::ShaderManager *sMan = sgct::ShaderManager::instance();

  sMan->bindShaderProgram(std::to_string(rc.id));
  rc.uniforms[uniform->getName()] = uniform;
  sgct::ShaderProgram program = sMan->getShaderProgram(std::to_string(rc.id));
  uniform->setLocation(program.getUniformLocation(uniform->getName()));

  sgct::ShaderManager::instance()->unBindShaderProgram();
};

/**
 * Renders the specified RenderConfig to its FBO, matching the corresponding stitchStep
 * @param configId   renderConfig ID
 * @param stitchStep (optional) sgct stitch step. Default 0
 */
void Renderer::renderToFBO(int configId, int stitchStep) {

  RenderConfig &renderConfig = renderConfigs.at(configId);

  GLint screenFBO;
  glGetIntegerv(GL_FRAMEBUFFER_BINDING, &screenFBO);

  // Generate FBOs
  if (renderConfig.framebuffers.size() < stitchStep + 1) {
    int fboWidth, fboHeight;
    gEngine->getActiveViewportSize(fboWidth, fboHeight);
    renderConfig.framebuffers.push_back(new FBO(fboWidth, fboHeight, renderConfig.useBilinear));

    // if we are attaching for the first time -> clear FBO
    renderConfig.framebuffers.at(stitchStep)->activate();
    glClearColor(0.0,0.0,0.0,0.0);
    glClear(GL_COLOR_BUFFER_BIT);
  }

  // Unbind screen FBO, render to our FBO, then bind back to screen FBO.
  renderConfig.framebuffers.at(stitchStep)->activate();
  render(configId);
  glBindFramebuffer(GL_FRAMEBUFFER, screenFBO);
};


/**
 * Resets FBO (for all stitchSteps) of a specified RenderConfig
 */
void Renderer::resetFBO(int configId) {
  auto rcIt = renderConfigs.find(configId);
  if (rcIt == renderConfigs.end()) {
    //std::cout << "Could not find renderable when resetting FBO." << std::endl;
    return;
  }
  RenderConfig &rc = rcIt->second;

  // delete FBOs
  for (auto fbo : rc.framebuffers) {
    delete fbo;
  }
  rc.framebuffers.clear();
}


/**
 * Renders the specified RenderConfig to the currently bound FBO
 * @param configId        renderConfig ID (i.e. position in renderConfigs vector)
 * @param configWithFBOId (optional) ID of renderConfig with FBO to send to shader. Default -1
 * @param stitchStep      (optional) sgct stitch step. Default 0
 */
void Renderer::render(int configId, int configWithFBOId, int stitchStep) {
  glEnable(GL_BLEND);
  glBlendEquationSeparate(GL_FUNC_ADD, GL_FUNC_ADD);
  glBlendFuncSeparate(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA, GL_ONE, GL_ONE);
  // glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

  // FXAA
  //glEnable(GL_MULTISAMPLE);
  //glEnable(GL_LINE_SMOOTH);
  // glEnable(GL_POLYGON_SMOOTH);
  //glHint(GL_LINE_SMOOTH_HINT, GL_NICEST);
  // glHint(GL_POLYGON_SMOOTH_HINT, GL_NICEST);

  //sgct::SGCTSettings *settings = sgct::SGCTSettings::instance();
  //settings->setDefaultFXAAState(true);
  //settings->setDefaultNumberOfAASamples(8);

  RenderConfig renderConfig = renderConfigs.at(configId);
  Renderable *renderable = renderConfig.renderable;

  // Upload new vertices if needed
  if (renderable->update) {
    renderable->loadToGPU(renderConfig.sphericalCoords);
    renderable->update = false;
  }

  // Define MVP matrix
  glm::mat4 MVP = gEngine->getActiveModelViewProjectionMatrix()*ROT_STAT;

  // Upload uniforms
  sgct::ShaderManager::instance()->bindShaderProgram(std::to_string(renderConfig.id));
  glUniformMatrix4fv(renderConfig.matrixLocation, 1, GL_FALSE, &MVP[0][0]);

  for (auto pair : renderConfig.uniforms) {
    if(pair.second->getLocation() != -1){
      pair.second->upload();
    }
  }

  if (configWithFBOId > -1 && renderConfigs.find(configWithFBOId) != renderConfigs.end()) {
    int fboWidth, fboHeight;
    gEngine->getActiveViewportSize(fboWidth, fboHeight);
    glUniform2f(renderConfig.fboTexSizeLocation, fboWidth, fboHeight);

    RenderConfig renderConfigFBO = renderConfigs.at(configWithFBOId);
    RenderConfig renderConfig = renderConfigs.at(configId);

    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D_MULTISAMPLE, *(renderConfigFBO.framebuffers.at(stitchStep)->getTexture()));
    // glBindTexture(GL_TEXTURE_2D, *(renderConfigFBO.framebuffers.at(stitchStep)->getTexture()));

    glUniform1i(renderConfig.textureLocation, 0);
  }

  // Bind position array
  glBindVertexArray(renderConfig.renderable->vertexArray);
  renderConfig.renderable->enableAttributes();

  // TODO: Don't seem to need to rebind buffers?
  // glBindBuffer(GL_ARRAY_BUFFER, renderConfig.positionBuffer);
  // glBindBuffer(GL_ARRAY_BUFFER, renderConfig.colorBuffer);
  // glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, renderConfig.indexBuffer);

  // Draw!
  int elementCount = renderable->getElementCount();
  int vertsPerElement = renderable->getVertsPerElement();
  glDrawElements(
    renderConfig.mode,
    elementCount*vertsPerElement,
    GL_UNSIGNED_INT,
    0
  );

  // Clean up + unbind
  renderable->disableAttributes();
  glBindVertexArray(0);
  sgct::ShaderManager::instance()->unBindShaderProgram();
  glDisable(GL_BLEND);
};

glm::mat4 Renderer::getMVP() {
  return gEngine->getActiveModelViewProjectionMatrix()*ROT_STAT;
}
