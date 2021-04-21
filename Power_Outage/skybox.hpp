#ifndef SKYBOX_HPP
#define SKYBOX_HPP

#include "shape.hpp"
#include "Shader.hpp"

class Skybox {
  private:
    Shader * shader;
    Shape shape;
    unsigned int texture;
  public:
    Skybox(Shader * shader, Shape shape, unsigned int texture);
    void render(glm::mat4 camera_view_matrix);
};

#endif //SKYBOX_HPP
