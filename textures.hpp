#ifndef TEXTURES_HPP
#define TEXTURES_HPP
#include <FreeImage.h>
#include <GL/glut.h>
#include <string>
void load_texture(const std::string &path, GLuint *idx);
#endif
