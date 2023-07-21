//----------------------------------------------------------------------------------------
/**
 * @file    settings.h : Settings and constants for the application.
 * @author  Stepan Manaenko
 * @date    2023
 * @brief   Settings and constants for the application.
 */
 //----------------------------------------------------------------------------------------

#pragma once

const char* WINDOW_TITLE = "Wild Island";
const float SCENE_WIDTH = 3.8f;  ///< defines how far can camera move left and right
const float SCENE_HEIGHT = 3.8f; ///< defines how far can camera move forward and backwards
const float SCENE_DEPTH = 1.0f;

int WINDOW_WIDTH = 1000;
int WINDOW_HEIGHT = 800;

int NUM_PALMS;   ///< number of palms displayed (max 16) (set in config.txt)
int FULL_SCREEN; ///< 0 - windowed, 1 - fullscreen (set in config.txt)
int BIG_PALMS;   ///< 0 - small palms, 1 - big palms (set in config.txt)
int BIG_DUCK;	 ///< 0 - small duck, 1 - big duck (set in config.txt)
int BIG_SNOWMAN; ///< 0 - small snowman, 1 - big snowman (set in config.txt)

constexpr unsigned char ESC_KEY = 27;
constexpr unsigned char W_KEY = 'w';
constexpr unsigned char A_KEY = 'a';
constexpr unsigned char S_KEY = 's';
constexpr unsigned char D_KEY = 'd';
constexpr unsigned char R_KEY = 'r';
constexpr unsigned char F_KEY = 'f';
constexpr unsigned char B_KEY = 'b';
constexpr unsigned char G_KEY = 'g';
constexpr unsigned char H_KEY = 'h';
constexpr unsigned char J_KEY = 'j';
constexpr unsigned char P_KEY = 'p';

glm::vec3 palmsPositions[] = {
	{2.0f, 0.0f, 0.25f},
	{-2.0f, 0.0f, 0.6f},
	{0.0f, -2.0f, 0.6f},
	{0.0f, 2.0f, 0.6f},
	{-2.0f, 2.0f, 0.6f},
	{2.0f, -2.0f, 0.6f},
	{-2.0f, -2.0f, 0.6f},
	{2.0f, 2.0f, 0.6f},
	{3.3f, 0.0f, 0.6f},
	{-3.4f, 0.0f, 0.6f},
	{0.0f, -3.3f, 0.6f},
	{0.0f, 3.4f, 0.6f},
	{-3.4f, 3.3f, 0.6f},
	{3.3f, -3.4f, 0.6f},
	{-3.3f, -3.3f, 0.6f},
	{3.3f, 3.4f, 0.6f}
};

glm::vec3 terrainElPositions[] = {
	{1.5f, 0.0f, -0.3f},
	{1.5f, 1.5f, -0.3f},
	{0.0f, 1.5f, -0.3f},
	{-1.5f, 0.0f, -0.3f},
	{-1.5f, -1.5f, -0.3f},
	{0.0f, -1.5f, -0.3f},
	{-1.5f, 1.5f, -0.3f},
	{1.5f, -1.5f, -0.3f},
	{0.0f, 0.0f, -0.3f},
	{3.0f, 0.0f, -0.3f},
	{-3.0f, 0.0f, -0.3f},
	{0.0f, 3.0f, -0.3f},
	{0.0f, -3.0f, -0.3f},
	{3.0f, 3.0f, -0.3f},
	{-3.0f, 3.0f, -0.3f},
	{3.0f, -3.0f, -0.3f},
	{-3.0f, -3.0f, -0.3f},
	{1.5f, 3.0f, -0.3f},
	{-1.5f, 3.0f, -0.3f},
	{3.0f, 1.5f, -0.3f},
	{3.0f, -1.5f, -0.3f},
	{-1.5f, -3.0f, -0.3f},
	{1.5f, -3.0f, -0.3f},
	{-3.0f, 1.5f, -0.3f},
	{-3.0f, -1.5f, -0.3f},
	{0.0f, 0.0f, -0.3f}
};
