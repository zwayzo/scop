#version 330 core

layout (location = 0) in vec3 aPos;
layout (location = 1) in vec3 aColor;

out vec3 vColor;
out vec2 vTexCoord;   // coordonnées UV pour la texture

uniform mat4 uMVP;

void main()
{
    gl_Position = uMVP * vec4(aPos, 1.0);
    vColor = aColor;

    // Coordonnées UV sphériques simples basées sur la position normalisée
    // Donne un placage correct sur n'importe quel objet
    vec3 n = normalize(aPos);
    vTexCoord = vec2(
        0.5 + atan(n.z, n.x) / (2.0 * 3.14159265),
        0.5 - asin(n.y) / 3.14159265
    );
}