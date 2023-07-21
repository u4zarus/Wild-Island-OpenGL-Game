//----------------------------------------------------------------------------------------
/**
 * @file    render.cpp : This file handles all the rendering and shader stuff.
 * @author  Stepan Manaenko
 * @date    2023
 * @brief   Initialize, draw and destroy objects and shaders.
 */
 //----------------------------------------------------------------------------------------

#include <iostream>
#include "render.h"
#include <iostream>
#include <fstream>
#include <iostream>
#include <vector>
#include "data.h"

using namespace manaeste;

bool useFog = true;

SingMeshGeom* amongusGeom = nullptr;  ///< moving texture object (banner) geometry
SingMeshGeom* sparklesGeom = nullptr; ///< spritesheet object (sparkles) geometry
SingMeshGeom* skyboxGeom = nullptr;   ///< skybox geometry
SingMeshGeom* terrainGeom = nullptr;  ///< terrain geometry
SingMeshGeom* raiderGeom = nullptr;   ///< raider (flying object) geometry
SingMeshGeom* palmGeom = nullptr;     ///< palm geometry
SingMeshGeom* duckGeom = nullptr;     ///< duck geometry
SingMeshGeom* diamondGeom = nullptr;  ///< diamond geometry
MultMeshGeom snowmanGeom;             ///< snowman geometry
MultMeshGeom couchGeom;               ///< couch geometry

const char* TERRAIN_MODEL = "data/ground/ground.obj";
const char* SNOWMAN_MODEL = "data/snehulak/snehulak.obj";
const char* RAIDER_MODEL = "data/raider/raider.obj";
const char* PALM_MODEL = "data/PalmTree/PalmTree.obj";
const char* COUCH_MODEL = "data/gauc/gauc.obj";
const char* DUCK_MODEL = "data/rubberduck/rubberduck.obj";

const char* SPARKLES_TEXTURE = "data/fire.png";
const char* AMONGUS_TEXTURE = "data/img.png";
const char* DIAMOND_TEXTURE = "data/diamond.png";
const char* SKYBOX_TEXTURE_PREFIX = "data/skybox1/skybox";

MainShaderProgram shaderProgram;
AmongusShaderProgram amongusShaderProgram;
SkyboxShaderProgram skyboxShaderProgram;
SparklesShaderProgram sparklesShaderProgram;

struct SingleMeshModelInfo
{
	const char* modelName;
	SingMeshGeom** geometryPtr;
};


/**
 * @brief Set PVM, V, M and normal matrices to shaders.
 * @param projMat projection matrix
 * @param viewMat view matrix
 * @param modelMat model matrix
*/
void manaeste::setUniformMatrices(const glm::mat4& projMat, const glm::mat4& viewMat, const glm::mat4& modelMat)
{
	GLint PVMLoc = glGetUniformLocation(shaderProgram.program, "PVMmatrix");
	GLint VmatLoc = glGetUniformLocation(shaderProgram.program, "Vmatrix");
	GLint MmatLoc = glGetUniformLocation(shaderProgram.program, "Mmatrix");
	GLint normalMatLoc = glGetUniformLocation(shaderProgram.program, "normalMatrix");
	GLint fogOnLoc = glGetUniformLocation(shaderProgram.program, "fogOn");
	CHECK_GL_ERROR();

	glm::mat4 PVM = projMat * viewMat * modelMat;
	glUniformMatrix4fv(PVMLoc, 1, GL_FALSE, glm::value_ptr(PVM));

	glUniformMatrix4fv(VmatLoc, 1, GL_FALSE, glm::value_ptr(viewMat));
	glUniformMatrix4fv(MmatLoc, 1, GL_FALSE, glm::value_ptr(modelMat));
	glm::mat4 normalMatrix = glm::transpose(glm::inverse(modelMat));
	glUniformMatrix4fv(normalMatLoc, 1, GL_FALSE, glm::value_ptr(normalMatrix));
	glUniform1i(fogOnLoc, useFog);
}

/**
 * @brief Sets material uniform values to shaders.
 * @param texture texture GLuint
 * @param shininess shine factor
 * @param ambient ambient part of material
 * @param diffuse diffuse part of material
 * @param specular specular part of material
*/
void manaeste::setUniformMaterial(GLuint texture, float shininess, const glm::vec3& ambient, const glm::vec3& diffuse,
	const glm::vec3& specular)
{
	auto diffuse_ptr = glm::value_ptr(diffuse);
	auto ambient_ptr = glm::value_ptr(ambient);
	auto specular_ptr = glm::value_ptr(specular);

	glUniform3fv(shaderProgram.diffuseLoc, 1, diffuse_ptr);
	glUniform3fv(shaderProgram.ambientLoc, 1, ambient_ptr);
	glUniform3fv(shaderProgram.specularLoc, 1, specular_ptr);
	glUniform1f(shaderProgram.shininessLoc, shininess);

	bool hasTexture = (texture != 0);
	glUniform1i(shaderProgram.useTextureLoc, hasTexture);

	if (hasTexture)
	{
		glUniform1i(shaderProgram.textureSamplerLoc, 0);
		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, texture);
	}
}

