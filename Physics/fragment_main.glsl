#version 330 core

in vec2 VertexTexture;

uniform sampler2D BlockTexture;

out vec4 FragColor;


void main()
{
   FragColor = texture(BlockTexture, VertexTexture);
}