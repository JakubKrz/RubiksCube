#version 460 core
out vec4 FragColor;

in vec2 TexCoords;
in vec3 Normal;
in vec3 FragPos;

struct Material {
    sampler2D diffuse1;
};

uniform Material material;

void main()
{
    FragColor = texture(material.diffuse1, TexCoords);
}