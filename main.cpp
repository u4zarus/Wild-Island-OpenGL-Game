//----------------------------------------------------------------------------------------
/**
 * @file    main.cpp : This file contains the 'main' function and callbacks.
			Program execution begins and ends there.
 * @author  Stepan Manaenko
 * @date    2023
 * @brief   Main application file.
 */
 //----------------------------------------------------------------------------------------

#include <iostream>
#include <list>
#include <fstream>
#include <glm/gtx/rotate_vector.hpp>
#include <unordered_map>

#include "pgr.h"
#include "render.h"
#include "utils.h"
#include "settings.h"

using namespace manaeste;

extern MainShaderProgram shaderProgram;

struct SceneObjects
{
	Object* snowman{};
	Object* amongus{};
	Object* duck{};
	Object* raider{};
	Object* sparkles{};
	Object* couch{};
	Object* diamond{};
	GameObjectsList terrainElementsList;
	GameObjectsList palmList;
} sceneObjects;

/**
 * @brief Initializes specified object.
 * @param type TERRAIN_ELEMENT, PALM, SNOWMAN, RAIDER, FIRE, BANNER, COUCH, DUCK, DIAMOND. (see render.h).
 * @param objPosition position of the object.
 * @return pointer to the object.
*/
Object* manaeste::setObject(ObjectType type, glm::vec3 objPosition)
{
	auto* object = new Object;

	object->startTime = sceneState.elapsedTime;
	object->currentTime = object->startTime;
	object->size = 1.0f;
	object->direction = glm::vec3(0.0f, 0.0f, 0.0f);
	object->direction = glm::normalize(object->direction);
	object->position = objPosition;
	object->viewAngle = 0.0f;
	object->speed = 0.0f;
	object->frameDuration = 0.0f;

	switch (type)
	{
	case TERRAIN_ELEMENT:
		object->size = 1.0f;
		break;
	case COUCH:
		object->size = 0.5f;
		object->position.z = -0.07f;
		break;
	case DUCK:
		if (!BIG_DUCK)
			object->size = 0.4f;
		else
		{
			object->size = 0.8f;
			object->position.z = 0.3f;
		}
		break;
	case SNOWMAN:
		if (!BIG_SNOWMAN)
			object->size = 0.5f;
		else
		{
			object->size = 0.8f;
			object->position.z = 0.3f;
		}
		object->direction = glm::vec3(0.3f, 0.0f, 0.0f);
		break;
	case PALM:
		if (!BIG_PALMS)
			object->size = 2.0f;
		else
			object->size = 4.0f;
		break;
	case BANNER:
		object->size = 3.0f;
		break;
	case DIAMOND:
		object->size = 0.1f;
		object->position = glm::vec3(-1.0f, 0.0f, 0.5f);
		break;
	case RAIDER:
		object->direction = glm::vec3(cos(glm::radians(object->viewAngle)), sin(glm::radians(object->viewAngle)), 0.0f);
		object->size = 0.3f;
		object->speed = 0.7f;
		break;
	case FIRE:
		if (!BIG_DUCK)
			object->size = 0.5f;
		else
			object->size = 1.0f;
		object->frameDuration = 0.1f;
		break;
	default:
		std::cerr << "Unknown object type: " << type << std::endl;
		delete object;
		return nullptr;
	}

	return object;
}

/**
 * @brief Deletes all objects from the scene.
*/
void manaeste::deleteObjects()
{
	for (auto& obj : sceneObjects.terrainElementsList) obj = nullptr;
	sceneObjects.terrainElementsList.clear();

	for (auto& obj : sceneObjects.palmList) obj = nullptr;
	sceneObjects.palmList.clear();

	delete sceneObjects.couch;
	sceneObjects.couch = nullptr;

	delete sceneObjects.duck;
	sceneObjects.duck = nullptr;

	delete sceneObjects.snowman;
	sceneObjects.snowman = nullptr;

	delete sceneObjects.sparkles;
	sceneObjects.sparkles = nullptr;

	delete sceneObjects.raider;
	sceneObjects.raider = nullptr;

	delete sceneObjects.amongus;
	sceneObjects.amongus = nullptr;
}

