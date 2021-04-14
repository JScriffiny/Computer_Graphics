#ifndef ENVIRONMENT_SETUP_HPP
#define ENVIRONMENT_SETUP_HPP

#include <glad/glad.h> //GLAD must be BEFORE GLFW
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>


#include <iostream>

//callback used when the user resizes the window
void framebuffer_size_callback(GLFWwindow* win, int width, int height) {
  glViewport(0,0,width,height);
}


//Initializes glfw and GLAD and returns a created GLFWwindow (width x height with the 
// provided title).
GLFWwindow* initialize_environment(int width, int height, std::string title) {
  glfwInit();  //initialize GLFW
  glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR,3); //Set OpenGL version to 3.3
  glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR,3);
  glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE); //Use Core Profile (vs Immediate)

  //Create a window object (width, height, title, montitor to use for full screen, a window object sharing context if any)
  GLFWwindow* window = glfwCreateWindow(width,height,title.c_str(),NULL,NULL);
  //Standard set up a check to make sure that the window was created...show an error message otherwise.
  if (window == NULL) {
    std::cout << "Failed to create GLFW Window"<<std::endl;
    glfwTerminate(); 
    return NULL; //return this value indicating something went wrong
  }

  glfwMakeContextCurrent(window); //window created so we can make it the current context
  
  //Get the function pointers for OpenGL
  if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) {
    std::cout << "Failed to initialize GLAD" << std::endl;
    return NULL;
  }

  //define window dimensions
  //first two args set location to the lower left corner of the window
  //third is width
  //fourth is height
  glViewport(0,0,width,height);
  //note that if the user resizes the window, the viewport must change too!
  glfwSetFramebufferSizeCallback(window,framebuffer_size_callback);
  return window;
}

#endif //ENVIRONMENT_SETUP_HPP