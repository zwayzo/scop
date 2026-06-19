#ifndef MAT4_H
# define MAT4_H

# include "vec3.h"

/*
** Matrice 4x4 stockée en colonne-majeure (column-major)
** C'est l'ordre qu'OpenGL attend nativement
**
** m[colonne][ligne]
**
** Visuellement :
** | m[0][0]  m[1][0]  m[2][0]  m[3][0] |
** | m[0][1]  m[1][1]  m[2][1]  m[3][1] |
** | m[0][2]  m[1][2]  m[2][2]  m[3][2] |
** | m[0][3]  m[1][3]  m[2][3]  m[3][3] |
*/
typedef struct s_mat4
{
    float   m[4][4];
}   t_mat4;

t_mat4  mat4_identity(void);
t_mat4  mat4_multiply(t_mat4 a, t_mat4 b);
t_mat4  mat4_translate(t_mat4 m, t_vec3 v);
t_mat4  mat4_scale(t_mat4 m, t_vec3 v);
t_mat4  mat4_rotate_x(t_mat4 m, float angle);
t_mat4  mat4_rotate_y(t_mat4 m, float angle);
t_mat4  mat4_rotate_z(t_mat4 m, float angle);
t_mat4  mat4_perspective(float fov, float aspect, float near, float far);
t_mat4  mat4_look_at(t_vec3 eye, t_vec3 center, t_vec3 up);
float   *mat4_ptr(t_mat4 *m);

#endif