/**
 * @brief Draws all objects of the scene. Just objects.
 * @param orthoProjectionMatrix orthoProjection matrix
 * @param orthoViewMatrix orthoView matrix
 * @param viewMatrix view matrix
 * @param projectionMatrix projection matrix
*/
void manaeste::drawAllObjects(const glm::mat4& orthoProjectionMatrix, const glm::mat4& orthoViewMatrix, const glm::mat4& viewMatrix,
	const glm::mat4& projectionMatrix)
{
	for (auto& it : sceneObjects.terrainElementsList)
	{
		auto* terrainElement = (Object*)it;
		drawObject(TERRAIN_ELEMENT, terrainElement, projectionMatrix, viewMatrix);
	}

	glEnable(GL_STENCIL_TEST);
	glStencilOp(GL_KEEP, GL_KEEP, GL_REPLACE);
	glStencilFunc(GL_ALWAYS, 3, 0xFF);
	auto it = sceneObjects.palmList.begin();
	std::advance(it, NUM_PALMS);
	for (auto it2 = sceneObjects.palmList.begin(); it2 != it; ++it2)
	{
		auto* palm = (Object*)*it2;
		drawObject(PALM, palm, projectionMatrix, viewMatrix);
	}
	glDisable(GL_STENCIL_TEST);
	glDisable(GL_DEPTH_TEST);
	glEnable(GL_DEPTH_TEST);

	glEnable(GL_STENCIL_TEST);
	glStencilOp(GL_KEEP, GL_KEEP, GL_REPLACE);
	glStencilFunc(GL_ALWAYS, 1, 0xFF);
	drawObject(SNOWMAN, sceneObjects.snowman, projectionMatrix, viewMatrix);
	glDisable(GL_STENCIL_TEST);
	glDisable(GL_DEPTH_TEST);
	glEnable(GL_DEPTH_TEST);

	glEnable(GL_STENCIL_TEST);
	glStencilOp(GL_KEEP, GL_KEEP, GL_REPLACE);
	glStencilFunc(GL_ALWAYS, 2, 0xFF);
	drawObject(RAIDER, sceneObjects.raider, projectionMatrix, viewMatrix);
	glDisable(GL_STENCIL_TEST);
	glDisable(GL_DEPTH_TEST);
	glEnable(GL_DEPTH_TEST);


	glEnable(GL_STENCIL_TEST);
	glStencilOp(GL_KEEP, GL_KEEP, GL_REPLACE);
	glStencilFunc(GL_ALWAYS, 4, 0xFF);
	drawObject(COUCH, sceneObjects.couch, projectionMatrix, viewMatrix);
	glDisable(GL_STENCIL_TEST);
	glDisable(GL_DEPTH_TEST);
	glEnable(GL_DEPTH_TEST);

	glEnable(GL_STENCIL_TEST);
	glStencilOp(GL_KEEP, GL_KEEP, GL_REPLACE);
	glStencilFunc(GL_ALWAYS, 5, 0xFF);
	drawObject(DUCK, sceneObjects.duck, projectionMatrix, viewMatrix);
	glDisable(GL_STENCIL_TEST);
	glDisable(GL_DEPTH_TEST);
	glEnable(GL_DEPTH_TEST);

	glEnable(GL_STENCIL_TEST);
	glStencilOp(GL_KEEP, GL_KEEP, GL_REPLACE);
	glStencilFunc(GL_ALWAYS, 6, 0xFF);
	drawObject(DIAMOND, sceneObjects.diamond, projectionMatrix, viewMatrix);
	glDisable(GL_STENCIL_TEST);
	glDisable(GL_DEPTH_TEST);
	glEnable(GL_DEPTH_TEST);

	drawCubeSkybox(projectionMatrix, viewMatrix);

	if (sceneState.sparklesOn)
		drawSparklesTexture(sceneObjects.sparkles, projectionMatrix, viewMatrix);

	glEnable(GL_STENCIL_TEST);
	glStencilOp(GL_KEEP, GL_KEEP, GL_REPLACE);
	glStencilFunc(GL_ALWAYS, 7, 0xFF);
	if (sceneState.amongusOn)
	{
		if (sceneObjects.amongus != nullptr)
			drawAmongusMovingTexture(sceneObjects.amongus, orthoProjectionMatrix, orthoViewMatrix);
	}
	glDisable(GL_STENCIL_TEST);
	glDisable(GL_DEPTH_TEST);
	glEnable(GL_DEPTH_TEST);
}

/**
 * @brief Does not let the camera go out of the scene.
 * @param position center of the object.
 * @return corrected position.
*/
glm::vec3 manaeste::correctCameraBoundsPosition(const glm::vec3& position)
{
	float boundX = clampFromStd(position.x, -(SCENE_WIDTH), SCENE_WIDTH);
	float boundY = clampFromStd(position.y, -(SCENE_HEIGHT), SCENE_HEIGHT);
	return { boundX, boundY, position.z };
}

