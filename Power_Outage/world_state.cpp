#include <glad/glad.h> //GLAD must be BEFORE GLFW
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <iostream>
#include <map>
#include <vector>
#include "world_state.hpp"
#include "moving_door.hpp"
#include "moving_plate.hpp"

World::World(int width, int height) {
    this->height = height;
    this->width = width;
}

//Singular key press booleans
bool print_flag = true;
bool spawn_pressed = false;

//for processing all input
void World::process_input (GLFWwindow *win, Camera camera1, Camera camera2, bool door_open) {
  //Press Escape key to exit
  if (glfwGetKey(win,GLFW_KEY_ESCAPE) == GLFW_PRESS) {
    glfwSetWindowShouldClose(win,true);
  }

  //Toggle camera mode with "Tab" key (First Person <-> Bird's eye view)
  if (glfwGetKey(win,GLFW_KEY_TAB)==GLFW_PRESS && !this->cameraView_key_pressed) {
      this->cameraView_key_pressed = true;
      if (!this->camera2_on) {
        this->camera2_on = true;
        this->temp_camera = this->camera;
        this->camera = &camera2;
        this->dir_light_on = true; //turn ON lights when in bird's eye view
      }
      else {
        this->dir_light_on = false; //turn OFF lights when in bird's eye view
        this->camera2_on = false;
        this->camera = temp_camera;
      }
  }
  if (glfwGetKey(win,GLFW_KEY_TAB)==GLFW_RELEASE) {
     this->cameraView_key_pressed = false;
  }

  //Press backspace to teleport back to spawn
  if (glfwGetKey(win,GLFW_KEY_BACKSPACE) == GLFW_PRESS && !spawn_pressed) {
    spawn_pressed = true;
    this->camera->set_position(glm::vec3(10.0f,-3.0f,0.0f));
  }
  if (glfwGetKey(win,GLFW_KEY_BACKSPACE)==GLFW_RELEASE) {
    spawn_pressed = false;
  }

  //First-Person Movement (WASD)
  if (!this->camera2_on) {
    glm::vec3 previous_pos = this->camera->get_position();
    if (glfwGetKey(win,GLFW_KEY_W)==GLFW_PRESS) {
        this->camera->process_keyboard(FORWARD,this->deltaTime); 
    }
    if (glfwGetKey(win,GLFW_KEY_S)==GLFW_PRESS) {
        this->camera->process_keyboard(BACKWARD,this->deltaTime); 
    }
    if (glfwGetKey(win,GLFW_KEY_A)==GLFW_PRESS) {
        this->camera->process_keyboard(LEFT,this->deltaTime); 
    }
    if (glfwGetKey(win,GLFW_KEY_D)==GLFW_PRESS) {
        this->camera->process_keyboard(RIGHT,this->deltaTime); 
    }
    //If player runs into wall, prevent them from going through it
    check_collision(previous_pos,door_open);
  }

  //Toggle flashlight's red lens
  if (glfwGetKey(win,GLFW_KEY_R)==GLFW_PRESS && !this->spot_light_redLens_flag) {
      this->spot_light_redLens_flag = true;
      this->spot_light_redLens = !this->spot_light_redLens;
      if (spot_light_redLens) {
        this->spot_light_ambient = glm::vec3(0.2f,0.0f,0.0f);
        this->spot_light_diffuse = glm::vec3(1.0f,0.0f,0.0f);
        this->spot_light_specular = glm::vec3(1.0f,0.0f,0.0f);
      }
      else {
        this->spot_light_ambient = glm::vec3(0.1f,0.1f,0.1f);
        this->spot_light_diffuse = glm::vec3(0.8f,0.8f,0.8f);
        this->spot_light_specular = glm::vec3(1.0f,1.0f,1.0f);
      }
  }
  if (glfwGetKey(win,GLFW_KEY_R)==GLFW_RELEASE) {
     this->spot_light_redLens_flag = false;
  }

  //Toggle spot light on and off
  if (glfwGetKey(win,GLFW_KEY_F)==GLFW_PRESS && !this->spot_light_on_flag) {
    this->spot_light_on_flag = true;
    this->spot_light_on = !this->spot_light_on;
  }
  if (glfwGetKey(win,GLFW_KEY_F)==GLFW_RELEASE) {
    this->spot_light_on_flag = false;
    this->spot_light_diffuse = glm::vec3(0.8f,0.8f,0.8f);
  }

  //Print player's current position
  if (glfwGetKey(win,GLFW_KEY_P)==GLFW_PRESS && print_flag) {
    //Print current position
    glm::vec3 pos = this->camera->get_position();
    std::cout << "My Position: (" + std::to_string(pos.x);
    std::cout << ", " + std::to_string(pos.y);
    std::cout << ", " + std::to_string(pos.z) + ")" << std::endl;
    print_flag = false;
  }
  if (glfwGetKey(win,GLFW_KEY_P)==GLFW_RELEASE) {
    print_flag = true;
  }

}

