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
Camera camera(glm::vec3(10.0f,-3.0f,-3.0f),glm::vec3(0.0f,1.0f,0.0f),115.0f, 0.0f);
//Bird's Eye Camera
Camera camera_bird(glm::vec3(-0.5f,18.0f,1.0f),glm::vec3(0.0f,-1.0f,0.0f),-90.0f, -85.0f);

//Post Processing Globals
int post_process_selection = 1;
bool post_process_flag = true;
bool nightvision_on = false;

//Capture the mouse position data on mouse movement
void render_skybox(Shader * shader, Shape shape, unsigned int texture);
void mouse_callback (GLFWwindow* win, double xpos, double ypos);
glm::mat4 getLightPOV();
void post_process_input(GLFWwindow* win);


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

  //Skybox shape
  Shape skybox;
  set_basic_cube(&skybox);

  //Cube Map
  std::vector<std::string> faces {
    "skybox/right.jpg",
    "skybox/left.jpg",
    "skybox/top.jpg",
    "skybox/bottom.jpg",
    "skybox/front.jpg",
    "skybox/back.jpg"
  };
  unsigned int cubemapTexture = get_cube_map(faces,false);

  //Post Processing Rectangle
  Shape post_rect;
  set_texture_rectangle(&post_rect,glm::vec3(-1.0f,-1.0f,0.0f),2.0f,2.0f,false,false,1.0f);
  
  //Initialize the shader programs
  Shader fill_program("shaders/vertexShader.glsl","shaders/fragmentShader.glsl");
  Shader texture_program("shaders/textureVertexShader.glsl","shaders/textureFragmentShader.glsl");
  Shader outline_program("shaders/vertexShader.glsl","shaders/outlineFragmentShader.glsl");
  Shader font_program ("shaders/fontVertexShader.glsl","shaders/fontFragmentShader.glsl");
  Shader import_program("shaders/importVertexShader.glsl","shaders/importFragmentShader.glsl");
  Shader depth_program("shaders/depthVertexShader.glsl","shaders/depthFragmentShader.glsl");
  Shader skybox_program("shaders/skyboxVertexShader.glsl","shaders/skyboxFragmentShader.glsl");
  Shader post_process_program("shaders/postVertexShader.glsl","shaders/postFragmentShader.glsl");

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
  std::vector<Shader*> shaders = {&fill_program,&outline_program,&texture_program,
                                  &import_program,&depth_program,&skybox_program,&post_process_program};
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
    shaders[i]->setVec4("view_position",glm::vec4(world.camera->get_position(),0.0f));
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
    //Spot Light
    shaders[i]->setVec3("spot_light.position",world.camera->get_position());
    shaders[i]->setVec3("spot_light.direction",world.camera->get_front());
    shaders[i]->setFloat("spot_light.cutOff",glm::cos(glm::radians(12.5f)));
    shaders[i]->setFloat("spot_light.outerCutOff",glm::cos(glm::radians(17.5f)));
    shaders[i]->setVec3("spot_light.ambient",world.spot_light_ambient);
    shaders[i]->setVec3("spot_light.diffuse",world.spot_light_diffuse);
    shaders[i]->setVec3("spot_light.specular",world.spot_light_specular);
    shaders[i]->setFloat("spot_light.constant",1.0f);
    shaders[i]->setFloat("spot_light.linear",0.09f);
    shaders[i]->setFloat("spot_light.quadratic",0.032f);
    shaders[i]->setBool("spot_light.on",world.spot_light_on);
    //Directional Light
    shaders[i]->setVec3("dir_light.direction",world.dir_light_direction);
    shaders[i]->setVec3("dir_light.ambient",0.2f*world.dir_light_color);
    shaders[i]->setVec3("dir_light.diffuse",world.dir_light_color);
    shaders[i]->setVec3("dir_light.specular",world.dir_light_color);
    shaders[i]->setBool("dir_light.on",world.dir_light_on);
    shaders[i]->setFloat("time",glfwGetTime());
  }

  //Enable depth testing to avoid managing ordering of 3D objects
  glEnable(GL_DEPTH_TEST);
  glPolygonMode(GL_BACK,GL_LINE);

  //Blending
  glEnable(GL_BLEND);
  glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

  //Framebuffer (Post Processing)
  //Make framebuffer object
  unsigned int framebuffer;
  glGenFramebuffers(1, &framebuffer);
  glBindFramebuffer(GL_FRAMEBUFFER, framebuffer); 
  //Create and bind a texture
  unsigned int texColorBuffer;
  glGenTextures(1, &texColorBuffer);
  glBindTexture(GL_TEXTURE_2D, texColorBuffer);
  glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, WIN_WIDTH, WIN_HEIGHT, 0, GL_RGB, GL_UNSIGNED_BYTE, NULL);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
  glBindTexture(GL_TEXTURE_2D, 0);
  //Attach texture to framebuffer object
  glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, texColorBuffer, 0);
  //Make renderbuffer object
  unsigned int rbo;
  glGenRenderbuffers(1, &rbo);
  glBindRenderbuffer(GL_RENDERBUFFER, rbo); 
  glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH24_STENCIL8, WIN_WIDTH, WIN_HEIGHT);
  glBindRenderbuffer(GL_RENDERBUFFER, 0);
  //Attach the renderbuffer to framebuffer depth and stencil attachments
  glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, GL_RENDERBUFFER, rbo);
  //Check if framebuffer is complete
  if(glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
	  std::cout << "ERROR::FRAMEBUFFER:: Framebuffer is not complete!" << std::endl;
  glBindFramebuffer(GL_FRAMEBUFFER, 0);

  /* //Framebuffer (Shadows)
  unsigned int depthMapFBO;
  glGenFramebuffers(1, &depthMapFBO);
  const unsigned int SHADOW_WIDTH = 1024, SHADOW_HEIGHT = 1024; //2048x2048 for better resolution
  //Generate 2D texture to hold depth information
  unsigned int depthMap;
  glGenTextures(1, &depthMap);
  glBindTexture(GL_TEXTURE_2D, depthMap);
  glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT, SHADOW_WIDTH, SHADOW_HEIGHT,
               0, GL_DEPTH_COMPONENT, GL_FLOAT, NULL);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
  //Bind and attach the texture
  glBindFramebuffer(GL_FRAMEBUFFER, depthMapFBO);
  glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, depthMap, 0);
  glDrawBuffer(GL_NONE);
  glReadBuffer(GL_NONE);
  //Reset to default
  glBindFramebuffer(GL_FRAMEBUFFER, 0); */

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

    //Bind new framebuffer
    glBindFramebuffer(GL_FRAMEBUFFER, framebuffer);

    //Set the clear color
    glm::vec4 clr = world.clear_color;
    glClearColor(clr.r,clr.g,clr.b,clr.a);

    /* glViewport(0,0,SHADOW_WIDTH,SHADOW_HEIGHT);
    glBindFramebuffer(GL_FRAMEBUFFER, depthMapFBO); */
    //Clear appropriate buffers
    glClear(GL_COLOR_BUFFER_BIT|GL_DEPTH_BUFFER_BIT);

    //1. Process Input
    world.process_input(window, camera, camera_bird,door.get_door_status());
    door.process_input(window,world.camera->get_position());
    pressurePlate.process_input(window,world.camera->get_position());
    post_process_input(window);

    //2. Render Scene
    /* world.render_scene(draw_map,pressurePlate.get_plate_status(),&depth_program);
    door.draw(&import_program,door_texture);
    pressurePlate.draw(&import_program,pressurePlate_texture);
    render_skybox(&skybox_program,skybox,cubemapTexture);

    glViewport(0,0,WIN_WIDTH,WIN_HEIGHT);
    glBindFramebuffer(GL_FRAMEBUFFER,0);
    glClear(GL_COLOR_BUFFER_BIT|GL_DEPTH_BUFFER_BIT);//clear stencil too if needed */

    //texture_program.use();
    //texture_program.setMat4("lightSpaceMatrix",getLightPOV());

    world.render_scene(draw_map,pressurePlate.get_plate_status());
    door.draw(&import_program,door_texture);
    pressurePlate.draw(&import_program,pressurePlate_texture);
    render_skybox(&skybox_program,skybox,cubemapTexture);
    
    /****Heads up display must be last so that the semi-transparency works***/
    char my_char[2] = "+";
    arialFont.draw_char(my_char[0],glm::vec2(-0.2,0.0),font_program);

    //Unbind framebuffer
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
    glClearColor(1.0f, 1.0f, 1.0f, 1.0f); 
    post_process_program.use();
    glBindTexture(GL_TEXTURE_2D, texColorBuffer);
    glDisable(GL_DEPTH_TEST);
    //Set uniform value for post_process_selection value
    post_process_program.setInt("post_process_selection", post_process_selection);
    post_rect.draw(post_process_program.ID);
    glEnable(GL_DEPTH_TEST);
    
    //3. Poll for events
    glfwPollEvents(); //checks for events -- mouse/keyboard input
    
    //4. Swap Buffers
    glfwSwapBuffers(window); //swap the color buffer used to display
    
  }

  glfwTerminate(); //clean/delete GLFW resources
  return 0;
}

