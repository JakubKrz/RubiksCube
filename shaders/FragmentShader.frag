#version 460 core

out vec4 FragColor;

in vec2 TexCoords;
in vec3 Normal;
in vec3 FragPos;

struct Material {
    sampler2D diffuse1;
    sampler2D diffuse2;
    float shininess;
};

struct DirectionalLight {
    vec3 direction;
    vec3 color;
};

struct SpotLight {
    vec3 position;
    vec3 direction;
    float cutOff;
    float outerCutOff;
  
    vec3 color;

    float constant;
    float linear;
    float quadratic;
};

vec3 CalcDirLight(DirectionalLight light, vec3 normal, vec3 viewDir, vec3 albedo);
vec3 CalcSpotLight(SpotLight spotLight, vec3 normal, vec3 fragPos, vec3 viewDir, vec3 albedo);

uniform Material material;
//Lights
#define MAX_DIR_LIGHTS 4
uniform int nrOfDirLights;
uniform DirectionalLight dirLights[MAX_DIR_LIGHTS];
uniform SpotLight spotLight;

uniform vec3 viewPos;
uniform float mixAlpha;
uniform float specularStrength;

void main()
{
    vec4 texColor1 = texture(material.diffuse1, TexCoords);
    vec4 texColor2 = texture(material.diffuse2, TexCoords);
    vec3 color = mix(texColor1, texColor2, mixAlpha).rgb;

    vec3 norm = normalize(Normal);
    vec3 viewDir = normalize(viewPos - FragPos);

    vec3 result = vec3(0.0);
    for(int i = 0; i < nrOfDirLights; i++)
    {
        if (i >= MAX_DIR_LIGHTS) break;    
        result += CalcDirLight(dirLights[i], norm, viewDir, color);
    }
    result += CalcSpotLight(spotLight, norm, FragPos, viewDir, color);

    float distance = length(viewPos - FragPos);

    //Fog
    float fogStart = 2.5;
    float fogEnd = 30.0;
    vec3 fogColor = vec3(0.1, 0.15, 0.2);
    float fogFactor = (distance - fogStart) / (fogEnd - fogStart);
    fogFactor = clamp(fogFactor, 0.0, 1.0);
    vec3 finalColor = mix(result, fogColor, fogFactor);

    FragColor = vec4(finalColor, 1.0);
}

vec3 CalcDirLight(DirectionalLight light, vec3 normal, vec3 viewDir, vec3 albedo)
{
    vec3 lightDir = normalize(-light.direction);

    float diff = max(dot(normal, lightDir), 0.0);

    vec3 halfVector = normalize(viewDir + lightDir);
    float spec = pow(max(dot(halfVector, normal), 0.0), material.shininess);

    vec3 ambient  = light.color * 0.1 * albedo;
    vec3 diffuse  = light.color * diff * albedo;
    vec3 specular = light.color * spec * specularStrength;

    return (ambient + diffuse + specular);
}

vec3 CalcSpotLight(SpotLight spotLight, vec3 normal, vec3 fragPos, vec3 viewDir, vec3 albedo)
{
    vec3 lightDir = normalize(spotLight.position - fragPos);
    
    float diff = max(dot(normal, lightDir), 0.0);

    vec3 halfVector = normalize(viewDir + lightDir);
    float spec = pow(max(dot(halfVector, normal), 0.0), material.shininess);

    float distance = length(spotLight.position - fragPos);
    float attenuation = 1.0 / (spotLight.constant + spotLight.linear * distance + spotLight.quadratic * (distance * distance));    

    float theta = dot(lightDir, normalize(-spotLight.direction)); 
    float epsilon = spotLight.cutOff - spotLight.outerCutOff;
    float intensity = clamp((theta - spotLight.outerCutOff) / epsilon, 0.0, 1.0);

    vec3 ambient = spotLight.color * 0.05 * albedo;
    vec3 diffuse = spotLight.color * diff * albedo;
    vec3 specular = spotLight.color * spec;

    ambient *= attenuation;
    diffuse *= attenuation * intensity;
    specular *= attenuation * intensity;

    return (ambient + diffuse + specular);
}