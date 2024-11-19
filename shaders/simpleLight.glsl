#version 330 core
out vec4 FragColor;

in vec3 FragPos;
in vec3 Normal;
in vec2 TexCoord;

uniform sampler2D uvTexture;

struct DiffuseTexture{
    sampler2D texture;
    float blend;
};

struct SpecularTexture{
    sampler2D texture;
    float blend;
};

struct Material{
    vec3 baseDiffuse;
    vec3 baseSpecular;
    int diffuseCount;
    int specularCount;
    DiffuseTexture diffuseTexture[3];
    SpecularTexture specularTexture[3];
    float shininess;
};

// Spot Light
struct SpotLight{
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
struct PointLight{
    vec3 position;
    //vec3 direction;
    
    vec3 ambient;
    vec3 diffuse;
    vec3 specular;
    
    float constant;
    float linear;
    float quadratic;
};
#define NR_POINT_LIGHTS 4
uniform PointLight pointLights[NR_POINT_LIGHTS];
uniform int usePointLight;

// Directional Light
struct DirLight{
    //vec3 position;"pointLights" + index +
    vec3 direction;
    vec3 ambient;
    vec3 diffuse;
    vec3 specular;
};
uniform DirLight dirLight;

uniform Material material;
uniform vec3 viewPos;

vec3 CalcDirLight(DirLight light,vec3 normal,vec3 viewDir);
vec3 CalcPointLight(PointLight light,vec3 normal,vec3 fragPos,vec3 viewDir);

vec3 evaluateDiffuse(){
    
    vec4 baseColor=vec4(material.baseDiffuse,1);
    //if(texColor.a<.5)
    //discard;
    
    for(int i=0;i<material.diffuseCount;i++){
        vec4 texColor=vec4(texture(material.diffuseTexture[i].texture,TexCoord));
        baseColor*=texColor;
        if(baseColor.a<.5)
        discard;
        //baseColor+=texColor;
    }
    return vec3(baseColor);
}

vec3 evaluateSpecular(){
    vec4 baseColor=vec4(material.baseSpecular,1);
    //if(texColor.a<.5)
    //discard;
    
    for(int i=0;i<material.specularCount;i++){
        vec4 texColor=vec4(texture(material.specularTexture[i].texture,TexCoord));
        baseColor*=texColor;
        if(baseColor.a<.5)
        discard;
        //baseColor+=texColor;
    }
    return vec3(baseColor);
}
/*
struct DiffuseTexture{
    sampler2D texture;
    float blend;
};

struct SpecularTexture{
    sampler2D texture;
    float blend;
};

struct Material{
    vec3 baseDiffuse;
    vec3 baseSpecular;
    DiffuseTexture diffuseTextures[3];
    SpecularTexture SpecularTexture[3];
    float shininess;
};
*/

void main()
{
    vec3 norm=normalize(Normal);
    vec3 viewDir=normalize(viewPos-FragPos);
    
    // phase 1: Directional light
    vec3 result=CalcDirLight(dirLight,norm,viewDir);
    //phase 2: Point Lights
    if(usePointLight==1){
           for(int i=0;i<4;i++)
           result+=CalcPointLight(pointLights[i],norm,FragPos,viewDir);
    }
    
    FragColor=vec4(result,1);
    return;
    // Phase 3: Spot Light
    // Not done
    
    // vec3 lightDir=normalize(light.position-FragPos);
    
    // float distance=length(light.position-FragPos);
    // float attenuation=1./(light.constant+light.linear*distance+light.quadratic*(distance*distance));
    
    // float theta=dot(lightDir,normalize(-light.direction));// cosine(theta), theta = angle from lightDirection to vector to fragment
    // //FragColor=vec4(1/distance);
    // // ambient
    // vec3 ambient=light.ambient*vec3(texture(material.diffuseMap,TexCoord));
    // //FragColor=vec4(theta>light.cutOff);
    // //FragColor=vec4(min(theta,.1));
    // if(theta>light.outerConeCutoff){
        //     //FragColor=vec4(1);
        
        //     float epsilon=light.cutOff-light.outerConeCutoff;
        //     float intensity=clamp((theta-light.outerConeCutoff)/epsilon,0.,1.);
        
        //     // diffuse
        //     vec3 norm=normalize(Normal);
        //     //vec3 lightDir=normalize(-light.direction);
        //     float diffusePortion=max(0.,dot(norm,lightDir));
        //     vec3 diffuse=light.diffuse*diffusePortion*vec3(texture(material.diffuseMap,TexCoord));
        
        //     // specular
        //     vec3 viewDir=normalize(viewPos-FragPos);
        //     vec3 reflectDir=reflect(-lightDir,norm);
        //     float spec=pow(max(dot(viewDir,reflectDir),0.),material.shininess);
        //     vec3 specular=light.specular*spec*vec3(texture(material.specularMap,TexCoord));
        
        //     vec3 result=(ambient+diffuse+specular)*attenuation*intensity;
        //     FragColor=vec4(max(result,ambient),1.);
    // }else{
        //     FragColor=vec4(ambient,1.);
    // }
    
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
vec3 CalcDirLight(DirLight light,vec3 normal,vec3 viewDir){
    
    //vec4 diffuseSample=texture(material.texture_diffuse1,TexCoord);
    //vec4 specularSample=texture(material.texture_specular1,TexCoord);
    vec4 diffuseSample=vec4(evaluateDiffuse(),1);
    vec4 specularSample=vec4(evaluateSpecular(),1);
    //vec4 diffuseSample=vec4(.5);
    //vec4 specularSample=vec4(1);
    
    // ambient
    vec3 ambient=light.ambient*vec3(diffuseSample);
    vec3 lightDirection=normalize(light.direction);
    // diffuse
    vec3 norm=normalize(Normal);
    vec3 lightDir=normalize(-lightDirection);
    float diffusePortion=max(0.,dot(norm,lightDir));
    vec3 diffuse=light.diffuse*diffusePortion*vec3(diffuseSample);
    
    // specular
    //vec3 viewDir=normalize(viewPos-FragPos);// Fragment to Eye
    vec3 reflectDir=reflect(-lightDir,norm);// norm pointing outside,
    float spec=pow(max(dot(viewDir,reflectDir),0.),material.shininess);
    vec3 specular=light.specular*spec*vec3(specularSample);
    
    vec3 result=(ambient+diffuse+specular);
    return result;
}

vec3 CalcPointLight(PointLight light,vec3 normal,vec3 fragPos,vec3 viewDir){
    // vec4 diffuseSample=texture(material.texture_diffuse1,TexCoord);
    // vec4 specularSample=texture(material.texture_specular1,TexCoord);
    vec4 diffuseSample=vec4(evaluateDiffuse(),1);
    vec4 specularSample=vec4(evaluateSpecular(),1);
    //vec4 diffuseSample=vec4(1);
    //vec4 specularSample=vec4(.5);
    
    float distance=length(light.position-fragPos);
    float attenuation=1./(light.constant+light.linear*distance+light.quadratic*(distance*distance));
    
    vec3 norm=normalize(normal);
    vec3 lightDir=normalize(light.position-fragPos);// frag to light
    float diffusePortion=max(0.,dot(norm,lightDir));
    
    // ambient
    vec3 ambient=light.ambient*vec3(diffuseSample);
    
    // diffuse
    vec3 diffuse=light.diffuse*diffusePortion*vec3(diffuseSample);
    
    // specular
    vec3 reflectDir=reflect(-lightDir,norm);
    float spec=pow(max(dot(viewDir,reflectDir),0.),material.shininess);
    vec3 specular=light.specular*spec*vec3(specularSample);
    
    vec3 result=(ambient+diffuse+specular)*attenuation;
    return result;
}