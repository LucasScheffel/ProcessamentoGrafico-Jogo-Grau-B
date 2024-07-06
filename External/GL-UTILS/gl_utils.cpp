#include "gl_utils.h"
#include <stdio.h>
#include <time.h>
#include <string.h>
#include <assert.h>
#include <iostream>
#include "stb_image.h"
#define GL_LOG_FILE "logs/gl.log"
#define MAX_SHADER_LENGTH 262144

using namespace std;

/*--------------------------------LOG FUNCTIONS-------------------------------*/
bool restartGlLog() {
	FILE* file = fopen(GL_LOG_FILE, "w");

	if (!file) {
		fprintf(stderr, "ERROR: could not open GL_LOG_FILE log file %s for writing\n", GL_LOG_FILE);
		return false;
	}

	time_t now = time(NULL);
	char* date = ctime(&now);
	fprintf(file, "GL_LOG_FILE log. local time %s\n", date);
	fclose(file);

	return true;
}

bool glLog(const char* message, ...) {
	va_list argptr;
	FILE* file = fopen(GL_LOG_FILE, "a");

	if (!file) {
		fprintf(stderr, "ERROR: could not open GL_LOG_FILE %s file for appending\n", GL_LOG_FILE);
		return false;
	}

	va_start(argptr, message);
	vfprintf(file, message, argptr);

	va_end(argptr);
	fclose(file);

	return true;
}

bool glLogError(const char* message, ...) {
	va_list argptr;
	FILE* file = fopen(GL_LOG_FILE, "a");

	if (!file) {
		fprintf(stderr, "ERROR: could not open GL_LOG_FILE %s file for appending\n", GL_LOG_FILE);
		return false;
	}

	va_start(argptr, message);
	vfprintf(file, message, argptr);
	va_end(argptr);

	va_start(argptr, message);
	vfprintf(stderr, message, argptr);
	va_end(argptr);

	fclose(file);
	return true;
}

/*--------------------------------GLFW3 and GLEW------------------------------*/
bool startGl(const char* titleWindow) {
	glLog("Inicializando GLFW %s", glfwGetVersionString());
	glfwSetErrorCallback(glfwErrorCallback);
	
	/****************************************************************************/
	// 1 - Inicializa��o da GLFW
	if (!glfwInit()) {
		fprintf(stderr, "ERRO: N�o foi possivel inicializar o GLFW3\n");
		return false;
	}

	// 2 - Cria��o do contexto gr�fico (window)
	window = glfwCreateWindow(glWidth, glHeight, titleWindow, NULL, NULL);
	if (!window) {
		fprintf(stderr, "ERRO: N�o foi poss�vel abrir janela com a GLFW3\n");
		glfwTerminate();
		return false;
	}

	glfwSetWindowSizeCallback(window, glfwWindowSizeCallback);
	glfwMakeContextCurrent(window);
	glfwWindowHint(GLFW_SAMPLES, 4);

	// 3 - Inicia manipulador da extens�o GLEW
	glewExperimental = GL_TRUE;
	if (glewInit() != GLEW_OK) {
		fprintf(stderr, "ERRO: N�o foi poss�vel inicializar o GLEW\n");
		return false;
	}

	// 4 - Objetivo do exemplo: detectar a vers�o atual do OpenGL e motor de renderiza��o.
	const GLubyte* renderer = glGetString(GL_RENDERER);
	const GLubyte* version = glGetString(GL_VERSION);
	printf("Renderer: %s\n", renderer);
	printf("OpenGL version supported %s\n", version);
	glLog("renderer: %s\nversion: %s\n", renderer, version);

	return true;
}

void glfwErrorCallback(int error, const char* description) {
	fputs(description, stderr);
	glLogError("%s\n", description);
}

void glfwWindowSizeCallback(GLFWwindow* window, int width, int height) {
	glWidth = width;
	glHeight = height;
	printf ("width %i height %i\n", width, height);
}

void updateFpsCounter(GLFWwindow* window) {
	static double previousSeconds = glfwGetTime();
	static int frameCount;
	double currentSeconds = glfwGetTime();
	double elapsedSeconds = currentSeconds - previousSeconds;

	if (elapsedSeconds > 0.25) {
		previousSeconds = currentSeconds;
		double fps = (double)frameCount / elapsedSeconds;
		char tmp[128];
		sprintf(tmp, "opengl @ fps: %.2f", fps);
		glfwSetWindowTitle(window, tmp);
		frameCount = 0;
	}
	frameCount++;
}

