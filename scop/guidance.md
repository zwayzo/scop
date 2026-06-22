--> OpenGL est une spécification (un standard), pas une librairie. C'est le driver de ta carte graphique qui l'implémente. Du coup, les fonctions OpenGL ne sont pas dans un .lib classique qu'on link normalement — elles sont cachées dans le driver, et il faut aller les chercher à la main au runtime.

📜 Une spécification vs une librairie
Une librairie c'est du code compilé, prêt à l'emploi. Par exemple libm.a contient le code de sin(), cos(), etc. Tu la links et c'est tout.
Une spécification c'est juste un document qui dit :

"La fonction glDrawArrays doit exister, prendre ces paramètres, et faire ce comportement."

Mais qui écrit le vrai code qui exécute glDrawArrays ? C'est NVIDIA, AMD, Apple, Intel — les fabricants de cartes graphiques. Chacun écrit son propre code optimisé pour son hardware, en respectant la spécification.

🎯 Analogie concrète
C'est comme le HTML :

Le W3C définit la spécification : "la balise <button> doit être cliquable"
Mais c'est Chrome, Firefox, Safari qui implémentent chacun leur moteur

Tu n'installes pas "HTML" sur ta machine — tu installes un navigateur qui l'implémente.
Pour OpenGL c'est pareil :

Khronos Group définit la spec
NVIDIA/AMD/Apple livrent l'implémentation dans leur driver GPU


---> GLFW = Graphics Library FrameWork
C'est une librairie C qui s'occupe de tout ce qui est autour d'OpenGL, mais pas OpenGL lui-même.

Le problème qu'elle résout
OpenGL ne sait faire qu'une chose : dessiner des pixels. Mais pour dessiner, il te faut d'abord :

Une fenêtre
Un contexte OpenGL attaché à cette fenêtre
Pouvoir lire le clavier et la souris

Or, créer une fenêtre est complètement différent selon l'OS :


GLFW                          GLAD
────────────────────          ────────────────────
Crée la fenêtre               Déverrouille les fonctions OpenGL
Gère clavier/souris           du driver de ta carte graphique
Gère le contexte OpenGL

glfwCreateWindow()            glDrawArrays()
glfwPollEvents()              glClearColor()
glfwSwapBuffers()             glUseProgram()


GLAD = GL Loader-Generator
On en a parlé rapidement avant, mais voilà l'explication complète.

Le problème qu'il résout
Les fonctions OpenGL modernes comme glDrawArrays(), glUseProgram(), etc. ne sont pas dans une librairie classique. Elles sont cachées dans le driver de ta carte graphique.
Pour les utiliser, il faudrait normalement faire ça à la main pour chaque fonction :
c// Sans GLAD — pour UNE SEULE fonction
typedef void (*GL_DrawArrays)(GLenum, GLint, GLsizei);
GL_DrawArrays glDrawArrays = NULL;
glDrawArrays = (GL_DrawArrays)glfwGetProcAddress("glDrawArrays");
Il y a plus de 500 fonctions OpenGL. C'est impossible à faire manuellement.

Ce que GLAD fait
Tu lui donnes juste une fonction qui sait chercher dans le driver (glfwGetProcAddress), et il fait le travail pour toutes les fonctions d'un coup :
cgladLoadGLLoader((GLADloadproc)glfwGetProcAddress);
// ✅ les 500+ fonctions sont maintenant disponibles

SHADERS

C'est quoi un shader ?
Un shader c'est un petit programme que tu écris et qui tourne sur le GPU (pas sur ton CPU comme le reste de ton code C).
Il y en a deux types, et ils s'exécutent dans cet ordre à chaque frame :
ton code C (CPU)
      │
      │  envoie les points 3D
      ▼
┌─────────────────────┐
│   VERTEX SHADER     │  → "où est ce point sur l'écran ?"
└─────────────────────┘
          │
          │  pour chaque pixel couvert
          ▼
┌─────────────────────┐
│   FRAGMENT SHADER   │  → "quelle couleur a ce pixel ?"
└─────────────────────┘
          │
          ▼
      écran 🖥️

Le langage : GLSL
Les shaders s'écrivent en GLSL (GL Shading Language). C'est du C, mais simplifié, avec des types spéciaux pour la 3D.
Les types importants à connaître :
glslfloat    // nombre décimal
vec2     // 2 floats (x, y)
vec3     // 3 floats (x, y, z)  ← position 3D
vec4     // 4 floats (x, y, z, w) ← position homogène
mat4     // matrice 4x4 ← transformations 3D