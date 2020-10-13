#ifndef ENUMS_H_
#define ENUMS_H_

enum Direction { Right, Down, Left, Up };

enum MoveAI {			// description													related members		(all members use the standard attributes)

    STATIONARY,			// target doesn't move
    DIRECTION_4,		// moves in a direction described by enum Direction in			target::speed	target::dir
    DIRECTION_ANGLE,	// moves in the direction of the angle stored in				target::speed				target::angle
	DIRECTION_ANGLE_W_BOUNCE,	// same as angle but doesn't wrap edges					target::speed				target::angle
    DIRECTION_SIN,		// moves (as a wave) in the direction of target::angle			target::speed				target::angle	target::sinPeriod	target::sinAmplitude																	target::sinPeriod	target::sinAmplitude
};

enum SpriteRectEnum
{
	RECT_ONE,	// Rectangle{0,0,100,100}
	RECT_TWO,	// Rectangle{100,0,100,100}
	RECT_THREE,	// Rectangle{200,0,100,100}
	RECT_FOUR	// Rectangle{300,0,100,100}
};


enum GAME_FLAG {
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

enum BUTTON_ID {
	PLAY,
	PLAY_AGAIN,
	QUIT,
	MUTE,
	PAUSE,
	PAUSE_QUIT
};

#endif
