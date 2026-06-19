#ifndef SHADER_H
# define SHADER_H

# include "glad/glad.h"

GLuint  shader_create(const char *vert_path, const char *frag_path);
void    shader_use(GLuint program);
void    shader_destroy(GLuint program);

#endif