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
	RECT_ONE,	// Rectangle{0,0,100,100}
	RECT_TWO,	// Rectangle{100,0,100,100}
	RECT_THREE,	// Rectangle{200,0,100,100}
	RECT_FOUR	// Rectangle{300,0,100,100}
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
  LOSE_SCREEN
};

enum BUTTON_ID
{
	PLAY,
	PLAY_AGAIN,
	QUIT,
	MUTE,
	PAUSE,
	PAUSE_QUIT
};

#endif
