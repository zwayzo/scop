#include "vec3.h"
#include <math.h>

// Addition de deux vecteurs : (a.x+b.x, a.y+b.y, a.z+b.z)
t_vec3  vec3_add(t_vec3 a, t_vec3 b)
{
    return ((t_vec3){a.x + b.x, a.y + b.y, a.z + b.z});
}

// Soustraction : direction de b vers a
t_vec3  vec3_sub(t_vec3 a, t_vec3 b)
{
    return ((t_vec3){a.x - b.x, a.y - b.y, a.z - b.z});
}

// Multiplication par un scalaire : agrandit/rétrécit le vecteur
t_vec3  vec3_scale(t_vec3 v, float s)
{
    return ((t_vec3){v.x * s, v.y * s, v.z * s});
}

// Produit scalaire : mesure l'alignement entre deux vecteurs
// Résultat = 0 si perpendiculaires, 1 si parallèles (normalisés)
float   vec3_dot(t_vec3 a, t_vec3 b)
{
    return (a.x * b.x + a.y * b.y + a.z * b.z);
}

// Produit vectoriel : retourne un vecteur perpendiculaire aux deux
// Utilisé pour calculer les normales de faces
t_vec3  vec3_cross(t_vec3 a, t_vec3 b)
{
    return ((t_vec3){
        a.y * b.z - a.z * b.y,
        a.z * b.x - a.x * b.z,
        a.x * b.y - a.y * b.x
    });
}

// Longueur (magnitude) du vecteur : théorème de Pythagore en 3D
float   vec3_length(t_vec3 v)
{
    return (sqrtf(v.x * v.x + v.y * v.y + v.z * v.z));
}

// Normalisation : ramène le vecteur à une longueur de 1.0
// Utile pour les directions (on veut juste l'orientation, pas la magnitude)
t_vec3  vec3_normalize(t_vec3 v)
{
    float len;

    len = vec3_length(v);
    if (len == 0.0f)
        return ((t_vec3){0.0f, 0.0f, 0.0f});
    return (vec3_scale(v, 1.0f / len));
}