void render_skybox(Shader * shader, Shape shape, unsigned int texture) {
  shader->use();
  glm::mat4 temp_view = glm::mat4(glm::mat3(camera.get_view_matrix())); 
  shader->setMat4("view",temp_view);
  glDepthFunc(GL_EQUAL);
  glBindTexture(GL_TEXTURE_CUBE_MAP,texture);
  glDrawArrays(GL_TRIANGLES, 0, 36);
  shape.draw(shader->ID);
  glDepthFunc(GL_LESS);
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

glm::mat4 getLightPOV() {
  glm::mat4 lightProjection = glm::ortho(-10.0f,10.0f,-10.0f,10.0f,1.0f,20.0f);
  glm::vec3 light_pos = glm::vec3(world.point_light_position);
  glm::vec3 front = world.camera->get_front();
  glm::mat4 lightView = glm::lookAt(light_pos,front*10.0f,glm::vec3(0.0f,1.0f,0.0f));
  glm::mat4 lightSpaceMatrix = lightProjection * lightView;
  return lightSpaceMatrix;
}

void post_process_input(GLFWwindow* win) {
  if (glfwGetKey(win,GLFW_KEY_1) == GLFW_PRESS && post_process_flag) {
    post_process_selection = 1;
    nightvision_on = false;
    post_process_flag = false;
  }
  if (glfwGetKey(win,GLFW_KEY_1) == GLFW_RELEASE) {
    post_process_flag = true;
  }
  if (glfwGetKey(win,GLFW_KEY_2) == GLFW_PRESS && post_process_flag) {
    post_process_selection = 2;
    nightvision_on = true;
    post_process_flag = false;
  }
  if (glfwGetKey(win,GLFW_KEY_2) == GLFW_RELEASE) {
    post_process_flag = true;
  }
  if (glfwGetKey(win,GLFW_KEY_3) == GLFW_PRESS && post_process_flag) {
    post_process_selection = 3;
    nightvision_on = false;
    post_process_flag = false;
  }
  if (glfwGetKey(win,GLFW_KEY_3) == GLFW_RELEASE) {
    post_process_flag = true;
  }
  if (glfwGetKey(win,GLFW_KEY_4) == GLFW_PRESS && post_process_flag) {
    post_process_selection = 4;
    nightvision_on = false;
    post_process_flag = false;
  }
  if (glfwGetKey(win,GLFW_KEY_4) == GLFW_RELEASE) {
    post_process_flag = true;
  }
  if (glfwGetKey(win,GLFW_KEY_5) == GLFW_PRESS && post_process_flag) {
    post_process_selection = 5;
    nightvision_on = false;
    post_process_flag = false;
  }
  if (glfwGetKey(win,GLFW_KEY_5) == GLFW_RELEASE) {
    post_process_flag = true;
  }
  if (glfwGetKey(win,GLFW_KEY_6) == GLFW_PRESS && post_process_flag) {
    post_process_selection = 6;
    nightvision_on = false;
    post_process_flag = false;
  }
  if (glfwGetKey(win,GLFW_KEY_6) == GLFW_RELEASE) {
    post_process_flag = true;
  }
  if (glfwGetKey(win,GLFW_KEY_7) == GLFW_PRESS && post_process_flag) {
    post_process_selection = 7;
    nightvision_on = false;
    post_process_flag = false;
  }
  if (glfwGetKey(win,GLFW_KEY_7) == GLFW_RELEASE) {
    post_process_flag = true;
  }
  if (nightvision_on) world.dir_light_on = true;
  else world.dir_light_on = false;
}