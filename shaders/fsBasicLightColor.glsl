#version 330 core

in vec3 Normal;
in vec3 FragPos;

out vec4 FragColor;
  
uniform vec3 objectColor;
uniform vec3 lightColor;
uniform vec3 lightPos;
uniform vec3 viewPos;

void main()
{
	float ambientStrength = 0.4;
	vec3 ambient = ambientStrength * lightColor;
	
	vec3 norm = normalize(Normal);
	vec3 lightDir = normalize(vec3(viewPos[0], viewPos[1], viewPos[2] + 1.0) - FragPos);
	float diff = max(dot(norm, lightDir), 0.0);
	vec3 diffuse = diff*lightColor;
	
	float specularStrength = 0.5;
	vec3 viewDir = normalize(vec3(viewPos[0], viewPos[1], viewPos[2] + 1.0) - FragPos);
	vec3 reflectDir = reflect(-lightDir, norm);  
	float spec = pow(max(dot(viewDir, reflectDir), 0.0), 2);
	vec3 specular = specularStrength * spec * lightColor;  
	
	vec3 result = (ambient + diffuse + specular) * objectColor;
    FragColor = vec4(result, 1.0);
}
