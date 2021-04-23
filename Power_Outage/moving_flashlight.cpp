#include "moving_flashlight.hpp"
#include <glm/gtc/matrix_transform.hpp>

MovingFlashlight::MovingFlashlight(Shape_Struct s,glm::vec3 scale,glm::vec3 pos,float orient,float yaw,float pitch) : Shape(s) {
    this->position = pos;
    this->scale_vec = scale;
    this->orientation = orient;
    this->rotation = 0.0f;
    this->yaw = yaw;
    this->pitch = pitch;
}

void MovingFlashlight::set_position(glm::vec3 position) {
    this->position = position;
}

void MovingFlashlight::set_texture(unsigned int texture) {
    this->texture = texture;
}

void MovingFlashlight::set_shader(Shader* shader_program) {
    this->shader_program = shader_program;
}

void MovingFlashlight::set_scale(glm::vec3 scale_vec) {
    this->scale_vec = scale_vec;
}

glm::vec3 MovingFlashlight::get_position() {
    return position;
}

void MovingFlashlight::draw(glm::vec3 camPos, float yaw_change, float pitch_change) {
    //x change is yaw
    //y change is pitch
    yaw_change *= 0.1f;
    pitch_change *= 0.1f;
    yaw += yaw_change;
    pitch += pitch_change;

    float new_y = 0.0f;
    //this->front.y = sin(glm::radians(this->pitch));

    //Radius
    float radius = 0.3f;

    //XZ Plane
    //front.x = cos(glm::radians(yaw)) * cos(glm::radians(pitch));
    //front.z = sin(glm::radians(yaw)) * cos(glm::radians(pitch));

    float x_offset = radius*cos(pitch_change);
    float z_offset = radius*sin(pitch_change);

    position = glm::vec3(camPos.x+x_offset,camPos.y-0.1,camPos.z+z_offset);

    shader_program->use();
    glm::mat4 shape_trans(1.0f);
    shape_trans = glm::translate(shape_trans,position);
    shape_trans = glm::rotate(shape_trans,glm::radians(90.0f),glm::vec3(1.0,0.0,0.0));
    shape_trans = glm::rotate(shape_trans,glm::radians(-30.0f),glm::vec3(0.0,1.0,0.0));
    shape_trans = glm::rotate(shape_trans,glm::radians(90.0f),glm::vec3(0.0,0.0,1.0));
    shape_trans = glm::scale(shape_trans,scale_vec);
    shape_trans = glm::rotate(shape_trans,glm::radians(orientation),glm::vec3(0.0,1.0,0.0));
    glBindTexture(GL_TEXTURE_2D,texture);
    shader_program->setMat4("model",shape_trans);
    shader_program->setBool("use_texture",true);
    Shape::draw(shader_program->ID);
    shader_program->setBool("use_texture",false);
}

void MovingFlashlight::process_input(GLFWwindow *win) {}