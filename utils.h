//----------------------------------------------------------------------------------------
/**
 * @file    utils.h : Header file used in main.cpp
 * @author  Stepan Manaenko
 * @date    2023
 * @brief   Functions used in main.cpp definitions, structures and enums.
 */
 //----------------------------------------------------------------------------------------

#pragma once

namespace manaeste
{
	enum
	{
		KEY_LEFT_ARROW,
		KEY_RIGHT_ARROW,
		KEY_UP_ARROW,
		KEY_DOWN_ARROW,
		KEYS_COUNT
	};

	enum class Direction
	{
		Right,
		Left,
		Forward,
		Backward,
		TurnLeft,
		TurnRight
	};

	struct SceneState
	{
		int windowWidth{};
		int windowHeight{};
		int cameraNum = 1;
		bool freeCameraMode{};
		float movementSpeed = 0.1f;
		bool gameOver{};
		bool keyMap[KEYS_COUNT]{};
		float elapsedTime{};
		bool flashlightOn{};
		bool sunOn{};
		bool fogOn{};
		bool amongusOn{};
		bool sparklesOn{};
		bool fullScreen = false;
	} sceneState;

	struct Camera
	{
		glm::vec3 position{};
		glm::vec3 direction{};
		float viewAngle{};
	} camera;

	typedef std::list<void*> GameObjectsList;

	template<typename T>
	constexpr const T& clampFromStd(const T& value, const T& lower, const T& upper)
	{
		return value < lower ? lower : (value > upper ? upper : value);
	}

	Object* setObject(ObjectType type, glm::vec3 pos);
	void deleteObjects();

	void drawAllObjects(const glm::mat4& orthoProjectionMatrix, const glm::mat4& orthoViewMatrix, const glm::mat4& viewMatrix,
		const glm::mat4& projectionMatrix);

	glm::vec3 correctCameraBoundsPosition(const glm::vec3& position);
	void moveCamera(Direction direction, float deltaAngle);
	void setCameraMode(int mode);

	glm::vec3 getAdjustedCameraPosition(const glm::vec3& cameraPosition, const glm::vec3& duckPosition, float duckRadius);
	bool isCameraCollidingWithDuck(const glm::vec3& cameraPosition, const glm::vec3& duckPosition, float duckRadius);

	void handleGameMenuChoice(int choice);
	void createMenu();

	void fullScreenToggle();
	void sparklesToggle();
	void flashlightToggle();
	void sunToggle();
	void bannerToggle();

	void drawScene();
	void updateScene(float elapsedTime);
	void resetScene();

	void clearGLbuffers();
	void displayCb();
	void reshapeCb(int newWidth, int newHeight);

	void keyboardCb(unsigned char keyPressed, int, int);
	void keyboardUpCb(unsigned char keyReleased, int, int);
	void specialKeyboardCb(int specKeyPressed, int, int);
	void specialKeyboardUpCb(int specKeyReleased, int, int);

	void mouseCb(int buttonPressed, int buttonState, int mouseX, int mouseY);
	void passiveMouseMotionCb(int mouseX, int mouseY);

	void timerCb(int);

	void loadConfig(const std::string& path);
	void initApplication();

	void finalizeApplication();
}
