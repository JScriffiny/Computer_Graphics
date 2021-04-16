#include "build_shapes.hpp"
#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"


unsigned int get_texture (std::string path) {
  unsigned int texture = 0;
  glGenTextures(1, &texture);
  glBindTexture(GL_TEXTURE_2D, texture);
  // set the texture wrapping/filtering options (on the currently bound texture object)
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);	
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
  // load and generate the texture
  int width, height, nrChannels;
  stbi_set_flip_vertically_on_load(true);  
  unsigned char *data = stbi_load(path.c_str(), &width, &height, &nrChannels,0);
  int image_type = GL_RGB;
  if (nrChannels > 3) {
      image_type = GL_RGBA;
  }
  if (data) 
  {
      glTexImage2D(GL_TEXTURE_2D, 0, image_type, width, height, 0, image_type, GL_UNSIGNED_BYTE, data);
     glGenerateMipmap(GL_TEXTURE_2D);
  }
  else
  {
      std::cout << "Failed to load texture" << std::endl;
  }
  stbi_image_free(data);
  return texture;
}

void set_up_shape(Shape* shape, void* data, int num_values,int num_vertex_vals, int data_size) {
    
    
    std::vector<Vertex_Attribute> va_vec;
    va_vec.push_back(build_vertex_attr(3,GL_FLOAT,num_vertex_vals*data_size,0));
    va_vec.push_back(build_vertex_attr(2,GL_FLOAT,num_vertex_vals*data_size,3*data_size));
    va_vec.push_back(build_vertex_attr(3,GL_FLOAT,num_vertex_vals*data_size,5*data_size));
    shape->initialize((float*)data,num_values*data_size,num_values/num_vertex_vals,va_vec,GL_TRIANGLE_FAN);

}

//Creates a basic triangle (xy plane) and defines the data members of the Shape object referenced by the first 
// argument. 
void set_basic_triangle(Shape* tri, glm::vec3 v1, glm::vec3 v2, glm::vec3 v3, bool flip_normal, bool outline) {
    glm::vec3 normal = glm::normalize(glm::cross(v2-v1,v3-v1));
    if (flip_normal) {
        normal = -normal;
    }
    float data[18] = {0.0f};
    for (int i = 0; i<3;i++) {
        data[i] = v1[i];
        data[i+3] = normal[i];
        data[i+6]=v2[i];
        data[i+9]=normal[i];
        data[i+12]=v3[i];
        data[i+15]=normal[i];
    }
    std::vector<Vertex_Attribute> va_vec;
    va_vec.push_back(build_vertex_attr(3,GL_FLOAT,6*sizeof(float),0));
    va_vec.push_back(build_vertex_attr(3,GL_FLOAT,6*sizeof(float),3*sizeof(float)));
    tri->initialize(data,sizeof(data),3,va_vec);
    if (outline) {
        unsigned int indices[] = {0,1,2};
        tri->set_EBO(indices,3);
    }
}
//Creates a basic triangle (xy plane) and defines the data members of the Shape object referenced by the first 
// argument. 
void set_texture_triangle (Shape* tri, glm::vec3 v1, glm::vec3 v2, glm::vec3 v3, bool flip_normal, bool outline, float text_size) {
    glm::vec3 normal = glm::normalize(glm::cross(v2-v1,v3-v1));
    if (flip_normal) {
        normal = -normal;
    }
    float data[24] = {0.0f};
    for (int i = 0; i<3;i++) {
        data[i] = v1[i];
        data[i+5]=normal[i];
        data[i+8]=v2[i];
        data[i+13]=normal[i];
        data[i+16]=v3[i];
        data[i+21]=normal[i];
    }

    data[3] = 0.0f*text_size;
    data[4] = 0.0f*text_size;
    data[11] = 1.0f*text_size;
    data[12] = 0.0f*text_size;
    data[19] = text_size*((data[10] + 1)/2.0f);
    data[20] = text_size*1.0f;
    std::vector<Vertex_Attribute> va_vec;
    va_vec.push_back(build_vertex_attr(3,GL_FLOAT,8*sizeof(float),0));
    va_vec.push_back(build_vertex_attr(2,GL_FLOAT,8*sizeof(float),3*sizeof(float)));
    va_vec.push_back(build_vertex_attr(3,GL_FLOAT,8*sizeof(float),5*sizeof(float)));
    tri->initialize(data,sizeof(data),3,va_vec);
    if (outline) {
        unsigned int indices[] = {0,1,2};
        tri->set_EBO(indices,3);
    }
}

