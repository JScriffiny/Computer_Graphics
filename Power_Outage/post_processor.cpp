#include <glad/glad.h> //GLAD must be BEFORE GLFW
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <iostream>
#include <map>
#include <vector>
#include "shape.hpp"
#include "build_shapes.hpp"
#include "Shader.hpp"
#include "post_processor.hpp"
#include "Font.hpp"

Post_Processor::Post_Processor(int post_process_selection,bool post_process_flag,bool nightvision_on) {
    this->post_process_selection = post_process_selection;
    this->post_process_flag = post_process_flag;
    this->nightvision_on = nightvision_on;
}

int Post_Processor::get_selection() {
  return post_process_selection;
}

bool Post_Processor::get_nightvision_status() {
  return nightvision_on;
}

void Post_Processor::render_effect(Shader * shader, unsigned int texture) {
    set_texture_rectangle(&post_rect,glm::vec3(-1.0f,-1.0f,0.0f),2.0f,2.0f,false,false,1.0f);
    //Unbind framebuffer
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
    glClearColor(1.0f, 1.0f, 1.0f, 1.0f); 
    shader->use();
    glBindTexture(GL_TEXTURE_2D, texture);
    glDisable(GL_DEPTH_TEST);
    shader->setInt("post_process_selection", post_process_selection);
    post_rect.draw(shader->ID);
    glEnable(GL_DEPTH_TEST);
}

void Post_Processor::process_input(GLFWwindow* win) {
  //Normal Display
  if (glfwGetKey(win,GLFW_KEY_1) == GLFW_PRESS && post_process_flag) {
    post_process_selection = 1;
    nightvision_on = false;
    post_process_flag = false;
  }
  if (glfwGetKey(win,GLFW_KEY_1) == GLFW_RELEASE) {
    post_process_flag = true;
  }
  //Night Vision
  if (glfwGetKey(win,GLFW_KEY_2) == GLFW_PRESS && post_process_flag) {
    post_process_selection = 2;
    nightvision_on = true;
    post_process_flag = false;
  }
  if (glfwGetKey(win,GLFW_KEY_2) == GLFW_RELEASE) {
    post_process_flag = true;
  }
  //Grayscale
  if (glfwGetKey(win,GLFW_KEY_3) == GLFW_PRESS && post_process_flag) {
    post_process_selection = 3;
    nightvision_on = false;
    post_process_flag = false;
  }
  if (glfwGetKey(win,GLFW_KEY_3) == GLFW_RELEASE) {
    post_process_flag = true;
  }
  //Inverse Color
  if (glfwGetKey(win,GLFW_KEY_4) == GLFW_PRESS && post_process_flag) {
    post_process_selection = 4;
    nightvision_on = false;
    post_process_flag = false;
  }
  if (glfwGetKey(win,GLFW_KEY_4) == GLFW_RELEASE) {
    post_process_flag = true;
  }
  //Sharpen
  if (glfwGetKey(win,GLFW_KEY_5) == GLFW_PRESS && post_process_flag) {
    post_process_selection = 5;
    nightvision_on = false;
    post_process_flag = false;
  }
  if (glfwGetKey(win,GLFW_KEY_5) == GLFW_RELEASE) {
    post_process_flag = true;
  }
  //Blur
  if (glfwGetKey(win,GLFW_KEY_6) == GLFW_PRESS && post_process_flag) {
    post_process_selection = 6;
    nightvision_on = false;
    post_process_flag = false;
  }
  if (glfwGetKey(win,GLFW_KEY_6) == GLFW_RELEASE) {
    post_process_flag = true;
  }
  //Edge detection
  if (glfwGetKey(win,GLFW_KEY_7) == GLFW_PRESS && post_process_flag) {
    post_process_selection = 7;
    nightvision_on = false;
    post_process_flag = false;
  }
  if (glfwGetKey(win,GLFW_KEY_7) == GLFW_RELEASE) {
    post_process_flag = true;
  }
}