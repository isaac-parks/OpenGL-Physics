#pragma once
unsigned int compileShaderFile(char filePath[], int shaderType);

unsigned int createGPUProgram(unsigned int shaders[], unsigned int size);