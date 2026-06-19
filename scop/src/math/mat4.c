#include "mat4.h"
#include <math.h>
#include <string.h>

/*
** mat4_identity — retourne la matrice identité
** Multiplier par elle ne change rien (comme × 1 pour les nombres)
**
** | 1 0 0 0 |
** | 0 1 0 0 |
** | 0 0 1 0 |
** | 0 0 0 1 |
*/
t_mat4  mat4_identity(void)
{
    t_mat4  m;

    memset(&m, 0, sizeof(t_mat4));
    m.m[0][0] = 1.0f;
    m.m[1][1] = 1.0f;
    m.m[2][2] = 1.0f;
    m.m[3][3] = 1.0f;
    return (m);
}

/*
** mat4_multiply — multiplie deux matrices ensemble
** L'ordre est important : A × B ≠ B × A en général
** On calcule : résultat[col][row] = somme des produits ligne/colonne
*/
t_mat4  mat4_multiply(t_mat4 a, t_mat4 b)
{
    t_mat4  res;
    int     col;
    int     row;
    int     k;

    memset(&res, 0, sizeof(t_mat4));
    col = 0;
    while (col < 4)
    {
        row = 0;
        while (row < 4)
        {
            k = 0;
            while (k < 4)
            {
                res.m[col][row] += a.m[k][row] * b.m[col][k];
                k++;
            }
            row++;
        }
        col++;
    }
    return (res);
}

/*
** mat4_translate — déplace un objet de (v.x, v.y, v.z)
** La translation va dans la dernière colonne de la matrice
**
** | 1 0 0 tx |
** | 0 1 0 ty |
** | 0 0 1 tz |
** | 0 0 0 1  |
*/
t_mat4  mat4_translate(t_mat4 m, t_vec3 v)
{
    t_mat4  res;

    res = m;
    res.m[3][0] += v.x;
    res.m[3][1] += v.y;
    res.m[3][2] += v.z;
    return (res);
}

/*
** mat4_scale — redimensionne un objet
**
** | sx 0  0  0 |
** | 0  sy 0  0 |
** | 0  0  sz 0 |
** | 0  0  0  1 |
*/
t_mat4  mat4_scale(t_mat4 m, t_vec3 v)
{
    t_mat4  res;

    res = m;
    res.m[0][0] *= v.x;
    res.m[1][1] *= v.y;
    res.m[2][2] *= v.z;
    return (res);
}

/*
** mat4_rotate_x — rotation autour de l'axe X (tangage)
** angle en radians
**
** | 1   0     0   0 |
** | 0  cos  -sin  0 |
** | 0  sin   cos  0 |
** | 0   0     0   1 |
*/
t_mat4  mat4_rotate_x(t_mat4 m, float angle)
{
    t_mat4  rot;
    float   c;
    float   s;

    rot = mat4_identity();
    c = cosf(angle);
    s = sinf(angle);
    rot.m[1][1] =  c;
    rot.m[1][2] =  s;
    rot.m[2][1] = -s;
    rot.m[2][2] =  c;
    return (mat4_multiply(m, rot));
}

/*
** mat4_rotate_y — rotation autour de l'axe Y (lacet)
**
** |  cos  0  sin  0 |
** |   0   1   0   0 |
** | -sin  0  cos  0 |
** |   0   0   0   1 |
*/
t_mat4  mat4_rotate_y(t_mat4 m, float angle)
{
    t_mat4  rot;
    float   c;
    float   s;

    rot = mat4_identity();
    c = cosf(angle);
    s = sinf(angle);
    rot.m[0][0] =  c;
    rot.m[0][2] = -s;
    rot.m[2][0] =  s;
    rot.m[2][2] =  c;
    return (mat4_multiply(m, rot));
}

/*
** mat4_rotate_z — rotation autour de l'axe Z (roulis)
**
** | cos  -sin  0  0 |
** | sin   cos  0  0 |
** |  0     0   1  0 |
** |  0     0   0  1 |
*/
t_mat4  mat4_rotate_z(t_mat4 m, float angle)
{
    t_mat4  rot;
    float   c;
    float   s;

    rot = mat4_identity();
    c = cosf(angle);
    s = sinf(angle);
    rot.m[0][0] =  c;
    rot.m[0][1] =  s;
    rot.m[1][0] = -s;
    rot.m[1][1] =  c;
    return (mat4_multiply(m, rot));
}

/*
** mat4_perspective — crée la matrice de projection perspective
** fov    : champ de vision vertical en radians (ex: 45° = M_PI/4)
** aspect : largeur / hauteur de la fenêtre
** near   : plan de clipping proche (ex: 0.1)
** far    : plan de clipping lointain (ex: 100.0)
**
** C'est elle qui donne l'effet "objets lointains = plus petits"
*/
t_mat4  mat4_perspective(float fov, float aspect, float near, float far)
{
    t_mat4  m;
    float   tan_half_fov;

    memset(&m, 0, sizeof(t_mat4));
    tan_half_fov = tanf(fov / 2.0f);
    m.m[0][0] = 1.0f / (aspect * tan_half_fov);
    m.m[1][1] = 1.0f / tan_half_fov;
    m.m[2][2] = -(far + near) / (far - near);
    m.m[2][3] = -1.0f;
    m.m[3][2] = -(2.0f * far * near) / (far - near);
    return (m);
}

/*
** mat4_look_at — crée la matrice "view" (point de vue caméra)
** eye    : position de la caméra
** center : point que la caméra regarde
** up     : direction "vers le haut" (généralement (0,1,0))
*/
t_mat4  mat4_look_at(t_vec3 eye, t_vec3 center, t_vec3 up)
{
    t_mat4  m;
    t_vec3  f;  // vecteur "forward" — direction du regard
    t_vec3  r;  // vecteur "right" — droite de la caméra
    t_vec3  u;  // vecteur "up" réel de la caméra

    // Calcule les 3 axes de la caméra
    f = vec3_normalize(vec3_sub(center, eye));
    r = vec3_normalize(vec3_cross(f, up));
    u = vec3_cross(r, f);

    memset(&m, 0, sizeof(t_mat4));
    m.m[0][0] =  r.x;
    m.m[1][0] =  r.y;
    m.m[2][0] =  r.z;
    m.m[0][1] =  u.x;
    m.m[1][1] =  u.y;
    m.m[2][1] =  u.z;
    m.m[0][2] = -f.x;
    m.m[1][2] = -f.y;
    m.m[2][2] = -f.z;
    m.m[3][0] = -vec3_dot(r, eye);
    m.m[3][1] = -vec3_dot(u, eye);
    m.m[3][2] =  vec3_dot(f, eye);
    m.m[3][3] =  1.0f;
    return (m);
}

/*
** mat4_ptr — retourne un pointeur vers les données brutes de la matrice
** C'est ce qu'on passe à glUniformMatrix4fv() pour envoyer la matrice au GPU
*/
float   *mat4_ptr(t_mat4 *m)
{
    return (&m->m[0][0]);
}