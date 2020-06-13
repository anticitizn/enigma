#define GL_GLEXT_PROTOTYPES 1
#include <iostream>
#include <sstream>
#include <fstream>
#include <string>
#include <vector>

#include <microui/microui.h>
#include "glm/glm.hpp"
#include "glm/gtc/matrix_transform.hpp"
#include "glm/gtc/type_ptr.hpp"
#include "glad/glad.h"
#include "SDL.h"
#include "SDL_opengl.h"

#include <shaders/shader.h>
#include "camera.h"

using namespace std;

string shadersPath = "dependencies/include/shaders/";
int running = 1;

// camera
Camera sceneCamera;

// timing
float deltaTime = 0.0f;
float lastFrame = 0.0f;

void processKeyboard(const Uint8* state);
void processMouse(SDL_Event* event);	
vector<float> addNormals(vector <float> inputVertices, int leap);

int main(int argc, char *argv[])
{
	// SDL window and OpenGL context initialization
	SDL_Window* screen;
	cout << "Initializing SDL" << endl;

	if (SDL_Init(SDL_INIT_VIDEO || SDL_INIT_AUDIO) == -1)
	{
		cout << "SDL initialization failed " << SDL_GetError();
		exit(-1);
	}

	atexit(SDL_Quit);

	screen = SDL_CreateWindow("SDL Playground", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, 800, 600, SDL_WINDOW_OPENGL);
	if (screen == NULL)
	{
		cout << "Couldn't set 640x480 video mode" << endl << SDL_GetError() << endl;
		exit(1);
	}
	
	SDL_GLContext context = SDL_GL_CreateContext(screen);
	SDL_Event userEvent;
	cout << "SDL initialized" << endl;

	SDL_SetRelativeMouseMode(SDL_TRUE);

	// Initializing GLAD, make sure it's after the OpenGL context initialization
	if (!gladLoadGLLoader((GLADloadproc)SDL_GL_GetProcAddress))
	{
		cout << "Failed to initialize GLAD" << endl;
		return -1;
	}

	// OpenGL logic
	glViewport(0, 0, 800, 600);

	glEnable(GL_DEPTH_TEST);

	vector <float> vertices = {
	-0.5f, -0.5f, -0.5f,  0.0f,  0.0f, -1.0f,
	 0.5f, -0.5f, -0.5f,  0.0f,  0.0f, -1.0f,
	 0.5f,  0.5f, -0.5f,  0.0f,  0.0f, -1.0f,
	 0.5f,  0.5f, -0.5f,  0.0f,  0.0f, -1.0f,
	-0.5f,  0.5f, -0.5f,  0.0f,  0.0f, -1.0f,
	-0.5f, -0.5f, -0.5f,  0.0f,  0.0f, -1.0f,

	-0.5f, -0.5f,  0.5f,  0.0f,  0.0f, 1.0f,
	 0.5f, -0.5f,  0.5f,  0.0f,  0.0f, 1.0f,
	 0.5f,  0.5f,  0.5f,  0.0f,  0.0f, 1.0f,
	 0.5f,  0.5f,  0.5f,  0.0f,  0.0f, 1.0f,
	-0.5f,  0.5f,  0.5f,  0.0f,  0.0f, 1.0f,
	-0.5f, -0.5f,  0.5f,  0.0f,  0.0f, 1.0f,

	-0.5f,  0.5f,  0.5f, -1.0f,  0.0f,  0.0f,
	-0.5f,  0.5f, -0.5f, -1.0f,  0.0f,  0.0f,
	-0.5f, -0.5f, -0.5f, -1.0f,  0.0f,  0.0f,
	-0.5f, -0.5f, -0.5f, -1.0f,  0.0f,  0.0f,
	-0.5f, -0.5f,  0.5f, -1.0f,  0.0f,  0.0f,
	-0.5f,  0.5f,  0.5f, -1.0f,  0.0f,  0.0f,

	 0.5f,  0.5f,  0.5f,  1.0f,  0.0f,  0.0f,
	 0.5f,  0.5f, -0.5f,  1.0f,  0.0f,  0.0f,
	 0.5f, -0.5f, -0.5f,  1.0f,  0.0f,  0.0f,
	 0.5f, -0.5f, -0.5f,  1.0f,  0.0f,  0.0f,
	 0.5f, -0.5f,  0.5f,  1.0f,  0.0f,  0.0f,
	 0.5f,  0.5f,  0.5f,  1.0f,  0.0f,  0.0f,

	-0.5f, -0.5f, -0.5f,  0.0f, -1.0f,  0.0f,
	 0.5f, -0.5f, -0.5f,  0.0f, -1.0f,  0.0f,
	 0.5f, -0.5f,  0.5f,  0.0f, -1.0f,  0.0f,
	 0.5f, -0.5f,  0.5f,  0.0f, -1.0f,  0.0f,
	-0.5f, -0.5f,  0.5f,  0.0f, -1.0f,  0.0f,
	-0.5f, -0.5f, -0.5f,  0.0f, -1.0f,  0.0f,

	-0.5f,  0.5f, -0.5f,  0.0f,  1.0f,  0.0f,
	 0.5f,  0.5f, -0.5f,  0.0f,  1.0f,  0.0f,
	 0.5f,  0.5f,  0.5f,  0.0f,  1.0f,  0.0f,
	 0.5f,  0.5f,  0.5f,  0.0f,  1.0f,  0.0f,
	-0.5f,  0.5f,  0.5f,  0.0f,  1.0f,  0.0f,
	-0.5f,  0.5f, -0.5f,  0.0f,  1.0f,  0.0f
	};

	unsigned int VBO, VAO, lampVAO;
	glGenVertexArrays(1, &VAO);
	glGenBuffers(1, &VBO);

	glBindVertexArray(VAO);
	
	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(float), vertices.data(), GL_STATIC_DRAW);

	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)0);
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)(3 * sizeof(float)));
	glEnableVertexAttribArray(1);

	// lamp shader buffers
	glGenVertexArrays(1, &lampVAO);
	glBindVertexArray(lampVAO);

	glBindBuffer(GL_ARRAY_BUFFER, VBO);

	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)0);
	glEnableVertexAttribArray(0);

	// shader initialization
	Shader sceneShader(shadersPath + "shader.vs", shadersPath + "shader.fs");
	sceneShader.use();

	glm::vec3 lightPos = glm::vec3(1.2f, 0.5f, 2.0f);
	glm::vec3 ambientColor = glm::vec3(0.1f, 0.1f, 0.1f);

	sceneShader.setUniform("LightPos", lightPos);

	Shader lampShader(shadersPath + "lampshader.vs", shadersPath + "lampshader.fs");
	lampShader.use();
	
	// Game loop
	while (running)
	{
		sceneShader.use();
		glClearColor(0.5f, 0.2f, 0.3f, 0.0f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);	

		glm::mat4 model = glm::mat4(1.0f);
		sceneShader.setUniform("model", model);

		glm::mat4 view = sceneCamera.getViewMatrix();
		sceneShader.setUniform("view", view);

		glm::mat4 projection = glm::mat4(1.0f);
		projection = glm::perspective(glm::radians(sceneCamera.fov), 800.0f / 600.0f, 0.01f, 1000.0f);
		sceneShader.setUniform("projection", projection);

		glm::mat3 normalMatrix = glm::mat3(glm::transpose(glm::inverse(view * model)));
		sceneShader.setUniform("normalMatrix", normalMatrix);

		glBindVertexArray(VAO);
		glDrawArrays(GL_TRIANGLES, 0, 36);

		lampShader.use();

		model = glm::mat4(1.0f);
		model = glm::translate(model, lightPos);
		model = glm::scale(model, glm::vec3(0.2f));
		lampShader.setUniform("model", model);
		lampShader.setUniform("view", view);
		lampShader.setUniform("projection", projection);

		glBindVertexArray(lampVAO);
		glDrawArrays(GL_TRIANGLES, 0, 36);

		SDL_GL_SwapWindow(screen);

		float currentTime = SDL_GetTicks();
		deltaTime = currentTime - lastFrame;
		lastFrame = currentTime;

		const Uint8* kbstate = SDL_GetKeyboardState(NULL);

		processKeyboard(kbstate);
		while (SDL_PollEvent(&userEvent))
		{
			processMouse(&userEvent);
		}
	}
	cout << "Quitting SDL" << endl;
	SDL_Quit();
	cout << "Quit SDL" << endl;
	exit(0);
}

