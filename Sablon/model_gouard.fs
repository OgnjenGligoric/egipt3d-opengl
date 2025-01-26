#version 330 core
out vec4 FragColor;

in vec3 LightingColor;  // Interpolated lighting color from vertex shader
in vec2 TexCoords;

uniform sampler2D texture_diffuse1;

void main()
{
    // Sample texture color
    vec3 color = texture(texture_diffuse1, TexCoords).rgb;

    // Apply interpolated lighting to the texture color
    FragColor = vec4(LightingColor * color, 1.0);
}
