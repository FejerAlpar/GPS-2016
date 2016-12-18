#version 400

in vec3 colour;
in vec2 passTexture;
out vec4 fragmentColour;

uniform sampler2D diffuseTexture;

void main() {
	fragmentColour = texture(diffuseTexture, passTexture);
}