#ifndef ENUMS_H_
#define ENUMS_H_

enum Direction { Right, Down, Left, Up };

enum MoveAI
{
    STATIONARY,
    DIRECTION_4,
    DIRECTION_ANGLE,
	DIRECTION_ANGLE_W_BOUNCE,
    DIRECTION_SIN,
};

enum SpriteRectEnum
{
	RECT_ONE,	// RayRectangle{0,0,100,100}
	RECT_TWO,	// RayRectangle{100,0,100,100}
	RECT_THREE,	// RayRectangle{200,0,100,100}
	RECT_FOUR	// RayRectangle{300,0,100,100}
};

enum GAME_FLAG
{
	GAME_IN_PLAY,
	PREROUND,
	DRUMROLL,
	TARGET_MISSED,
	TARGET_HIGHLIGHT,
	ROUND_BUFFER,
	COUNTUP,
	LOSE_TIMER,	// not implemented
	FLAG_MUTE,
	GAME_PAUSED,
  LOSE_SCREEN,
  SET_NAME
};

enum BUTTON_ID
{
	PLAY,
	PLAY_AGAIN,
	QUIT,
	MUTE,
	PAUSE,
	PAUSE_QUIT,
  SKIP
};

enum STORAGE_VALUE_POSITION
{
  HI_SCORE,
  HI_SCORE2,
  HI_SCORE3,
  HI_SCORE4,
  HI_SCORE5,
  TEXTURE_PREFERENCE
};

enum SOUND_EFFECTS_INDEX
{
	MISS_SND =0,
	LOSE_SND =1,
	POINTS_SND =2
};

#endif
