#include "mesh.h"
#include <stdlib.h>
#include <stdio.h>
#include <math.h>

/*
** face_color — génère une couleur grise unique par face
** On utilise l'index de la face pour varier la teinte
** Entre 0.3 (gris foncé) et 0.95 (gris clair)
*/
static void face_color(int face_index, int face_count, float *r, float *g, float *b)
{
    float   t;

    // t entre 0.0 et 1.0 selon la position de la face
    t = (float)face_index / (float)face_count;

    // Nuance de gris : tous les canaux égaux
    *r = 0.3f + t * 0.65f;
    *g = 0.3f + t * 0.65f;
    *b = 0.3f + t * 0.65f;
}

/*
** mesh_create — construit le VBO/VAO avec positions + couleurs
** Format du buffer : x y z r g b (6 floats par vertex)
*/
t_mesh  *mesh_create(t_obj *obj)
{
    t_mesh  *mesh;
    float   *buf;
    int     i;
    int     j;
    int     idx;
    t_vec3  v;
    float   r;
    float   g;
    float   b;

    mesh = calloc(1, sizeof(t_mesh));
    if (!mesh)
        return (NULL);

    mesh->vertex_count = obj->face_count * 3;

    // 6 floats par vertex : x y z r g b
    buf = malloc(sizeof(float) * mesh->vertex_count * 6);
    if (!buf)
    {
        free(mesh);
        return (NULL);
    }

    i = 0;
    idx = 0;
    while (i < obj->face_count)
    {
        // Calcule la couleur de cette face
        face_color(i, obj->face_count, &r, &g, &b);

        j = 0;
        while (j < 3)
        {
            v = obj->vertices[obj->faces[i].v[j]];

            // Position
            buf[idx++] = v.x;
            buf[idx++] = v.y;
            buf[idx++] = v.z;

            // Couleur (même pour les 3 vertices de la face)
            buf[idx++] = r;
            buf[idx++] = g;
            buf[idx++] = b;

            j++;
        }
        i++;
    }

    glGenVertexArrays(1, &mesh->vao);
    glGenBuffers(1, &mesh->vbo);
    glBindVertexArray(mesh->vao);
    glBindBuffer(GL_ARRAY_BUFFER, mesh->vbo);
    glBufferData(GL_ARRAY_BUFFER,
        sizeof(float) * mesh->vertex_count * 6,
        buf, GL_STATIC_DRAW);

    // location 0 → position (3 floats, stride = 6 floats)
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE,
        6 * sizeof(float), (void *)0);
    glEnableVertexAttribArray(0);

    // location 1 → couleur (3 floats, commence après les 3 premiers)
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE,
        6 * sizeof(float), (void *)(3 * sizeof(float)));
    glEnableVertexAttribArray(1);

    glBindVertexArray(0);
    free(buf);

    printf("✅ Mesh créé : %d triangles\n", obj->face_count);
    return (mesh);
}

void    mesh_draw(t_mesh *mesh)
{
    glBindVertexArray(mesh->vao);
    glDrawArrays(GL_TRIANGLES, 0, mesh->vertex_count);
    glBindVertexArray(0);
}

void    mesh_free(t_mesh *mesh)
{
    if (!mesh)
        return ;
    glDeleteVertexArrays(1, &mesh->vao);
    glDeleteBuffers(1, &mesh->vbo);
    free(mesh);
}