#version 330 core

layout (location = 0) in vec3 Position;
layout (location = 1) in vec2 Texture;

uniform mat4 Orthographic;
uniform mat4 Model;
uniform mat4 View;

out vec3 VertexColor;
out vec2 VertexTexture;

void main()
{
   gl_Position = Orthographic * View * Model * vec4(Position, 1.0);
   VertexTexture = Texture;
}