/**
 * @brief Creates shader programs and gets locations of shader variables.
*/
void manaeste::createShaders() {
	auto createProgram = [](const char* vert, const char* frag)
	{
		std::vector<GLuint> shaderList{
				pgr::createShaderFromFile(GL_VERTEX_SHADER, vert),
				pgr::createShaderFromFile(GL_FRAGMENT_SHADER, frag)
		};
		return pgr::createProgram(shaderList);
	};

	shaderProgram.program = createProgram("lights.vert", "lights.frag");
	shaderProgram.positionLoc = glGetAttribLocation(shaderProgram.program, "position");
	shaderProgram.normalLoc = glGetAttribLocation(shaderProgram.program, "normal");
	shaderProgram.textureCoordLoc = glGetAttribLocation(shaderProgram.program, "textureCoord");
	shaderProgram.PVMmatrixLoc = glGetUniformLocation(shaderProgram.program, "PVMmatrix");
	shaderProgram.VmatrixLoc = glGetUniformLocation(shaderProgram.program, "Vmatrix");
	shaderProgram.MmatrixLoc = glGetUniformLocation(shaderProgram.program, "Mmatrix");
	shaderProgram.normalMatrixLoc = glGetUniformLocation(shaderProgram.program, "normalMatrix");
	shaderProgram.timeLoc = glGetUniformLocation(shaderProgram.program, "currentTime");
	shaderProgram.ambientLoc = glGetUniformLocation(shaderProgram.program, "material.ambient");
	shaderProgram.diffuseLoc = glGetUniformLocation(shaderProgram.program, "material.diffuse");
	shaderProgram.specularLoc = glGetUniformLocation(shaderProgram.program, "material.specular");
	shaderProgram.shininessLoc = glGetUniformLocation(shaderProgram.program, "material.shininess");
	shaderProgram.textureSamplerLoc = glGetUniformLocation(shaderProgram.program, "textureSampler");
	shaderProgram.useTextureLoc = glGetUniformLocation(shaderProgram.program, "material.useTexture");
	shaderProgram.reflectorPositionLoc = glGetUniformLocation(shaderProgram.program, "reflectorPosition");
	shaderProgram.reflectorDirectionLoc = glGetUniformLocation(shaderProgram.program, "reflectorDirection");
	shaderProgram.sunOnLoc = glGetUniformLocation(shaderProgram.program, "sunOn");
	shaderProgram.flashOnLoc = glGetUniformLocation(shaderProgram.program, "flashOn");
	shaderProgram.pointLightLoc = glGetUniformLocation(shaderProgram.program, "positionPointLight");
	shaderProgram.pointLightOnLoc = glGetUniformLocation(shaderProgram.program, "pointLightOn");
	shaderProgram.fogOnLoc = glGetUniformLocation(shaderProgram.program, "fogOn");

	sparklesShaderProgram.program = createProgram("sparkles.vert", "sparkles.frag");
	sparklesShaderProgram.positionLoc = glGetAttribLocation(sparklesShaderProgram.program, "position");
	sparklesShaderProgram.textureCoordLoc = glGetAttribLocation(sparklesShaderProgram.program, "textureCoord");
	sparklesShaderProgram.PVMmatrixLoc = glGetUniformLocation(sparklesShaderProgram.program, "PVMmatrix");
	sparklesShaderProgram.VmatrixLoc = glGetUniformLocation(sparklesShaderProgram.program, "Vmatrix");
	sparklesShaderProgram.timeLoc = glGetUniformLocation(sparklesShaderProgram.program, "currentTime");
	sparklesShaderProgram.textureSamplerLoc = glGetUniformLocation(sparklesShaderProgram.program, "textureSampler");
	sparklesShaderProgram.frameDurationLoc = glGetUniformLocation(sparklesShaderProgram.program, "frameDuration");

	amongusShaderProgram.program = createProgram("amongusMovingTexture.vert", "amongusMovingTexture.frag");
	amongusShaderProgram.positionLoc = glGetAttribLocation(amongusShaderProgram.program, "position");
	amongusShaderProgram.textureCoordLoc = glGetAttribLocation(amongusShaderProgram.program, "textureCoord");
	amongusShaderProgram.PVMmatrixLoc = glGetUniformLocation(amongusShaderProgram.program, "PVMmatrix");
	amongusShaderProgram.currentTimeLoc = glGetUniformLocation(amongusShaderProgram.program, "currentTime");
	amongusShaderProgram.textureSamplerLoc = glGetUniformLocation(amongusShaderProgram.program, "textureSampler");

	skyboxShaderProgram.program = createProgram("cubeSkybox.vert", "cubeSkybox.frag");
	skyboxShaderProgram.screenCoordLoc = glGetAttribLocation(skyboxShaderProgram.program, "screenCoord");
	skyboxShaderProgram.skyboxSamplerLoc = glGetUniformLocation(skyboxShaderProgram.program, "skyboxSampler");
	skyboxShaderProgram.inversePVmatrixLoc = glGetUniformLocation(skyboxShaderProgram.program, "inversePVmatrix");
}

/**
 * @brief Deletes shader programs.
*/
void manaeste::deleteShaders()
{
	pgr::deleteProgramAndShaders(shaderProgram.program);
	pgr::deleteProgramAndShaders(skyboxShaderProgram.program);
	pgr::deleteProgramAndShaders(sparklesShaderProgram.program);
	pgr::deleteProgramAndShaders(amongusShaderProgram.program);
}

