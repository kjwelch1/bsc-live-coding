// 01_FirstOpenGL.cpp : Defines the entry point for the application.
//

#include "stdafx.h"
#include "01_FirstOpenGL.h"

// NOTE: this code is intended to illustrate usage of OpenGL.
// It is NOT intended to illustrate good coding style or naming conventions!
// Code has been copied and pasted from tutorials which use different conventions.
// In your own projects, make sure you rename identifiers to follow a consistent style.

bool compileShader(GLuint shaderId, const std::string& shaderFileName)
{
	// Read the source code from the file
	std::string shaderSource;
	std::ifstream sourceStream(shaderFileName, std::ios::in);
	if (sourceStream.is_open())
	{
		std::stringstream buffer;
		buffer << sourceStream.rdbuf();
		shaderSource = buffer.str();
		sourceStream.close();
	}
	else
	{
		MessageBoxA(nullptr, shaderFileName.c_str(), "File not found", MB_OK | MB_ICONERROR);
		return false;
	}

	// Compile the shader
	const char* sourcePointer = shaderSource.c_str();
	glShaderSource(shaderId, 1, &sourcePointer, NULL);
	glCompileShader(shaderId);

	// Check the results of compilation
	GLint result = GL_FALSE;
	int infoLogLength = 0;
	glGetShaderiv(shaderId, GL_COMPILE_STATUS, &result);
	glGetShaderiv(shaderId, GL_INFO_LOG_LENGTH, &infoLogLength);
	if (infoLogLength > 0)
	{
		// Display the compilation log
		std::vector<char> errorMessage(infoLogLength + 1);
		glGetShaderInfoLog(shaderId, infoLogLength, NULL, errorMessage.data());
		MessageBoxA(nullptr, errorMessage.data(), shaderFileName.c_str(), MB_OK | MB_ICONWARNING);
	}

	return (result != GL_FALSE);
}

GLuint loadShaders(const std::string& vertex_file_path, const std::string& fragment_file_path) {

	// Create the shaders
	GLuint vertexShaderId = glCreateShader(GL_VERTEX_SHADER);
	compileShader(vertexShaderId, vertex_file_path);

	GLuint fragmentShaderId = glCreateShader(GL_FRAGMENT_SHADER);
	compileShader(fragmentShaderId, fragment_file_path);

	// Link the program
	GLuint programId = glCreateProgram();
	glAttachShader(programId, vertexShaderId);
	glAttachShader(programId, fragmentShaderId);
	glLinkProgram(programId);

	// Check the program
	GLint result = GL_FALSE;
	int infoLogLength = 0;
	glGetProgramiv(programId, GL_LINK_STATUS, &result);
	glGetProgramiv(programId, GL_INFO_LOG_LENGTH, &infoLogLength);
	if (infoLogLength > 0) {
		std::vector<char> errorMessage(infoLogLength + 1);
		glGetProgramInfoLog(programId, infoLogLength, NULL, errorMessage.data());
		MessageBoxA(nullptr, errorMessage.data(), "glLinkProgram error", MB_OK | MB_ICONWARNING);
	}

	glDetachShader(programId, vertexShaderId);
	glDetachShader(programId, fragmentShaderId);

	glDeleteShader(vertexShaderId);
	glDeleteShader(fragmentShaderId);

	return programId;
}

int main(int argc, char* args[])
{
	if (SDL_Init(SDL_INIT_VIDEO) < 0)
	{
		MessageBoxA(nullptr, "SDL_Init failed", SDL_GetError(), MB_OK | MB_ICONERROR);
		return 1;
	}

	SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_CORE);
	SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 3);
	SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 2);
	SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1);

	SDL_Window* window = SDL_CreateWindow("My first OpenGL program", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, 800, 600, SDL_WINDOW_SHOWN | SDL_WINDOW_OPENGL);

	if (window == nullptr)
	{
		MessageBoxA(nullptr, "SDL_CreateWindow failed", SDL_GetError(), MB_OK | MB_ICONERROR);
		return 1;
	}

	SDL_GLContext glContext = SDL_GL_CreateContext(window);

	if (glContext == nullptr)
	{
		MessageBoxA(nullptr, "SDL_GL_CreateContext failed", SDL_GetError(), MB_OK | MB_ICONERROR);
		return 1;
	}

	if (glewInit() != GLEW_OK)
	{
		MessageBoxA(nullptr, "glewInit failed", ":(", MB_OK | MB_ICONERROR);
	}

	GLuint VertexArrayID;
	glGenVertexArrays(1, &VertexArrayID);
	glBindVertexArray(VertexArrayID);

	// An array of 3 vectors which represents 3 vertices
	static const GLfloat g_vertex_buffer_data[] = {
		-1.0f, -1.0f, 0.0f,
		1.0f, -1.0f, 0.0f,
		0.0f,  1.0f, 0.0f,
	};

	// This will identify our vertex buffer
	GLuint vertexbuffer;
	// Generate 1 buffer, put the resulting identifier in vertexbuffer
	glGenBuffers(1, &vertexbuffer);
	// The following commands will talk about our 'vertexbuffer' buffer
	glBindBuffer(GL_ARRAY_BUFFER, vertexbuffer);
	// Give our vertices to OpenGL.
	glBufferData(GL_ARRAY_BUFFER, sizeof(g_vertex_buffer_data), g_vertex_buffer_data, GL_STATIC_DRAW);

	GLuint programID = loadShaders("vertex.glsl", "fragment.glsl");

	bool running = true;
	while (running)
	{
		SDL_Event ev;
		while (SDL_PollEvent(&ev))
		{
			switch (ev.type)
			{
			case SDL_QUIT:
				running = false;
				break;
			}
		}

		glClearColor(0.0f, 0.5f, 1.0f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT);

		glUseProgram(programID);

		// 1rst attribute buffer : vertices
		glEnableVertexAttribArray(0);
		glBindBuffer(GL_ARRAY_BUFFER, vertexbuffer);
		glVertexAttribPointer(
			0,                  // attribute 0. No particular reason for 0, but must match the layout in the shader.
			3,                  // size
			GL_FLOAT,           // type
			GL_FALSE,           // normalized?
			0,                  // stride
			(void*)0            // array buffer offset
			);
		// Draw the triangle !
		glDrawArrays(GL_TRIANGLES, 0, 3); // Starting from vertex 0; 3 vertices total -> 1 triangle
		glDisableVertexAttribArray(0);

		SDL_GL_SwapWindow(window);
	}

	SDL_GL_DeleteContext(glContext);
	SDL_DestroyWindow(window);

	SDL_Quit();

	return 0;
}
