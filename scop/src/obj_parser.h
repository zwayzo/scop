#ifndef OBJ_PARSER_H
# define OBJ_PARSER_H

# include "math/vec3.h"

# define MAX_VERTICES  100000
# define MAX_FACES     100000

typedef struct s_face
{
    int v[3];   // indices des 3 vertices de la face
}   t_face;

typedef struct s_obj
{
    t_vec3  *vertices;      // tableau des points 3D
    t_face  *faces;         // tableau des faces
    int     vertex_count;
    int     face_count;
    t_vec3  center;         // centre géométrique de l'objet
}   t_obj;

t_obj   *obj_load(const char *path);
void    obj_free(t_obj *obj);

#endif