#include <glad/glad.h> //GLAD must be BEFORE GLFW
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <iostream>
#include <map>
#include <vector>
#include "Font.hpp"
#include "world_state.hpp"
#include "moving_door.hpp"
#include "moving_plate.hpp"
#include "moving_key.hpp"
#include "post_processor.hpp"

World::World(int width, int height) {
    this->height = height;
    this->width = width;
}

//for processing all input
void World::process_input (GLFWwindow *win) {
  //Press Escape key to exit
  if (glfwGetKey(win,GLFW_KEY_ESCAPE) == GLFW_PRESS) {
    glfwSetWindowShouldClose(win,true);
  }

  //First-Person Movement (WASD)
  glm::vec3 previous_pos = camera->get_position();
  if (glfwGetKey(win,GLFW_KEY_W)==GLFW_PRESS) {
      camera->process_keyboard(FORWARD,deltaTime); 
  }
  if (glfwGetKey(win,GLFW_KEY_S)==GLFW_PRESS) {
      camera->process_keyboard(BACKWARD,deltaTime); 
  }
  if (glfwGetKey(win,GLFW_KEY_A)==GLFW_PRESS) {
      camera->process_keyboard(LEFT,deltaTime); 
  }
  if (glfwGetKey(win,GLFW_KEY_D)==GLFW_PRESS) {
      camera->process_keyboard(RIGHT,deltaTime); 
  }
  //If player runs into wall, prevent them from going through it
  if (!bird_cam_on) {
    check_collision(previous_pos);
  }
  //Update player position if player walks through portal
  check_portal_teleport();

  //Toggle camera mode with "Tab" key (First Person <-> Bird's eye view)
  if (glfwGetKey(win,GLFW_KEY_TAB)==GLFW_PRESS && !cameraView_key_pressed) {
      cameraView_key_pressed = true;
      if (!bird_cam_on) {
        bird_cam_on = true;
        saved_player_pos = camera->get_position();
        camera->set_position(bird_cam_pos);
      }
      else {
        bird_cam_on = false;
        bird_cam_pos = camera->get_position();
        camera->set_position(saved_player_pos);
      }
  }
  if (glfwGetKey(win,GLFW_KEY_TAB)==GLFW_RELEASE) {
     cameraView_key_pressed = false;
  }

  //Press backspace to teleport back to spawn
  if (glfwGetKey(win,GLFW_KEY_LEFT_SHIFT) == GLFW_PRESS && !bird_cam_on && !spawn_pressed) {
    spawn_pressed = true;
    camera->set_position(glm::vec3(20.0f,-3.0f,0.0f)); //spawn point
  }
  if (glfwGetKey(win,GLFW_KEY_LEFT_SHIFT)==GLFW_RELEASE) {
    spawn_pressed = false;
  }

  //Toggle flashlight's red lens
  if (glfwGetKey(win,GLFW_KEY_R)==GLFW_PRESS && !spot_light_redLens_flag) {
      spot_light_redLens_flag = true;
      spot_light_redLens = !spot_light_redLens;
      if (spot_light_redLens) {
        spot_light_ambient = glm::vec3(0.2f,0.0f,0.0f);
        spot_light_diffuse = glm::vec3(1.0f,0.0f,0.0f);
        spot_light_specular = glm::vec3(1.0f,0.0f,0.0f);
      }
      else {
        spot_light_ambient = glm::vec3(0.1f,0.1f,0.1f);
        spot_light_diffuse = glm::vec3(0.8f,0.8f,0.8f);
        spot_light_specular = glm::vec3(1.0f,1.0f,1.0f);
      }
  }
  if (glfwGetKey(win,GLFW_KEY_R)==GLFW_RELEASE) {
     spot_light_redLens_flag = false;
  }

  //Toggle spot light on and off
  if (glfwGetKey(win,GLFW_KEY_F)==GLFW_PRESS && !spot_light_on_flag) {
    spot_light_on_flag = true;
    spot_light_on = !spot_light_on;
  }
  if (glfwGetKey(win,GLFW_KEY_F)==GLFW_RELEASE) {
    spot_light_on_flag = false;
    spot_light_diffuse = glm::vec3(0.8f,0.8f,0.8f);
  }

  //Toggle Anything (Development Purposes)
  if (glfwGetKey(win,GLFW_KEY_P)==GLFW_PRESS && my_toggle) {
    //std::cout << "Print something" << std::endl;
    my_toggle = false;
  }
  if (glfwGetKey(win,GLFW_KEY_P)==GLFW_RELEASE) {
    my_toggle = true;
  }

  //Separate input processing
  door->process_input(win,camera->get_position(),office_key->inserted);
  pressure_plate->process_input(win,camera->get_position());
  office_key->process_input(win,camera->get_position());
  text_display->process_input(win);
  post_processor->process_input(win);
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

void World::render_scene (std::map<std::string, Draw_Data> objects,Shader *optional_shader) {
  if (optional_shader != NULL) {
    glViewport(0,0,2048,2048);
    glBindFramebuffer(GL_FRAMEBUFFER, shadow_buffer);
    glClear(GL_COLOR_BUFFER_BIT|GL_DEPTH_BUFFER_BIT|GL_STENCIL_BUFFER_BIT);
  }
  else {
    glViewport(0,0,width,height);
    glBindFramebuffer(GL_FRAMEBUFFER,post_buffer);
    glClear(GL_COLOR_BUFFER_BIT|GL_DEPTH_BUFFER_BIT|GL_STENCIL_BUFFER_BIT);
    glActiveTexture(GL_TEXTURE1);
    glBindTexture(GL_TEXTURE_2D,shadow_depthMap);
  }

  //Clear the stencil mask before rendering scene
  glStencilMask(0x00);

  glm::vec3 cam_pos = camera->get_position();
  spot_light_position = glm::vec4(cam_pos,1.0f);
  glm::mat4 wv = camera->get_view_matrix();
  std::vector<Shader*> seen_vec;

  bool special_conditions = false;
  if (bird_cam_on || pressure_plate->get_plate_status() || post_processor->get_nightvision_status()) {
    special_conditions = true;
  }

  //Initialize common shader uniforms
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
    current_shader->setVec3("point_light.ambient",0.2f*point_light_color);
    current_shader->setVec3("point_light.diffuse",point_light_color);
    current_shader->setVec3("point_light.specular",point_light_color);
    current_shader->setFloat("point_light.constant",1.0f);
    current_shader->setFloat("point_light.linear",0.14f);
    current_shader->setFloat("point_light.quadratic",0.07f);
    current_shader->setBool("point_light.on",point_light_on);
    //Spot Light
    current_shader->setVec3("spot_light.position",cam_pos);
    current_shader->setVec3("spot_light.direction",camera->get_front());
    current_shader->setFloat("spot_light.cutOff",glm::cos(glm::radians(12.5f)));
    current_shader->setFloat("spot_light.outerCutOff",glm::cos(glm::radians(17.5f)));
    current_shader->setVec3("spot_light.ambient",spot_light_ambient);
    current_shader->setVec3("spot_light.diffuse",spot_light_diffuse);
    current_shader->setVec3("spot_light.specular",spot_light_specular);
    current_shader->setFloat("spot_light.constant",1.0f);
    current_shader->setFloat("spot_light.linear",0.09f);
    current_shader->setFloat("spot_light.quadratic",0.032f);
    current_shader->setBool("spot_light.on",spot_light_on);
    //Directional Light
    current_shader->setVec3("dir_light.direction",dir_light_direction);
    current_shader->setVec3("dir_light.ambient",0.2f*dir_light_color);
    current_shader->setVec3("dir_light.diffuse",dir_light_color);
    current_shader->setVec3("dir_light.specular",dir_light_color);
    current_shader->setBool("dir_light.on",(dir_light_on || special_conditions));
    current_shader->setFloat("time",glfwGetTime());

    //Shadow Setup
    current_shader->setMat4("lightSpaceMatrix",getLightPOV());
    current_shader->setInt("texture_image",0);
    current_shader->setInt("depth_image",1);
  }

  //Set depthShader's point of view
  if (optional_shader != NULL) {
    optional_shader->use();
    optional_shader->setMat4("lightSpaceMatrix",getLightPOV());
  }

  //Draw worldFloor
  Shape* worldFloor = objects["worldFloor"].shape;
  Shader* worldFloor_shader = objects["worldFloor"].shader;
  worldFloor_shader->use();
  worldFloor_shader->setMat4("transform",glm::mat4(1.0f));
  worldFloor_shader->setMat4("lightSpaceMatrix",getLightPOV());
  glm::mat4 worldFloor_model(1.0f);
  worldFloor_model = glm::translate(worldFloor_model,glm::vec3(0.0,-4.0,0.0));
  worldFloor_model = glm::scale(worldFloor_model,glm::vec3(150.0f,150.0f,150.0f));
  worldFloor_model = glm::rotate(worldFloor_model,glm::radians(-90.0f),glm::vec3(1.0,0.0,0.0));
  worldFloor_shader->setMat4("model",worldFloor_model);
  glActiveTexture(GL_TEXTURE0);
  glBindTexture(GL_TEXTURE_2D,floor_texture);
  worldFloor->draw(worldFloor_shader->ID);

  //Draw officeFloor
  Shape* officeFloor = objects["officeFloor"].shape;
  Shader* officeFloor_shader = objects["officeFloor"].shader;
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
  if (optional_shader != NULL) furniture_shader = optional_shader;
  furniture_shader->use();
  glm::mat4 furniture_transform(1.0f);
  furniture_transform = glm::translate(furniture_transform,glm::vec3(0.0f,-3.99f,0.0f));
  furniture_transform = glm::scale(furniture_transform,glm::vec3(0.5f,0.5f, 0.5f));
  glActiveTexture(GL_TEXTURE0);
  glBindTexture(GL_TEXTURE_2D,objects["furniture"].texture);
  furniture_shader->setMat4("model",furniture_transform);
  furniture_shader->setBool("use_texture",true);
  furniture->draw(furniture_shader->ID);
  furniture_shader->setBool("use_texture",false);

  //Draw keyhole
  Shape* keyhole = objects["keyhole"].shape;
  Shader* keyhole_shader = objects["keyhole"].shader;
  if (optional_shader != NULL) keyhole_shader = optional_shader;
  keyhole_shader->use();
  glm::mat4 keyhole_transform(1.0f);
  keyhole_transform = glm::translate(keyhole_transform,glm::vec3(5.159f,-3.7f,0.0f));
  keyhole_transform = glm::rotate(keyhole_transform,glm::radians(-90.0f),glm::vec3(0.0,1.0,0.0));
  keyhole_transform = glm::scale(keyhole_transform,glm::vec3(0.25f,0.25f,0.25f));
  glActiveTexture(GL_TEXTURE0);
  glBindTexture(GL_TEXTURE_2D,objects["keyhole"].texture);
  keyhole_shader->setMat4("model",keyhole_transform);
  keyhole_shader->setBool("use_texture",true);
  keyhole->draw(keyhole_shader->ID);
  keyhole_shader->setBool("use_texture",false);

  //Draw lamppost
  Shape* lamppost = objects["lamppost"].shape;
  Shader* lamppost_shader = objects["lamppost"].shader;
  if (optional_shader != NULL) lamppost_shader = optional_shader;
  lamppost_shader->use();
  glm::mat4 lamppost_transform(1.0f);
  lamppost_transform = glm::translate(lamppost_transform,glm::vec3(15.0f,-3.99f,0.0f));
  lamppost_transform = glm::rotate(lamppost_transform,glm::radians(-90.0f),glm::vec3(0.0,1.0,0.0));
  lamppost_transform = glm::scale(lamppost_transform,glm::vec3(0.2f,0.2f,0.2f));
  glActiveTexture(GL_TEXTURE0);
  lamppost_shader->setMat4("model",lamppost_transform);
  lamppost_shader->setBool("use_texture",false);
  lamppost->draw(lamppost_shader->ID);
  lamppost_shader->setBool("use_texture",false);

  //Draw Portals
  Shader* portal_shader = objects["portal1"].shader; //same shader for each portal
  portal_shader->use();
  //Portal 1
  Shape* portal1 = objects["portal1"].shape;
  glm::mat4 portal1_transform(1.0f);
  portal1_transform = glm::translate(portal1_transform,glm::vec3(10.0f,-3.99f,7.5f));
  portal1_transform = glm::rotate(portal1_transform,glm::radians(0.0f),glm::vec3(0.0,1.0,0.0));
  portal1_transform = glm::scale(portal1_transform,glm::vec3(0.6f,0.6f,0.6f));
  glActiveTexture(GL_TEXTURE0);
  portal_shader->setMat4("model",portal1_transform);
  portal_shader->setBool("use_texture",false);
  portal1->draw(portal_shader->ID);
  portal_shader->setBool("use_texture",false);
  //Portal 2
  Shape* portal2 = objects["portal2"].shape;
  glm::mat4 portal2_transform(1.0f);
  portal2_transform = glm::translate(portal2_transform,glm::vec3(20.0f,-3.99f,7.5f));
  portal2_transform = glm::rotate(portal2_transform,glm::radians(0.0f),glm::vec3(0.0,1.0,0.0));
  portal2_transform = glm::scale(portal2_transform,glm::vec3(0.6f,0.6f,0.6f));
  glActiveTexture(GL_TEXTURE0);
  portal_shader->setMat4("model",portal2_transform);
  portal_shader->setBool("use_texture",false);
  portal2->draw(portal_shader->ID);
  portal_shader->setBool("use_texture",false);
  //Portal 3
  Shape* portal3 = objects["portal3"].shape;
  glm::mat4 portal3_transform(1.0f);
  portal3_transform = glm::translate(portal3_transform,glm::vec3(10.0f,-3.99f,-7.5f));
  portal3_transform = glm::rotate(portal3_transform,glm::radians(0.0f),glm::vec3(0.0,1.0,0.0));
  portal3_transform = glm::scale(portal3_transform,glm::vec3(0.6f,0.6f,0.6f));
  glActiveTexture(GL_TEXTURE0);
  portal_shader->setMat4("model",portal3_transform);
  portal_shader->setBool("use_texture",false);
  portal3->draw(portal_shader->ID);
  portal_shader->setBool("use_texture",false);
  //Portal 4
  Shape* portal4 = objects["portal4"].shape;
  glm::mat4 portal4_transform(1.0f);
  portal4_transform = glm::translate(portal4_transform,glm::vec3(20.0f,-3.99f,-7.5f));
  portal4_transform = glm::rotate(portal4_transform,glm::radians(0.0f),glm::vec3(0.0,1.0,0.0));
  portal4_transform = glm::scale(portal4_transform,glm::vec3(0.6f,0.6f,0.6f));
  glActiveTexture(GL_TEXTURE0);
  portal_shader->setMat4("model",portal4_transform);
  portal_shader->setBool("use_texture",false);
  portal4->draw(portal_shader->ID);
  portal_shader->setBool("use_texture",false);

  //Draw Buildings
  Shader* building_shader = objects["building1"].shader; //same shader for each building
  building_shader->use();
  //Building 1
  Shape* building1 = objects["building1"].shape;
  glm::mat4 building1_transform(1.0f);
  building1_transform = glm::translate(building1_transform,glm::vec3(-80.0f,-3.99f,20.0f));
  building1_transform = glm::rotate(building1_transform,glm::radians(-90.0f),glm::vec3(0.0,1.0,0.0));
  building1_transform = glm::scale(building1_transform,glm::vec3(0.6f,0.6f,0.6f));
  glActiveTexture(GL_TEXTURE0);
  building_shader->setMat4("model",building1_transform);
  building_shader->setBool("use_texture",false);
  building1->draw(building_shader->ID);
  building_shader->setBool("use_texture",false);
  //Building 2
  Shape* building2 = objects["building2"].shape;
  glm::mat4 building2_transform(1.0f);
  building2_transform = glm::translate(building2_transform,glm::vec3(38.0f,-3.99f,20.0f));
  building2_transform = glm::rotate(building2_transform,glm::radians(-90.0f),glm::vec3(0.0,1.0,0.0));
  building2_transform = glm::scale(building2_transform,glm::vec3(0.6f,0.6f,0.6f));
  glActiveTexture(GL_TEXTURE0);
  building_shader->setMat4("model",building2_transform);
  building_shader->setBool("use_texture",false);
  building2->draw(building_shader->ID);
  building_shader->setBool("use_texture",false);
  //Building 3
  Shape* building3 = objects["building3"].shape;
  glm::mat4 building3_transform(1.0f);
  building3_transform = glm::translate(building3_transform,glm::vec3(-7.5f,-3.99f,-20.0f));
  building3_transform = glm::rotate(building3_transform,glm::radians(90.0f),glm::vec3(0.0,1.0,0.0));
  building3_transform = glm::scale(building3_transform,glm::vec3(0.6f,0.6f,0.6f));
  glActiveTexture(GL_TEXTURE0);
  building_shader->setMat4("model",building3_transform);
  building_shader->setBool("use_texture",false);
  building3->draw(building_shader->ID);
  building_shader->setBool("use_texture",false);
  //Building 4
  Shape* building4 = objects["building4"].shape;
  glm::mat4 building4_transform(1.0f);
  building4_transform = glm::translate(building4_transform,glm::vec3(110.0f,-3.99f,-15.0f));
  building4_transform = glm::rotate(building4_transform,glm::radians(90.0f),glm::vec3(0.0,1.0,0.0));
  building4_transform = glm::scale(building4_transform,glm::vec3(0.6f,0.6f,0.6f));
  glActiveTexture(GL_TEXTURE0);
  building_shader->setMat4("model",building4_transform);
  building_shader->setBool("use_texture",false);
  building4->draw(building_shader->ID);
  building_shader->setBool("use_texture",false);
  
  //Draw cube1 (silver)
  Shape* cube1 = objects["cube1"].shape;
  Shader* cube1_shader = objects["cube1"].shader;
  if (optional_shader != NULL) cube1_shader = optional_shader;
  cube1_shader->use();
  glm::mat4 cube1_transform(1.0f);
  cube1_transform = glm::translate(cube1_transform,glm::vec3(-1.0f,-3.35f,1.0f));
  cube1_transform = glm::scale(cube1_transform,glm::vec3(0.25f,0.25f, 0.25f));
  cube1_shader->setMat4("transform",glm::mat4(1.0f));
  cube1_shader->setMat4("model",cube1_transform);
  cube1->use_material(cube1_shader);
  cube1->draw(cube1_shader->ID);

  //Draw cube2 (pearl)
  Shape* cube2 = objects["cube2"].shape;
  Shader* cube2_shader = objects["cube2"].shader;
  if (optional_shader != NULL) cube2_shader = optional_shader;
  cube2_shader->use();
  glm::mat4 cube2_transform(1.0f);
  cube2_transform = glm::translate(cube2_transform,glm::vec3(-1.05f,-3.35f,-1.0f)); 
  cube2_transform = glm::scale(cube2_transform,glm::vec3(0.25f,0.25f, 0.25f));
  cube2_shader->setMat4("transform",glm::mat4(1.0f));
  cube2_shader->setMat4("model",cube2_transform);
  cube2->use_material(cube2_shader);
  cube2->draw(cube2_shader->ID);

  //Stenciled Objects Section
  glStencilFunc(GL_ALWAYS,1,0xFF);
  glStencilMask(0xFF);

  office_key->draw(optional_shader);
  door->draw(NULL);
  pressure_plate->draw(NULL);

  render_stencils(objects["stencil_fill"].shader,objects["stencil_import"].shader);

  //Render skybox
  skybox->render(camera->get_view_matrix());

  //Render text displays
  text_display->render_player_coordinates(camera->get_position());
  text_display->render_effects_list(post_processor->get_selection());
  text_display->render_key_status(office_key->collected);
}