/**
 * @brief Initialize geometry for diamond.
 * @param geom pointer to the geometry
 * @param shader shader program
*/
void manaeste::initDiamondGeom(SingMeshGeom** geom)
{
	*geom = new SingMeshGeom();

	glGenVertexArrays(1, &(*geom)->vao);
	glBindVertexArray((*geom)->vao);

	GLuint vbo;
	glGenBuffers(1, &vbo);
	glBindBuffer(GL_ARRAY_BUFFER, vbo);
	glBufferData(GL_ARRAY_BUFFER, sizeof(diamondVerteces), diamondVerteces, GL_STATIC_DRAW);

	glEnableVertexAttribArray(shaderProgram.positionLoc);
	glVertexAttribPointer(shaderProgram.positionLoc, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), 0);

	glEnableVertexAttribArray(shaderProgram.textureCoordLoc);
	glVertexAttribPointer(shaderProgram.textureCoordLoc, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)(3 * sizeof(float)));

	glEnableVertexAttribArray(shaderProgram.normalLoc);
	glVertexAttribPointer(shaderProgram.normalLoc, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)(6 * sizeof(float)));

	GLuint ebo;
	glGenBuffers(1, &ebo);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ebo);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(diamondIndices), diamondIndices, GL_STATIC_DRAW);

	glBindVertexArray(0);

	(*geom)->vbo = vbo;
	(*geom)->ebo = ebo;
	(*geom)->numTriangles = diamondNumTriangles;
	(*geom)->texture = pgr::createTexture(DIAMOND_TEXTURE);
}

/**
 * @brief Initialize skybox geometry.
 * @param geom pointer to the geometry
*/
void manaeste::initCubeSkyboxGeom(SingMeshGeom** geom)
{
	*geom = new SingMeshGeom;

	static const float screenCoords[] = {
			-1.0f, -1.0f,
			 1.0f, -1.0f,
			-1.0f,  1.0f,
			 1.0f,  1.0f
	};

	GLuint vao, vbo;
	glGenVertexArrays(1, &vao);
	glBindVertexArray(vao);
	glGenBuffers(1, &vbo);
	glBindBuffer(GL_ARRAY_BUFFER, vbo);
	glBufferData(GL_ARRAY_BUFFER, sizeof(screenCoords), screenCoords, GL_STATIC_DRAW);

	glEnableVertexAttribArray(skyboxShaderProgram.screenCoordLoc);
	glVertexAttribPointer(skyboxShaderProgram.screenCoordLoc, 2, GL_FLOAT, GL_FALSE, 0, 0);

	glBindVertexArray(0);
	glBindBuffer(GL_ARRAY_BUFFER, 0);

	(*geom)->vao = vao;
	(*geom)->vbo = vbo;
	(*geom)->numTriangles = 2;

	const std::vector<std::string> suffixes = { "posx", "negx", "posy", "negy", "posz", "negz" };
	const std::vector<GLenum> targets = {
			GL_TEXTURE_CUBE_MAP_POSITIVE_X, GL_TEXTURE_CUBE_MAP_NEGATIVE_X,
			GL_TEXTURE_CUBE_MAP_POSITIVE_Y, GL_TEXTURE_CUBE_MAP_NEGATIVE_Y,
			GL_TEXTURE_CUBE_MAP_POSITIVE_Z, GL_TEXTURE_CUBE_MAP_NEGATIVE_Z
	};
	for (const auto& suffix : suffixes)
	{
		std::string texName = std::string(SKYBOX_TEXTURE_PREFIX) + "_" + suffix + ".jpg";

		auto target = targets[&suffix - &suffixes[0]];
		if (!pgr::loadTexImage2D(texName, target))
		{
			pgr::dieWithError("ERROR: Skybox textures loading failed");
		}
	}

	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_CUBE_MAP, (*geom)->texture);
	glTexParameterf(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameterf(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
	glTexParameterf(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameterf(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	glTexParameterf(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);
	glGenerateMipmap(GL_TEXTURE_CUBE_MAP);
	glBindTexture(GL_TEXTURE_CUBE_MAP, 0);
}

/**
 * @brief Initialize geometry for the sparkles.
*/
void manaeste::initSparklesGeom(SingMeshGeom** geom)
{
	*geom = new SingMeshGeom;

	GLuint tempVao, tempVbo;
	glGenVertexArrays(1, &tempVao);
	glGenBuffers(1, &tempVbo);

	glBindVertexArray(tempVao);
	glBindBuffer(GL_ARRAY_BUFFER, tempVbo);
	glBufferData(GL_ARRAY_BUFFER, sizeof(sparklesVertices), sparklesVertices, GL_STATIC_DRAW);

	GLint positionLoc = sparklesShaderProgram.positionLoc;
	glEnableVertexAttribArray(positionLoc);
	glVertexAttribPointer(positionLoc, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), 0);

	GLint textureCoordLoc = sparklesShaderProgram.textureCoordLoc;
	glEnableVertexAttribArray(textureCoordLoc);
	glVertexAttribPointer(textureCoordLoc, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)(3 * sizeof(float)));

	glBindVertexArray(0);
	(*geom)->vao = tempVao;
	(*geom)->vbo = tempVbo;
	(*geom)->numTriangles = sparklesNumVertices;
	(*geom)->texture = pgr::createTexture(SPARKLES_TEXTURE);
}

/**
 * @brief Initialize geometry for the moving texture.
 * @param geom pointer to the geometry
*/
void manaeste::initAmongusGeom(SingMeshGeom** geom)
{
	*geom = new SingMeshGeom;

	GLuint vao, vbo;
	glGenVertexArrays(1, &vao);
	glGenBuffers(1, &vbo);

	glBindVertexArray(vao);
	glBindBuffer(GL_ARRAY_BUFFER, vbo);
	glBufferData(GL_ARRAY_BUFFER, sizeof(amongusVertices), amongusVertices, GL_STATIC_DRAW);

	GLint positionLoc = amongusShaderProgram.positionLoc;
	glEnableVertexAttribArray(positionLoc);
	glVertexAttribPointer(positionLoc, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), 0);

	GLint textureCoordLoc = amongusShaderProgram.textureCoordLoc;
	glEnableVertexAttribArray(textureCoordLoc);
	glVertexAttribPointer(textureCoordLoc, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)(3 * sizeof(float)));

	glBindVertexArray(0);
	(*geom)->vao = vao;
	(*geom)->vbo = vbo;
	(*geom)->numTriangles = amongusNumVertices;
	(*geom)->texture = pgr::createTexture(AMONGUS_TEXTURE);
}

