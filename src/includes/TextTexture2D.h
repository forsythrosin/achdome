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
  }
  
  ~Font(){
    FT_Done_Face(face);
    FT_Done_FreeType(library);
  }

  FT_Library getLibrary(){
    return library;
  }

  FT_Face getFace(){
    return face;
  }
private:
  FT_Library library;
  FT_Face face;
  int height;
};


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


class TextTexture2D {
public:
  TextTexture2D(Font *f, std::string text){
    this->font = f;
    setText(text);
  }
  void setText(std::string text){
    auto characters = std::vector<GlyphStore*>();
    int height = 0;
    int width = 0;
    for(int i = 0; i < text.size(); i++){
      auto characterTexture = generateGlyphStore(text[i]);
      height = std::max(height, characterTexture->getHeight());
      width += characterTexture->getWidth();
      characters.push_back(characterTexture);
    }
    if(texture){
      delete texture;
    }
    texture = new Texture2D(width, height);

    int stringCoordX = 0;

    for(auto glyphIndex = 0u; glyphIndex < characters.size(); glyphIndex++){
      auto currentGlyph = characters[glyphIndex];
      
      for (unsigned int i = 0; i < currentGlyph->getWidth(); ++i) {
        for (unsigned int j = 0; j < currentGlyph->getHeight(); ++j){
          texture->set(i+stringCoordX, j, 0, currentGlyph->get(i, j));
          texture->set(i+stringCoordX, j, 1, currentGlyph->get(i, j));
          texture->set(i+stringCoordX, j, 2, currentGlyph->get(i, j));
          texture->set(i+stringCoordX, j, 3, currentGlyph->get(i, j));
        }
      }
      stringCoordX += currentGlyph->getWidth();
      delete currentGlyph;
    }
  }

  Texture2D* getTexture(){
    return texture;
  }
  
private:
  Font *font;
  Texture2D *texture = nullptr;
  
  GlyphStore* generateGlyphStore(char c){
    auto face = font->getFace();
    auto library = font->getLibrary();
    if(FT_Load_Glyph( face, FT_Get_Char_Index( face, c ), FT_LOAD_DEFAULT ))
      throw std::runtime_error("FT_Load_Glyph failed");
 
    // Move The Face's Glyph Into A Glyph Object.
    FT_Glyph glyph;
    if(FT_Get_Glyph( face->glyph, &glyph ))
      throw std::runtime_error("FT_Get_Glyph failed");
 
    // Convert The Glyph To A Bitmap.
    FT_Glyph_To_Bitmap( &glyph, ft_render_mode_normal, 0, 1 );
    FT_BitmapGlyph bitmap_glyph = (FT_BitmapGlyph)glyph;
 
    // This Reference Will Make Accessing The Bitmap Easier.
    FT_Bitmap& bitmap=bitmap_glyph->bitmap;
    
    GlyphStore *glyphTexture = new GlyphStore(bitmap.width, bitmap.rows);
    
    for(int j=0; j < bitmap.rows; j++) {
      for(int i=0; i < bitmap.width; i++){
        glyphTexture->set(i, j, (float)bitmap.buffer[i + bitmap.width*j]/256.0);
      }
    }
    return glyphTexture;
  }

};