glm::mat4 World::getLightPOV() {
  glm::mat4 lightProjection = glm::ortho(-10.0f,10.0f,-10.0f,10.0f,1.0f,20.0f);
  glm::vec3 light_pos = camera->get_position();
  glm::vec3 pos = glm::vec3(light_pos.x,light_pos.y+3.2f,light_pos.z);
  glm::vec3 front = camera->get_front();
  glm::mat4 lightView = glm::lookAt(pos,front*40.0f,glm::vec3(0.0f,1.0f,0.0f));
  glm::mat4 lightSpaceMatrix = lightProjection * lightView; 
  return lightSpaceMatrix;
}

void World::render_stencils(Shader* fill_program, Shader* import_program) {
  float dist_to_door = glm::length(camera->get_position()-door->get_position());
  if (dist_to_door <= door->range) {
    glStencilFunc(GL_NOTEQUAL,1,0xFF);
    glStencilMask(0x00);
    glDisable(GL_DEPTH_TEST);
    door->set_scale(glm::vec3(0.655,0.655,0.655));
    fill_program->use();
    fill_program->setBool("use_set_color",true);
    //Door outline is red until key is inserted
    if (!office_key->inserted) fill_program->setVec4("set_color",glm::vec4(1.0,0.0,0.0,0.5));
    else fill_program->setVec4("set_color",glm::vec4(0.3,0.7,1.0,0.5));
    door->set_shader(fill_program);
    door->draw(NULL);
    door->set_shader(import_program);
    door->set_scale(glm::vec3(0.638,0.638,0.638));
    fill_program->setBool("use_set_color",false);
    glStencilMask(0xFF);
    glStencilFunc(GL_ALWAYS,1,0xFF);
    glEnable(GL_DEPTH_TEST);
  }

  float dist_to_plate = glm::length(camera->get_position()-pressure_plate->get_position());
  if (dist_to_plate <= (pressure_plate->range)+0.8) {
    glStencilFunc(GL_NOTEQUAL,1,0xFF);
    glStencilMask(0x00);
    glDisable(GL_DEPTH_TEST);
    pressure_plate->set_scale(glm::vec3(0.52,0.52,0.52));
    fill_program->use();
    fill_program->setBool("use_set_color",true);
    fill_program->setVec4("set_color",glm::vec4(0.3,0.7,1.0,0.5));
    pressure_plate->set_shader(fill_program);
    pressure_plate->draw(NULL);
    pressure_plate->set_shader(import_program);
    pressure_plate->set_scale(glm::vec3(0.5,0.5,0.5));
    fill_program->setBool("use_set_color",false);
    glStencilMask(0xFF);
    glStencilFunc(GL_ALWAYS,1,0xFF);
    glEnable(GL_DEPTH_TEST);
  }

  float dist_to_key = glm::length(camera->get_position()-office_key->get_position());
  if (dist_to_key <= (office_key->key_range) && !office_key->inserted) {
    glStencilFunc(GL_NOTEQUAL,1,0xFF);
    glStencilMask(0x00);
    glDisable(GL_DEPTH_TEST);
    office_key->set_scale(glm::vec3(0.27,0.27,0.27));
    fill_program->use();
    fill_program->setBool("use_set_color",true);
    fill_program->setVec4("set_color",glm::vec4(0.3,0.7,1.0,0.5));
    office_key->set_shader(fill_program);
    office_key->draw(NULL);
    office_key->set_shader(import_program);
    office_key->set_scale(glm::vec3(0.25,0.25,0.25));
    fill_program->setBool("use_set_color",false);
    glStencilMask(0xFF);
    glStencilFunc(GL_ALWAYS,1,0xFF);
    glEnable(GL_DEPTH_TEST);
  }
}

