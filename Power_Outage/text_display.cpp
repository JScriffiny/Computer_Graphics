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

#define TOTAL_EFFECTS 7

Text_Display::Text_Display(Display_Data data) {
    this->data = data;
}

void Text_Display::initialize() {
  set_basic_rectangle(&rect_player_coordinates,glm::vec3(0.8,-5.0,0.0),5.0,0.4);
  set_basic_rectangle(&rect_effects_list,glm::vec3(-5.0,2.1,0.0),2.3,3.9);
  set_basic_rectangle(&rect_key_status,glm::vec3(3.2,4.65,0.0),1.8,0.4);

  //Effects Selection Highlight
  effects.push_back("1) Default");
  effects.push_back("2) Night Vision");
  effects.push_back("3) Grayscale");
  effects.push_back("4) Inverse Colors");
  effects.push_back("5) Sharpen");
  effects.push_back("6) Blur");
  effects.push_back("7) Edge Detection");
  rect_selects.push_back(&rect_selected_1);
  rect_selects.push_back(&rect_selected_2);
  rect_selects.push_back(&rect_selected_3);
  rect_selects.push_back(&rect_selected_4);
  rect_selects.push_back(&rect_selected_5);
  rect_selects.push_back(&rect_selected_6);
  rect_selects.push_back(&rect_selected_7);
  for (int i = 0; i < TOTAL_EFFECTS; i++) {
    set_basic_rectangle(rect_selects.at(i),glm::vec3(-5.0,4.55-(i*0.40),0.0),2.3,0.40);
  }
}

float Text_Display::get_alpha_value() {
    return alpha_value;
}

void Text_Display::process_input(GLFWwindow* win) {
  //Effects List
  if (glfwGetKey(win,GLFW_KEY_E) == GLFW_PRESS && effects_list_flag) {
    effects_list_activated = !effects_list_activated;
    effects_list_flag = false;
  }
  if (glfwGetKey(win,GLFW_KEY_E) == GLFW_RELEASE) effects_list_flag = true;
}

void Text_Display::render_player_coordinates(glm::vec3 camPos) {
  data.fill_program->use();
  data.fill_program->setMat4("model",glm::mat4(1.0f));
  data.fill_program->setMat4("view",glm::mat4(1.0));
  data.fill_program->setMat4("projection",glm::ortho(-5.0,5.0,-5.0,5.0,-1.0,1.0));
  data.fill_program->setBool("use_set_color",true);
  data.fill_program->setVec4("set_color",glm::vec4(0.0f,0.0f,0.7f,0.3f));
  rect_player_coordinates.draw(data.fill_program->ID);
  data.fill_program->setMat4("view",data.view);
  data.fill_program->setMat4("projection",data.projection);
  data.fill_program->setBool("use_set_color",false);

  //Display String
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
  data.font_program->setFloat("alpha",alpha_value);
}

void Text_Display::render_effects_list(int effect_id) {
  if (effects_list_activated) {
    data.fill_program->use();
    data.fill_program->setMat4("model",glm::mat4(1.0f));
    data.fill_program->setMat4("view",glm::mat4(1.0));
    data.fill_program->setMat4("projection",glm::ortho(-5.0,5.0,-5.0,5.0,-1.0,1.0));
    data.fill_program->setBool("use_set_color",true);
    data.fill_program->setVec4("set_color",glm::vec4(1.0f,1.0f,0.0f,0.7f));
    rect_selects.at(effect_id-1)->draw(data.fill_program->ID);
    data.fill_program->setMat4("view",data.view);
    data.fill_program->setMat4("projection",data.projection);
    data.fill_program->setBool("use_set_color",false);

    data.fill_program->use();
    data.fill_program->setMat4("model",glm::mat4(1.0f));
    data.fill_program->setMat4("view",glm::mat4(1.0));
    data.fill_program->setMat4("projection",glm::ortho(-5.0,5.0,-5.0,5.0,-1.0,1.0));
    data.fill_program->setBool("use_set_color",true);
    data.fill_program->setVec4("set_color",glm::vec4(0.6f,0.6f,0.6f,0.5f));
    rect_effects_list.draw(data.fill_program->ID);
    data.fill_program->setMat4("view",data.view);
    data.fill_program->setMat4("projection",data.projection);
    data.fill_program->setBool("use_set_color",false);

    double y_pos = 4.55;
    for (int i = 0; i < effects.size(); i++) {
      data.font->draw_text(effects[i],glm::vec2(-4.9,y_pos),*data.font_program);
      y_pos -= 0.40;
    }

    data.font_program->use();
    data.font_program->setFloat("alpha",alpha_value);
  }
}

void Text_Display::render_key_status(bool key_collected) {
  if (key_collected) {
    data.fill_program->use();
    data.fill_program->setMat4("model",glm::mat4(1.0f));
    data.fill_program->setMat4("view",glm::mat4(1.0));
    data.fill_program->setMat4("projection",glm::ortho(-5.0,5.0,-5.0,5.0,-1.0,1.0));
    data.fill_program->setBool("use_set_color",true);
    data.fill_program->setVec4("set_color",glm::vec4(0.0f,1.0f,0.0f,0.5f));
    rect_key_status.draw(data.fill_program->ID);
    data.fill_program->setMat4("view",data.view);
    data.fill_program->setMat4("projection",data.projection);
    data.fill_program->setBool("use_set_color",false);

    //Display String
    std::string disp_string = "Key Collected!";

    data.font->draw_text(disp_string,glm::vec2(3.25,4.65),*data.font_program);
    data.font_program->use();
    data.font_program->setFloat("alpha",alpha_value);
  }
}