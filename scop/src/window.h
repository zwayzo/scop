#ifndef WINDOW_H
# define WINDOW_H

# include "glad/glad.h"
# include <GLFW/glfw3.h>
# include "math/vec3.h"
# include "math/mat4.h"

# define WIN_WIDTH  800
# define WIN_HEIGHT 600
# define WIN_TITLE  "SCOP"

# define ROT_SPEED   0.03f
# define TRANS_SPEED 0.05f

typedef struct s_transform
{
    t_vec3  rotation;       // angles de rotation sur chaque axe
    t_vec3  translation;    // déplacement sur chaque axe
}   t_transform;

GLFWwindow  *window_create(void);
void        window_destroy(GLFWwindow *window);
void        window_handle_input(GLFWwindow *window, t_transform *tr);
t_mat4      transform_to_model(t_transform *tr, t_vec3 center);

#endif