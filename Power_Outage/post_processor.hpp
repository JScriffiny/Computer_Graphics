#ifndef POST_PROCESSOR_HPP
#define POST_PROCESSOR_HPP

#include "shape.hpp"
#include "Shader.hpp"

class Post_Processor {
  private:
    int post_process_selection = 1;
    bool post_process_flag = true;
    bool nightvision_on = false;
  public:
    Post_Processor(int post_process_selection,bool post_process_flag,bool nightvision_on);
    int get_selection();
    bool get_nightvision_status();
    void render_effect(Shader * shader, unsigned int texture);
    void process_input(GLFWwindow* win);
    Shape post_rect;
};

#endif //POST_PROCESSOR_HPP