/**
 * @brief Moves the camera in the given direction by the given angle.
 * @param direction Forward, Backward, Left, Right, TurnLeft, TurnRight. (see utils.h).
 * @param deltaAngle
*/
void manaeste::moveCamera(Direction direction, float deltaAngle)
{
	glm::vec3 newPosition = camera.position;
	switch (direction)
	{
	case Direction::Forward:
		newPosition += sceneState.movementSpeed * camera.direction / glm::vec3(2);
		break;
	case Direction::Backward:
		newPosition -= sceneState.movementSpeed * camera.direction / glm::vec3(2);
		break;
	case Direction::Left:
		newPosition += sceneState.movementSpeed * glm::vec3(-camera.direction.y, camera.direction.x, camera.direction.z) / glm::vec3(2);
		break;
	case Direction::Right:
		newPosition -= sceneState.movementSpeed * glm::vec3(-camera.direction.y, camera.direction.x, camera.direction.z) / glm::vec3(2);
		break;
	case Direction::TurnLeft:
		camera.viewAngle += deltaAngle;
		if (camera.viewAngle >= 360) camera.viewAngle -= 360;
		camera.direction = glm::vec3(cos(glm::radians(camera.viewAngle)), sin(glm::radians(camera.viewAngle)), 0);
		break;
	case Direction::TurnRight:
		camera.viewAngle -= deltaAngle;
		if (camera.viewAngle < 0) camera.viewAngle += 360;
		camera.direction = glm::vec3(cos(glm::radians(camera.viewAngle)), sin(glm::radians(camera.viewAngle)), 0);
		break;
	}

	glm::vec3 duckPosition = sceneObjects.duck->position;
	duckPosition.z = 0;
	float duckRadius = 0.7f;

	if (isCameraCollidingWithDuck(newPosition, duckPosition, duckRadius))
	{
		newPosition = getAdjustedCameraPosition(newPosition, duckPosition, duckRadius);
	}
	newPosition = correctCameraBoundsPosition(newPosition);
	camera.position = newPosition;
}

/**
 * @brief Bool function that checks if the camera is colliding with the duck.
 * @param cameraPosition current camera position.
 * @param duckPosition current duck position.
 * @param duckRadius duck radius.
 * @return true if the camera is colliding with the duck, false otherwise.
*/
bool manaeste::isCameraCollidingWithDuck(const glm::vec3& cameraPosition, const glm::vec3& duckPosition, float duckRadius)
{
	float distance = glm::length(cameraPosition - duckPosition);
	if (distance < (0.5f + duckRadius))
	{
		return true;
	}
	return false;
}

/**
 * @brief Returns the adjusted camera position.
 * @param cameraPosition current camera position.
 * @param duckPosition current duck position.
 * @param duckRadius duck radius.
 * @return adjusted camera position.
*/
glm::vec3 manaeste::getAdjustedCameraPosition(const glm::vec3& cameraPosition, const glm::vec3& duckPosition, float duckRadius)
{
	glm::vec3 duckToCamera = cameraPosition - duckPosition;

	float distance = glm::length(duckToCamera);

	if (distance < duckRadius)
	{
		float penetrationDepth = duckRadius - distance;

		glm::vec3 adjustment = glm::normalize(duckToCamera) * penetrationDepth;
		glm::vec3 adjustedPosition = cameraPosition + adjustment;

		return adjustedPosition;
	}
	return cameraPosition;
}

/**
 * @brief Set camera view mode.
 * @param mode number
*/
void manaeste::setCameraMode(int mode)
{
	sceneState.cameraNum = mode;
	(mode == 3) ? sceneState.freeCameraMode = true : sceneState.freeCameraMode = false;
	glutPassiveMotionFunc(NULL);
}

/**
 * @brief Sets the application modes according to menu entry.
 * @param choice
*/
void manaeste::handleGameMenuChoice(int choice)
{
	switch (choice)
	{
	case 1:
		setCameraMode(1);
		break;
	case 2:
		setCameraMode(2);
		break;
	case 3:
		exit(0);
		break;
	case 4:
		setCameraMode(4);
		break;
	case 5:
		setCameraMode(5);
		break;
	case 6:
		flashlightToggle();
		break;
	case 7:
		sunToggle();
		break;
	case 8:
		sceneState.fogOn = !sceneState.fogOn;
		setFogState(sceneState.fogOn);
		break;
	case 9:
		sparklesToggle();
		break;
	case 10:
		bannerToggle();
		break;
	case 11:
		resetScene();
		break;
	default:
		break;
	}

	glutPostRedisplay();
}

