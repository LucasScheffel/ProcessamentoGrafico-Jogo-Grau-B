// Includes default
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <math.h>
#include <assert.h>
#include <time.h>
#include <iostream>
#include <vector>
#include <fstream>

// Includes GL e dependencias
#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include "gl_utils.h"
#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"

// Includes do projeto
#include "includes/TileMap.h"
#include "includes/DiamondView.h"
#include "includes/SlideView.h"
#include "includes/object.h"
#include "ltMath.h"

// Variaveis e defs
#define GL_LOG_FILE "logs/gl.log"
using namespace std;
int glWidth = 800;
int glHeight = 800;
float xi = -1.0f;
float xf = 1.0f;
float yi = -1.0f;
float yf = 1.0f;
float w = xf - xi;
float h = yf - yi;
float tw, th, tw2, th2;
int tileSetCols = 9, tileSetRows = 9;
float tileW, tileW2;
float tileH, tileH2;
int cx = 0, cy = 0;
bool endGame = false;
float fw = 0.25f, fh = 0.25f;
float offsetx = 0, offsety = 0;
int currentFrame = 0, objectAction = 3, sign = 1;

TilemapView* tview = new DiamondView();
TileMap* tileMap = NULL;
TileMap* collisionMap = NULL;
GLFWwindow* window = NULL;

TileMap* readMap(const char* filename) {
	ifstream arq(filename);
	int w, h;
	arq >> w >> h;
	TileMap* tmap = new TileMap(w, h, 0);
	for (int r = 0; r < h; r++) {
		for (int c = 0; c < w; c++) {
			int tid;
			arq >> tid;
			tmap->setTile(c, h - r - 1, tid);
		}
	}
	arq.close();
	return tmap;
}

void moveDog(int c, int r, const int direction) {
	tview->computeTileWalking(c, r, direction);
	
	int xMax = collisionMap->getWidth();
	int yMax = collisionMap->getHeight();
	if ((c < 0) || (c >= xMax) || (r < 0) || (r >= yMax)) {
		cout << "Fora do mapa: " << c << ", " << r << endl;
		return;
	}

	unsigned char t_id = collisionMap->getTile(c, r);
	switch (t_id) {
		case 0:
			cout << "Spok caminhou em um tile de grama" << endl;
			cx = c; cy = r;
			break;
		case 1:
			cout << "A nao, Spok caiu na lava... Pressione espaço para reiniciar e tente novamente!" << endl;
			endGame = true;
			cx = -1;
			cy = -1;
			break;
		case 2:
			cout << "Hmm... Parece que Spok precisa encontrar um caminho diferente" << endl;
			break;
		case 3:
			cout << "Parabens, voce reuniu Spok e sua dona! Pressione espaco para reiniciar" << endl;
			endGame = true;
			cx = -1;
			cy = -1;
			break;
	}
	
	return;
}

void keyCallback(GLFWwindow* window, int key, int scancode, int action, int mods) {
	if (key == GLFW_KEY_ESCAPE && action == GLFW_RELEASE) {
		glfwSetWindowShouldClose(window, 1);
	} else if (key == GLFW_KEY_W && action == GLFW_RELEASE) {
		moveDog(cx, cy, DIRECTION_EAST);
		objectAction = 1;
		currentFrame = (currentFrame + 1) % 4;
		offsetx = fw * (float)currentFrame;
		offsety = fh * (float)objectAction;
	} else if (key == GLFW_KEY_S && action == GLFW_RELEASE) {
		moveDog(cx, cy, DIRECTION_WEST);
		objectAction = 2;
		currentFrame = (currentFrame + 1) % 4;
		offsetx = fw * (float)currentFrame;
		offsety = fh * (float)objectAction;
	} else if (key == GLFW_KEY_A && action == GLFW_RELEASE) {
		moveDog(cx, cy, DIRECTION_NORTH);
		objectAction = 0;
		currentFrame = (currentFrame + 1) % 4;
		offsetx = fw * (float)currentFrame;
		offsety = fh * (float)objectAction;
	} else if (key == GLFW_KEY_D && action == GLFW_RELEASE) {
		moveDog(cx, cy, DIRECTION_SOUTH);
		objectAction = 3;
		currentFrame = (currentFrame + 1) % 4;
		offsetx = fw * (float)currentFrame;
		offsety = fh * (float)objectAction;
	} else if (key == GLFW_KEY_SPACE && action == GLFW_RELEASE) {
		cx = 0;
		cy = 0;
		endGame = 0;
	}

	return;
}

