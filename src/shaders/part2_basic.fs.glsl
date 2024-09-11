#version 330 core

out vec4 FragColor;

in vec2 TexCoord;

uniform sampler2D texture1;
uniform sampler2D texture2;

void main()
{
  // linearly interpolate between both textures (container 80%, awesomeface 20%)
  FragColor = mix(texture(texture1, TexCoord), texture(texture2, TexCoord), 0.2);
}