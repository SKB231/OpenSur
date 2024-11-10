#version 330 core
out vec4 FragColor;

in vec3 FragPos;
in vec3 Normal;

struct Material{
    vec3 ambient;
    vec3 diffuse;
    vec3 specular;
    float shininess;
};

struct Light{
    vec3 position;
    vec3 ambient;
    vec3 diffuse;
    vec3 specular;
};

uniform Material material;
uniform Light light;
uniform vec3 viewPos;
uniform vec3 lightDir;

void main()
{
    
    // ambient
    vec3 ambient=light.ambient*material.ambient;
    
    // diffuse
    vec3 norm=normalize(Normal);
    vec3 lightDir=normalize(light.position-FragPos);
    float diffusePortion=max(0.,dot(norm,lightDir));
    vec3 diffuse=light.diffuse*diffusePortion*material.diffuse;
    
    // specular
    vec3 viewDir=normalize(viewPos-FragPos);
    vec3 reflectDir=reflect(-lightDir,norm);
    float spec=pow(max(dot(viewDir,reflectDir),0.),material.shininess);
    vec3 specular=light.specular*spec*material.specular;
    
    vec3 result=ambient+diffuse+specular;
    FragColor=vec4(result,1.);
}