/**
 * @brief Deletes all vertex buffers and vertex arrays for all objects.
*/
void manaeste::deleteAmongusAndSkyboxGeoms()
{
	std::vector<std::reference_wrapper<SingMeshGeom>> geometries = { *amongusGeom, *skyboxGeom };

	for (auto& geometry : geometries)
	{
		glDeleteVertexArrays(1, &geometry.get().vao);
		glDeleteBuffers(1, &geometry.get().ebo);
		glDeleteBuffers(1, &geometry.get().vbo);

		if (geometry.get().texture != 0)
			glDeleteTextures(1, &geometry.get().texture);
	}
}

/**
 * @brief Univeral function for drawing objects besides sparkles, amongus and skybox.
 * @param type object type
 * @param object pointer to object
 * @param projMat projection matrix
 * @param viewMat view matrix
*/
void manaeste::drawObject(ObjectType type, Object* object, const glm::mat4& projMat, const glm::mat4& viewMat)
{
	glUseProgram(shaderProgram.program);

	glm::mat4 modelMat = setModelMat(type, object);

	setUniformMatrices(projMat, viewMat, modelMat);
	glUniform1i(shaderProgram.useTextureLoc, true);

	setMaterial(type, projMat, viewMat, modelMat);
	glBindVertexArray(0);
	glUseProgram(0);
}

/**
 * @brief Draw the skybox.
 * @param projMat projection matrix
 * @param viewMat view matrix
*/
void manaeste::drawCubeSkybox(const glm::mat4& projMat, const glm::mat4& viewMat)
{
	glUseProgram(skyboxShaderProgram.program);

	glm::mat4 mat = projMat * viewMat;
	glm::mat4 viewRotation = viewMat;
	viewRotation[3] = glm::vec4(0.0f, 0.0f, 0.0f, 1.0f);
	glm::mat4 invViewRotMatrix = glm::inverse(projMat * viewRotation);

	glUniformMatrix4fv(skyboxShaderProgram.inversePVmatrixLoc, 1, GL_FALSE, glm::value_ptr(invViewRotMatrix));
	glUniform1i(skyboxShaderProgram.skyboxSamplerLoc, 0);

	glBindVertexArray(skyboxGeom->vao);
	glBindTexture(GL_TEXTURE_CUBE_MAP, skyboxGeom->texture);

	glDrawArrays(GL_TRIANGLE_STRIP, 0, skyboxGeom->numTriangles + 2);

	glBindVertexArray(0);
	glBindTexture(GL_TEXTURE_CUBE_MAP, 0);

	glUseProgram(0);
}

/**
 * @brief Draw a sparkles texture object.
 * @param sparkles object to draw
 * @param projMat projection matrix
 * @param viewMat view matrix
*/
void manaeste::drawSparklesTexture(Object* sparkles, const glm::mat4& projMat, const glm::mat4& viewMat)
{
	glEnable(GL_BLEND);
	glBlendFunc(GL_ONE, GL_ONE);

	glUseProgram(sparklesShaderProgram.program);
	glUniform1i(sparklesShaderProgram.textureSamplerLoc, 0);
	glUniform1f(sparklesShaderProgram.frameDurationLoc, sparkles->frameDuration);

	glm::mat4 rotMat = glm::transpose(glm::mat4(viewMat[0], viewMat[1], viewMat[2], glm::vec4(0.0f, 0.0f, 0.0f, 1.0f)));
	glm::mat4 scaleMat = glm::scale(glm::mat4(1.0f), glm::vec3(sparkles->size));
	glm::mat4 transMat = glm::translate(glm::mat4(1.0f), sparkles->position);
	glm::mat4 modelMat = transMat * rotMat * scaleMat;
	glm::mat4 PVM = projMat * viewMat * modelMat;

	glUniformMatrix4fv(sparklesShaderProgram.PVMmatrixLoc, 1, GL_FALSE, glm::value_ptr(PVM));
	glUniformMatrix4fv(sparklesShaderProgram.VmatrixLoc, 1, GL_FALSE, glm::value_ptr(viewMat));
	glUniform1f(sparklesShaderProgram.timeLoc, sparkles->currentTime - sparkles->startTime);

	glBindVertexArray(sparklesGeom->vao);
	glBindTexture(GL_TEXTURE_2D, sparklesGeom->texture);
	glDrawArrays(GL_TRIANGLE_STRIP, 0, sparklesGeom->numTriangles);

	glBindVertexArray(0);
	glUseProgram(0);
	glDisable(GL_BLEND);
}

