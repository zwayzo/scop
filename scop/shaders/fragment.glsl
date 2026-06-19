#version 330 core

in vec3 vColor;
in vec2 vTexCoord;

out vec4 FragColor;

uniform sampler2D uTexture;  // la texture
uniform float     uBlend;    // 0.0 = couleur pure, 1.0 = texture pure

void main()
{
    vec4 color_part   = vec4(vColor, 1.0);
    vec4 texture_part = texture(uTexture, vTexCoord);

    // mix() fait l'interpolation : color*(1-blend) + texture*blend
    FragColor = mix(color_part, texture_part, uBlend);
}