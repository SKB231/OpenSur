#version 330 core
out vec4 FragColor;

in vec3 FragPos;
in vec3 Normal;
  
uniform vec3 objectColor;
uniform vec3 lightColor;
uniform vec3 lightPos;
uniform vec3 viewPos;

float ambientStrength = 0.1;
float specularStrength = 0.5;

void main()
{
    vec3 norm = normalize(Normal);
    vec3 lightDir = normalize(lightPos - FragPos);
    float diffusePortion = max(0.0, dot(norm, lightDir));
    vec3 diffuse = diffusePortion * lightColor;

    vec3 noYNormal = vec3(norm.x, 0, norm.z);



    vec3 viewDir = normalize(viewPos - FragPos);
    vec3 reflectDir = reflect(-lightDir, norm);
    
    
    vec3 noYReflect = vec3(reflectDir.x, 0, reflectDir.z);
    vec3 noYView = vec3(viewDir.x, 0, viewDir.z);

    float spec = pow(max(dot(noYView, noYReflect), 0.0), 32);

    vec3 specular = specularStrength * spec * lightColor;

    vec3 ambient = ambientStrength * lightColor;
    vec3 result = (ambient + diffuse + specular) * objectColor;
    FragColor = vec4(result, 1.0);
}