/**
 * @brief Creates right-click menu.
*/
void manaeste::createMenu()
{
	int submenuCamera = glutCreateMenu(handleGameMenuChoice);
	glutAddMenuEntry("Camera 1", 1);
	glutAddMenuEntry("Camera 2", 2);
	glutAddMenuEntry("Free Camera", 4);
	glutAddMenuEntry("Raider Camera", 5);
	glutSetMenuFont(submenuCamera, GLUT_BITMAP_HELVETICA_18);

	int mainMenu = glutCreateMenu(handleGameMenuChoice);
	glutAddSubMenu("Select camera view", submenuCamera);
	glutAddMenuEntry("Toggle Sun", 7);
	glutAddMenuEntry("Toggle Flashlight", 6);
	glutAddMenuEntry("Toggle Fog", 8);
	glutAddMenuEntry("Toggle Fire", 9);
	glutAddMenuEntry("Toggle Banner", 10);
	glutAddMenuEntry("Reset Scene", 11);
	glutAddMenuEntry("Exit", 3);
	glutSetMenuFont(mainMenu, GLUT_BITMAP_HELVETICA_18);

	glutAttachMenu(GLUT_RIGHT_BUTTON);
}

/**
 * @brief Toggle full screen mode on/off.
*/
void manaeste::fullScreenToggle()
{
	if (!sceneState.fullScreen)
	{
		sceneState.windowHeight = glutGet(GLUT_SCREEN_HEIGHT);
		sceneState.windowWidth = glutGet(GLUT_SCREEN_WIDTH);
		glutFullScreen();
	}
	else
	{
		sceneState.windowHeight = WINDOW_HEIGHT;
		sceneState.windowWidth = WINDOW_WIDTH;
		glutReshapeWindow(sceneState.windowWidth, sceneState.windowHeight);
		glutPositionWindow(100, 100);
	}
	sceneState.fullScreen = !sceneState.fullScreen;
}

/**
 * @brief Toggle fire on/off.
*/
void manaeste::sparklesToggle()
{
	sceneState.sparklesOn = !sceneState.sparklesOn;
}

/**
 * @brief Toggle flashlight on/off.
*/
void manaeste::flashlightToggle()
{
	sceneState.flashlightOn = !sceneState.flashlightOn;
}

/**
 * @brief Toggle sun on/off.
*/
void manaeste::sunToggle()
{
	sceneState.sunOn = !sceneState.sunOn;
}

/**
 * @brief Toggle banner on/off.
*/
void manaeste::bannerToggle()
{
	sceneState.amongusOn = !sceneState.amongusOn;
}

