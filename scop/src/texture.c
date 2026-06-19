#include "texture.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

GLuint  texture_load(const char *path)
{
    FILE            *f;
    unsigned char   header[54];
    int             width;
    int             height;
    int             data_offset;
    unsigned char   *data;
    unsigned char   *rgb_data;
    int             image_size;
    GLuint          texture;
    int             i;
    int             flip;   // 1 si on doit inverser les lignes

    f = fopen(path, "rb");
    if (!f)
    {
        fprintf(stderr, "❌ Impossible d'ouvrir la texture : %s\n", path);
        return (0);
    }

    if (fread(header, 1, 54, f) != 54)
    {
        fprintf(stderr, "❌ Header BMP invalide : %s\n", path);
        fclose(f);
        return (0);
    }

    if (header[0] != 'B' || header[1] != 'M')
    {
        fprintf(stderr, "❌ Pas un fichier BMP : %s\n", path);
        fclose(f);
        return (0);
    }

    data_offset = *(int *)&header[10];
    width       = *(int *)&header[18];
    height      = *(int *)&header[22];

    // hauteur négative = BMP top-down (cas sips macOS)
    flip = 1;
    if (height < 0)
    {
        height = -height;
        flip = 0;   // déjà dans le bon sens, pas besoin d'inverser
    }

    image_size = width * height * 3;
    data = malloc(image_size);
    if (!data)
    {
        fclose(f);
        return (0);
    }

    fseek(f, data_offset, SEEK_SET);
    fread(data, 1, image_size, f);
    fclose(f);

    // Convertit BGR → RGB et gère le flip vertical si nécessaire
    rgb_data = malloc(image_size);
    if (!rgb_data)
    {
        free(data);
        return (0);
    }

    i = 0;
    while (i < width * height)
    {
        int src_row;
        int src_i;

        if (flip)
            // BMP classique : lignes inversées (bas → haut)
            src_row = (height - 1 - (i / width));
        else
            // BMP top-down (sips) : lignes dans l'ordre
            src_row = i / width;

        src_i = src_row * width + (i % width);

        rgb_data[i * 3 + 0] = data[src_i * 3 + 2];  // R ← B
        rgb_data[i * 3 + 1] = data[src_i * 3 + 1];  // G ← G
        rgb_data[i * 3 + 2] = data[src_i * 3 + 0];  // B ← R
        i++;
    }

    glGenTextures(1, &texture);
    glBindTexture(GL_TEXTURE_2D, texture);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height,
        0, GL_RGB, GL_UNSIGNED_BYTE, rgb_data);
    glGenerateMipmap(GL_TEXTURE_2D);

    free(data);
    free(rgb_data);

    printf("✅ Texture chargée : %s (%dx%d)\n", path, width, height);
    return (texture);
}

void    texture_destroy(GLuint texture)
{
    glDeleteTextures(1, &texture);
}