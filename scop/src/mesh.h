#ifndef MESH_H
# define MESH_H

# include "glad/glad.h"
# include "obj_parser.h"

typedef struct s_mesh
{
    GLuint  vao;
    GLuint  vbo;
    int     vertex_count;   // nombre total de vertices à dessiner
}   t_mesh;

t_mesh  *mesh_create(t_obj *obj);
void    mesh_draw(t_mesh *mesh);
void    mesh_free(t_mesh *mesh);

#endif