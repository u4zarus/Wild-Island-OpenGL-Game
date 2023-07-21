//----------------------------------------------------------------------------------------
/**
 * @file    render.h : Header file for render.cpp.
 * @author  Stepan Manaenko
 * @date    2023
 * @brief   Structures, function definitions and enums used in render.cpp.
 */
 //----------------------------------------------------------------------------------------

#pragma once

#include "pgr.h"

namespace manaeste
{
	enum ObjectType { TERRAIN_ELEMENT, PALM, SNOWMAN, RAIDER, FIRE, BANNER, COUCH, DUCK, DIAMOND };

	typedef struct SingMeshGeom
	{
		GLuint vbo{};
		GLuint ebo{};
		GLuint vao{};
		GLsizei numTriangles{};

		GLuint texture{};
		float shininess{};
		glm::vec3 ambient{};
		glm::vec3 diffuse{};
		glm::vec3 specular{};
	} SingMeshGeom;

	typedef std::vector<SingMeshGeom*> MultMeshGeom;

	typedef struct Object
	{
		glm::vec3 position{};
		glm::vec3 direction{};
		float speed{};
		float size{};
		float startTime{};
		float currentTime{};
		float viewAngle{};
		float frameDuration{};
	} Object;

	typedef struct MainShaderProgram
	{
		GLuint program{};

		GLint positionLoc{};
		GLint normalLoc{};
		GLint textureCoordLoc{};

		GLint PVMmatrixLoc{};
		GLint VmatrixLoc{};
		GLint MmatrixLoc{};
		GLint normalMatrixLoc{};
		GLint timeLoc{};

		GLint diffuseLoc{};
		GLint ambientLoc{};
		GLint specularLoc{};
		GLint shininessLoc{};

		GLint useTextureLoc{};
		GLint textureSamplerLoc{};

		GLint reflectorPositionLoc{};
		GLint reflectorDirectionLoc{};

		GLint sunOnLoc{};
		GLint flashOnLoc{};
		GLint fogOnLoc{};

		GLint pointLightLoc{};
		GLint pointLightOnLoc{};
	} MainShaderProgram;

	typedef struct AmongusShaderProgram
	{
		GLuint program;
		GLint positionLoc;
		GLint textureCoordLoc;
		GLint PVMmatrixLoc;
		GLint currentTimeLoc;
		GLint textureSamplerLoc;
	} AmongusShaderProgram;

	typedef struct SkyboxShaderProgram
	{
		GLuint program;
		GLint screenCoordLoc;
		GLint inversePVmatrixLoc;
		GLint skyboxSamplerLoc;
	} SkyboxShaderProgram;

	typedef struct SparklesShaderProgram
	{
		GLuint program;
		GLint positionLoc;
		GLint textureCoordLoc;
		GLint PVMmatrixLoc;
		GLint VmatrixLoc;
		GLint timeLoc;
		GLint textureSamplerLoc;
		GLint frameDurationLoc;
	} SparklesShaderProgram;

	void createShaders();
	void deleteShaders();

	void setUniformMatrices(const glm::mat4& projMat, const glm::mat4& viewMat, const glm::mat4& modelMat);
	void setUniformMaterial(GLuint texture, float shininess, const glm::vec3& ambient, const glm::vec3& diffuse,
		const glm::vec3& specular);

	void initDiamondGeom(SingMeshGeom** geom);
	void initCubeSkyboxGeom(SingMeshGeom** geom);
	void initSparklesGeom(SingMeshGeom** geom);
	void initAmongusGeom(SingMeshGeom** geom);
	void deleteAmongusAndSkyboxGeoms();

	void drawObject(ObjectType type, Object* object, const glm::mat4& projMat, const glm::mat4& viewMat);
	void drawCubeSkybox(const glm::mat4& projMat, const glm::mat4& viewMat);
	void drawSparklesTexture(Object* fire, const glm::mat4& projMat, const glm::mat4& viewMat);
	void drawAmongusMovingTexture(Object* banner, const glm::mat4& projMat, const glm::mat4& viewMat);

	void setFogState(bool fogOn);

	glm::mat4 setModelMat(const ObjectType& type, const Object* object);
	void setMaterial(const ObjectType& type, const glm::mat4& projMat, const glm::mat4& viewMat,
		const glm::mat4& modelMat);

	bool loadSingMesh(const std::string& fileName, MainShaderProgram& shader, SingMeshGeom** singMeshGeometry);
	bool loadMultMesh(const std::string& fileName, MainShaderProgram& shader, MultMeshGeom& multMeshGeometry);
	void loadMeshes();

	glm::mat4 getFrontDirectionMat(const glm::vec3& position, const glm::vec3& front, const glm::vec3& up);
}