/**
 * @brief Draws the complete scene.
*/
void manaeste::drawScene()
{
	glm::mat4 orthoProjectionMatrix = glm::ortho(
		-SCENE_WIDTH, SCENE_WIDTH,
		-SCENE_HEIGHT, SCENE_HEIGHT,
		-10.0f * SCENE_DEPTH, 10.0f * SCENE_DEPTH
	);
	glm::mat4 orthoViewMatrix = glm::lookAt(
		glm::vec3(0.0f, 0.0f, 1.0f),
		glm::vec3(0.0f, 0.0f, 0.0f),
		glm::vec3(0.0f, 1.0f, 0.0f)
	);

	glm::mat4 projectionMatrix, viewMatrix;
	if (sceneState.freeCameraMode)
	{
		projectionMatrix = glm::perspective(glm::radians(60.0f), sceneState.windowWidth / (float)sceneState.windowHeight, 0.1f, 10.0f);

		glm::vec3 targetPos = camera.position;
		glm::vec3 targetDir = camera.direction;

		const float transitionSpeed = 0.1f;
		const float threshold = 0.01f;
		bool posReached = glm::length(camera.position - targetPos) < threshold;
		bool dirReached = glm::length(camera.direction - targetDir) < threshold;
		if (!posReached) camera.position = glm::mix(camera.position, targetPos, transitionSpeed);
		if (!dirReached) camera.direction = glm::mix(camera.direction, targetDir, transitionSpeed);

		viewMatrix = glm::lookAt(camera.position, camera.position + camera.direction, glm::vec3(0.0f, 0.0f, 1.0f));
	}
	else if (sceneState.cameraNum == 2)
	{
		projectionMatrix = glm::perspective(glm::radians(60.0f), sceneState.windowWidth / (float)sceneState.windowHeight, 0.1f, 10.0f);
		viewMatrix = glm::lookAt(camera.position, camera.position + camera.direction, glm::vec3(0, 0, 1));
	}
	else if (sceneState.cameraNum == 5)
	{
		projectionMatrix = glm::perspective(glm::radians(60.0f), sceneState.windowWidth / (float)sceneState.windowHeight, 0.1f, 10.0f);
		viewMatrix = glm::lookAt(sceneObjects.raider->position, glm::vec3(0, 0, 0), glm::vec3(0, 0, 1));
	}
	else
	{
		projectionMatrix = glm::ortho(-SCENE_WIDTH, SCENE_WIDTH, -SCENE_HEIGHT, SCENE_HEIGHT, -10.0f * SCENE_DEPTH, 10.0f * SCENE_DEPTH);
		viewMatrix = glm::lookAt(glm::vec3(0.0f, 0.0f, 1.0f), glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f, 1.0f, 0.0f));
	}

	glUseProgram(shaderProgram.program);
	glUniform1f(shaderProgram.timeLoc, sceneState.elapsedTime);
	glUniform3fv(shaderProgram.reflectorPositionLoc, 1, glm::value_ptr(camera.position));
	glUniform3fv(shaderProgram.reflectorDirectionLoc, 1, glm::value_ptr(camera.direction));
	glUniform1i(shaderProgram.sunOnLoc, sceneState.sunOn);
	glUniform1i(shaderProgram.flashOnLoc, sceneState.flashlightOn);
	glUniform1i(shaderProgram.pointLightOnLoc, sceneState.sparklesOn);
	glUniform4fv(shaderProgram.pointLightLoc, 1, glm::value_ptr(glm::vec4(sceneObjects.sparkles->position, 1.0f)));
	glUniform1i(shaderProgram.fogOnLoc, sceneState.fogOn);
	drawAllObjects(orthoProjectionMatrix, orthoViewMatrix, viewMatrix, projectionMatrix);
}

/**
 * @brief Updates the scene and objects in it.
 * @param elapsedTime time elapsed since last update.
*/
void manaeste::updateScene(float elapsedTime)
{
	switch (sceneState.cameraNum) {
	case 1:
		camera.position = glm::vec3(0.0f);
		camera.direction = glm::vec3(0.0f);
		sceneState.freeCameraMode = false;
		glutPassiveMotionFunc(NULL);
		break;
	case 2:
		camera.position = glm::vec3(3.0f, 3.0f, 0.0f);
		camera.direction = glm::vec3(-1.0f, 0.0f, 0.0f);
		sceneState.freeCameraMode = false;
		glutPassiveMotionFunc(NULL);
		break;
	case 5:
		sceneState.flashlightOn = false;
		sceneState.freeCameraMode = false;
		camera.position = sceneObjects.raider->position;
		camera.direction = sceneObjects.raider->direction;
		break;
	default:
		sceneState.freeCameraMode = true;
		break;
	}

	camera.position = correctCameraBoundsPosition(camera.position);

	const float raiderSpeed = sceneObjects.raider->speed;
	const float raiderElapsedTime = elapsedTime * raiderSpeed;
	const glm::vec3 raiderPos = glm::vec3(sin(raiderElapsedTime), cos(raiderElapsedTime), 1.0f);
	const glm::vec3 raiderVel = glm::vec3(-cos(raiderElapsedTime), sin(raiderElapsedTime), 0.0f);
	const glm::vec3 raiderDir = glm::normalize(raiderVel);
	sceneObjects.raider->position = raiderPos;
	sceneObjects.raider->direction = raiderDir;

	if (sceneObjects.sparkles)
	{
		sceneObjects.sparkles->currentTime = elapsedTime;
	}
}

