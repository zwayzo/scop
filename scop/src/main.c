#include "window.h"
#include "shader.h"
#include "math/mat4.h"
#include "obj_parser.h"
#include "mesh.h"
#include "texture.h"
#include <stdio.h>

int main(int argc, char **argv)
{
    GLFWwindow  *window;
    GLuint      shader;
    GLuint      texture;
    t_obj       *obj;
    t_mesh      *mesh;
    t_transform tr;
    t_vec3      center;
    int         loc;
    float       blend;
    int         tex_on;
    int         t_pressed;

    // Vérifie qu'on a bien un fichier .obj en argument
    if (argc != 2)
    {
        fprintf(stderr, "Usage : ./scop <fichier.obj>\n");
        fprintf(stderr, "Exemple : ./scop assets/42.obj\n");
        return (1);
    }

    window = window_create();
    if (!window)
        return (1);

    // Utilise le fichier passé en argument
    obj = obj_load(argv[1]);
    if (!obj)
    {
        window_destroy(window);
        return (1);
    }

    shader = shader_create("shaders/vertex.glsl", "shaders/fragment.glsl");
    if (!shader)
    {
        obj_free(obj);
        window_destroy(window);
        return (1);
    }

    texture = texture_load("assets/texture.bmp");
    if (!texture)
    {
        obj_free(obj);
        shader_destroy(shader);
        window_destroy(window);
        return (1);
    }

    mesh = mesh_create(obj);
    center = obj->center;
    obj_free(obj);
    if (!mesh)
    {
        shader_destroy(shader);
        texture_destroy(texture);
        window_destroy(window);
        return (1);
    }

    tr.rotation    = (t_vec3){0, 0, 0};
    tr.translation = (t_vec3){0, 0, 0};
    blend          = 0.0f;
    tex_on         = 0;
    t_pressed      = 0;

    printf("✅ OpenGL %s\n", glGetString(GL_VERSION));
    printf("🎮 Contrôles :\n");
    printf("   WASD       → rotation\n");
    printf("   Q/E        → roulis\n");
    printf("   Flèches    → translation X/Y\n");
    printf("   +/-        → translation Z\n");
    printf("   T          → toggle texture\n");
    printf("   R          → reset\n");
    printf("   Echap      → quitter\n");

    while (!glfwWindowShouldClose(window))
    {
        glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT);

        window_handle_input(window, &tr);

        if (glfwGetKey(window, GLFW_KEY_T) == GLFW_PRESS && !t_pressed)
        {
            tex_on = !tex_on;
            t_pressed = 1;
        }
        if (glfwGetKey(window, GLFW_KEY_T) == GLFW_RELEASE)
            t_pressed = 0;

        if (tex_on && blend < 1.0f)
            blend += 0.02f;
        else if (!tex_on && blend > 0.0f)
            blend -= 0.02f;
        if (blend > 1.0f) blend = 1.0f;
        if (blend < 0.0f) blend = 0.0f;

        t_mat4 model = transform_to_model(&tr, center);
        t_mat4 view  = mat4_look_at(
            (t_vec3){0, 0, 5.0f},
            (t_vec3){0, 0, 0},
            (t_vec3){0, 1, 0}
        );
        t_mat4 proj = mat4_perspective(
            0.785398f,
            (float)WIN_WIDTH / WIN_HEIGHT,
            0.1f, 100.0f
        );
        t_mat4 mvp = mat4_multiply(mat4_multiply(proj, view), model);

        shader_use(shader);
        loc = glGetUniformLocation(shader, "uMVP");
        glUniformMatrix4fv(loc, 1, GL_FALSE, mat4_ptr(&mvp));
        loc = glGetUniformLocation(shader, "uBlend");
        glUniform1f(loc, blend);
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, texture);
        loc = glGetUniformLocation(shader, "uTexture");
        glUniform1i(loc, 0);

        mesh_draw(mesh);

        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    mesh_free(mesh);
    texture_destroy(texture);
    shader_destroy(shader);
    window_destroy(window);
    return (0);
}




//  sips -s format bmp  ../../../Downloads/pngtree-abstract-cloudy-background-beautiful-natural-streaks-of-sky-and-clouds-red-image_15684333.jpg --out assets/texture.bmp\n