/**
 * @brief Draw a moving texture.
 * @param amongus object to draw
 * @param projMat projection matrix
 * @param viewMat view matrix
*/
void manaeste::drawAmongusMovingTexture(Object* amongus, const glm::mat4& projMat, const glm::mat4& viewMat)
{
	static bool blendEnabled = false;
	static bool depthTestEnabled = true;
	static GLuint shaderProgramId = 0;
	static GLint pvmMatrixLoc = -1;
	static GLint timeLoc = -1;
	static GLint texSamplerLoc = -1;
	static GLuint textureId = 0;
	static GLuint vertexArrayObject = 0;
	static GLsizei numTriangles = 0;

	if (!shaderProgramId)
	{
		shaderProgramId = amongusShaderProgram.program;
		pvmMatrixLoc = amongusShaderProgram.PVMmatrixLoc;
		timeLoc = amongusShaderProgram.currentTimeLoc;
		texSamplerLoc = amongusShaderProgram.textureSamplerLoc;
		textureId = amongusGeom->texture;
		vertexArrayObject = amongusGeom->vao;
		numTriangles = amongusGeom->numTriangles;
	}

	if (!blendEnabled)
	{
		glEnable(GL_BLEND);
		glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
		blendEnabled = true;
	}

	if (depthTestEnabled)
	{
		glDisable(GL_DEPTH_TEST);
		depthTestEnabled = false;
	}

	glUseProgram(shaderProgramId);

	glm::mat4 modelMat = glm::translate(glm::mat4(1.0f), amongus->position);
	modelMat = glm::scale(modelMat, glm::vec3(amongus->size));

	glm::mat4 PVM = projMat * viewMat * modelMat;
	glUniformMatrix4fv(pvmMatrixLoc, 1, GL_FALSE, glm::value_ptr(PVM));
	glUniform1f(timeLoc, amongus->currentTime - amongus->startTime);
	glUniform1i(texSamplerLoc, 0);

	glBindTexture(GL_TEXTURE_2D, textureId);
	glBindVertexArray(vertexArrayObject);
	glDrawArrays(GL_TRIANGLE_STRIP, 0, numTriangles);

	glBindVertexArray(0);
	glUseProgram(0);

	if (!depthTestEnabled)
	{
		glEnable(GL_DEPTH_TEST);
		depthTestEnabled = true;
	}

	if (blendEnabled)
	{
		glDisable(GL_BLEND);
		blendEnabled = false;
	}
}

/**
 * @brief Toggle fog on/off.
*/
void manaeste::setFogState(bool fogOn)
{
	if (fogOn)
		useFog = true;
	else
		useFog = false;
}

/**
 * @brief Sets model matrix for object based on its type
 * @param type object type
 * @param object pointer to object
 * @return model matrix
*/
glm::mat4 manaeste::setModelMat(const ObjectType& type, const Object* object)
{
	glm::mat4 modelMat;
	switch (type)
	{
	case TERRAIN_ELEMENT:
		modelMat = glm::scale(glm::translate(glm::mat4(1.0f), object->position), glm::vec3(object->size, object->size, 0.2f));
		break;
	case RAIDER:
		modelMat = glm::scale(getFrontDirectionMat(object->position, object->direction, glm::vec3(0.0f, 0.0f, 1.0f)), glm::vec3(object->size));
		break;
	case DUCK:
		modelMat = glm::rotate(glm::scale(glm::translate(glm::mat4(1.0f), object->position), glm::vec3(object->size)), glm::half_pi<float>(), glm::vec3(1, 0, 0));
		break;
	case PALM:
		modelMat = glm::scale(glm::translate(glm::mat4(1.0f), object->position), glm::vec3(object->size));
		break;
	case SNOWMAN:
		modelMat = glm::rotate(glm::scale(glm::translate(glm::mat4(1.0f), object->position), glm::vec3(object->size)), glm::radians(90.0f), glm::vec3(1, 0, 0));
		break;
	case COUCH:
		modelMat = glm::rotate(glm::scale(glm::translate(glm::mat4(1.0f), object->position), glm::vec3(object->size, object->size, object->size)), glm::radians(90.0f), glm::vec3(1, 0, 0));
		break;
	case DIAMOND:
		modelMat = glm::scale(glm::translate(glm::mat4(1.0f), object->position), glm::vec3(object->size, object->size, object->size));
		break;
	default:
		break;
	}
	return modelMat;
}

/**
 * @brief Sets model matrix for object based on its type
 * @param type object type
 * @param projMat projection matrix
 * @param viewMat view matrix
 * @param modelMat model matrix
*/
void manaeste::setMaterial(const ObjectType& type, const glm::mat4& projMat, const glm::mat4& viewMat,
	const glm::mat4& modelMat)
{
	switch (type)
	{
	case TERRAIN_ELEMENT:
		setUniformMaterial(terrainGeom->texture, 3.0f, terrainGeom->ambient, terrainGeom->diffuse, terrainGeom->specular);
		glBindVertexArray(terrainGeom->vao);
		glDrawElements(GL_TRIANGLES, terrainGeom->numTriangles * 3, GL_UNSIGNED_INT, 0);
		break;
	case RAIDER:
		setUniformMaterial(raiderGeom->texture, raiderGeom->shininess, raiderGeom->ambient, raiderGeom->diffuse, raiderGeom->specular);
		glBindVertexArray(raiderGeom->vao);
		glDrawElements(GL_TRIANGLES, raiderGeom->numTriangles * 3, GL_UNSIGNED_INT, 0);
		break;
	case DUCK:
		setUniformMaterial(duckGeom->texture, 3.0f, duckGeom->ambient, duckGeom->diffuse, duckGeom->specular);
		glBindVertexArray(duckGeom->vao);
		glDrawElements(GL_TRIANGLES, duckGeom->numTriangles * 3, GL_UNSIGNED_INT, 0);
		break;
	case PALM:
		setUniformMaterial(palmGeom->texture, palmGeom->shininess, palmGeom->ambient, palmGeom->diffuse, palmGeom->specular);
		glBindVertexArray(palmGeom->vao);
		glDrawElements(GL_TRIANGLES, palmGeom->numTriangles * 3, GL_UNSIGNED_INT, 0);
		break;
	case SNOWMAN:
		for (auto& snowGeomEl : snowmanGeom)
		{
			setUniformMaterial(snowGeomEl->texture, 2.0f, snowGeomEl->ambient, snowGeomEl->diffuse, snowGeomEl->specular);
			glBindVertexArray(snowGeomEl->vao);
			glDrawElements(GL_TRIANGLES, snowGeomEl->numTriangles * 3, GL_UNSIGNED_INT, 0);
		}
		break;
	case COUCH:
		setUniformMatrices(projMat, viewMat, modelMat);
		for (auto& couchGeomEl : couchGeom)
		{
			setUniformMaterial(couchGeomEl->texture, 2.0f, couchGeomEl->ambient, couchGeomEl->diffuse, couchGeomEl->specular);
			glBindVertexArray(couchGeomEl->vao);
			glDrawElements(GL_TRIANGLES, couchGeomEl->numTriangles * 3, GL_UNSIGNED_INT, 0);
		}
		break;
	case DIAMOND:
		setUniformMaterial(diamondGeom->texture, 3.0f, glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.7f, 0.7f, 0.7f), glm::vec3(1.0f, 1.0f, 1.0f));
		glBindVertexArray(diamondGeom->vao);
		glDrawElements(GL_TRIANGLES, diamondGeom->numTriangles * 3, GL_UNSIGNED_INT, 0);
		break;
	}
}

