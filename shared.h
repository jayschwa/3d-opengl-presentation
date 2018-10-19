#ifndef SHARED_H
#define SHARED_H

#include <stdbool.h>

#define GL_GLEXT_PROTOTYPES
#define GLFW_INCLUDE_GLCOREARB
#include <GLFW/glfw3.h>

extern const char *title;
extern GLFWwindow *window;

extern bool sceneInit();
extern void sceneDraw();

extern GLuint loadprogram(const char *name);

#endif