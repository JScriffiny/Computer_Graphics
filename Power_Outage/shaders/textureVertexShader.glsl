#version 330 core
layout (location = 0) in vec3 aPos;
layout (location = 1) in vec2 texture_coordinates;
layout (location = 2) in vec3 normal;

out vec2 texture_coords;
out vec3 normal_vector;
out vec3 FragPos;
uniform mat4 transform;
uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;

void main()
{
    texture_coords = texture_coordinates;
    normal_vector = mat3(transpose(inverse(model*transform))) * normal;
    FragPos = vec3(model*transform*vec4(aPos.x, aPos.y, aPos.z, 1.0));
    gl_Position = projection*view*model*transform * vec4(aPos.x, aPos.y, aPos.z, 1.0);
}
