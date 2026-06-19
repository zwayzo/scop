#include "window.h"
#include "shader.h"
#include "math/mat4.h"
#include "obj_parser.h"
#include "mesh.h"
#include "texture.h"
#include <stdio.h>

int main(void)
{
    GLFWwindow  *window;
    GLuint      shader;
    GLuint      texture;
    t_obj       *obj;
    t_mesh      *mesh;
    t_transform tr;
    t_vec3      center;
    int         loc;
    float       blend;       // 0.0 = couleur, 1.0 = texture
    int         tex_on;      // est-ce qu'on va vers la texture ?
    int         t_pressed;   // évite la répétition de la touche T

    window = window_create();
    if (!window)
        return (1);

    obj = obj_load("assets/cube.obj");
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
    printf("   T → toggle texture\n");

    while (!glfwWindowShouldClose(window))
    {
        glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT);

        window_handle_input(window, &tr);

        // Toggle texture avec T (détection appui unique)
        if (glfwGetKey(window, GLFW_KEY_T) == GLFW_PRESS && !t_pressed)
        {
            tex_on = !tex_on;
            t_pressed = 1;
        }
        if (glfwGetKey(window, GLFW_KEY_T) == GLFW_RELEASE)
            t_pressed = 0;

        // Transition douce : blend s'approche de 0.0 ou 1.0
        if (tex_on && blend < 1.0f)
            blend += 0.02f;
        else if (!tex_on && blend > 0.0f)
            blend -= 0.02f;

        // Clamp entre 0 et 1
        if (blend > 1.0f) blend = 1.0f;
        if (blend < 0.0f) blend = 0.0f;

        // MVP
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

        // Envoie la MVP
        loc = glGetUniformLocation(shader, "uMVP");
        glUniformMatrix4fv(loc, 1, GL_FALSE, mat4_ptr(&mvp));

        // Envoie le blend
        loc = glGetUniformLocation(shader, "uBlend");
        glUniform1f(loc, blend);

        // Bind la texture sur l'unité 0
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