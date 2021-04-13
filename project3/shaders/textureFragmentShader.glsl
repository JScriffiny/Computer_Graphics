#version 330 core
out vec4 FragColor;
in vec2 texture_coords;
in vec3 normal_vector;
in vec3 FragPos;

uniform sampler2D texture_image;
uniform vec4 view_position;
uniform float shininess;

struct PointLight {
  vec3 position;
  vec3 ambient;
  vec3 diffuse;
  vec3 specular;
  bool on;
  float constant;
  float linear;
  float quadratic;
};

struct DirLight {
  vec3 direction;
  vec3 ambient;
  vec3 diffuse;
  vec3 specular;
  bool on;
};

uniform PointLight point_light;
uniform DirLight dir_light;

vec4 calc_point_light ();
vec4 calc_dir_light ();

void main()
{
  FragColor = (calc_point_light()+calc_dir_light())*texture(texture_image,texture_coords);
}

vec4 calc_point_light () {
//Calculate the point light effect
  if (!point_light.on) {
    return vec4(0.0,0.0,0.0,1.0);
  }

  //ambient light
  vec3 ambient_light = point_light.ambient;

  //diffuse light
  vec3 norm = normalize(normal_vector);
  vec3 light_direction = normalize(point_light.position-FragPos.xyz);

  float diff_coeff = max(dot(norm,light_direction),0.0f);
  vec3 diffuse_light = diff_coeff * point_light.diffuse;

  //specular light
  vec3 view_direction = normalize(view_position.xyz - FragPos);
  vec3 reflect_dir = reflect(-light_direction,norm);
  float spec_coeff = pow(max(dot(view_direction,reflect_dir),0.0),shininess);
  vec3 specular_light = spec_coeff * point_light.specular;

  //attenuation
  float distance    = length(point_light.position - FragPos);
  float attenuation = 1.0 / (point_light.constant + point_light.linear * distance + 
  			                    point_light.quadratic * (distance * distance));

  ambient_light  *= attenuation;
  diffuse_light  *= attenuation;
  specular_light *= attenuation;

  return vec4(ambient_light+diffuse_light+specular_light,1.0);
}

vec4 calc_dir_light() {
  if (!dir_light.on) {
    return vec4(0.0,0.0,0.0,0.0);
  }

  vec3 lightDir = normalize(-dir_light.direction);

  // diffuse shading
  vec3 norm = normalize(normal_vector);
  float diff = max(dot(norm, lightDir), 0.0);

  // specular shading
  vec3 view_direction = normalize(view_position.xyz - FragPos);
  vec3 reflectDir = reflect(-lightDir, norm);
  float spec = pow(max(dot(view_direction, reflectDir), 0.0), shininess);

  // combine results
  vec3 ambient, diffuse, specular;
  ambient = dir_light.ambient;
  diffuse = diff*dir_light.diffuse;
  specular = spec*dir_light.specular;

  return vec4(ambient + diffuse + specular,1.0);
}