/**
 * @brief Restarts the game. Creates everything from scratch.
*/
void manaeste::resetScene()
{
	loadConfig("config.txt");
	deleteObjects();

	if (sceneState.freeCameraMode)
	{
		sceneState.freeCameraMode = false;
		glutPassiveMotionFunc(NULL);
	}
	sceneState.cameraNum = 4;

	std::fill(std::begin(sceneState.keyMap), std::end(sceneState.keyMap), false);

	camera.position = glm::vec3(0.0f, 0.0f, 0.0f);
	camera.direction = glm::vec3(cos(glm::radians(camera.viewAngle)), sin(glm::radians(camera.viewAngle)), 0.0f);
	camera.viewAngle = 90.0f;

	sceneObjects.duck = setObject(DUCK, glm::vec3(0.4f, 2.0f, 0.0f));
	sceneObjects.diamond = setObject(DIAMOND, glm::vec3(0.0f, 0.0f, 0.0f));
	sceneObjects.couch = setObject(COUCH, glm::vec3(1.0f, 1.0f, 0.0f));
	sceneObjects.snowman = setObject(SNOWMAN, glm::vec3(2.0f, 1.0f, 0.1f));
	sceneObjects.raider = (sceneObjects.raider == nullptr) ? setObject(RAIDER, glm::vec3(1.0f, 0.0f, 0.5f)) : sceneObjects.raider;

	for (auto& position : terrainElPositions)
	{
		sceneObjects.terrainElementsList.push_back(setObject(TERRAIN_ELEMENT, position));
	}

	for (auto& position : palmsPositions)
	{
		sceneObjects.palmList.push_back(setObject(PALM, position));
	}

	if (sceneObjects.sparkles == nullptr)
	{
		sceneObjects.sparkles = setObject(FIRE, glm::vec3(0.4f, 2.0f, 0.0f));
	}

	sceneState.fogOn = false;
	setFogState(sceneState.fogOn);
	sceneState.sparklesOn = false;
	sceneState.sunOn = true;
	sceneState.amongusOn = false;
	sceneState.flashlightOn = false;

	if (FULL_SCREEN)
	{
		glutFullScreen();
	}
	else
	{
		glutReshapeWindow(WINDOW_WIDTH, WINDOW_HEIGHT);
		glutPositionWindow(100, 100);
	}
}

/**
 * @brief Clean OpenGL buffers.
*/
void manaeste::clearGLbuffers()
{
	GLbitfield mask = GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT;
	mask |= GL_STENCIL_BUFFER_BIT;

	glClear(mask);
}

/**
 * @brief Draw the window contents.
*/
void manaeste::displayCb()
{
	clearGLbuffers();
	drawScene();
	glutSwapBuffers();
}

/**
 * @brief Callback responsible for the window resize.
 * @param newWidth new window width
 * @param newHeight new window height
*/
void manaeste::reshapeCb(int newWidth, int newHeight)
{
	sceneState.windowWidth = newWidth;
	sceneState.windowHeight = newHeight;
	glViewport(0, 0, (GLsizei)newWidth, (GLsizei)newHeight);
}

/**
 * @brief Handle the key pressed event.
 * @param keyPressed
*/
void manaeste::keyboardCb(unsigned char keyPressed, int, int)
{
	switch (keyPressed)
	{
	case ESC_KEY:
		glutLeaveMainLoop();
		break;
	case W_KEY:
		sceneState.keyMap[KEY_UP_ARROW] = true;
		break;
	case S_KEY:
		sceneState.keyMap[KEY_DOWN_ARROW] = true;
		break;
	case A_KEY:
		sceneState.keyMap[KEY_LEFT_ARROW] = true;
		break;
	case D_KEY:
		sceneState.keyMap[KEY_RIGHT_ARROW] = true;
		break;
	}
}

/**
 * @brief Called whenever a key on the keyboard was released.
 * @param keyReleased
*/
void manaeste::keyboardUpCb(unsigned char keyReleased, int, int)
{
	switch (keyReleased)
	{
	case W_KEY:
		sceneState.keyMap[KEY_UP_ARROW] = false;
		break;
	case S_KEY:
		sceneState.keyMap[KEY_DOWN_ARROW] = false;
		break;
	case A_KEY:
		sceneState.keyMap[KEY_LEFT_ARROW] = false;
		break;
	case D_KEY:
		sceneState.keyMap[KEY_RIGHT_ARROW] = false;
		break;
	case F_KEY:
		flashlightToggle();
		break;
	case G_KEY:
		sceneState.fogOn = !sceneState.fogOn;
		setFogState(sceneState.fogOn);
		break;
	case B_KEY:
		bannerToggle();
		break;
	case H_KEY:
		sunToggle();
		break;
	case J_KEY:
		sparklesToggle();
		break;
	case P_KEY:
		fullScreenToggle();
		break;
	case R_KEY:
		resetScene();
		break;
	}
}