//Creates a basic circle (xy plane) and defines data members of the Shape object referenced by the first argument.
void set_basic_circle (Shape* circle, glm::vec3 location, float radius, bool outline, int points) {
    float data[6*points] = {0.0f};
    float angle = 0.0f;
    for(int i = 0; i<points; i++) {
        data[i*6] = location.x + radius*cos(glm::radians(angle));
        data[i*6+1] = location.y + radius*sin(glm::radians(angle));
        data[i*6+2] = 0.0f;
        data[i*6+3] = 0.0f;
        data[i*6+4] = 0.0f;
        data[i*6+5] = 1.0f;
        angle+=360.0f/points;
    }
    std::vector<Vertex_Attribute> va_vec;
    va_vec.push_back(build_vertex_attr(3,GL_FLOAT,6*sizeof(float),0));
    va_vec.push_back(build_vertex_attr(3,GL_FLOAT,6*sizeof(float),3*sizeof(float)));
    circle->initialize(data,sizeof(data),points,va_vec,GL_TRIANGLE_FAN);

    if (outline) {
        unsigned int indices[points] = {0};
        for (int i = 0; i < points; i++) {
            indices[i] = i;
        }
        circle->set_EBO(indices,points);
    }
}

//Creates a basic circle (xy plane) and defines data members of the Shape object referenced by the first argument.
void set_texture_circle (Shape* circle, glm::vec3 location, float radius, bool outline, int points) {
    float data[5*points] = {0.0f};
    float angle = 0.0f;
    for(int i = 0; i<points; i++) {
        data[i*5] = location.x + radius*cos(glm::radians(angle));
        data[i*5+1] = location.y + radius*sin(glm::radians(angle));
        data[i*5+2] = 0.0f;
        data[i*5+3] = 0.5f + 0.5f*cos(glm::radians(angle));
        data[i*5+4] = 0.5f + 0.5f*sin(glm::radians(angle));
        angle+=360.0f/points;
    }
    std::vector<Vertex_Attribute> va_vec;
    va_vec.push_back(build_vertex_attr(3,GL_FLOAT,5*sizeof(float),0));
    va_vec.push_back(build_vertex_attr(2,GL_FLOAT,5*sizeof(float),3*sizeof(float)));
    circle->initialize(data,sizeof(data),points,va_vec,GL_TRIANGLE_FAN);

    if (outline) {
        unsigned int indices[points] = {0};
        for (int i = 0; i < points; i++) {
            indices[i] = i;
        }
        circle->set_EBO(indices,points);
    }
}

//Creates a basic rectangle (xy plane) and defines the members of the Shape object referenced by the first argument.
void set_basic_rectangle (Shape* rect, glm::vec3 lower_left, float width, float height, bool flip_normal, bool outline) {
    
    float data[] = {
        lower_left.x, lower_left.y, lower_left.z,
        lower_left.x+width, lower_left.y, lower_left.z,
        lower_left.x+width, lower_left.y+height, lower_left.z,
        lower_left.x, lower_left.y+height, lower_left.z
    };
    std::vector<Vertex_Attribute> va_vec;
    va_vec.push_back(build_vertex_attr(3,GL_FLOAT,3*sizeof(float),0));
    rect->initialize(data,sizeof(data),4,va_vec,GL_TRIANGLE_FAN);
    if (outline) {
        unsigned int indices[4] = {0, 1, 2, 3};
        rect->set_EBO(indices,4);
    }
}

