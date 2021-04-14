#include "environment_setup.hpp"
#include "shape.hpp"
#include "Shader.hpp"
#include "build_shapes.hpp"
#include <glm/gtc/matrix_transform.hpp>
#include "camera.hpp"
#include "Font.hpp"
#include "import_object.hpp"
#include <map>
#include "world_state.hpp"
#include "moving_door.hpp"
#include "moving_plate.hpp"

//Constants
#define WIN_WIDTH 960
#define WIN_HEIGHT 720

//Create the world state object
World world(WIN_WIDTH,WIN_HEIGHT);

//Materials
Material pearl{glm::vec3(0.25,0.20725,0.20725),
                 glm::vec3(1, 0.829, 0.829),
                 glm::vec3(0.296648,0.296648,0.296648),
                 0.088*128};

Material silver{glm::vec3(0.19225,0.19225,0.19225),
                 glm::vec3(0.50754, 0.50754, 0.50754),
                 glm::vec3(0.508273,0.508273,0.508273),
                 0.4*128};

//First-Person Camera
Camera camera(glm::vec3(10.0f,-3.0f,0.0f),glm::vec3(0.0f,1.0f,0.0f),180.0f, 0.0f);

//Bird's Eye Camera
Camera camera_bird(glm::vec3(-0.5f,18.0f,1.0f),glm::vec3(0.0f,-1.0f,0.0f),-90.0f, -85.0f);

//Capture the mouse position data on mouse movement
void mouse_callback (GLFWwindow* win, double xpos, double ypos);

//Create fonts
Font arialFont("fonts/ArialBlackLarge.bmp","fonts/ArialBlack.csv",0.3,0.4);