/**
 * @brief Load mesh using assimp library.
 * @param fileName file to open/load
 * @param shader vao will connect loaded data to shader
 * @param singMeshGeometry single mesh geometry
 * @return true if loading was successful
*/
bool manaeste::loadSingMesh(const std::string& fileName, MainShaderProgram& shader, SingMeshGeom** singMeshGeometry)
{
	Assimp::Importer importer;

	importer.SetPropertyInteger(AI_CONFIG_PP_PTV_NORMALIZE, 1);
	const aiScene* scn = importer.ReadFile(fileName.c_str(), 0
		| aiProcess_Triangulate
		| aiProcess_PreTransformVertices
		| aiProcess_GenSmoothNormals
		| aiProcess_JoinIdenticalVertices);

	if (scn == NULL)
	{
		std::cerr << "loadSingMesh(): assimp error - " << importer.GetErrorString() << std::endl;
		*singMeshGeometry = nullptr;
		return false;
	}

	if (scn->mNumMeshes != 1)
	{
		std::cerr << "loadSingMesh(): this simplified loader can only process files with only one mesh" << std::endl;
		*singMeshGeometry = nullptr;
		return false;
	}

	const aiMesh* mesh = scn->mMeshes[0];

	*singMeshGeometry = new SingMeshGeom;

	glGenBuffers(1, &((*singMeshGeometry)->vbo));
	glBindBuffer(GL_ARRAY_BUFFER, (*singMeshGeometry)->vbo);
	glBufferData(GL_ARRAY_BUFFER, 8 * sizeof(float) * mesh->mNumVertices, 0, GL_STATIC_DRAW); // allocate memory for vertices, normals, and texture coordinates
	glBufferSubData(GL_ARRAY_BUFFER, 0, 3 * sizeof(float) * mesh->mNumVertices, mesh->mVertices);
	glBufferSubData(GL_ARRAY_BUFFER, 3 * sizeof(float) * mesh->mNumVertices, 3 * sizeof(float) * mesh->mNumVertices, mesh->mNormals);

	auto* textureCoords = new float[2 * mesh->mNumVertices];
	float* currentTextureCoord = textureCoords;

	aiVector3D vect;

	if (mesh->HasTextureCoords(0))
	{
		for (unsigned int idx = 0; idx < mesh->mNumVertices; idx++)
		{
			vect = (mesh->mTextureCoords[0])[idx];
			*currentTextureCoord++ = vect.x;
			*currentTextureCoord++ = vect.y;
		}
	}

	glBufferSubData(GL_ARRAY_BUFFER, 6 * sizeof(float) * mesh->mNumVertices, 2 * sizeof(float) * mesh->mNumVertices, textureCoords);

	auto* indices = new unsigned int[mesh->mNumFaces * 3];
	for (unsigned int f = 0; f < mesh->mNumFaces; ++f)
	{
		indices[f * 3 + 0] = mesh->mFaces[f].mIndices[0];
		indices[f * 3 + 1] = mesh->mFaces[f].mIndices[1];
		indices[f * 3 + 2] = mesh->mFaces[f].mIndices[2];
	}

	glGenBuffers(1, &((*singMeshGeometry)->ebo));
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, (*singMeshGeometry)->ebo);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, 3 * sizeof(unsigned) * mesh->mNumFaces, indices, GL_STATIC_DRAW);

	delete[] indices;

	const aiMaterial* mat = scn->mMaterials[mesh->mMaterialIndex];
	aiColor4D color;
	aiString name;
	aiReturn retValue = AI_SUCCESS;

	mat->Get(AI_MATKEY_NAME, name);

	if ((retValue = aiGetMaterialColor(mat, AI_MATKEY_COLOR_DIFFUSE, &color)) != AI_SUCCESS)
		color = aiColor4D(0.0f, 0.0f, 0.0f, 0.0f);

	(*singMeshGeometry)->diffuse = glm::vec3(color.r, color.g, color.b);

	if ((retValue = aiGetMaterialColor(mat, AI_MATKEY_COLOR_AMBIENT, &color)) != AI_SUCCESS)
		color = aiColor4D(0.0f, 0.0f, 0.0f, 0.0f);
	(*singMeshGeometry)->ambient = glm::vec3(color.r, color.g, color.b);

	if ((retValue = aiGetMaterialColor(mat, AI_MATKEY_COLOR_SPECULAR, &color)) != AI_SUCCESS)
		color = aiColor4D(0.0f, 0.0f, 0.0f, 0.0f);
	(*singMeshGeometry)->specular = glm::vec3(color.r, color.g, color.b);

	ai_real shininess, strength;
	unsigned int max;

	max = 1;
	if ((retValue = aiGetMaterialFloatArray(mat, AI_MATKEY_SHININESS, &shininess, &max)) != AI_SUCCESS)
		shininess = 1.0f;
	max = 1;
	if ((retValue = aiGetMaterialFloatArray(mat, AI_MATKEY_SHININESS_STRENGTH, &strength, &max)) != AI_SUCCESS)
		strength = 1.0f;
	(*singMeshGeometry)->shininess = shininess * strength;

	(*singMeshGeometry)->texture = 0;

	if (mat->GetTextureCount(aiTextureType_DIFFUSE) > 0)
	{
		aiString path;

		aiReturn texFound = mat->GetTexture(aiTextureType_DIFFUSE, 0, &path);
		std::string textureName = path.data;

		size_t found = fileName.find_last_of("/\\");
		if (found != std::string::npos)
		{
			textureName.insert(0, fileName.substr(0, found + 1));
		}

		std::cout << "Loading texture file: " << textureName << std::endl;
		(*singMeshGeometry)->texture = pgr::createTexture(textureName);
	}
	CHECK_GL_ERROR();

	glGenVertexArrays(1, &((*singMeshGeometry)->vao));
	glBindVertexArray((*singMeshGeometry)->vao);

	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, (*singMeshGeometry)->ebo); // bind our element array buffer (indices) to vao
	glBindBuffer(GL_ARRAY_BUFFER, (*singMeshGeometry)->vbo);

	glEnableVertexAttribArray(shader.positionLoc);
	glVertexAttribPointer(shader.positionLoc, 3, GL_FLOAT, GL_FALSE, 0, 0);

	glEnableVertexAttribArray(shader.normalLoc);
	glVertexAttribPointer(shader.normalLoc, 3, GL_FLOAT, GL_FALSE, 0, (void*)(3 * sizeof(float) * mesh->mNumVertices));

	glEnableVertexAttribArray(shader.textureCoordLoc);
	glVertexAttribPointer(shader.textureCoordLoc, 2, GL_FLOAT, GL_FALSE, 0, (void*)(6 * sizeof(float) * mesh->mNumVertices));
	CHECK_GL_ERROR();

	glBindVertexArray(0);

	(*singMeshGeometry)->numTriangles = mesh->mNumFaces;

	return true;
}