//Creates a basic rectangle (xy plane) and defines the members of the Shape object referenced by the first argument.
void set_texture_rectangle (Shape* rect, glm::vec3 lower_left, float width, float height, bool flip_normal, bool outline, float text_size) {
    glm::vec3 v1 = lower_left;
    glm::vec3 v2 (v1.x+width,v1.y,v1.z);
    glm::vec3 v3 (v1.x, v1.y+height,v1.z);
    glm::vec3 normal = glm::normalize(glm::cross(v2-v1,v3-v1));
    if (flip_normal) {
        normal = -normal;
    }
    float data[] = {
        lower_left.x, lower_left.y, lower_left.z, 0.0f,0.0f, normal.x, normal.y, normal.z, 
        lower_left.x+width, lower_left.y, lower_left.z, 1.0f, 0.0f, normal.x, normal.y, normal.z,
        lower_left.x+width, lower_left.y+height, lower_left.z, 1.0f, 1.0f, normal.x, normal.y, normal.z,
        lower_left.x, lower_left.y+height, lower_left.z, 0.0f, 1.0f, normal.x, normal.y, normal.z,
        lower_left.x, lower_left.y, lower_left.z, 0.0f,0.0f, normal.x, normal.y, normal.z
    };

    for (int i = 0; i < 5; i++) {
        data[i*8+3] = data[i*8+3]*text_size;
        data[i*8+4] = data[i*8+4]*text_size;
    }
    std::vector<Vertex_Attribute> va_vec;



    va_vec.push_back(build_vertex_attr(3,GL_FLOAT,8*sizeof(float),0));
    va_vec.push_back(build_vertex_attr(2,GL_FLOAT,8*sizeof(float),3*sizeof(float)));
    va_vec.push_back(build_vertex_attr(3,GL_FLOAT,8*sizeof(float),5*sizeof(float)));
    rect->initialize(data,sizeof(data),5,va_vec,GL_TRIANGLE_FAN);
    if (outline) {
        unsigned int indices[4] = {0, 1, 2, 3};
        rect->set_EBO(indices,4);
    }
}

void set_basic_starfield (Shape* stars, int num_stars) {
    float data[num_stars*3] = {0.0f};
    for (int i = 0; i < num_stars; i+=3) {
        data[i] = 1.0f*(((rand() % 1000)-500)/500.0f);
        data[i+1] = 1.0f*(((rand()%1000)-500)/500.0f);
    }
    std::vector<Vertex_Attribute> va_vec;
    va_vec.push_back(build_vertex_attr(3,GL_FLOAT,3*sizeof(float),0));
    stars->initialize(data,sizeof(data),sizeof(data)/sizeof(float)/3.0,va_vec,GL_POINTS);
}

