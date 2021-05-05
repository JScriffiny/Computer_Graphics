#include "moving_door.hpp"
#include <glad/glad.h> //GLAD must be BEFORE GLFW
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

//Singular key press boolean
bool door_press_once = true;

MovingDoor::MovingDoor(Shape_Struct s,glm::vec3 scale,glm::vec3 pos,float orient) : Shape(s) {
    this->position = pos;
    this->original_position = pos;
    this->scale_vec = scale;
    this->orientation = orient;
    this->rotation = 0.0f;
    this->is_open = false;
}

void MovingDoor::set_texture(unsigned int texture) {
    this->texture = texture;
}

void MovingDoor::set_shader(Shader* shader_program) {
    this->shader_program = shader_program;
    original_shader = shader_program;
}

void MovingDoor::set_scale(glm::vec3 scale_vec) {
    this->scale_vec = scale_vec;
}

bool MovingDoor::get_door_status() {
    return is_open;
}

glm::vec3 MovingDoor::get_position() {
    return position;
}

void MovingDoor::draw(Shader *optional_shader) {
    if (optional_shader != NULL) this->shader_program = optional_shader;
    else this->shader_program = original_shader;
    shader_program->use();
    glm::mat4 shape_trans(1.0f);
    shape_trans = glm::translate(shape_trans, this->position);
    shape_trans = glm::rotate(shape_trans,glm::radians(this->rotation),glm::vec3(0.0,1.0,0.0));
    shape_trans = glm::scale(shape_trans,this->scale_vec);
    shape_trans = glm::rotate(shape_trans,glm::radians(this->orientation),glm::vec3(0.0,1.0,0.0));
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D,texture);
    shader_program->setMat4("model",shape_trans);
    shader_program->setBool("use_texture",true);
    Shape::draw(shader_program->ID);
    shader_program->setBool("use_texture",false);
}

void MovingDoor::process_input(GLFWwindow *win, glm::vec3 camera_pos,bool key_inserted) {
    //Only open door if close enough
    float dist_to_door = glm::length(camera_pos-this->position);
    bool within_range = false;
    if (dist_to_door < range) within_range = true;

    //Press space bar to open door
    if (key_inserted) {
        if (glfwGetKey(win,GLFW_KEY_SPACE)==GLFW_PRESS && within_range && door_press_once) {
            this->is_open = !this->is_open;
            if (this->is_open) {
                this->rotation = 90.0f;
                this->position = glm::vec3(3.42,-3.99,4.9);
            }
            else {
                this->rotation = 0.0f;
                this->position = this->original_position;
            }
            door_press_once = false;
        }
        if (glfwGetKey(win,GLFW_KEY_SPACE)==GLFW_RELEASE) {
            door_press_once = true;
        }
    }
}