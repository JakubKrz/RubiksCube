#version 460 core
out vec4 FragColor;

in vec2 TexCoords;
in vec3 Normal;
in vec3 FragPos;

struct Material {
    sampler2D diffuse1;
    float shininess;
};

uniform Material material;

uniform vec3 lightPos;
uniform vec3 viewPos;
uniform vec3 lightColor;


void main()
{
    vec3 color = texture(material.diffuse1, TexCoords).rgb;

    vec3 norm = normalize(Normal);
    vec3 lightDir = normalize(lightPos - FragPos);
    vec3 viewDir = normalize(viewPos - FragPos);

    float ambientStrength = 0.1;
    vec3 ambient = ambientStrength * lightColor;

    float diff = max(dot(norm, lightDir), 0.0);
    vec3 diffuse = diff * lightColor;

    vec3 halfwayDir = normalize(lightDir + viewDir);

    float spec = pow(max(dot(norm, halfwayDir), 0.0), material.shininess);
    vec3 specular =  spec * lightColor;

    vec3 result = (ambient + diffuse + specular) * color;
    FragColor = vec4(result, 1.0);
}