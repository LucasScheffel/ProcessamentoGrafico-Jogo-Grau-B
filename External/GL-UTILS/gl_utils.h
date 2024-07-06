/******************************************************************************\
| OpenGL 4 Example Code.                                                       |
| Accompanies written series "Anton's OpenGL 4 Tutorials"                      |
| Email: anton at antongerdelan dot net                                        |
| First version 27 Jan 2014                                                    |
| Copyright Dr Anton Gerdelan, Trinity College Dublin, Ireland.                |
| See individual libraries separate legal notices                              |
|******************************************************************************|
| This is just a file holding some commonly-used "utility" functions to keep   |
| the main file a bit easier to read. You can might build up something like    |
| this as you learn more GL. Note that you don't need much code here to do     |
| good GL. If you have a big object-oriented engine then maybe you can ask     |
| yourself if  it is really making life easier.                                |
\******************************************************************************/
#ifndef _GL_UTILS_H_
#define _GL_UTILS_H_

#include <stdarg.h>
#include <GL/glew.h>
#include <GLFW/glfw3.h>

/*------------------------------GLOBAL VARIABLES------------------------------*/
extern int glWidth;
extern int glHeight;
extern GLFWwindow* window;
/*--------------------------------LOG FUNCTIONS-------------------------------*/
bool restartGlLog();
bool glLog(const char* message, ...);
bool glLogError(const char* message, ...);
/*--------------------------------GLFW3 and GLEW------------------------------*/
bool startGl(const char* titleWindow);
void glfwErrorCallback(int error, const char* description);
void glfwWindowSizeCallback(GLFWwindow* window, int width, int height);
void updateFpsCounter(GLFWwindow* window);
/*-----------------------------------SHADERS----------------------------------*/
bool parseFileIntoStr(const char* filename, char* shaderStr, int maxLen);
void printShaderInfoLog(GLuint shaderIndex);
bool createShader(const char* filename, GLuint* shader, GLenum type);
bool isProgrammeValid(GLuint sp);
bool createProgramme(GLuint vs, GLuint fs, GLuint* programme);
GLuint createProgrammeFromFiles(const char* vsFilename, const char* fsFilename);
int loadTexture(unsigned int& texture, const char* filename);
#endif