/**
 * @brief Load multiple meshes using assimp library.
 * @param fileName file to open/load
 * @param shader vao will connect loaded data to shader
 * @param multMeshGeometry geometry
 * @return true if loading was successful
*/
bool manaeste::loadMultMesh(const std::string& fileName, MainShaderProgram& shader, MultMeshGeom& multMeshGeometry)
{
	Assimp::Importer importer;

	importer.SetPropertyInteger(AI_CONFIG_PP_PTV_NORMALIZE, 1);
	const aiScene* scn = importer.ReadFile(fileName.c_str(), 0
		| aiProcess_Triangulate
		| aiProcess_PreTransformVertices
		| aiProcess_GenSmoothNormals
		| aiProcess_JoinIdenticalVertices);
	if (scn == NULL)
	{
		std::cerr << "loadMultMesh(): assimp error: " << importer.GetErrorString() << std::endl;
		return false;
	}

	for (size_t i = 0; i < scn->mNumMeshes; i++)
	{
		const aiMesh* mesh = scn->mMeshes[i];

		auto* geometry = new SingMeshGeom;

		glGenBuffers(1, &((geometry)->vbo));
		glBindBuffer(GL_ARRAY_BUFFER, (geometry)->vbo);
		glBufferData(GL_ARRAY_BUFFER, 8 * sizeof(float) * mesh->mNumVertices, 0, GL_STATIC_DRAW); // allocate memory for vertices, normals, and texture coordinates
		glBufferSubData(GL_ARRAY_BUFFER, 0, 3 * sizeof(float) * mesh->mNumVertices, mesh->mVertices);
		glBufferSubData(GL_ARRAY_BUFFER, 3 * sizeof(float) * mesh->mNumVertices, 3 * sizeof(float) * mesh->mNumVertices, mesh->mNormals);

		auto* textureCoords = new float[2 * mesh->mNumVertices];  // 2 floats per vertex
		float* currentTextureCoord = textureCoords;

		aiVector3D vect;

		if (mesh->HasTextureCoords(0))
		{
			for (unsigned int idx = 0; idx < mesh->mNumVertices; idx++)
			{
				vect = (mesh->mTextureCoords[0])[idx];
				*currentTextureCoord++ = vect.x;
				*currentTextureCoord++ = vect.y;
			}
		}

		glBufferSubData(GL_ARRAY_BUFFER, 6 * sizeof(float) * mesh->mNumVertices, 2 * sizeof(float) * mesh->mNumVertices, textureCoords);

		auto* indices = new unsigned int[mesh->mNumFaces * 3];
		for (unsigned int f = 0; f < mesh->mNumFaces; ++f)
		{
			indices[f * 3 + 0] = mesh->mFaces[f].mIndices[0];
			indices[f * 3 + 1] = mesh->mFaces[f].mIndices[1];
			indices[f * 3 + 2] = mesh->mFaces[f].mIndices[2];
		}

		glGenBuffers(1, &((geometry)->ebo));
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, (geometry)->ebo);
		glBufferData(GL_ELEMENT_ARRAY_BUFFER, 3 * sizeof(unsigned int) * mesh->mNumFaces, indices, GL_STATIC_DRAW);

		delete[] indices;

		const aiMaterial* mat = scn->mMaterials[mesh->mMaterialIndex];
		aiColor4D color;
		aiString name;
		aiReturn retValue = AI_SUCCESS;

		mat->Get(AI_MATKEY_NAME, name);

		if ((retValue = aiGetMaterialColor(mat, AI_MATKEY_COLOR_DIFFUSE, &color)) != AI_SUCCESS)
			color = aiColor4D(0.0f, 0.0f, 0.0f, 0.0f);

		(geometry)->diffuse = glm::vec3(color.r, color.g, color.b);

		if ((retValue = aiGetMaterialColor(mat, AI_MATKEY_COLOR_AMBIENT, &color)) != AI_SUCCESS)
			color = aiColor4D(0.0f, 0.0f, 0.0f, 0.0f);
		(geometry)->ambient = glm::vec3(color.r, color.g, color.b);

		if ((retValue = aiGetMaterialColor(mat, AI_MATKEY_COLOR_SPECULAR, &color)) != AI_SUCCESS)
			color = aiColor4D(0.0f, 0.0f, 0.0f, 0.0f);
		(geometry)->specular = glm::vec3(color.r, color.g, color.b);

		ai_real shininess, strength;
		unsigned int max;

		max = 1;
		if ((retValue = aiGetMaterialFloatArray(mat, AI_MATKEY_SHININESS, &shininess, &max)) != AI_SUCCESS)
			shininess = 1.0f;
		max = 1;
		if ((retValue = aiGetMaterialFloatArray(mat, AI_MATKEY_SHININESS_STRENGTH, &strength, &max)) != AI_SUCCESS)
			strength = 1.0f;
		(geometry)->shininess = shininess * strength;

		(geometry)->texture = 0;

		if (mat->GetTextureCount(aiTextureType_DIFFUSE) > 0)
		{
			aiString path;

			aiReturn texFound = mat->GetTexture(aiTextureType_DIFFUSE, 0, &path);
			std::string textureName = path.data;

			size_t found = fileName.find_last_of("/\\");
			if (found != std::string::npos)
			{
				textureName.insert(0, fileName.substr(0, found + 1));
			}

			std::cout << "Loading texture file: " << textureName << std::endl;
			(geometry)->texture = pgr::createTexture(textureName);
		}
		CHECK_GL_ERROR();

		glGenVertexArrays(1, &((geometry)->vao));
		glBindVertexArray((geometry)->vao);

		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, (geometry)->ebo);
		glBindBuffer(GL_ARRAY_BUFFER, (geometry)->vbo);

		glEnableVertexAttribArray(shader.positionLoc);
		glVertexAttribPointer(shader.positionLoc, 3, GL_FLOAT, GL_FALSE, 0, 0);

		glEnableVertexAttribArray(shader.normalLoc);
		glVertexAttribPointer(shader.normalLoc, 3, GL_FLOAT, GL_FALSE, 0, (void*)(3 * sizeof(float) * mesh->mNumVertices));

		glEnableVertexAttribArray(shader.textureCoordLoc);
		glVertexAttribPointer(shader.textureCoordLoc, 2, GL_FLOAT, GL_FALSE, 0, (void*)(6 * sizeof(float) * mesh->mNumVertices));
		CHECK_GL_ERROR();

		glBindVertexArray(0);

		(geometry)->numTriangles = mesh->mNumFaces;

		multMeshGeometry.push_back(geometry);
	}
	return true;
}

