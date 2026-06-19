#ifndef TEXTURE_H
# define TEXTURE_H

# include "glad/glad.h"

GLuint  texture_load(const char *path);
void    texture_destroy(GLuint texture);

#endif