/**
 * @brief @brief Handle the non-ASCII key pressed event (such as arrows or F1).
 * @param specKeyPressed
*/
void manaeste::specialKeyboardCb(int specKeyPressed, int, int)
{
	if (sceneState.gameOver)
		return;

	switch (specKeyPressed)
	{
	case GLUT_KEY_UP:
		sceneState.keyMap[KEY_UP_ARROW] = true;
		break;
	case GLUT_KEY_RIGHT:
		sceneState.keyMap[KEY_RIGHT_ARROW] = true;
		break;
	case GLUT_KEY_LEFT:
		sceneState.keyMap[KEY_LEFT_ARROW] = true;
		break;
	case GLUT_KEY_DOWN:
		sceneState.keyMap[KEY_DOWN_ARROW] = true;
		break;
	}
}

/**
 * @brief Handle the non-ASCII key pressed (released) event (such as arrows or F1).
 * @param specKeyReleased The key that was released.
*/
void manaeste::specialKeyboardUpCb(int specKeyReleased, int, int)
{
	if (sceneState.gameOver)
		return;

	switch (specKeyReleased)
	{
	case GLUT_KEY_UP:
		sceneState.keyMap[KEY_UP_ARROW] = false;
		break;
	case GLUT_KEY_RIGHT:
		sceneState.keyMap[KEY_RIGHT_ARROW] = false;
		break;
	case GLUT_KEY_DOWN:
		sceneState.keyMap[KEY_DOWN_ARROW] = false;
		break;
	case GLUT_KEY_LEFT:
		sceneState.keyMap[KEY_LEFT_ARROW] = false;
		break;
	case GLUT_KEY_F1:
		sceneState.cameraNum = 1;
		break;
	case GLUT_KEY_F2:
		sceneState.cameraNum = 2;
		break;
	case GLUT_KEY_F3:
		sceneState.cameraNum = 4;
		break;
	case GLUT_KEY_F4:
		sceneState.cameraNum = 5;
		break;
	}
}

/**
 * @brief React to mouse button press and release (mouse click).
 * @param buttonPressed button code (GLUT_LEFT_BUTTON, GLUT_MIDDLE_BUTTON, or GLUT_RIGHT_BUTTON)
 * @param buttonState GLUT_DOWN when pressed, GLUT_UP when released
 * @param mouseX mouse (cursor) X position
 * @param mouseY mouse (cursor) Y position
*/
void manaeste::mouseCb(int buttonPressed, int buttonState, int mouseX, int mouseY)
{
	if ((buttonPressed == GLUT_LEFT_BUTTON) && (buttonState == GLUT_DOWN))
	{
		unsigned char objStencilID = 0;
		glReadPixels(mouseX, sceneState.windowHeight - mouseY - 1, 1, 1, GL_STENCIL_INDEX, GL_UNSIGNED_BYTE, &objStencilID);

		switch (objStencilID)
		{
		case 1:
			sparklesToggle();
			break;
		case 2:
			setCameraMode(5);
			break;
		case 4:
			sceneObjects.couch->size = 0.0f;
			break;
		}
	}
}

/**
 * @brief Handle mouse movement over the window (with no button pressed).
 * @param mouseX mouse (cursor) X position
 * @param mouseY mouse (cursor) Y position
*/
void manaeste::passiveMouseMotionCb(int mouseX, int mouseY)
{
	if (mouseX != sceneState.windowWidth / 2)
	{
		const float delta = 0.5f * static_cast<float>(mouseX - sceneState.windowWidth / 2);
		const Direction direction = std::signbit(delta) ? Direction::TurnLeft : Direction::TurnRight;
		moveCamera(direction, std::abs(delta));
		glutWarpPointer(sceneState.windowWidth / 2, sceneState.windowHeight / 2);
	}

	if (mouseX != sceneState.windowWidth / 2 || mouseY != sceneState.windowHeight / 2)
	{
		glutWarpPointer(sceneState.windowWidth / 2, sceneState.windowHeight / 2);
	}
}

