#version 330 core
layout(location=0)in vec3 aPos;
layout(location=1)in vec3 aNormal;
layout(location=2)in vec2 aTexCoord;
uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;
uniform float outlineAmt;

out vec3 Normal;
out vec3 FragPos;
out vec2 TexCoord;

void main()
{
    gl_Position=projection*view*model*vec4(aPos+aNormal*outlineAmt,1.);
    FragPos=vec3(model*vec4(aPos,1.));
    
    Normal=mat3(transpose(inverse(model)))*aNormal;
    //Normal=vec3(projection*view*model*vec4(aNormal,1.);
    TexCoord=aTexCoord;
}
