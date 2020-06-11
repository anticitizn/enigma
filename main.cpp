#define GL_GLEXT_PROTOTYPES 1
#include <iostream>
#include <sstream>
#include <fstream>
#include <string>

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
void processInput(SDL_Event* event);

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

	float vertices[] = {
	-0.5f, -0.5f, -0.5f,
	 0.5f, -0.5f, -0.5f,
	 0.5f,  0.5f, -0.5f,
	 0.5f,  0.5f, -0.5f,
	-0.5f,  0.5f, -0.5f,
	-0.5f, -0.5f, -0.5f,

	-0.5f, -0.5f,  0.5f,
	 0.5f, -0.5f,  0.5f,
	 0.5f,  0.5f,  0.5f,
	 0.5f,  0.5f,  0.5f,
	-0.5f,  0.5f,  0.5f,
	-0.5f, -0.5f,  0.5f,

	-0.5f,  0.5f,  0.5f,
	-0.5f,  0.5f, -0.5f,
	-0.5f, -0.5f, -0.5f,
	-0.5f, -0.5f, -0.5f,
	-0.5f, -0.5f,  0.5f,
	-0.5f,  0.5f,  0.5f,

	 0.5f,  0.5f,  0.5f,
	 0.5f,  0.5f, -0.5f,
	 0.5f, -0.5f, -0.5f,
	 0.5f, -0.5f, -0.5f,
	 0.5f, -0.5f,  0.5f,
	 0.5f,  0.5f,  0.5f,

	-0.5f, -0.5f, -0.5f,
	 0.5f, -0.5f, -0.5f,
	 0.5f, -0.5f,  0.5f,
	 0.5f, -0.5f,  0.5f,
	-0.5f, -0.5f,  0.5f,
	-0.5f, -0.5f, -0.5f,

	-0.5f,  0.5f, -0.5f,
	 0.5f,  0.5f, -0.5f,
	 0.5f,  0.5f,  0.5f,
	 0.5f,  0.5f,  0.5f,
	-0.5f,  0.5f,  0.5f,
	-0.5f,  0.5f, -0.5f
	};

	unsigned int VBO, VAO;
	glGenVertexArrays(1, &VAO);
	glGenBuffers(1, &VBO);

	glBindVertexArray(VAO);
	
	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
	glEnableVertexAttribArray(0);

	Shader sceneShader(shadersPath + "shader.vs", shadersPath + "shader.fs");
	

	// Matrix operations
	glm::mat4 model = glm::mat4(1.0f);


	glm::mat4 projection = glm::mat4(1.0f);
	projection = glm::perspective(glm::radians(60.0f), 800.0f / 600.0f, 0.1f, 1000.0f);

	// Game loop
	while (running)
	{
		
		glClearColor(0.5f, 0.2f, 0.3f, 0.0f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);	

		sceneShader.use();

		sceneShader.setUniform("model", model);
		glm::mat4 view = sceneCamera.getViewMatrix();
		sceneShader.setUniform("view", view);
		sceneShader.setUniform("projection", projection);

		glDrawArrays(GL_TRIANGLES, 0, 36);
		SDL_GL_SwapWindow(screen);

		float currentTime = SDL_GetTicks();
		deltaTime = currentTime - lastFrame;
		lastFrame = currentTime;

		const Uint8* kbstate = SDL_GetKeyboardState(NULL);

		processKeyboard(kbstate);
		while (SDL_PollEvent(&userEvent))
		{
			processInput(&userEvent);
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

void processInput(SDL_Event* event)
{
	switch (event->type)
	{
		
		case SDL_MOUSEMOTION:
			sceneCamera.processMouse(event->motion.xrel, event->motion.yrel);
			break;
	}

}