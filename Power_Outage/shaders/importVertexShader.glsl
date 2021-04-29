#version 330 core
layout (location = 0) in vec3 aPos;
layout (location = 1) in vec2 aTexCoord;
layout (location = 2) in vec3 aNormal;
layout (location = 3) in vec3 aColor;
layout (location = 4) in vec3 specColor;

out vec3 Normal;
out vec3 FragPos;
out vec3 fColor;
out vec3 sColor;
out vec2 TexCoord;
out vec4 FragPosLightSpace;

uniform mat4 view;
uniform mat4 projection;
uniform mat4 model;
uniform mat4 lightSpaceMatrix;

void main()
{
    gl_Position = projection*view*model * vec4(aPos, 1.0);
    Normal = mat3(transpose(inverse(model)))*aNormal;
    vec4 tempVec = model*vec4(aPos,1.0);
    FragPos = vec3(tempVec.x, tempVec.y,tempVec.z);
    fColor = vec3(aColor.x,aColor.y,aColor.z);
    sColor = vec3(specColor.x,specColor.y,specColor.z);
    TexCoord = aTexCoord;
    FragPosLightSpace = lightSpaceMatrix * vec4(FragPos,1.0);
}