void processKeyboard(const Uint8* state)
{
	if (state[SDL_SCANCODE_ESCAPE])
		running = 0;

	// camera movement
	if(state[SDL_SCANCODE_W])
		sceneCamera.processKeyboard("FRONT", deltaTime);
	if (state[SDL_SCANCODE_S])
		sceneCamera.processKeyboard("BACK", deltaTime);
	if (state[SDL_SCANCODE_A])
		sceneCamera.processKeyboard("LEFT", deltaTime);
	if (state[SDL_SCANCODE_D])
		sceneCamera.processKeyboard("RIGHT", deltaTime);
	if (state[SDL_SCANCODE_SPACE])
		sceneCamera.processKeyboard("UP", deltaTime);
	if (state[SDL_SCANCODE_C])
		sceneCamera.processKeyboard("DOWN", deltaTime);
}

void processMouse(SDL_Event* event)
{
	switch (event->type)
	{
		case SDL_MOUSEWHEEL:
			sceneCamera.processScroll(event->wheel.y, deltaTime);
			break;
		case SDL_MOUSEMOTION:
			sceneCamera.processMouse(event->motion.xrel, event->motion.yrel);
			break;
	}

}

vector<float> addNormals(vector <float> inputVertices, int leap)
{
	// unfinished, using preset normals for now
	vector <float> vertices = inputVertices;
	int len = vertices.size();
	for (double i = 0; i < len / leap; i++)
	{
		//cout << "i info: " << i << " " << leap << " " << len << " " << len / leap << " " << i*leap+8 << endl << endl;
		glm::vec3 p1 = glm::vec3(vertices[i*leap], vertices[i * leap + 1], vertices[i * leap + 2]);
		glm::vec3 p2 = glm::vec3(vertices[i * leap + 3], vertices[i * leap + 4], vertices[i * leap + 5]);
		glm::vec3 p3 = glm::vec3(vertices[i * leap + 6], vertices[i * leap + 7], vertices[i * leap + 8]);

		glm::vec3 U = p2 - p1;
		glm::vec3 V = p3 - p1;

		glm::vec3 normal; 
		normal.x = U.y * V.z - U.z * V.y;
		normal.y = U.z * V.x - U.x * V.z;
		normal.z = U.x * V.y - U.y * V.x;
		//cout << "normal vals: " << p1.x << " " << p1.y << " " << p1.z << endl;

		// normalizing causes lots of random -(nan) ind
		// inserting normals into the vector increases the index of the elements after the insertion, therefore making previously gathered indexes invalid
		// should normals be inserted after every triangle or after every vertex? 
		// the current vertex attrib pointer assumes that they are inserted after every vertex

		//cout << "normal: " << normal.x << " " << normal.y << " " << normal.z << endl;
		vector<float>::iterator it = vertices.begin() + i + 9;
		vertices.insert(it, normal.x);
		it = vertices.begin() + i + 10;
		vertices.insert(it, normal.y);
		it = vertices.begin() + i + 11;
		vertices.insert(it, normal.z);
	}
	return vertices;
}