/*-----------------------------------SHADERS----------------------------------*/
bool parseFileIntoStr(const char* filename, char* shaderStr, int maxLen) {
	int currentLen = 0;
	char line[2048];
	strcpy(line, "");

	FILE* file = fopen(filename, "r");
	if (!file) {
		glLogError("ERROR: opening file for reading: %s\n", filename);
		return false;
	}

	shaderStr[0] = '\0';
	while (!feof(file)) {
		if (NULL != fgets(line, 2048, file)) {
			currentLen += strlen(line);
			if (currentLen >= maxLen) {
				glLogError("ERROR: shader length is longer than string buffer length %i\n", maxLen);
			}
			strcat(shaderStr, line);
		}
	}

	if (EOF == fclose(file)) {
		glLogError("ERROR: closing file from reading %s\n", filename);
		return false;
	}

	return true;
}

void printShaderInfoLog(GLuint shaderIndex) {
	int maxLength = 2048;
	int actualLength = 0;
	char log[2048];

	glGetShaderInfoLog(shaderIndex, maxLength, &actualLength, log);
	printf("shader info log for GL index %i:\n%s\n", shaderIndex, log);
	glLog("shader info log for GL index %i:\n%s\n", shaderIndex, log);
}

bool createShader(const char* filename, GLuint* shader, GLenum type) {
	glLog("creating shader from %s...\n", filename);
	
	char shaderString[MAX_SHADER_LENGTH];
	assert(parseFileIntoStr(filename, shaderString, MAX_SHADER_LENGTH));
	
	*shader = glCreateShader(type);
	const GLchar* p = (const GLchar*)shaderString;
	glShaderSource(*shader, 1, &p, NULL);
	glCompileShader(*shader);

	int params = -1;
	glGetShaderiv (*shader, GL_COMPILE_STATUS, &params);

	if (GL_TRUE != params) {
		glLogError("ERROR: GL shader index %i did not compile\n", *shader);
		printShaderInfoLog(*shader);
		return false;
	}

	glLog("shader compiled. index %i\n", *shader);
	return true;
}

void printProgrammeInfoLog(GLuint sp) {
	int maxLength = 2048;
	int actualLength = 0;
	char log[2048];

	glGetProgramInfoLog(sp, maxLength, &actualLength, log);
	printf("program info log for GL index %u:\n%s", sp, log);
	glLog("program info log for GL index %u:\n%s", sp, log);
}

bool isProgrammeValid(GLuint sp) {
	glValidateProgram (sp);
	GLint params = -1;
	glGetProgramiv (sp, GL_VALIDATE_STATUS, &params);
	
	if (GL_TRUE != params) {
		glLogError("program %i GL_VALIDATE_STATUS = GL_FALSE\n", sp);
		printProgrammeInfoLog(sp);
		return false;
	}
	
	glLog("program %i GL_VALIDATE_STATUS = GL_TRUE\n", sp);
	return true;
}

bool createProgramme(GLuint vs, GLuint fs, GLuint* programme) {
	*programme = glCreateProgram();
	glLog("created programme %u. attaching shaders %u and %u...\n", *programme, vs, fs);
	
	glAttachShader(*programme, vs);
	glAttachShader(*programme, fs);
	glLinkProgram(*programme);

	GLint params = -1;
	glGetProgramiv (*programme, GL_LINK_STATUS, &params);

	if (GL_TRUE != params) {
		glLogError("ERROR: could not link shader programme GL index %u\n", *programme);
		printProgrammeInfoLog(*programme);
		return false;
	}

	assert(isProgrammeValid(*programme));
	glDeleteShader(vs);
	glDeleteShader(fs);

	return true;
}

GLuint createProgrammeFromFiles(const char* vsFilename, const char* fsFilename) {
	GLuint vs, fs, programme;
	assert(createShader(vsFilename, &vs, GL_VERTEX_SHADER));
	assert(createShader(fsFilename, &fs, GL_FRAGMENT_SHADER));
	assert(createProgramme(vs, fs, &programme));

	return programme;
}

int loadTexture(unsigned int& texture, const char* filename) {
	glGenTextures(1, &texture);
	glBindTexture(GL_TEXTURE_2D, texture);

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);

	GLfloat max_aniso = 0.0f;
	glGetFloatv(GL_MAX_TEXTURE_MAX_ANISOTROPY_EXT, &max_aniso);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAX_ANISOTROPY_EXT, max_aniso);

	int width, height, nrChannels;

	unsigned char* data = stbi_load(filename, &width, &height, &nrChannels, 0);
	if (data) {
		if (nrChannels == 4) {
			cout << "Alpha channel" << endl;
			glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, data);
		}
		else {
			cout << "Without Alpha channel" << endl;
			glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, data);
		}
		glGenerateMipmap(GL_TEXTURE_2D);
	}
	else {
		std::cout << "Failed to load texture" << std::endl;
	}

	stbi_image_free(data);
	return 1;
}