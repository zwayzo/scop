#include "window.h"
#include <stdio.h>
#include <stdlib.h>


// Un callback c'est une fonction que tu donnes à GLFW, et c'est lui qui l'appelle quand quelque chose 
// se passe. Ici, si GLFW rencontre une erreur, il appelle automatiquement cette fonction. 
// Le (void)error sert juste à éviter un warning du compilateur car on n'utilise pas ce paramètre.

static void callback_error(int error, const char *description)
{
    (void)error;
    fprintf(stderr, "❌ GLFW Error: %s\n", description);
}
/*
Si l'utilisateur redimensionne la fenêtre, GLFW appelle ce callback.
 glViewport dit à OpenGL : "la zone de dessin fait maintenant cette taille".
  Sans ça, le rendu resterait coincé dans la taille d'origine.*/

static void callback_resize(GLFWwindow *window, int width, int height)
{
    (void)window;
    glViewport(0, 0, width, height); // adapte le rendu si on redimensionne
}

GLFWwindow  *window_create(void)
{
    GLFWwindow  *window;

    glfwSetErrorCallback(callback_error);

    if (!glfwInit())
    {
        fprintf(stderr, "❌ Échec de glfwInit()\n");
        return (NULL);
    }

    // On demande OpenGL 3.3 Core Profile
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    // Nécessaire sur macOS
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);


    /*
    Crée la fenêtre avec la largeur, hauteur, et le titre. 
    Les deux NULL à la fin : le premier serait pour le mode plein écran, 
    le second pour partager des ressources entre fenêtres — on n'en a pas besoin.
    */

    window = glfwCreateWindow(WIN_WIDTH, WIN_HEIGHT, WIN_TITLE, NULL, NULL);
    if (!window)
    {
        fprintf(stderr, "❌ Échec de glfwCreateWindow()\n");
        glfwTerminate();
        return (NULL);
    }

    glfwMakeContextCurrent(window); // active le contexte OpenGL

    // Charge toutes les fonctions OpenGL via GLAD

    /*
    C'est ici que GLAD fait son travail. On lui passe glfwGetProcAddress
     — une fonction de GLFW qui sait où chercher les fonctions OpenGL dans le driver.
    GLAD va parcourir toutes les fonctions OpenGL (glDrawArrays, glClearColor, etc.)
     et récupérer leur adresse dans le driver. Après cette ligne, toutes les fonctions gl* sont utilisables.*/
    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
    {
        fprintf(stderr, "❌ Échec de gladLoadGLLoader()\n");
        glfwTerminate();
        return (NULL);
    }

    glViewport(0, 0, WIN_WIDTH, WIN_HEIGHT);
    glfwSetFramebufferSizeCallback(window, callback_resize);

    return (window);
}

void    window_destroy(GLFWwindow *window)
{
    glfwDestroyWindow(window);
    glfwTerminate();
}
/*
** window_handle_input — lit les touches et met à jour le transform
** Appelé à chaque frame dans la boucle principale
*/
void    window_handle_input(GLFWwindow *window, t_transform *tr)
{
    // Fermeture
    if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
        glfwSetWindowShouldClose(window, 1);

    // Rotation axe Y (gauche / droite)
    if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
        tr->rotation.y -= ROT_SPEED;
    if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
        tr->rotation.y += ROT_SPEED;

    // Rotation axe X (haut / bas)
    if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
        tr->rotation.x -= ROT_SPEED;
    if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
        tr->rotation.x += ROT_SPEED;

    // Rotation axe Z (roulis)
    if (glfwGetKey(window, GLFW_KEY_Q) == GLFW_PRESS)
        tr->rotation.z -= ROT_SPEED;
    if (glfwGetKey(window, GLFW_KEY_E) == GLFW_PRESS)
        tr->rotation.z += ROT_SPEED;

    // Translation X
    if (glfwGetKey(window, GLFW_KEY_LEFT) == GLFW_PRESS)
        tr->translation.x -= TRANS_SPEED;
    if (glfwGetKey(window, GLFW_KEY_RIGHT) == GLFW_PRESS)
        tr->translation.x += TRANS_SPEED;

    // Translation Y
    if (glfwGetKey(window, GLFW_KEY_UP) == GLFW_PRESS)
        tr->translation.y += TRANS_SPEED;
    if (glfwGetKey(window, GLFW_KEY_DOWN) == GLFW_PRESS)
        tr->translation.y -= TRANS_SPEED;

    // Translation Z (zoom)
    if (glfwGetKey(window, GLFW_KEY_KP_ADD) == GLFW_PRESS ||
        glfwGetKey(window, GLFW_KEY_EQUAL) == GLFW_PRESS)
        tr->translation.z += TRANS_SPEED;
    if (glfwGetKey(window, GLFW_KEY_KP_SUBTRACT) == GLFW_PRESS ||
        glfwGetKey(window, GLFW_KEY_MINUS) == GLFW_PRESS)
        tr->translation.z -= TRANS_SPEED;

    // Reset
    if (glfwGetKey(window, GLFW_KEY_R) == GLFW_PRESS)
    {
        tr->rotation    = (t_vec3){0, 0, 0};
        tr->translation = (t_vec3){0, 0, 0};
    }
}

/*
** transform_to_model — convertit le t_transform en matrice model
** On centre d'abord l'objet, puis on applique rotation et translation
** Ordre important : center → rotate → translate
*/
t_mat4  transform_to_model(t_transform *tr, t_vec3 center)
{
    t_mat4  model;

    model = mat4_identity();

    // 1. Centre l'objet sur l'origine (pour tourner sur son axe)
    model = mat4_translate(model, (t_vec3){
        -center.x,
        -center.y,
        -center.z
    });

    // 2. Applique les rotations sur les 3 axes
    model = mat4_rotate_x(model, tr->rotation.x);
    model = mat4_rotate_y(model, tr->rotation.y);
    model = mat4_rotate_z(model, tr->rotation.z);

    // 3. Applique la translation utilisateur
    model = mat4_translate(model, tr->translation);

    return (model);
}