#pragma once

#include <external/freetype/ft2build.h>
#include <external/freetype/freetype.h>
#include <external/freetype/ftglyph.h>
#include <external/freetype/ftoutln.h>
#include <external/freetype/fttrigon.h>
#include <string>
#include <vector>
#include <texture2D.h>
#include <stdexcept>
#include <map>
#include <algorithm>
#include <iostream>
class GlyphStore{
public:
  GlyphStore(int w, int h){
    width = w;
    height = h;
    data = new GLfloat[w*h];
  }

  ~GlyphStore(){
    delete[] data;
  }
  
  GLfloat get(int i, int j){
    return data[indexTranslation(i,j)];
  }

  void set(int i, int j, float v){
    data[indexTranslation(i,j)] = v;
  }
  
  int getWidth(){ return width; };
  int getHeight(){ return height; };
  
private:
  GLuint indexTranslation(GLuint i, GLuint j) const {
    return j*width + i;
  }
  int width, height;
  GLfloat *data;
};


class Font{
public:
  Font(std::string pathToFont, int h){
    this->height=h;
 
    if (FT_Init_FreeType( &library )){
      throw std::runtime_error("FT_Init_FreeType failed");
    }
    
 
    if (FT_New_Face( library, pathToFont.c_str(), 0, &face )){
      throw std::runtime_error("FT_New_Face failed (there is probably a problem with your font file)");
    }
    
    FT_Set_Char_Size( face, height << 6, height << 6, 96, 96);
    unsigned char i = 0 ;
    int maxHeight = 0;
    do {
      auto glyph = generateGlyphStore(i);
      maxHeight = std::max(maxHeight, glyph->getHeight());
    }
    while ( ++i ) ;
    fontMaxHeight = maxHeight;

  }
  
  ~Font(){
    FT_Done_Face(face);
    FT_Done_FreeType(library);
    for(auto glyphIt : glyphCache){
      delete glyphIt.second;
    }
  }

  int getFontHeight(){
    return fontMaxHeight;
  }
  
  GlyphStore* generateGlyphStore(unsigned char c){
    if(glyphCache.count(c) != 0){
      return glyphCache[c];
    }
    if(FT_Load_Glyph( face, FT_Get_Char_Index( face, c ), FT_LOAD_DEFAULT ))
      throw std::runtime_error("FT_Load_Glyph failed");
 
    // Move The Face's Glyph Into A Glyph Object.
    FT_Glyph glyph;
    if(FT_Get_Glyph( face->glyph, &glyph ))
      throw std::runtime_error("FT_Get_Glyph failed");
 
    // Convert The Glyph To A Bitmap.
    int advanceX = glyph->advance.x / 65536.0;
    
    FT_Glyph_To_Bitmap( &glyph, ft_render_mode_normal, 0, 1 );
    FT_BitmapGlyph bitmap_glyph = (FT_BitmapGlyph)glyph;

    int bearing = bitmap_glyph->left; 
    if(bearing < 0) {
      bearing = 0;
    }
 
    // This Reference Will Make Accessing The Bitmap Easier.
    FT_Bitmap& bitmap=bitmap_glyph->bitmap;
    
    GlyphStore *glyphTexture = new GlyphStore(advanceX, bitmap.rows);
    
    for(int j=0; j < bitmap.rows; j++) {
      for(int i=0; i < bitmap.width; i++){
        glyphTexture->set(i+bearing, j, (float)bitmap.buffer[i + bitmap.width*j]/256.0);
      }
    }
    glyphCache[c] = glyphTexture;
    return glyphTexture;
  }

private:
  FT_Library library;
  FT_Face face;
  int height;
  int fontMaxHeight;
  std::map<unsigned char, GlyphStore*> glyphCache;
};




class TextTexture2D {
public:
  TextTexture2D(Font *f, std::string text){
    this->font = f;
    setText(text);
  }
  void setText(std::string text){
    text = UTF8toISO8859_1(text.c_str());
    if(textCache.count(text) > 0){
      texture = textCache[text];
      return;
    }
    else{
      texture = nullptr;
    }

    auto characters = std::vector<GlyphStore*>();
    int height = font->getFontHeight();
    int width = 0;
    for(int i = 0; i < text.size(); i++){
      auto characterTexture = font->generateGlyphStore(text[i]);
      width += characterTexture->getWidth();
      characters.push_back(characterTexture);
    }

    texture = new Texture2D(width, height);
    textCache[text] = texture;
    int stringCoordX = 0;

    for(auto glyphIndex = 0u; glyphIndex < characters.size(); glyphIndex++){
      auto currentGlyph = characters[glyphIndex];
      int heightCorrection = height - currentGlyph->getHeight();
      for (unsigned int i = 0; i < currentGlyph->getWidth(); ++i) {
        for (unsigned int j = 0; j < currentGlyph->getHeight(); ++j){
          texture->set(i+stringCoordX, j+heightCorrection, 0, currentGlyph->get(i, j));
          texture->set(i+stringCoordX, j+heightCorrection, 1, currentGlyph->get(i, j));
          texture->set(i+stringCoordX, j+heightCorrection, 2, currentGlyph->get(i, j));
          texture->set(i+stringCoordX, j+heightCorrection, 3, currentGlyph->get(i, j));
        }
      }
      stringCoordX += currentGlyph->getWidth();
    }
  }

  Texture2D* getTexture(){
    return texture;
  }
  
private:
  Font *font;
  Texture2D *texture = nullptr;

  std::map<std::string, Texture2D*> textCache;
  
  std::string UTF8toISO8859_1(const char * in){
    std::string out;
    if (in == NULL)
      return out;

    unsigned int codepoint;
    while (*in != 0)
      {
        unsigned char ch = static_cast<unsigned char>(*in);
        if (ch <= 0x7f)
          codepoint = ch;
        else if (ch <= 0xbf)
          codepoint = (codepoint << 6) | (ch & 0x3f);
        else if (ch <= 0xdf)
          codepoint = ch & 0x1f;
        else if (ch <= 0xef)
          codepoint = ch & 0x0f;
        else
          codepoint = ch & 0x07;
        ++in;
        if (((*in & 0xc0) != 0x80) && (codepoint <= 0x10ffff))
          {
            if (codepoint <= 255)
              {
                out.append(1, static_cast<char>(codepoint));
              }
            else
              {
                // do whatever you want for out-of-bounds characters
              }
          }
      }
    return out;
  }

};

