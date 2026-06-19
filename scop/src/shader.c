#include "shader.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

/*
** file_read — lit un fichier depuis le disque et retourne son contenu
** sous forme de string allouée dynamiquement.
** Le appelant est responsable de libérer la mémoire avec free().
*/
static char *file_read(const char *path)
{
    FILE    *f;
    long    size;
    char    *buf;

    // Ouvre le fichier en mode binaire ("rb") pour éviter
    // les conversions de caractères sur Windows (\r\n → \n)
    f = fopen(path, "rb");
    if (!f)
    {
        fprintf(stderr, "❌ Impossible d'ouvrir : %s\n", path);
        return (NULL);
    }

    // Se déplace à la fin du fichier pour mesurer sa taille
    fseek(f, 0, SEEK_END);
    size = ftell(f);        // ftell() retourne la position = taille en octets
    fseek(f, 0, SEEK_SET);  // revient au début pour lire

    // Alloue size + 1 octet pour le '\0' terminal
    buf = malloc(size + 1);
    if (!buf)
        return (NULL);

    // Lit tout le fichier d'un coup dans le buffer
    fread(buf, 1, size, f);
    buf[size] = '\0';   // termine la string manuellement (fread ne le fait pas)

    fclose(f);
    return (buf);
}

/*
** shader_compile — compile un shader GLSL depuis un fichier.
** type : GL_VERTEX_SHADER ou GL_FRAGMENT_SHADER
** Retourne l'identifiant GPU du shader compilé, ou 0 en cas d'erreur.
*/
static GLuint shader_compile(const char *path, GLenum type)
{
    GLuint      shader;
    char        *src;
    int         success;
    char        log[512];

    // Lit le code source GLSL depuis le fichier
    src = file_read(path);
    if (!src)
        return (0);

    // Crée un objet shader vide sur le GPU
    shader = glCreateShader(type);

    // Envoie le code source au GPU
    // Le "1" signifie qu'on envoie 1 seule string
    // NULL = pas de longueurs explicites, OpenGL utilise le '\0'
    glShaderSource(shader, 1, (const char **)&src, NULL);

    // Demande au driver GPU de compiler le code GLSL
    glCompileShader(shader);

    // Le source est maintenant dans le GPU, on libère la copie CPU
    free(src);

    // Demande le statut de compilation au GPU
    // GL_COMPILE_STATUS met 1 dans success si ok, 0 si erreur
    glGetShaderiv(shader, GL_COMPILE_STATUS, &success);
    if (!success)
    {
        // Récupère le message d'erreur du compilateur GLSL (max 512 chars)
        glGetShaderInfoLog(shader, 512, NULL, log);
        fprintf(stderr, "❌ Erreur shader (%s):\n%s\n", path, log);
        return (0);
    }
    return (shader);
}

/*
** shader_create — compile le vertex et fragment shader, puis les lie
** ensemble dans un "program" utilisable par OpenGL.
** Retourne l'identifiant du program, ou 0 en cas d'erreur.
*/
GLuint  shader_create(const char *vert_path, const char *frag_path)
{
    GLuint  vert;       // identifiant du vertex shader compilé
    GLuint  frag;       // identifiant du fragment shader compilé
    GLuint  program;    // identifiant du program final (les deux liés)
    int     success;
    char    log[512];

    // Compile chacun des deux shaders séparément
    vert = shader_compile(vert_path, GL_VERTEX_SHADER);
    frag = shader_compile(frag_path, GL_FRAGMENT_SHADER);
    if (!vert || !frag)
        return (0);

    // Crée un program vide — c'est le conteneur final
    program = glCreateProgram();

    // Attache les deux shaders au program
    glAttachShader(program, vert);
    glAttachShader(program, frag);

    // "Link" : vérifie que les sorties du vertex shader
    // correspondent aux entrées du fragment shader
    glLinkProgram(program);

    // Vérifie si le link a réussi
    glGetProgramiv(program, GL_LINK_STATUS, &success);
    if (!success)
    {
        glGetProgramInfoLog(program, 512, NULL, log);
        fprintf(stderr, "❌ Erreur link shader:\n%s\n", log);
        return (0);
    }

    // Les shaders individuels sont "cuits" dans le program,
    // on peut les supprimer du GPU pour libérer de la mémoire
    glDeleteShader(vert);
    glDeleteShader(frag);

    return (program);
}

/*
** shader_use — active ce program pour tous les draw calls suivants.
** Tous les glDraw*() après cet appel utiliseront ce shader.
*/
void    shader_use(GLuint program)
{
    glUseProgram(program);
}

/*
** shader_destroy — supprime le program du GPU et libère sa mémoire.
** À appeler quand le shader n'est plus nécessaire.
*/
void    shader_destroy(GLuint program)
{
    glDeleteProgram(program);
}