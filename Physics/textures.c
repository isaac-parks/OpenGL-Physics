#include <stb/stb_image.h>
#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <stdbool.h>
#include "textures.h"

char* loadImage(char path[], int* width, int* height, int* nrChannels) {
	unsigned char* image = stbi_load(path, width, height, nrChannels, 0);

	return image;
}

 unsigned int createTexture() {
	int width, height, nrChannels;
	stbi_set_flip_vertically_on_load(false);
	unsigned char* image = loadImage("grass.jpg", &width, &height, &nrChannels);
	unsigned int texture;
	glGenTextures(1, &texture);
	glBindTexture(GL_TEXTURE_2D, texture);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, image);
	glGenerateMipmap(GL_TEXTURE_2D);

	stbi_image_free(image);

	return texture;
}