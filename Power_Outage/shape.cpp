#include "shape.hpp"

//define the functions declared in the Shape class

Shape::Shape(): VBO(0),VAO(0),
                num_of_vertices(0),
                clear_objs(false),
                primitive(GL_TRIANGLES) {

}

Shape::Shape(Shape &obj) {
  this->VBO = obj.VBO;
  this->VAO = obj.VAO;
  this->clear_objs = false;
  this->EBO = obj.EBO;
  this->num_indices = obj.num_indices;
  this->num_of_vertices = obj.num_of_vertices;
  this->primitive = obj.primitive;
  this->clear_objs = false;
}

Shape::Shape(Shape_Struct obj) {
  this->VBO = obj.VBO;
  this->VAO = obj.VAO;
  this->clear_objs = obj.clear_objs;
  this->EBO = obj.EBO;
  this->num_indices = obj.num_indices;
  this->num_of_vertices = obj.num_of_vertices;
  this->primitive = obj.primitive;
}

void Shape::initialize (float* data, int data_bytes, int num_vertices, 
                         std::vector<Vertex_Attribute> vao, 
                         GLuint prim) 
{
    glGenBuffers(1,&(this->VBO));

  glBindBuffer(GL_ARRAY_BUFFER,this->VBO);
  this->num_of_vertices = num_vertices;
  this->primitive = prim;

  //use glBufferData to push data to the buffer memory
  glBufferData(GL_ARRAY_BUFFER,data_bytes,data,GL_STATIC_DRAW);

  //VAO setup
  //create a vertex array object
  
  glGenVertexArrays(1,&(this->VAO));

  //bind the VAO to configure it.
  glBindVertexArray(this->VAO);

  for (int i = 0; i < vao.size(); i++) {
  //Vertex shader lets us specify anything we want...
  //we need to specify what the data is
  glVertexAttribPointer(i,vao[i].num_per_vertex,
                        vao[i].data_type,GL_FALSE,vao[i].stride_bytes,(void*)(intptr_t)vao[i].offset_bytes);
  //enable our pointer
  glEnableVertexAttribArray(i);
  }

  this->clear_objs = true;
}

void Shape::set_EBO (unsigned int* data, int num_indices) {
  glGenBuffers(1,&(this->EBO));
  glBindVertexArray(this->VAO);
  glBindBuffer(GL_ELEMENT_ARRAY_BUFFER,this->EBO);
  glBufferData(GL_ELEMENT_ARRAY_BUFFER,num_indices*sizeof(unsigned int),data,GL_STATIC_DRAW);
  glBindVertexArray(0);
  this->num_indices = num_indices;
}


//define destructor
Shape::~Shape() {
  if (this->clear_objs) {
    //std::cout<<"Deleted shape."<<std::endl;
    glDeleteBuffers(1,&(this->VBO));
    glDeleteVertexArrays(1,&(this->VAO));
  }
}
//define draw
void Shape::draw (unsigned int shader_program,unsigned int outline_program) {
  if (this->VBO<1 || this->VAO<1) {
    std::cout<<"SHAPE NOT INITIALIZED."<<std::endl;
    return;
  }
  glUseProgram(shader_program);
  glBindVertexArray(this->VAO);
  
  glDrawArrays(this->primitive,0,this->num_of_vertices);

  if (outline_program>0 && this->EBO > 0) {
    glUseProgram(outline_program);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER,this->EBO);
    glDrawElements(GL_LINE_LOOP,this->num_indices,GL_UNSIGNED_INT,0);
    
  }

  glBindVertexArray(0);
}

void Shape::set_material(Material m) {
  this->material = m;
}

void Shape::use_material (Shader* s) {
  s->setVec3("material.ambient",this->material.ambient);
  s->setVec3("material.diffuse",this->material.diffuse);
  s->setVec3("material.specular",this->material.specular);
  s->setFloat("material.shininess",this->material.shininess);
}



