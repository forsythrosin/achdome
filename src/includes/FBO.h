#pragma once

#include <iostream>
#include <texture2D.h>

class FBO {
public:
  FBO(GLuint w, GLuint h) {
    this->w = w;
    this->h = h;

    // create new framebuffer
    glGenFramebuffers(1, &framebufferId);
    glBindFramebuffer(GL_FRAMEBUFFER, framebufferId);

    // create texture
    texture = new Texture2D(w, h);

    // init config
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D_MULTISAMPLE, *texture, 0);
    // glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, *texture, 0);

    // fail check
    assert(glCheckFramebufferStatus(GL_FRAMEBUFFER) == GL_FRAMEBUFFER_COMPLETE && "failed to init FBO");

    // unbind
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
  };

  ~FBO() {
    activate();
    glClearColor(0.0,0.0,0.0,0.0);
    glClear(GL_COLOR_BUFFER_BIT);

    delete texture;
    glDeleteFramebuffers(1, &framebufferId);
  };

  void activate() {
    glBindFramebuffer(GL_FRAMEBUFFER, framebufferId);
  };

  static void deactivate() {
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
  };

  operator GLuint() const {
    return framebufferId;
  }

  static void deactivateFramebuffers() {
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
  };

  Texture2D const * getTexture() const {
    return texture;
  };

  void download() {
    texture->download();
  }

private:
  GLuint w, h;
  GLuint framebufferId;
  Texture2D *texture;
  GLenum texUnit;
};
