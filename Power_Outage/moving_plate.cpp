#include "moving_plate.hpp"
#include <glm/gtc/matrix_transform.hpp>

//Constants
bool plate_press_once = true;

MovingPlate::MovingPlate(Shape_Struct s,glm::vec3 scale,glm::vec3 pos,float orient) : Shape(s) {
    this->position = pos;
    this->original_position = pos;
    this->scale_vec = scale;
    this->orientation = orient;
    this->rotation = 0.0f;
    this->is_pressed = false;
    this->status_flag = false;
}

void MovingPlate::set_texture(unsigned int texture) {
    this->texture = texture;
}

void MovingPlate::set_shader(Shader* shader_program) {
    this->shader_program = shader_program;
}

void MovingPlate::draw() {
    shader_program->use();
    glm::mat4 shape_trans(1.0f);
    shape_trans = glm::translate(shape_trans, this->position);
    shape_trans = glm::rotate(shape_trans,glm::radians(this->rotation),glm::vec3(0.0,1.0,0.0));
    shape_trans = glm::scale(shape_trans,this->scale_vec);
    shape_trans = glm::rotate(shape_trans,glm::radians(this->orientation),glm::vec3(0.0,1.0,0.0));
    glBindTexture(GL_TEXTURE_2D,texture);
    shader_program->setMat4("model",shape_trans);
    shader_program->setBool("use_texture",true);
    Shape::draw(shader_program->ID);
    shader_program->setBool("use_texture",false);
}

void MovingPlate::process_input(GLFWwindow *win, glm::vec3 camera_pos) {
    //Only turn on light if standing on pressure plate
    float dist_to_plate = glm::length(camera_pos-this->position);
    bool within_range = false;
    if (dist_to_plate < 1.2) within_range = true;

    //Step over plate to turn on light; press space to keep it pressed
    if (within_range) {
        this->is_pressed = true;
        if (glfwGetKey(win,GLFW_KEY_SPACE)==GLFW_PRESS && plate_press_once) {
            this->status_flag = !this->status_flag;
            plate_press_once = false;
        }
        if (glfwGetKey(win,GLFW_KEY_SPACE)==GLFW_RELEASE) plate_press_once = true;
    }
    if (!within_range && !status_flag) this->is_pressed = false;    
    if (this->is_pressed) this->position = glm::vec3(this->position.x,-4.03,this->position.z);
    else this->position = this-> original_position;
}

bool MovingPlate::get_plate_status() {
    return this->is_pressed;
}