bool has_been_seen (std::vector<Shader*>* seen_vec, Shader* shader) {
  bool seen = false;
  for (int i = 0; i < seen_vec->size(); i++) {
    if (seen_vec->at(i) == shader) {
      seen = true;
      break;
    }
  }
  return seen;
}

void World::render_scene (std::map<std::string, Draw_Data> objects,bool plate_pressed,Shader *optional_shader) {

  glm::vec3 cam_pos = this->camera->get_position();
  glm::mat4 wv = this->camera->get_view_matrix();
  std::vector<Shader*> seen_vec;

  //for each structure (including a reference to a shape and its associated shader program)
  //   check to see if the shader has already been set (skip if so)
  //   initialize common shader uniforms (camera and lighting for example)
  for (std::map<std::string,Draw_Data>::iterator it = objects.begin(); it != objects.end(); ++it) {
    if (has_been_seen(&seen_vec,it->second.shader)) {
      continue;
    }
    seen_vec.push_back(it->second.shader);
    Shader* current_shader = it->second.shader;
    
    current_shader->use();
    current_shader->setVec4("view_position", glm::vec4(cam_pos.x,cam_pos.y,cam_pos.z,1.0f));
    current_shader->setMat4("view",wv);
    //Point Light
    current_shader->setVec3("point_light.position",point_light_position);
    current_shader->setVec3("point_light.ambient",0.2f*this->point_light_color);
    current_shader->setVec3("point_light.diffuse",this->point_light_color);
    current_shader->setVec3("point_light.specular",this->point_light_color);
    current_shader->setFloat("point_light.constant",1.0f);
    current_shader->setFloat("point_light.linear",0.14f);
    current_shader->setFloat("point_light.quadratic",0.07f);
    current_shader->setBool("point_light.on",this->point_light_on);
    //Spot Light
    current_shader->setVec3("spot_light.position",cam_pos);
    current_shader->setVec3("spot_light.direction",this->camera->get_front());
    current_shader->setFloat("spot_light.cutOff",glm::cos(glm::radians(12.5f)));
    current_shader->setFloat("spot_light.outerCutOff",glm::cos(glm::radians(17.5f)));
    current_shader->setVec3("spot_light.ambient",this->spot_light_ambient);
    current_shader->setVec3("spot_light.diffuse",this->spot_light_diffuse);
    current_shader->setVec3("spot_light.specular",this->spot_light_specular);
    current_shader->setFloat("spot_light.constant",1.0f);
    current_shader->setFloat("spot_light.linear",0.09f);
    current_shader->setFloat("spot_light.quadratic",0.032f);
    current_shader->setBool("spot_light.on",this->spot_light_on);
    //Directional Light
    current_shader->setVec3("dir_light.direction",this->dir_light_direction);
    current_shader->setVec3("dir_light.ambient",0.2f*this->dir_light_color);
    current_shader->setVec3("dir_light.diffuse",this->dir_light_color);
    current_shader->setVec3("dir_light.specular",this->dir_light_color);
    current_shader->setBool("dir_light.on",(this->dir_light_on || plate_pressed));
    current_shader->setFloat("time",glfwGetTime());
  }

  if (optional_shader != NULL) {
    //Create light's point of view
    glm::mat4 lightProjection = glm::ortho(-10.0f,10.0f,-10.0f,10.0f,1.0f,20.0f);
    glm::vec3 light_pos = glm::vec3(this->point_light_position);
    glm::vec3 front = this->camera->get_front();
    glm::mat4 lightView = glm::lookAt(light_pos,front*10.0f,glm::vec3(0.0f,1.0f,0.0f));
    glm::mat4 lightSpaceMatrix = lightProjection * lightView; 
    //Set depthShader's point of view
    optional_shader->use();
    optional_shader->setMat4("lightSpaceMatrix",lightSpaceMatrix);
  }

  //Draw officeFloor
  Shape* officeFloor = objects["officeFloor"].shape;
  Shader* officeFloor_shader = objects["officeFloor"].shader;
  if (optional_shader != NULL) {
    officeFloor_shader = optional_shader;
  }
  officeFloor_shader->use();
  glm::mat4 officeFloor_transform(1.0f);
  officeFloor_transform = glm::translate(officeFloor_transform,glm::vec3(0.0f,-3.99f,0.0f));
  officeFloor_transform = glm::scale(officeFloor_transform,glm::vec3(0.5f,0.5f, 0.5f));
  glBindTexture(GL_TEXTURE_2D,objects["officeFloor"].texture);
  officeFloor_shader->setMat4("model",officeFloor_transform);
  officeFloor_shader->setBool("use_texture",true);
  officeFloor->draw(officeFloor_shader->ID);
  officeFloor_shader->setBool("use_texture",false);

  //Draw walls
  Shape* walls = objects["walls"].shape;
  Shader* walls_shader = objects["walls"].shader;
  walls_shader->use();
  glm::mat4 walls_transform(1.0f);
  walls_transform = glm::translate(walls_transform,glm::vec3(0.0f,-3.99f,0.0f));
  walls_transform = glm::scale(walls_transform,glm::vec3(1.0f,1.0f, 1.0f));
  glBindTexture(GL_TEXTURE_2D,objects["walls"].texture);
  walls_shader->setMat4("model",walls_transform);
  walls_shader->setBool("use_texture",true);
  walls->draw(walls_shader->ID);
  walls_shader->setBool("use_texture",false);

  //Draw furniture
  Shape* furniture = objects["furniture"].shape;
  Shader* furniture_shader = objects["furniture"].shader;
  furniture_shader->use();
  glm::mat4 furniture_transform(1.0f);
  furniture_transform = glm::translate(furniture_transform,glm::vec3(0.0f,-3.99f,0.0f));
  furniture_transform = glm::scale(furniture_transform,glm::vec3(0.5f,0.5f, 0.5f));
  glBindTexture(GL_TEXTURE_2D,objects["furniture"].texture);
  furniture_shader->setMat4("model",furniture_transform);
  furniture_shader->setBool("use_texture",true);
  furniture->draw(furniture_shader->ID);
  furniture_shader->setBool("use_texture",false);

  //Draw worldFloor
  Shape* worldFloor = objects["worldFloor"].shape;
  Shader* worldFloor_shader = objects["worldFloor"].shader;
  worldFloor_shader->use();
  worldFloor_shader->setMat4("transform",glm::mat4(1.0f));
  glm::mat4 worldFloor_model(1.0f);
  worldFloor_model = glm::translate(worldFloor_model,glm::vec3(0.0,-4.0,0.0));
  worldFloor_model = glm::scale(worldFloor_model,glm::vec3(100.0f,100.0f,100.0f));
  worldFloor_model = glm::rotate(worldFloor_model,glm::radians(-90.0f),glm::vec3(1.0,0.0,0.0));
  worldFloor_shader->setMat4("model",worldFloor_model);
  glBindTexture(GL_TEXTURE_2D,this->floor_texture);
  worldFloor->draw(worldFloor_shader->ID);
  
  //Draw cube1 (silver)
  Shape* cube1 = objects["cube1"].shape;
  Shader* cube1_shader = objects["cube1"].shader;
  glm::mat4 cube1_transform(1.0f);
  cube1_transform = glm::translate(cube1_transform,glm::vec3(-1.0f,-3.35f,1.0f));
  cube1_transform = glm::scale(cube1_transform,glm::vec3(0.25f,0.25f, 0.25f));
  cube1_shader->use();
  cube1_shader->setMat4("transform",glm::mat4(1.0f));
  cube1_shader->setMat4("model",cube1_transform);
  cube1->use_material(cube1_shader);
  cube1->draw(cube1_shader->ID);

  //Draw cube2 (pearl)
  Shape* cube2 = objects["cube2"].shape;
  Shader* cube2_shader = objects["cube2"].shader;
  glm::mat4 cube2_transform(1.0f);
  cube2_transform = glm::translate(cube2_transform,glm::vec3(-1.05f,-3.35f,-1.0f)); 
  cube2_transform = glm::scale(cube2_transform,glm::vec3(0.25f,0.25f, 0.25f));
  cube2_shader->use();
  cube2_shader->setMat4("transform",glm::mat4(1.0f));
  cube2_shader->setMat4("model",cube2_transform);
  cube2->use_material(cube2_shader);
  cube2->draw(cube2_shader->ID);
}

