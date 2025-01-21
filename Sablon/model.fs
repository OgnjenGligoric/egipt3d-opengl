#version 330 core
out vec4 FragColor;

in vec2 TexCoords;
in vec3 Normal;  
in vec3 FragPos;  

uniform sampler2D texture_diffuse1;
uniform sampler2D texture_normal1;
uniform sampler2D texture_specular1;

uniform vec3 lightColor;
uniform vec3 lightPos;  
uniform vec3 viewPos;

void main()
{    
    vec3 viewDir = normalize(viewPos - FragPos);

    vec3 color = texture(texture_diffuse1, TexCoords).rgb;
    
    vec3 ambient = 0.1 * color;
    
    vec3 lightDir = normalize(lightPos - FragPos);
    vec3 normal = normalize(texture(texture_normal1, TexCoords).xyz * 2.0f - 1.0f);
    if (!gl_FrontFacing) normal = -normal;
    float diff = max(dot(lightDir, normal), 0.0);
    vec3 diffuse = diff * color;
    normal = normalize(Normal);
    vec3 reflectDir = reflect(lightDir, normal);
    float spec = pow(max(dot(viewDir, reflectDir), 0.0), 32.0);
    vec3 specular = vec3(1) * spec;

    FragColor = vec4((ambient + diffuse + specular * texture(texture_specular1, TexCoords).r ) , 1.0);
}