#version 330 core

out vec4 FragColor;

in vec3 ourColor;
in vec2 TexCoord;

// Used to hold the texture object
uniform sampler2D texture1;
uniform sampler2D texture2;

void main()
{
  // mix(...) linearly interpolates between 2 colours. If the third argument is 0.0 the first input will be returned, if it's 1.0 then the second input will be returned.
  // A value of 0.2 will return 80% of the first input colour and 20% of the second input colour
  FragColor = mix(texture(texture1, TexCoord), texture(texture2, vec2(1.0 - TexCoord.x, TexCoord.y)), 0.2) * vec4(ourColor, 1.0);
}