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
	RECT_LUIGI,	// = Rectangle{0,0,25,32},
	RECT_MARIO,	// = Rectangle{25,0,28,32},
	RECT_YOSHI,	// = Rectangle{53,0,23,32},
	RECT_WARIO	// = Rectangle{76,0,30,32}
};

enum GAME_FLAG {
	GAME_IN_PLAY,
	PREROUND,
	DRUMROLL,
	TARGET_MISSED,
	TARGET_HIGHLIGHT,
	hamFIVE,	// not implemented	
	COUNTUP,		// not implemented
	hamSEVEN,	// not implemented
	hamEIGHT,	// not implemented
	GAME_PAUSED		
};

#endif