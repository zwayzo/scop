#include "obj_parser.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

/*
** parse_vertex — lit une ligne "v x y z" et stocke le point
*/
static void parse_vertex(t_obj *obj, const char *line)
{
    t_vec3  v;

    // sscanf lit les 3 floats après le "v "
    if (sscanf(line, "%f %f %f", &v.x, &v.y, &v.z) != 3)
        return ;
    obj->vertices[obj->vertex_count++] = v;
}

/*
** parse_face — lit une ligne "f v1 v2 v3" ou "f v1/vt/vn v2/vt/vn v3/vt/vn"
** On ne garde que l'indice du vertex (avant le premier '/')
** Les indices .obj commencent à 1 → on fait -1 pour le tableau C
*/
static void parse_face(t_obj *obj, const char *line)
{
    t_face  face;
    int     i;
    int     v;
    int     dummy;  // pour absorber vt et vn qu'on ignore pour l'instant

    i = 0;
    while (i < 3)
    {
        // Essaie le format "v/vt/vn"
        if (sscanf(line, "%d/%d/%d", &v, &dummy, &dummy) >= 1 ||
            sscanf(line, "%d//%d", &v, &dummy) >= 1 ||
            sscanf(line, "%d", &v) == 1)
        {
            face.v[i] = v - 1;  // converti en index base 0
        }
        // Avance au prochain token de la ligne
        while (*line && *line != ' ')
            line++;
        while (*line == ' ')
            line++;
        i++;
    }
    obj->faces[obj->face_count++] = face;
}

/*
** obj_compute_center — calcule le centre géométrique de l'objet
** On en a besoin pour faire tourner l'objet sur son propre axe
** et non autour d'un coin
*/
static void obj_compute_center(t_obj *obj)
{
    t_vec3  min;
    t_vec3  max;
    int     i;

    if (obj->vertex_count == 0)
        return ;

    // Initialise min et max avec le premier vertex
    min = obj->vertices[0];
    max = obj->vertices[0];

    // Trouve le min et max sur chaque axe
    i = 1;
    while (i < obj->vertex_count)
    {
        if (obj->vertices[i].x < min.x) min.x = obj->vertices[i].x;
        if (obj->vertices[i].y < min.y) min.y = obj->vertices[i].y;
        if (obj->vertices[i].z < min.z) min.z = obj->vertices[i].z;
        if (obj->vertices[i].x > max.x) max.x = obj->vertices[i].x;
        if (obj->vertices[i].y > max.y) max.y = obj->vertices[i].y;
        if (obj->vertices[i].z > max.z) max.z = obj->vertices[i].z;
        i++;
    }

    // Le centre = milieu entre min et max
    obj->center.x = (min.x + max.x) / 2.0f;
    obj->center.y = (min.y + max.y) / 2.0f;
    obj->center.z = (min.z + max.z) / 2.0f;
}

/*
** obj_load — ouvre et parse un fichier .obj
** Retourne un t_obj alloué, ou NULL en cas d'erreur
*/
t_obj   *obj_load(const char *path)
{
    FILE    *f;
    t_obj   *obj;
    char    line[1024];

    f = fopen(path, "r");
    if (!f)
    {
        fprintf(stderr, "❌ Impossible d'ouvrir : %s\n", path);
        return (NULL);
    }

    // Alloue la structure principale
    obj = calloc(1, sizeof(t_obj));
    if (!obj)
        return (NULL);

    obj->vertices = malloc(sizeof(t_vec3) * MAX_VERTICES);
    obj->faces    = malloc(sizeof(t_face) * MAX_FACES);
    if (!obj->vertices || !obj->faces)
    {
        obj_free(obj);
        return (NULL);
    }

    // Lit le fichier ligne par ligne
    while (fgets(line, sizeof(line), f))
    {
        if (strncmp(line, "v ", 2) == 0)        // vertex
            parse_vertex(obj, line + 2);
        else if (strncmp(line, "f ", 2) == 0)   // face
            parse_face(obj, line + 2);
        // vt, vn, #, o, g, s → ignorés pour l'instant
    }

    fclose(f);

    obj_compute_center(obj);

    printf("✅ Modèle chargé : %d vertices, %d faces\n",
        obj->vertex_count, obj->face_count);
    printf("   Centre : (%.2f, %.2f, %.2f)\n",
        obj->center.x, obj->center.y, obj->center.z);

    return (obj);
}

/*
** obj_free — libère toute la mémoire allouée
*/
void    obj_free(t_obj *obj)
{
    if (!obj)
        return ;
    free(obj->vertices);
    free(obj->faces);
    free(obj);
}