void set_basic_cube (Shape* cube, bool flip_normal, bool outline) {
    float data[] = {  //outline: lbf,rbf,rtf,ltf,lbf,lbn,rbn, rtn, ltn, lbn, ltf, ltn, rbf, rbn, rtf, rtn
                      //           0,  1,  2,  4,  5,  6,  7,   8,  10,   6,  4,   10,   1,   7,   3,  8
    -0.5f, -0.5f, -0.5f,  0.0f,  0.0f, -1.0f, //lbf 0
     0.5f, -0.5f, -0.5f,  0.0f,  0.0f, -1.0f, //rbf 1
     0.5f,  0.5f, -0.5f,  0.0f,  0.0f, -1.0f, //rtf 2
     0.5f,  0.5f, -0.5f,  0.0f,  0.0f, -1.0f, //rtf 3
    -0.5f,  0.5f, -0.5f,  0.0f,  0.0f, -1.0f, //ltf 4
    -0.5f, -0.5f, -0.5f,  0.0f,  0.0f, -1.0f, //lbf 5

    -0.5f, -0.5f,  0.5f,  0.0f,  0.0f, 1.0f, //lbn 6
     0.5f, -0.5f,  0.5f,  0.0f,  0.0f, 1.0f, //rbn 7
     0.5f,  0.5f,  0.5f,  0.0f,  0.0f, 1.0f, //rtn 8
     0.5f,  0.5f,  0.5f,  0.0f,  0.0f, 1.0f, //rtn 9
    -0.5f,  0.5f,  0.5f,  0.0f,  0.0f, 1.0f, //ltn 10
    -0.5f, -0.5f,  0.5f,  0.0f,  0.0f, 1.0f, //lbn 11

    -0.5f,  0.5f,  0.5f, -1.0f,  0.0f,  0.0f, //ltn 12
    -0.5f,  0.5f, -0.5f, -1.0f,  0.0f,  0.0f, //ltf 13
    -0.5f, -0.5f, -0.5f, -1.0f,  0.0f,  0.0f, //lbf 14
    -0.5f, -0.5f, -0.5f, -1.0f,  0.0f,  0.0f, //lbf 15
    -0.5f, -0.5f,  0.5f, -1.0f,  0.0f,  0.0f, //lbn 16
    -0.5f,  0.5f,  0.5f, -1.0f,  0.0f,  0.0f, //ltn 17

     0.5f,  0.5f,  0.5f,  1.0f,  0.0f,  0.0f, //rtn 18
     0.5f,  0.5f, -0.5f,  1.0f,  0.0f,  0.0f, //rtf 19
     0.5f, -0.5f, -0.5f,  1.0f,  0.0f,  0.0f, //rbf 20
     0.5f, -0.5f, -0.5f,  1.0f,  0.0f,  0.0f, //rbf 21
     0.5f, -0.5f,  0.5f,  1.0f,  0.0f,  0.0f, //rbn 22
     0.5f,  0.5f,  0.5f,  1.0f,  0.0f,  0.0f, //rtn 23

    -0.5f, -0.5f, -0.5f,  0.0f, -1.0f,  0.0f, //lbf 24
     0.5f, -0.5f, -0.5f,  0.0f, -1.0f,  0.0f, //rbf 25
     0.5f, -0.5f,  0.5f,  0.0f, -1.0f,  0.0f, //rbn 26
     0.5f, -0.5f,  0.5f,  0.0f, -1.0f,  0.0f, //rbn 27
    -0.5f, -0.5f,  0.5f,  0.0f, -1.0f,  0.0f, //lbn 28
    -0.5f, -0.5f, -0.5f,  0.0f, -1.0f,  0.0f, //lbf 29

    -0.5f,  0.5f, -0.5f,  0.0f,  1.0f,  0.0f, //ltf 30
     0.5f,  0.5f, -0.5f,  0.0f,  1.0f,  0.0f, //rtf 31
     0.5f,  0.5f,  0.5f,  0.0f,  1.0f,  0.0f, //rtn 32
     0.5f,  0.5f,  0.5f,  0.0f,  1.0f,  0.0f, //rtn 33
    -0.5f,  0.5f,  0.5f,  0.0f,  1.0f,  0.0f, //ltn 34
    -0.5f,  0.5f, -0.5f,  0.0f,  1.0f,  0.0f //ltf 35
    };

    if (flip_normal) {
        int num_vertices = 36;
        for (int i = 0; i < num_vertices; i++) {
            data[i*6+3] = -1.0f*data[i*6+3];
            data[i*6+4] = -1.0f*data[i*6+4];
            data[i*6+5] = -1.0f*data[i*6+5];
        }
    }

    std::vector<Vertex_Attribute> va_vec;
    va_vec.push_back(build_vertex_attr(3,GL_FLOAT,6*sizeof(float),0));
    va_vec.push_back(build_vertex_attr(3,GL_FLOAT,6*sizeof(float),3*sizeof(float)));
    cube->initialize(data,sizeof(data),sizeof(data)/sizeof(float)/6.0,va_vec,GL_TRIANGLES);
    if (outline) {
        int num_points = sizeof(data)/sizeof(float)/6;
        unsigned int indices[num_points] = {0};
        for (int i = 0; i < num_points; i++) {
            indices[i] = i;    
        }
        
        cube->set_EBO(indices,num_points);
    }
}

unsigned int get_cube_map(std::vector<std::string> faces, bool cube_map_flag) {
    unsigned int textureID;
    glGenTextures(1, &textureID);
    glBindTexture(GL_TEXTURE_CUBE_MAP, textureID);

    int width, height, nrChannels;
    stbi_set_flip_vertically_on_load(cube_map_flag);
    for (unsigned int i = 0; i < faces.size(); i++)
    {
        unsigned char *data = stbi_load(faces[i].c_str(), &width, &height, &nrChannels, 0);
        if (data)
        {
            glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 
                         0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, data
            );
            stbi_image_free(data);
        }
        else
        {
            std::cout << "Cubemap tex failed to load at path: " << faces[i] << std::endl;
            stbi_image_free(data);
        }
    }
    stbi_set_flip_vertically_on_load(!cube_map_flag);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);

    return textureID;
}