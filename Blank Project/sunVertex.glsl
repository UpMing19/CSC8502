// sunVertex.glsl
#version 330 core

layout(location = 0) in vec3 position;
layout(location = 1) in vec2 texCoord;

out vec2 fragTexCoord;

uniform mat4 modelMatrix;
uniform mat4 viewMatrix;
uniform mat4 projMatrix;

void main() {
    fragTexCoord = texCoord;
    gl_Position = projMatrix * viewMatrix * modelMatrix * vec4(position, 1.0);
}
