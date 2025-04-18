#include "shaders.h"
#include <stdio.h>
#include <stdlib.h>
#include <glad/glad.h>
#include <GLFW/glfw3.h>

int success;
char infoLog[512];

char* _loadToBuffer(char filePath[]) {
	// open file
	FILE* fpoint;
	fpoint = fopen(filePath, "r");

	if (fpoint == NULL) {
		printf("An error occurred opening the file\n");

		return NULL;
	}
	// read the file
	fseek(fpoint, 0, SEEK_END);
	size_t size = ftell(fpoint);
	fseek(fpoint, 0, SEEK_SET);

	char* buffer = malloc(size + 1);

	if (buffer) {
		size_t bytesRead = fread(buffer, 1, size, fpoint);
		buffer[bytesRead] = '\0';
	}

	fclose(fpoint);
	return buffer;
}

unsigned int _compile(char source[], int shaderType) {
	unsigned int shader = glCreateShader(shaderType);
	glShaderSource(shader, 1, &source, NULL);
	glCompileShader(shader);
	glGetShaderiv(shader, GL_COMPILE_STATUS, &success);
	if (!success)
	{
		glGetShaderInfoLog(shader, 512, NULL, infoLog);
		printf("compile failed");
		printf(infoLog);
		return -1;
	}
	printf("successfully compiled shader\n\n");

	return shader;
}

unsigned int compileShaderFile(char filePath[], int shaderType) {
	char* buffer = _loadToBuffer(filePath);
	unsigned int shader = _compile(buffer, shaderType);
	free(buffer);
	return shader;
}

unsigned int createGPUProgram(unsigned int shaders[], unsigned int size) {
	unsigned int shaderProgram = glCreateProgram();
	for (unsigned int i = 0; i < size; i++) {
		glAttachShader(shaderProgram, shaders[i]);
	}
	glLinkProgram(shaderProgram);
	glGetProgramiv(shaderProgram, GL_LINK_STATUS, &success);
	if (!success) {
		printf("Shader Program Link Failed: \n");
		glGetProgramInfoLog(shaderProgram, 512, NULL, infoLog);
	}

	return shaderProgram;
}