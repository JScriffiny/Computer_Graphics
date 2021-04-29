#include "moving_key.hpp"
#include <glm/gtc/matrix_transform.hpp>

MovingKey::MovingKey(Shape_Struct s,glm::vec3 scale,glm::vec3 pos,float orient) : Shape(s) {
    this->position = pos;
    this->scale_vec = scale;
    this->orientation = orient;
    this->rotation = 0.0f;
}

void MovingKey::set_texture(unsigned int texture) {
    this->texture = texture;
}

void MovingKey::set_shader(Shader* shader_program) {
    this->shader_program = shader_program;
    original_shader = shader_program;
}

void MovingKey::set_scale(glm::vec3 scale_vec) {
    this->scale_vec = scale_vec;
}

glm::vec3 MovingKey::get_position() {
    return position;
}

void MovingKey::draw(Shader *optional_shader) {
    if (optional_shader != NULL) this->shader_program = optional_shader;
    else this->shader_program = original_shader;
    //Draw key to default position until collected
    if (!collected && !first_collect) {
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
    //Once collected, do not draw key again until inserted
    if (inserted) {
        position = glm::vec3(6.14f,-2.85f,0.0f);
        shader_program->use();
        glm::mat4 shape_trans(1.0f);
        shape_trans = glm::translate(shape_trans, this->position);
        shape_trans = glm::rotate(shape_trans,glm::radians(90.0f),glm::vec3(1.0,0.0,0.0));
        shape_trans = glm::rotate(shape_trans,glm::radians(-90.0f),glm::vec3(0.0,1.0,0.0));
        shape_trans = glm::scale(shape_trans,this->scale_vec);
        shape_trans = glm::rotate(shape_trans,glm::radians(this->orientation),glm::vec3(0.0,1.0,0.0));
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D,texture);
        shader_program->setMat4("model",shape_trans);
        shader_program->setBool("use_texture",true);
        Shape::draw(shader_program->ID);
        shader_program->setBool("use_texture",false);
    }
}

void MovingKey::process_input(GLFWwindow *win, glm::vec3 camera_pos) {
    //Only pick up key if close enough
    if (!collected || inserted) {
        float dist_to_key = glm::length(camera_pos-this->position);
        bool within_range1 = false;
        if (dist_to_key < key_range) within_range1 = true;

        //Press 'K' to collect key
        if (glfwGetKey(win,GLFW_KEY_K)==GLFW_PRESS && within_range1 && collect_flag) {
            first_collect = true;
            collect_flag = false;
            collected = true;
            inserted = false;
        }
        if (glfwGetKey(win,GLFW_KEY_K)==GLFW_RELEASE) {
            collect_flag = true;
        }
    }
    if (collected && !inserted) {
        float dist_to_keyhole = glm::length(camera_pos-glm::vec3(5.7f,-3.7f,2.1f));
        bool within_range2 = false;
        if (dist_to_keyhole < keyhole_range) within_range2 = true;

        //Press 'K' to insert key
        if (glfwGetKey(win,GLFW_KEY_K)==GLFW_PRESS && within_range2 && insert_flag) {
            insert_flag = false;
            inserted = true;
            collected = false;
        }
        if (glfwGetKey(win,GLFW_KEY_K)==GLFW_RELEASE) {
            insert_flag = true;
        }
    }
}