int main() {
#pragma region Initialize OpenGL
	glEnable(GL_DEPTH_TEST);
	glDepthFunc(GL_LESS);
	restartGlLog();
	startGl("Prova Grau B");
#pragma endregion

#pragma region Load Tile Map on Scenario
	tileMap = readMap("texture/terrain1.tmap");
	tw = w / (float)tileMap->getWidth();
	th = tw / 2.0f;
	tw2 = th;
	th2 = th / 2.0f;
	tileW = 1.0f / (float)tileSetCols;
	tileW2 = tileW / 2.0f;
	tileH = 1.0f / (float)tileSetRows;
	tileH2 = tileH / 2.0f;
#pragma endregion

#pragma region Load Textures
	GLuint tid;
	loadTexture(tid, "texture/lava.png");
	tileMap->setTid(tid);
	for (int r = 0; r < tileMap->getHeight(); r++) {
		for (int c = 0; c < tileMap->getWidth(); c++) {
			unsigned char tileId = tileMap->getTile(c, r);
		}
	}

	vector<Object*> objects;
	Object* dog = new Object;
	dog->filename = "texture/dog.png";
	dog->offsetx = 0;
	dog->offsety = 0;
	dog->z = 0.44;
	dog->currentFrame = 0;
	objects.push_back(dog);
	loadTexture(dog->tid, dog->filename);

	Object* girl = new Object;
	girl->filename = "texture/girl.png";
	girl->offsetx = 0;
	girl->offsety = 0;
	girl->z = 0.44;
	girl->currentFrame = 0;
	objects.push_back(girl);
	loadTexture(girl->tid, girl->filename);

	Object* tree = new Object;
	tree->filename = "texture/tree.png";
	tree->offsetx = 0;
	tree->offsety = 0;
	tree->z = 0.44;
	tree->currentFrame = 0;
	objects.push_back(tree);
	loadTexture(tree->tid, tree->filename);
	collisionMap = readMap("texture/collide.tmap");
#pragma endregion

#pragma region Vertex
	float vertexScene[] = {
		xi      , yi + th2,   0.0f, tileH2,
		xi + tw2, yi      , tileW2,   0.0f,
		xi + tw , yi + th2,  tileW, tileH2,
		xi + tw2, yi + th , tileW2,  tileH,
	};

	float vertexObject[] = {
		 -2.6f, -0.8f, 0.25f, 0.25f,
		 -2.6f, -1.0f, 0.25f,  0.0f,
		 -2.8f, -1.0f,  0.0f,  0.0f,
		 -2.8f, -0.8f,  0.0f, 0.25f,
	};

	unsigned int index[] = {
		0, 1, 3,
		3, 1, 2
	};
#pragma endregion
	
#pragma region VBO and GPU Binding
	unsigned int vboScene, vboDog, vboGirl, vboTree, vao, ebo;
	glGenBuffers(1, &ebo);
	glGenVertexArrays(1, &vao);
	glBindVertexArray(vao);

	// VBO para o cenário
	glGenBuffers(1, &vboScene);
	glBindBuffer(GL_ARRAY_BUFFER, vboScene);
	glBufferData(GL_ARRAY_BUFFER, sizeof(vertexScene), vertexScene, GL_STATIC_DRAW);
	glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)0);
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)(2 * sizeof(float)));
	glEnableVertexAttribArray(1);

	// VBO Para Spok (Cão)
	glGenBuffers(1, &vboDog);
	glBindBuffer(GL_ARRAY_BUFFER, vboDog);
	glBufferData(GL_ARRAY_BUFFER, sizeof(vertexObject), vertexObject, GL_STATIC_DRAW);
	glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)0);
	glEnableVertexAttribArray(2);
	glVertexAttribPointer(3, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)(2 * sizeof(float)));
	glEnableVertexAttribArray(3);

	// VBO Para A Dona
	glGenBuffers(1, &vboGirl);
	glBindBuffer(GL_ARRAY_BUFFER, vboGirl);
	glVertexAttribPointer(4, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)0);
	glEnableVertexAttribArray(4);
	glVertexAttribPointer(5, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)(2 * sizeof(float)));
	glEnableVertexAttribArray(5);

	// VBO Para A árvore
	glGenBuffers(1, &vboTree);
	glBindBuffer(GL_ARRAY_BUFFER, vboTree);
	glVertexAttribPointer(6, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)0);
	glEnableVertexAttribArray(6);
	glVertexAttribPointer(7, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)(2 * sizeof(float)));
	glEnableVertexAttribArray(7);

	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ebo);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(index), index, GL_STATIC_DRAW);
#pragma endregion

#pragma region Shaders and shader program
	GLuint vsScene;
	createShader("shaders/vertex_shader.glsl", &vsScene, GL_VERTEX_SHADER);
	GLuint fsScene;
	createShader("shaders/fragment_shader.glsl", &fsScene, GL_FRAGMENT_SHADER);
	GLuint spScene;
	createProgramme(vsScene, fsScene, &spScene);
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	glfwSetKeyCallback(window, keyCallback);
#pragma endregion
	