/**
 * @brief Loads all meshes used in the scene.
*/
void manaeste::loadMeshes()
{
	std::vector<SingleMeshModelInfo> models = {
			{ TERRAIN_MODEL, &terrainGeom },
			{ RAIDER_MODEL, &raiderGeom },
			{ PALM_MODEL, &palmGeom },
			{ DUCK_MODEL, &duckGeom }
	};

	for (auto& model : models)
	{
		if (!loadSingMesh(model.modelName, shaderProgram, model.geometryPtr))
			std::cerr << model.modelName << " loading failed." << std::endl;
	}

	if (!loadMultMesh(SNOWMAN_MODEL, shaderProgram, snowmanGeom))
		std::cerr << "Snowman loading failed." << std::endl;
	if (!loadMultMesh(COUCH_MODEL, shaderProgram, couchGeom))
		std::cerr << "Couch loading failed." << std::endl;

	initDiamondGeom(&diamondGeom);
	initSparklesGeom(&sparklesGeom);
	initCubeSkyboxGeom(&skyboxGeom);
	initAmongusGeom(&amongusGeom);

	useFog = false;
}

/**
 * @brief Aligns object to the given position, front and up vectors.
 * @param position given position
 * @param front where is the front of the object
 * @param up where is the up of the object
 * @return model matrix
*/
glm::mat4 manaeste::getFrontDirectionMat(const glm::vec3& position, const glm::vec3& front, const glm::vec3& up)
{
	const glm::vec3 negFront = -glm::normalize(front);
	const glm::vec3 right = glm::normalize(glm::cross(up, negFront));
	const glm::vec3 newUp = glm::cross(negFront, right);

	return glm::mat4(
		glm::vec4(right, 0.0f),
		glm::vec4(newUp, 0.0f),
		glm::vec4(negFront, 0.0f),
		glm::vec4(position, 1.0f)
	);
}
