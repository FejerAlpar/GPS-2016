#version 410 core

in vec3 normal;
in vec4 fragPosEye;
in vec3 lightPosEye;
in vec2 TexCoords;

out vec4 fColor;

//lighting
uniform	mat3 normalMatrix;

uniform	vec3 lightDir;
uniform	vec3 lightColor;
uniform sampler2D diffuseTexture;
uniform sampler2D specularTexture;
vec3 viewDirN;

vec3 ambient;
vec3 diffuse;
vec3 specular;
float ambientStrength = 10.0f;
float specularStrength = 10.0f;
float shininess = 32.0f;
float constant = 1.0f;
float linear = 0.0045f;
float quadratic = 0.0075f;

void computeLightComponents()
{		
	vec3 cameraPosEye = vec3(0.0f);//in eye coordinates, the viewer is situated at the origin
	
	//transform normal
	vec3 normalEye = normalize(normalMatrix * normal);	
	
	//compute light direction
	//vec3 lightDirN = normalize(lightDir);
	vec3 lightDirN = normalize(lightPosEye - fragPosEye.xyz);
	
	//compute half vector
	vec3 halfVector = normalize(lightDirN + viewDirN);
	
	//compute view direction 
	/*vec3*/viewDirN = normalize(cameraPosEye - fragPosEye.xyz);
	
	//compute distance to light
	float dist = length(lightPosEye - fragPosEye.xyz);
	//compute attenuation
	float att = 1.0f / (constant + linear * dist + quadratic * (dist * dist));
		
	//compute ambient light
	ambient = att*ambientStrength * lightColor;
	
	//compute diffuse light
	diffuse = att*max(dot(normalEye, lightDirN), 0.0f) * lightColor;
	
	//compute specular light
	//vec3 reflection = reflect(-lightDirN, normalEye);
	float specCoeff = pow(max(dot(normalEye, halfVector), 0.0f), shininess);
	specular = att*specularStrength * specCoeff * lightColor;
}

void main() 
{
	computeLightComponents();
	
	vec3 baseColor = vec3(1.0f, 1.0f, 1.0f);//orange
	
	ambient *= baseColor;
	diffuse *= baseColor;
	specular *= baseColor;
	ambient *= texture(diffuseTexture, TexCoords).rgb;
	diffuse *= texture(diffuseTexture, TexCoords).rgb;
	specular *= texture(specularTexture, TexCoords).rgb;
	
	vec3 color = min((ambient + diffuse) + specular, 1.0f);
    
    fColor = vec4(color, 1.0f);
}
