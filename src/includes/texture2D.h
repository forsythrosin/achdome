#pragma once

class Texture2D{
public:
  Texture2D(GLuint w, GLuint h, bool multisample = false){
    this->w = w;
    this->h = h;
    this->multisample = multisample;
    d = 4;
    glGenTextures(1, &textureID);

    glBindTexture(GL_TEXTURE_2D, textureID);
    glPixelStorei(GL_UNPACK_ALIGNMENT,1);
    data = new GLfloat[h*w*d];

    for (unsigned int i = 0; i < w; ++i) {
      for (unsigned int j = 0; j < h; ++j){
        data[indexTranslation(i,j,0)] = 0.0f;
        data[indexTranslation(i,j,1)] = 0.0f;
        data[indexTranslation(i,j,2)] = 0.0f;
        data[indexTranslation(i,j,3)] = 0.0f;
      }
    }

    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA32F, w, h, 0, GL_RGBA, GL_FLOAT, data);

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    // enforce no texture wrap
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
  }

  ~Texture2D(){
    glDeleteTextures(1, &textureID);
    delete[] data;
  }

  GLfloat get(GLuint i, GLuint j, GLuint k) const{
    return data[indexTranslation(i,j,k)];
  }

  void set(GLuint i, GLuint j, GLuint k, GLfloat v){
    data[indexTranslation(i,j,k)] = v;
  }

  GLuint getWidth(){
    return w;
  }

  GLuint getHeight(){
    return h;
  }

  operator GLuint() const {
    return textureID;
  }

  void operator()(GLenum texture) {
    glActiveTexture(texture);
    if (multisample) {
      glBindTexture(GL_TEXTURE_2D_MULTISAMPLE, textureID);
      glTexImage2DMultisample(GL_TEXTURE_2D_MULTISAMPLE, 8, GL_RGBA8, w, h, false);
    } else {
      glBindTexture(GL_TEXTURE_2D, textureID);
      glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA32F, w, h, 0, GL_RGBA, GL_FLOAT, data);
    }
  }

  void download() {
    glReadBuffer(GL_COLOR_ATTACHMENT0);
    glBindTexture(GL_TEXTURE_2D, textureID);
    glReadPixels(0, 0, this->w, this->h, GL_RGBA, GL_FLOAT, data);
  }

private:
  GLuint w, h, d;
  GLuint textureID;
  GLfloat *data;
  bool multisample;
  GLuint indexTranslation(GLuint i, GLuint j, GLuint k) const {
    return j*w*d + i*d + k;
  }
};
