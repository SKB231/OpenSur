#version 330 core
out vec4 FragColor;

in vec3 FragPos;
in vec3 Normal;
in vec2 TexCoord;

struct Material{
    sampler2D diffuseMap;
    sampler2D specularMap;
    float shininess;
};

struct Light{
    //vec3 position;
    vec3 direction;
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
    vec3 ambient=light.ambient*vec3(texture(material.diffuseMap,TexCoord));
    
    // diffuse
    vec3 norm=normalize(Normal);
    //vec3 lightDir=normalize(light.position-FragPos);
    vec3 lightDir=normalize(-light.direction);
    float diffusePortion=max(0.,dot(norm,lightDir));
    vec3 diffuse=light.diffuse*diffusePortion*vec3(texture(material.diffuseMap,TexCoord));
    
    // specular
    vec3 viewDir=normalize(viewPos-FragPos);
    vec3 reflectDir=reflect(-lightDir,norm);
    float spec=pow(max(dot(viewDir,reflectDir),0.),material.shininess);
    vec3 specular=light.specular*spec*vec3(texture(material.specularMap,TexCoord));
    
    vec3 result=ambient+diffuse+specular;
    FragColor=vec4(result,1.);
}