/**
 * @brief Callback responsible for the scene update.
*/
void manaeste::timerCb(int)
{
	sceneState.elapsedTime = 0.001f * (float)glutGet(GLUT_ELAPSED_TIME);

	if (sceneState.cameraNum == 4)
	{
		std::unordered_map<int, Direction> keyMap = {
			{KEY_UP_ARROW, Direction::Forward},
			{KEY_DOWN_ARROW, Direction::Backward},
			{KEY_RIGHT_ARROW, Direction::Right},
			{KEY_LEFT_ARROW, Direction::Left}
		};

		for (const auto& pair : keyMap)
		{
			int key = pair.first;
			Direction dir = pair.second;

			if (sceneState.keyMap[key])
			{
				moveCamera(dir, 0.0f);
			}
		}
	}

	updateScene(sceneState.elapsedTime);

	if (sceneState.freeCameraMode)
	{
		glutPassiveMotionFunc(passiveMouseMotionCb);
	}

	if (sceneState.amongusOn && sceneObjects.amongus == nullptr)
	{
		sceneObjects.amongus = setObject(BANNER, glm::vec3(0.0f, 0.0f, 0.0f));
	}

	if (sceneObjects.amongus != nullptr)
	{
		sceneObjects.amongus->currentTime = sceneState.elapsedTime;
	}


	glutTimerFunc(33, timerCb, 33);

	glutPostRedisplay();
}

/**
 * @brief Loads the scene parameters from the file.
 * If the file is in wrong format or does not exist, default values are used.
 * @param path path to the file.
*/
void manaeste::loadConfig(const std::string& path)
{
	std::ifstream file(path);
	bool opened;
	if (opened = !file.is_open())
	{
		std::cerr << "Could not open config file: " << path << std::endl << "Using default values" << std::endl;
		std::cerr << "Config file must be named \"config.txt\" and be in the same directory as the executable." << std::endl;
	}

	int numLines = 0;
	std::string unused;
	while (std::getline(file, unused))
	{
		++numLines;
	}

	if (numLines != 5)
	{
		NUM_PALMS = 16;
		FULL_SCREEN = false;
		BIG_DUCK = BIG_PALMS = BIG_SNOWMAN = true;
	}
	else
	{
		file.clear();
		file.seekg(0, std::ios::beg);

		file >> NUM_PALMS >> FULL_SCREEN >> BIG_PALMS >> BIG_DUCK >> BIG_SNOWMAN;

		NUM_PALMS = clampFromStd(NUM_PALMS, 0, 16);
	}

	file.close();
}

/**
 * @brief Called when the application is starting. Initialize all objects.
*/
void manaeste::initApplication()
{
	std::srand(static_cast<unsigned int>(std::time(nullptr)));

	glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
	glClearStencil(0);
	glEnable(GL_DEPTH_TEST);
	glutSetCursor(GLUT_CURSOR_CROSSHAIR);

	sceneObjects.amongus = nullptr;

	createShaders();
	loadMeshes();
	resetScene();
}

/**
 * @brief Called when the application is exiting. Destroy all created objects.
*/
void manaeste::finalizeApplication()
{
	deleteObjects();
	deleteAmongusAndSkyboxGeoms();
	delete sceneObjects.raider;
	sceneObjects.raider = nullptr;
	deleteShaders();
}

/**
 * @brief Entry point of the application.
 * @param argc number of command-line arguments
 * @param argv command-line arguments array
 * @return program exit code
*/
int main(int argc, char** argv)
{
	loadConfig("config.txt");
	glutInit(&argc, argv);

	glutInitContextVersion(pgr::OGL_VER_MAJOR, pgr::OGL_VER_MINOR);
	glutInitContextFlags(GLUT_FORWARD_COMPATIBLE);
	glutInitDisplayMode(GLUT_RGB | GLUT_DOUBLE | GLUT_DEPTH | GLUT_STENCIL);

	glutInitWindowSize(WINDOW_WIDTH, WINDOW_HEIGHT);
	glutCreateWindow(WINDOW_TITLE);

	glutDisplayFunc(displayCb);
	glutReshapeFunc(reshapeCb);
	glutKeyboardFunc(keyboardCb);
	glutKeyboardUpFunc(keyboardUpCb);
	glutSpecialFunc(specialKeyboardCb);
	glutSpecialUpFunc(specialKeyboardUpCb);
	glutMouseFunc(mouseCb);

	createMenu();

	glutTimerFunc(33, timerCb, 0);

	if (!pgr::initialize(pgr::OGL_VER_MAJOR, pgr::OGL_VER_MINOR))
		pgr::dieWithError("pgr init failed, required OpenGL not supported?");
	initApplication();
	glutCloseFunc(finalizeApplication);


	glutMainLoop();

	return EXIT_SUCCESS;
}
