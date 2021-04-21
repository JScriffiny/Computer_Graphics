#include <glad/glad.h> //GLAD must be BEFORE GLFW
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <iostream>
#include <map>
#include <vector>
#include "skybox.hpp"
#include "shape.hpp"
#include "Shader.hpp"

Skybox::Skybox(Shader * shader, Shape shape, unsigned int texture) {
    this->shader = shader;
    this->shape = shape;
    this->texture = texture;
}

void Skybox::render(glm::mat4 camera_view_matrix) {
  shader->use();
  glm::mat4 temp_view = glm::mat4(glm::mat3(camera_view_matrix)); 
  shader->setMat4("view",temp_view);
  glDepthFunc(GL_EQUAL);
  glBindTexture(GL_TEXTURE_CUBE_MAP,texture);
  glDrawArrays(GL_TRIANGLES, 0, 36);
  shape.draw(shader->ID);
  glDepthFunc(GL_LESS);
}