int main() {
  //Initialize the environment
  GLFWwindow* window = initialize_environment(WIN_WIDTH,WIN_HEIGHT,"Power Outage");
  if (window == NULL) {
    return -1; //error msg already printed to screen.
  }

  //Initialize world camera
  world.camera = &camera;

  //The font must be initialized -after- the environment.
  arialFont.initialize();

  //Import an objects
  ImportOBJ new_importer;

  /** Office Scene setup **/
  //Office Floor
  Shape_Struct new_officeFloor = new_importer.loadFiles("models/floor");
  Shape officeFloor(new_officeFloor);
  unsigned int officeFloor_texture = new_importer.getTexture();
  //Office Walls
  Shape_Struct new_walls = new_importer.loadFiles("models/walls");
  Shape walls(new_walls);
  unsigned int walls_texture = new_importer.getTexture();
  //Office Furniture
  Shape_Struct new_furniture = new_importer.loadFiles("models/furniture");
  Shape furniture(new_furniture);
  unsigned int furniture_texture = new_importer.getTexture();

  //Material Cubes
  Shape cube1,cube2;
  set_basic_cube(&cube1);
  cube1.set_material(silver);
  set_basic_cube(&cube2);
  cube2.set_material(pearl);

  //Pressure Plate
  MovingPlate pressurePlate(new_importer.loadFiles("models/pressurePlate"),
                            glm::vec3(0.5,0.5,0.5),glm::vec3(1.2,-3.99,-0.8),0.0f);
  unsigned int pressurePlate_texture = new_importer.getTexture();
  
  //Door
  MovingDoor door(new_importer.loadFiles("models/door"),
                  glm::vec3(0.5,0.5,0.5),glm::vec3(5.0,-3.99,3.75),0.0f);
  unsigned int door_texture = new_importer.getTexture();
  
  //Brick floor
  Shape worldFloor;
  world.floor_texture = get_texture("images/bricks.jpg");
  set_texture_rectangle(&worldFloor,glm::vec3(-1.0,-1.0,0.0f),2.0f,2.0f,false,false,50.0f);
  
  //Initialize the shader programs
  Shader fill_program("shaders/vertexShader.glsl","shaders/fragmentShader.glsl");
  Shader texture_program("shaders/textureVertexShader.glsl","shaders/textureFragmentShader.glsl");
  Shader outline_program("shaders/vertexShader.glsl","shaders/outlineFragmentShader.glsl");
  Shader font_program ("shaders/fontVertexShader.glsl","shaders/fontFragmentShader.glsl");
  Shader import_program("shaders/importVertexShader.glsl","shaders/importFragmentShader.glsl");

  //Map structure used to pass objects to render scene function
  //Draw_Data is a structure that has a shape and a shader
  std::map<std::string,Draw_Data> draw_map;
  //Add floor to map
  draw_map["worldFloor"].shape = &worldFloor;
  draw_map["worldFloor"].shader = &texture_program;
  //Add officeFloor to map
  draw_map["officeFloor"].shape = &officeFloor;
  draw_map["officeFloor"].shader = &import_program;
  draw_map["officeFloor"].texture = officeFloor_texture;
  //Add walls to map
  draw_map["walls"].shape = &walls;
  draw_map["walls"].shader = &import_program;
  draw_map["walls"].texture = walls_texture;
  //Add furniture to map
  draw_map["furniture"].shape = &furniture;
  draw_map["furniture"].shader = &import_program;
  draw_map["furniture"].texture = furniture_texture;
  //Add cubes to map
  draw_map["cube1"].shape = &cube1;
  draw_map["cube1"].shader = &fill_program;
  draw_map["cube2"].shape = &cube2;
  draw_map["cube2"].shader = &fill_program;
  
  //Initialize the shaders.
  std::vector<Shader*> shaders = {&fill_program,&outline_program,&texture_program,&import_program};
  glm::mat4 identity(1.0f);
  glm::mat4 model = identity;
  glm::mat4 view = identity;
  view = camera.get_view_matrix();
  glm::mat4 projection = glm::perspective(glm::radians(45.0f),(float)WIN_WIDTH/(float)WIN_HEIGHT,0.1f,100.0f);
  for (int i = 0; i < shaders.size(); i++) {
    shaders[i]->use();
    shaders[i]->setMat4("transform",identity);
    shaders[i]->setMat4("model",model);
    shaders[i]->setMat4("view",view);
    shaders[i]->setMat4("projection",projection);
    shaders[i]->setFloat("ambient_strength",0.1);
    shaders[i]->setFloat("specular_strength",1.0f);
    //Point Light
    glm::vec3 wp = world.point_light_position;
    shaders[i]->setVec3("point_light.position",glm::vec3(wp.x,wp.y,wp.z));
    shaders[i]->setVec3("point_light.ambient",0.2f*world.point_light_color);
    shaders[i]->setVec3("point_light.diffuse",world.point_light_color);
    shaders[i]->setVec3("point_light.specular",world.point_light_color);
    shaders[i]->setBool("point_light.on",world.point_light_on);
    shaders[i]->setFloat("point_light.constant",1.0);
    shaders[i]->setFloat("point_light.linear",0.14);
    shaders[i]->setFloat("point_light.quadratic",0.07);
    shaders[i]->setFloat("shininess",256);
    //Directional Light
    shaders[i]->setVec3("dir_light.direction",world.dir_light_direction);
    shaders[i]->setVec3("dir_light.ambient",0.2f*world.dir_light_color);
    shaders[i]->setVec3("dir_light.diffuse",world.dir_light_color);
    shaders[i]->setVec3("dir_light.specular",world.dir_light_color);
    shaders[i]->setBool("dir_light.on",world.dir_light_on);
  }

  //Enable depth testing to avoid managing ordering of 3D objects
  glEnable(GL_DEPTH_TEST);
  glPolygonMode(GL_BACK,GL_LINE);

  //Blending
  glEnable(GL_BLEND);
  glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

  //font_program shader setup
  font_program.use();
  font_program.setMat4("view",glm::mat4(1.0));
  font_program.setMat4("projection", glm::ortho(-5.0, 5.0, -5.0, 5.0, -1.0, 1.0));
  font_program.setVec4("transparentColor", glm::vec4(0.0f, 0.0f, 0.0f, 1.0f));
  font_program.setFloat("alpha", 0.3);
  font_program.setInt("texture1", 0);

  //Cursor setup
  glfwSetInputMode(window,GLFW_CURSOR,GLFW_CURSOR_DISABLED);
  glfwSetCursorPosCallback(window,mouse_callback);

  //glfwWindowShouldClose checks if GLFW has been instructed to close
  while(!glfwWindowShouldClose(window)) {
    float currentFrame = glfwGetTime();
    world.deltaTime = currentFrame - world.lastFrame;
    world.lastFrame = currentFrame;

    //Set the clear color
    glm::vec4 clr = world.clear_color;
    glClearColor(clr.r,clr.g,clr.b,clr.a);

    //Clear the colorbuffer using the clear color
    //If you are doing depth testing, you must clear the GL_DEPTH_BUFFER too.
    glClear(GL_COLOR_BUFFER_BIT|GL_DEPTH_BUFFER_BIT);

    //1. Process Input
    world.process_input(window, camera, camera_bird,door.get_door_status());
    door.process_input(window,world.camera->get_position());
    pressurePlate.process_input(window,world.camera->get_position());

    //2. Render Scene
    world.render_scene(draw_map,pressurePlate.get_plate_status());
    door.draw(&import_program,door_texture);
    pressurePlate.draw(&import_program,pressurePlate_texture);
    
    /****Heads up display must be last so that the semi-transparency works***/
    char my_char[2] = "+";
    arialFont.draw_char(my_char[0],glm::vec2(-0.2,0.0),font_program);
    
    //3. Poll for events
    glfwPollEvents(); //checks for events -- mouse/keyboard input
    
    //4. Swap Buffers
    glfwSwapBuffers(window); //swap the color buffer used to display
    
  }

  glfwTerminate(); //clean/delete GLFW resources
  return 0;
}

void mouse_callback(GLFWwindow* win, double xpos, double ypos) {
  if (world.first_mouse) {
    world.lastX = xpos;
    world.lastY = ypos;
    world.first_mouse = false;
  }

  float offsetx = xpos-world.lastX;
  float offsety = world.lastY-ypos;

  world.lastX = xpos;
  world.lastY = ypos;

  camera.process_mouse_movement(offsetx,offsety);
}

