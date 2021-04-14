#version 330 core
layout (location = 0) in vec3 aPos;
layout (location = 1) in vec2 TexCoord;
layout (location = 2) in vec3 Normal;


out vec2 TextureCoords;
out vec3 FragPos;

uniform mat4 view;
uniform mat4 projection;
uniform mat4 model;
void main()
{
    gl_Position = projection*view*model* vec4(aPos, 1.0);
    vec4 tempVec = model*vec4(aPos,1.0);
    FragPos = vec3(tempVec.x, tempVec.y,tempVec.z);
    TextureCoords = TexCoord;
}
