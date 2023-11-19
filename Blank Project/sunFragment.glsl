// sunFragment.glsl
#version 330 core

in vec2 fragTexCoord;
out vec4 fragColor;

uniform sampler2D diffuseTex; 

void main() {
    fragColor = texture(diffuseTex, fragTexCoord);
}