#pragma region Main loop
	cout << "Ajude Spok a chegar ate sua dona, mas cuidado para que Spok nao caia na lava!" << endl;

	while (!glfwWindowShouldClose(window)) {
		glViewport(0, 0, glWidth, glHeight);

		// Limpando a tela
		glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		glUseProgram(spScene);
		glBindVertexArray(vao);

	#pragma region Draw Scenario
		glBindBuffer(GL_ARRAY_BUFFER, vboScene);
		glUniform1f(glGetUniformLocation(spScene, "isObject"), false);

		float x, y;
		int r = 0, c = 0;
		for (int r = 0; r < tileMap->getHeight(); r++) {
			for (int c = 0; c < tileMap->getWidth(); c++) {
				int t_id = (int)tileMap->getTile(c, r);
				int u = t_id % tileSetCols;
				int v = t_id / tileSetCols;

				tview->positionTile(c, r, tw, th, x, y);
				glUniform1f(glGetUniformLocation(spScene, "offsetx"), u * tileW);
				glUniform1f(glGetUniformLocation(spScene, "offsety"), v * tileH);
				glUniform1f(glGetUniformLocation(spScene, "tx"), x);
				glUniform1f(glGetUniformLocation(spScene, "ty"), y + 1.0);
				glUniform1f(glGetUniformLocation(spScene, "layer_z"), 0.50);
				glUniform1f(glGetUniformLocation(spScene, "weight"), (c == cx) && (r == cy) ? 0.5 : 0.0);
				glBindTexture(GL_TEXTURE_2D, tileMap->getTileSet());
				glUniform1i(glGetUniformLocation(spScene, "sprite"), 0);
				glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
			}
		}
	#pragma endregion

		glActiveTexture(GL_TEXTURE0);

	#pragma region Dog
		float tx, ty;
		tview->positionTile(cx, cy, tw, th, tx, ty);
		glBindBuffer(GL_ARRAY_BUFFER, vboDog);
		glUniform1f(glGetUniformLocation(spScene, "isObject"), true);
		glUniform1i(glGetUniformLocation(spScene, "sprite"), 0);
		glUniform1f(glGetUniformLocation(spScene, "tx"), 1.8 + tx);
		glUniform1f(glGetUniformLocation(spScene, "ty"), 1.0 + ty);
		glUniform1f(glGetUniformLocation(spScene, "offsetx"), offsetx);
		glUniform1f(glGetUniformLocation(spScene, "offsety"), offsety);
		glUniform1f(glGetUniformLocation(spScene, "layer_z"), 0.10);
		glBindTexture(GL_TEXTURE_2D, objects[0]->tid);
		if (!endGame) glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
	#pragma endregion

	#pragma region Girl
		tview->positionTile(cx, cy, tw, th, tx, ty);
		glBindBuffer(GL_ARRAY_BUFFER, vboGirl);
		glUniform1f(glGetUniformLocation(spScene, "isObject"), true);
		glUniform1i(glGetUniformLocation(spScene, "sprite"), 0);
		glUniform1f(glGetUniformLocation(spScene, "tx"), 3.2);
		glUniform1f(glGetUniformLocation(spScene, "ty"), 0.90);
		glUniform1f(glGetUniformLocation(spScene, "offsetx"), 0.5);
		glUniform1f(glGetUniformLocation(spScene, "offsety"), 0.5);
		glUniform1f(glGetUniformLocation(spScene, "layer_z"), 0.10);
		glBindTexture(GL_TEXTURE_2D, objects[1]->tid);
		glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
	#pragma endregion

	#pragma region Tree
		tview->positionTile(cx, cy, tw, th, tx, ty);
		glBindBuffer(GL_ARRAY_BUFFER, vboTree);
		glUniform1f(glGetUniformLocation(spScene, "isObject"), true);
		glUniform1i(glGetUniformLocation(spScene, "sprite"), 0);
		glUniform1f(glGetUniformLocation(spScene, "tx"), 2.17);
		glUniform1f(glGetUniformLocation(spScene, "ty"), 0.92);
		glUniform1f(glGetUniformLocation(spScene, "offsetx"), 0.5);
		glUniform1f(glGetUniformLocation(spScene, "offsety"), 0.5);
		glUniform1f(glGetUniformLocation(spScene, "layer_z"), 0.10);
		glBindTexture(GL_TEXTURE_2D, objects[2]->tid);
		glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
	#pragma endregion

		glfwPollEvents();
		glfwSwapBuffers(window);
	}

	glfwTerminate();
	delete tileMap;
	delete collisionMap;
	return 0;
}
#pragma endregion