void World::check_portal_teleport() {
  glm::vec3 cur_pos = camera->get_position();
  double x = cur_pos.x*1.0;
  double z = cur_pos.z*1.0;
  double ref_x1 = 9.5;
  double ref_x2 = 10.5;
  double ref_z1 = 7.2;
  double ref_z2 = 7.5;

  //Portal 1 (Red)
  if (x > ref_x1 && x < ref_x2 && z > ref_z1 && z < ref_z2) {
    camera->set_position(teleport1_pos);
  }
  //Portal 2 (Blue)
  if (x > (ref_x1+10.0) && x < (ref_x2+10.0) && z > ref_z1 && z < ref_z2) {
    camera->set_position(teleport2_pos);
  }
  //Portal 3 (Green)
  if (x > ref_x1 && x < ref_x2 && (z*-1.0) > ref_z1 && (z*-1.0) < ref_z2) {
    camera->set_position(teleport3_pos);
  }
  //Portal 4 (Pink)
  if (x > (ref_x1+10.0) && x < (ref_x2+10.0) && (z*-1.0) > ref_z1 && (z*-1.0) < ref_z2) {
    camera->set_position(teleport4_pos);
  }
}

void World::check_collision(glm::vec3 previous_pos) {
  glm::vec3 cur_pos = camera->get_position();
  double x = cur_pos.x*1.0;
  double z = cur_pos.z*1.0;
  double ref1 = 4.86;
  double ref2 = 5.14;
  if (x > ref1 && x < ref2 && z > -ref2 && z < ref2) { //front wall
    if (z < 2.5 || (z >= 2.5 && !door->get_door_status())) camera->set_position(previous_pos);
  }
  if (x > (ref1-5.0) && x < (ref2-5.0) && z > -2.5 && z < 2.5) { //middle-z wall
    camera->set_position(previous_pos);
  }
  if (x < -ref1 && x > -ref2 && z > -ref2 && z < ref2) { //back wall
    camera->set_position(previous_pos);
  }
  if (x > -ref2 && x < ref2 && z > ref1 && z < ref2) { //left wall
    camera->set_position(previous_pos);
  }
  if (x > -2.5 && x < ref2 && z > (ref1-5.0) && z < (ref2-5.0)) { //middle-x wall
    camera->set_position(previous_pos);
  }
  if (x > -ref2 && x < ref2 && z < -ref1 && z > -ref2) { //right wall
    camera->set_position(previous_pos);
  }
}