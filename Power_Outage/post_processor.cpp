#include <glad/glad.h> //GLAD must be BEFORE GLFW
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <iostream>
#include <map>
#include <vector>
#include "shape.hpp"
#include "Shader.hpp"
#include "post_processor.hpp"
#include "Font.hpp"

Post_Processor::Post_Processor(int post_process_selection,bool post_process_flag,bool nightvision_on) {
    this->post_process_selection = post_process_selection;
    this->post_process_flag = post_process_flag;
    this->nightvision_on = nightvision_on;
}

void Post_Processor::apply_post_processing(Shader * shader, Shape shape, unsigned int texture) {
    //Unbind framebuffer
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
    glClearColor(1.0f, 1.0f, 1.0f, 1.0f); 
    shader->use();
    glBindTexture(GL_TEXTURE_2D, texture);
    glDisable(GL_DEPTH_TEST);
    shader->setInt("post_process_selection", this->post_process_selection);
    shape.draw(shader->ID);
    glEnable(GL_DEPTH_TEST);
}

void Post_Processor::post_process_input(GLFWwindow* win) {
  //Normal Display
  if (glfwGetKey(win,GLFW_KEY_1) == GLFW_PRESS && this->post_process_flag) {
    this->post_process_selection = 1;
    this->nightvision_on = false;
    this->post_process_flag = false;
  }
  if (glfwGetKey(win,GLFW_KEY_1) == GLFW_RELEASE) {
    this->post_process_flag = true;
  }
  //Night Vision
  if (glfwGetKey(win,GLFW_KEY_2) == GLFW_PRESS && this->post_process_flag) {
    this->post_process_selection = 2;
    this->nightvision_on = true;
    this->post_process_flag = false;
  }
  if (glfwGetKey(win,GLFW_KEY_2) == GLFW_RELEASE) {
    this->post_process_flag = true;
  }
  //Grayscale
  if (glfwGetKey(win,GLFW_KEY_3) == GLFW_PRESS && this->post_process_flag) {
    this->post_process_selection = 3;
    this->nightvision_on = false;
    this->post_process_flag = false;
  }
  if (glfwGetKey(win,GLFW_KEY_3) == GLFW_RELEASE) {
    this->post_process_flag = true;
  }
  //Inverse Color
  if (glfwGetKey(win,GLFW_KEY_4) == GLFW_PRESS && this->post_process_flag) {
    this->post_process_selection = 4;
    this->nightvision_on = false;
    this->post_process_flag = false;
  }
  if (glfwGetKey(win,GLFW_KEY_4) == GLFW_RELEASE) {
    post_process_flag = true;
  }
  //Sharpen
  if (glfwGetKey(win,GLFW_KEY_5) == GLFW_PRESS && this->post_process_flag) {
    this->post_process_selection = 5;
    this->nightvision_on = false;
    this->post_process_flag = false;
  }
  if (glfwGetKey(win,GLFW_KEY_5) == GLFW_RELEASE) {
    this->post_process_flag = true;
  }
  //Blur
  if (glfwGetKey(win,GLFW_KEY_6) == GLFW_PRESS && this->post_process_flag) {
    this->post_process_selection = 6;
    this->nightvision_on = false;
    this->post_process_flag = false;
  }
  if (glfwGetKey(win,GLFW_KEY_6) == GLFW_RELEASE) {
    this->post_process_flag = true;
  }
  //Edge detection
  if (glfwGetKey(win,GLFW_KEY_7) == GLFW_PRESS && this->post_process_flag) {
    this->post_process_selection = 7;
    this->nightvision_on = false;
    this->post_process_flag = false;
  }
  if (glfwGetKey(win,GLFW_KEY_7) == GLFW_RELEASE) {
    this->post_process_flag = true;
  }
}