#include <glad/glad.h> //GLAD must be BEFORE GLFW
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <iostream>
#include <map>
#include <vector>
#include "build_shapes.hpp"
#include "shape.hpp"
#include "Shader.hpp"
#include "Font.hpp"
#include "text_display.hpp"

Text_Display::Text_Display(float alpha_value,Display_Data data) {
    this->alpha_value = alpha_value;
    this->data = data;
}

float Text_Display::get_alpha_value() {
    return this->alpha_value;
}

void Text_Display::render_player_coordinates(glm::vec3 camPos) {
  //Heads-Up Display Rectangle
  set_basic_rectangle(&(this->rect_player_coordinates),glm::vec3(0.8,-5.0,0.0),5.0,0.4);

  data.fill_program->use();
  data.fill_program->setMat4("model",glm::mat4(1.0f));
  data.fill_program->setMat4("view",glm::mat4(1.0));
  data.fill_program->setMat4("projection",glm::ortho(-5.0,5.0,-5.0,5.0,-1.0,1.0));
  data.fill_program->setBool("use_set_color",true);
  data.fill_program->setVec4("set_color",glm::vec4(0.0f,0.0f,0.7f,0.3f));
  this->rect_player_coordinates.draw(data.fill_program->ID);
  data.fill_program->setMat4("view",data.view);
  data.fill_program->setMat4("projection",data.projection);
  data.fill_program->setBool("use_set_color",false);

  std::string labels[3] = {"X","Y","Z"};
  std::string disp_string = " Camera: ";
  for (int k = 0; k < 3; k++) {
    std::string num_string = std::to_string(camPos[k]);
    num_string = num_string.substr(0,num_string.find(".")+2);
    if (k == 0) disp_string += "(" + labels[k] + ": " + num_string + ", ";
    else if (k == 1) disp_string += labels[k] + ": " + num_string + ", ";
    else disp_string += labels[k] + ": " + num_string + ")";
  }
  
  data.font->draw_text(disp_string,glm::vec2(0.8,-5.0),*data.font_program);
  data.font_program->use();
  data.font_program->setFloat("alpha",this->alpha_value);
}