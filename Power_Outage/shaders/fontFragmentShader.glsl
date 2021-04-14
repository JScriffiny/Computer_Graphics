#version 330 core

out vec4 FragColor;

in vec2 TextureCoords;
in vec3 FragPos;

uniform sampler2D texture1;
uniform vec4 transparentColor;

uniform float alpha;

void main()
{
   vec4 tempColor = texture(texture1,TextureCoords);
   if (tempColor == transparentColor) {
       tempColor = vec4(1.0,1.0,1.0,alpha);
   }

   FragColor = tempColor;
}
