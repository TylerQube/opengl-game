#version 330 core
out vec4 FragColor;

in vec2 TexCoord;
in vec3 Normal;  
in vec3 FragPos;  

  
uniform bool phong;

uniform vec3 lightPos; 
uniform vec3 lightColor;
uniform vec3 objectColor;
uniform vec3 viewPos;

uniform sampler2D myTexture;

void main()
{
    vec3 normal1 = Normal;
    float lightDot = dot(lightPos - FragPos, Normal);
    if(lightDot < 0)
	normal1 *= -1;
    // ambient
    float ambientStrength = 0.7;
    vec3 ambient = ambientStrength * lightColor;
  	
    // diffuse 
    vec3 norm = normalize(normal1);
    vec3 lightDir = normalize(lightPos - FragPos);
    float diff = max(dot(norm, lightDir), 0.0);
    vec3 diffuse = diff * lightColor;

    // specular
    float specularStrength = 0.5;

    vec3 viewDir = normalize(viewPos - FragPos);
    vec3 reflectDir = reflect(-lightDir, norm);

    float spec = pow(max(dot(viewDir, reflectDir), 0.0), 32);
    vec3 specular = specularStrength * spec * lightColor;
            
    float dist = length(FragPos - lightPos);
    vec3 result = (ambient + diffuse + specular) * objectColor / (dist / 5);
    FragColor = texture(myTexture, TexCoord) * vec4(phong ? result : objectColor, 1.0);
} 
