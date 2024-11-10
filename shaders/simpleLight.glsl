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

// Spot Light
struct Light{
    vec3 position;
    vec3 direction;
    float cutOff;
    float outerConeCutoff;
    
    vec3 ambient;
    vec3 diffuse;
    vec3 specular;
    
    float constant;
    float linear;
    float quadratic;
};
// Point Light
//struct Light{
    //vec3 position;
    ////vec3 direction;
    
    //vec3 ambient;
    //vec3 diffuse;
    //vec3 specular;
    
    //float constant;
    //float linear;
    //float quadratic;
//};

// Directional Light
//struct Light{
    //    //vec3 position;
    //    vec3 direction;
    //    vec3 ambient;
    //    vec3 diffuse;
    //    vec3 specular;
//};

uniform Material material;
uniform Light light;
uniform vec3 viewPos;

void main()
{
    vec3 lightDir=normalize(light.position-FragPos);
    
    float distance=length(light.position-FragPos);
    float attenuation=1./(light.constant+light.linear*distance+light.quadratic*(distance*distance));
    
    float theta=dot(lightDir,normalize(-light.direction));// cosine(theta), theta = angle from lightDirection to vector to fragment
    //FragColor=vec4(1/distance);
    // ambient
    vec3 ambient=light.ambient*vec3(texture(material.diffuseMap,TexCoord));
    //FragColor=vec4(theta>light.cutOff);
    //FragColor=vec4(min(theta,.1));
    if(theta>light.outerConeCutoff){
        //FragColor=vec4(1);
        
        float epsilon=light.cutOff-light.outerConeCutoff;
        float intensity=clamp((theta-light.outerConeCutoff)/epsilon,0.,1.);
        
        // diffuse
        vec3 norm=normalize(Normal);
        //vec3 lightDir=normalize(-light.direction);
        float diffusePortion=max(0.,dot(norm,lightDir));
        vec3 diffuse=light.diffuse*diffusePortion*vec3(texture(material.diffuseMap,TexCoord));
        
        // specular
        vec3 viewDir=normalize(viewPos-FragPos);
        vec3 reflectDir=reflect(-lightDir,norm);
        float spec=pow(max(dot(viewDir,reflectDir),0.),material.shininess);
        vec3 specular=light.specular*spec*vec3(texture(material.specularMap,TexCoord));
        
        vec3 result=(ambient+diffuse+specular)*attenuation*intensity;
        FragColor=vec4(max(result,ambient),1.);
    }else{
        FragColor=vec4(ambient,1.);
    }
    
    //Point Light
    // float distance=length(light.position-FragPos);
    // float attenuation=1./(light.constant+light.linear*distance+light.quadratic*(distance*distance));
    
    // // ambient
    // vec3 ambient=light.ambient*vec3(texture(material.diffuseMap,TexCoord));
    
    // // diffuse
    // vec3 norm=normalize(Normal);
    // vec3 lightDir=normalize(light.position-FragPos);
    // //vec3 lightDir=normalize(-light.direction);
    // float diffusePortion=max(0.,dot(norm,lightDir));
    // vec3 diffuse=light.diffuse*diffusePortion*vec3(texture(material.diffuseMap,TexCoord));
    
    // // specular
    // vec3 viewDir=normalize(viewPos-FragPos);
    // vec3 reflectDir=reflect(-lightDir,norm);
    // float spec=pow(max(dot(viewDir,reflectDir),0.),material.shininess);
    // vec3 specular=light.specular*spec*vec3(texture(material.specularMap,TexCoord));
    
    // vec3 result=(ambient+diffuse+specular)*attenuation;
    // FragColor=vec4(result,1.);
}