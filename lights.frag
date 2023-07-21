#version 140

struct Material
{
	bool useTexture;
	float shininess;
	vec3 ambient;
	vec3 diffuse;
	vec3 specular;
};

struct Light
{
	vec3 ambient;
	vec3 diffuse;
	vec3 specular;
	vec3 position;
	vec3 spotDirection;
	float spotCosCutOff;
	float spotExponent;
};

uniform sampler2D textureSampler;
uniform Material material;
uniform float currentTime;
uniform bool fogOn;

uniform mat4 PVMmatrix;
uniform mat4 Vmatrix;
uniform mat4 Mmatrix;
uniform mat4 normalMatrix;

smooth in vec2 textureCoord_v;
smooth in vec3 normal_v;
smooth in vec3 position_v;
out vec4 color_f;

Light sunDirect;
float sunSpeed = 0.8;
uniform bool sunOn;

Light sparklesPoint;
uniform vec4 positionPointLight;
uniform bool pointLightOn;
uniform bool flashOn;

Light flashReflector;
uniform vec3 reflectorPosition;
uniform vec3 reflectorDirection;
float fogFactor;

vec4 directionalForSun(Light light, Material material, vec3 vertexPosition, vec3 vertexNormal, vec2 texCoords)
{
	vec3 lightDirection = normalize(light.position);
	vec3 viewDirection = normalize(-vertexPosition);
	vec3 reflectionDirection = reflect(-lightDirection, vertexNormal);
	
	float cosTheta = max(dot(vertexNormal, lightDirection), 0.0);
	float cosAlpha = max(dot(viewDirection, reflectionDirection), 0.0);
	
	vec3 texColor = texture(textureSampler, texCoords).rgb;
	
	vec3 ambientTerm = material.ambient * light.ambient;
	vec3 diffuseTerm = cosTheta * material.diffuse * light.diffuse;
	vec3 specularTerm = pow(cosAlpha, material.shininess) * material.specular * light.specular;
	
	vec3 color = texColor * (ambientTerm + diffuseTerm + specularTerm);
	
	return vec4(color, 1.0);
}

vec4 pointForSparkles(Light light, Material material, vec3 vertexPosition, vec3 vertexNormal, vec2 texCoords)
{
	float dist = length(light.position - vertexPosition.xyz);
	vec3 toLight = normalize(light.position - vertexPosition);
	vec3 R = reflect(-toLight, vertexNormal);
	vec3 N = normalize(toLight - vertexNormal);

	float normalLightAngleCosinus = dot(vertexNormal, toLight);
	float reflectionViewAngleCosinus = dot(R, N);
	
	vec3 textureColor = texture(textureSampler, texCoords).rgb;

	vec3 ambientTerm = material.ambient * light.ambient;
	vec3 diffuseTerm = max(normalLightAngleCosinus, 0.0) * light.diffuse * material.diffuse;
	vec3 specularTerm = pow(max(reflectionViewAngleCosinus, 0.0), material.shininess) * light.specular * material.specular;

	vec3 color = textureColor * (ambientTerm + diffuseTerm + specularTerm);
	color /= pow(dist, 2)*10;
	color *= material.shininess;
	
	return vec4(color, 1.0);
}

vec4 spotForFlashlight(Light light, Material material, vec3 vertexPosition, vec3 vertexNormal, vec2 texCoords)
{
	vec3 direction = normalize(light.position - vertexPosition);
	float diffuseCoef = max(0.0, dot(vertexNormal, direction));
	float specularCoef = max(0.0, dot(reflect(vertexNormal, -direction), normalize(-vertexPosition)));

	float angleCosine = dot(-direction, light.spotDirection);
	float spotCoef = 0.0;
	if (angleCosine >= light.spotCosCutOff)
	{
		spotCoef = pow(angleCosine, light.spotExponent);
	}

	vec3 textureColor = texture(textureSampler, texCoords).rgb;

	vec3 color = material.diffuse * light.diffuse * diffuseCoef * textureColor * material.shininess;
	color += material.specular * light.specular * pow(specularCoef, material.shininess) * textureColor;
	color += material.ambient * light.ambient * textureColor;

	color *= spotCoef;

	return vec4(color, 1.0);
}

void setupLights()
{
	float fogDist = abs(position_v.z);
	float fogDensity = 0.3;
	fogFactor = exp(-fogDensity * fogDist);

	sunDirect.ambient = vec3(0.5);
	sunDirect.diffuse = vec3(0.7);
	sunDirect.specular = vec3(0.6);
	float sunAngle = currentTime * sunSpeed;
	sunDirect.position = vec3(cos(sunAngle), 0.0, sin(sunAngle));

	if (pointLightOn) {
		sparklesPoint.ambient = vec3(1.0);
		sparklesPoint.diffuse = vec3(0.5);
		sparklesPoint.specular = vec3(0.1);
		sparklesPoint.position = (Vmatrix * positionPointLight).xyz;
	}

	mat4 MVmatrix = Vmatrix * Mmatrix;
	vec4 reflectorPosition4 = vec4(reflectorPosition, 1.0);
	vec4 worldSpacePosition = MVmatrix * reflectorPosition4;

	flashReflector.position = worldSpacePosition.xyz;
	flashReflector.ambient = vec3(0.1);
	flashReflector.diffuse = vec3(0.9);
	flashReflector.specular = vec3(1.0);
	flashReflector.spotCosCutOff = 0.95;
	flashReflector.spotExponent = 1.0;
	flashReflector.position = (Vmatrix * vec4(reflectorPosition, 1.0)).xyz;
	flashReflector.spotDirection = normalize((Vmatrix * vec4(reflectorDirection, 0.0))).xyz;

	float reflectorDistance = length(flashReflector.position);
	vec3 reflectorColor = mix(vec3(1.0), vec3(1.0, 0.5, 0.5), reflectorDistance / 10.0);
	flashReflector.diffuse *= reflectorColor;
}


void main()
{
	setupLights();

	vec3 normal = normalize(normal_v);
	vec3 globalAmbientLight = vec3(0.2);
	vec4 ambientColor = vec4(material.ambient * globalAmbientLight, 0.0);
	vec4 outputColor = ambientColor;

	if (sunOn)
	{
		vec4 directionalColor = directionalForSun(sunDirect, material, position_v, normal, textureCoord_v);
		outputColor.rgb += directionalColor.rgb;
	}

	if (material.useTexture)
	{
		vec4 textureColor = texture(textureSampler, textureCoord_v);
		outputColor.rgb *= textureColor.rgb;
	}

	if (fogOn)
	{
		vec3 fogColor = vec3(0.65);
		outputColor.rgb = mix(fogColor, outputColor.rgb, fogFactor);
	}

	if (flashOn)
	{
		vec4 spotColor = spotForFlashlight(flashReflector, material, position_v, normal, textureCoord_v);
		outputColor.rgb += spotColor.rgb;
	}

	if (pointLightOn)
	{
		vec4 pointColor = pointForSparkles(sparklesPoint, material, position_v, normal, textureCoord_v);
		outputColor.rgb += pointColor.rgb;
	}

	color_f = outputColor;
}
