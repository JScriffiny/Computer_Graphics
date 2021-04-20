#ifndef POST_PROCESSOR_HPP
#define POST_PROCESSOR_HPP

#include "shape.hpp"
#include "Shader.hpp"

class Post_Processor {
  private:
    int post_process_selection = 1;
    bool post_process_flag = true;
  public:
    Post_Processor(int post_process_selection,bool post_process_flag,bool nightvision_on);
    void render_post_processing(Shader * shader, unsigned int texture);
    void process_input(GLFWwindow* win);
    bool nightvision_on = false;
    Shape post_rect;
};

#endif //POST_PROCESSOR_HPP