void World::check_collision(glm::vec3 previous_pos,bool door_open) {
  glm::vec3 cur_pos = this->camera->get_position();
  int x = cur_pos.x;
  int z = cur_pos.z;
  float ref1 = 4.9f;
  float ref2 = 5.1f;
  if (x > ref1 && x < ref2 && z > -ref2 && z < ref2) { //front wall
    if (z < 2.5f || (z >= 2.5f && !door_open)) this->camera->set_position(previous_pos);
  }
  if (x > (ref1-5.0f) && x < (ref2-5.0f) && z > -2.5f && z < 2.5f) { //middle-z wall
    this->camera->set_position(previous_pos);
  }
  if (x < -ref1 && x > -ref2 && z > -ref2 && z < ref2) { //back wall
    this->camera->set_position(previous_pos);
  }
  if (x > -ref2 && x < ref2 && z > ref1 && z < ref2) { //left wall
    this->camera->set_position(previous_pos);
  }
  if (x > -2.5f && x < ref2 && z > (ref1-5.0f) && z < (ref2-5.0f)) { //middle-x wall
    this->camera->set_position(previous_pos);
  }
  if (x > -ref2 && x < ref2 && z < -ref1 && z > -ref2) { //right wall
    this->camera->set_position(previous_pos);
  }
  
}