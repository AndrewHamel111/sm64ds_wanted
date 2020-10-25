 /*******************************************************************************************

idea is to create a game very similar to the mario 64 DS game WANTED!
https://www.youtube.com/watch?v=WePBtu63J_4

    POTENTIALLY RUBBISH IDEAS (ordered from least to most rubbish)
- have a mode that uses the original mario sprites/sounds
-
-
-
- have an option for the user to provide their own sprites for the game

	IMPORTANT THINGS TO REMEMBER
- put related flags and alarms on the same index
- use an enum to reference alarms/flags to assist development

	NEXT STEPS
- form a distinction (by var name) between SourceRects and DestRects. i.e. RECT_TIME -> SOURCE_RECT_TIME

********************************************************************************************/

#include "raylib.h"
#define NEARBLACK CLITERAL(Color){ 20, 20, 20, 255}
#define MUSTARD CLITERAL(Color){ 203, 182, 51, 255}

#include <iostream>
#include <cmath>
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <ctime>
#include <vector>
#include <utility>
#include <random>
#include <algorithm>

// CONSTANTS.HPP

//#define USE_DEV_SQUARES
#define USE_DS_STYLE

#define FPS_TARGET 60
#define FIRST_LEVEL 1
#define INITIAL_SECONDS 10

#define SCREEN_WIDTH 800
#define SCREEN_HEIGHT 792

#define AREA_WIDTH 800
#define AREA_HEIGHT 600

#define EXCESSIVE_DISTANCE 640000
#define TARGET_HEIGHT 100
#define TARGET_MINIMUM_DISTANCE 2500
#define HIGHEST_TEXTURE_INDEX 1

#define BOUNCE_OFFSET 2
#define BASE_SPEED 1.5f
#define BASE_SIN_AMPLITUDE 1

#define CLICK_RANGE 6000

#define ALARMFLAG_COUNT 11
#define BGM_TRACK_COUNT 7
#define POINTS_SFX_COUNT 4

// times are given in seconds
#define PREROUND_ALARM_DURATION 1
#define DRUMROLL_ALARM_DURATION 0
#define TARGET_MISSED_ALARM_DURATION 1
#define TARGET_HIGHLIGHT_DURATION 2
#define ROUND_BUFFER_DURATION 2
#define SCORE_COUNTUP_DURATION 1
#define LOSE_TIMER_DURATION 4

#define EXAMPLE_STRING "Example String"


// SPRITE SOURCE RECTANGLES, IMPORTANT TO CHANGE IF/WHEN SOURCE SPRITE CHANGES
Rectangle criminalPosterRects[] = { Rectangle{76,0,100,100}, Rectangle{332,0,100,100},Rectangle{588,0,100,100},Rectangle{844,0,100,100}};
Rectangle posterSourceRects[] = { Rectangle{76,316,100,100}, Rectangle{332,316,100,100},Rectangle{588,316,100,100},Rectangle{844,316,100,100}};

Rectangle posterSourceRect = Rectangle{0,100, 256, 192};

Rectangle RECT_TIME 		= Rectangle{256, 100, 64, 16};
Rectangle RECT_LEVEL 		= Rectangle{320, 100, 150, 30};
Rectangle RECT_PLAY 		= Rectangle{256, 130, 256, 76};
Rectangle RECT_PLAY_AGAIN 	= Rectangle{512, 130, 256, 76};
Rectangle RECT_QUIT 		= Rectangle{768, 130, 256, 76};
Rectangle RECT_NONTENDO	 	= Rectangle{256, 206, 312, 80};
Rectangle RECT_2020 		= Rectangle{256, 286, 112, 30};
Rectangle RECT_UNMUTED		= Rectangle{568, 206, 80, 80};
Rectangle RECT_MUTED		= Rectangle{648, 206, 80, 80};
Rectangle RECT_PAUSE		= Rectangle{768, 206, 256, 76};

/// NOT YET IMPLEMENTED
Rectangle RECT_STAR			= Rectangle{770, 100, 30, 30};
Rectangle RECT_TIMES		= Rectangle{770 + 30, 100, 30, 30};
Rectangle RECT_HIGHSCORE= Rectangle{830, 			100, 133, 15};
Rectangle RECT_SCORE		= Rectangle{830 + 63, 100, 70, 15};

Rectangle RECT_POG_MINI = Rectangle{963, 100, 24, 30};

// Alarm variables #{
/*										flag						constant
alarm[0] -	EMPTY ALARM
alarm[1] -	preRoundAlarm				|	preRoundFlag			|	PREROUND_ALARM_DURATION
alarm[2] -	drumrollAlarm				|	drumrollFlag			|	DRUMROLL_ALARM_DURATION
alarm[3] - 	targetMissedAlarm			|	targetMissedFlag		|	TARGET_MISSED_ALARM_DURATION
alarm[4] -	targetHighlightAlarm		|	targetHighlightFlag		|	TARGET_HIGHLIGHT_DURATION
alarm[5] -	roundBuffer
alarm[6] -	scoreCountupAlarm
alarm[7] -	loseAlarm
alarm[8] -
alarm[9] -
}#		*/
int alarm[ALARMFLAG_COUNT] = {0};	// linked with flags by enum GAME_FLAG

// flags
// preRoundFlag = false;
// drumrollFlag = false;		// TODO To Be Implemented
// targetMissedFlag = false;	// TODO To Be Implemented
// targetHighlightFlag = false;
// roundTimeDepletedFlag = false;
// scoreCountupFlag
bool flags[ALARMFLAG_COUNT] = {false};// linked with alarms by enum GAME_FLAG

// counter[0] - used for countup
// counter[1] - seconds left
// counter[2] - frames left in current second
int counter[4] = {0};

unsigned short int scores[5] = {0};

// ENUMS.HPP

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

// TARGET.HPP

// helper struct target_template used to create a consistent relationship between targets sharing a like sprite
struct target_template
{
	MoveAI movetype;
	double angle;
	float speed;
	Rectangle rect;

	target_template(Rectangle r): movetype{STATIONARY}, angle{0}, speed{0}, rect{r}
	{}

	target_template(MoveAI m, double a, float s, Rectangle r): movetype{m}, angle{a}, speed{s}, rect{r}
	{}
};

// declaration of class target/*#{*/
class target
{
	// standard attributes
	MoveAI movementType;
	Vector2 position;		/// starting position of the target / circular pivot when movementType == circular;
	bool _isWanted;			/// determines if this particular target is the win condition on the level.
	bool isMenuTarget;		/// targets created only for display purposes, they will wrap on screen boundaries instead of area boundaries

	//float sinPeriod;
	float sinAmplitude;
	Direction dir;
	double angle;	// TODO write an access method that makes sure this shit is always radians o.O
	float speed;

	Rectangle spriteRect;

	// update methods
	void move(double);
	void wrap();
	void bounce();

public:
	// constructors
	target(float x, float y, MoveAI movetype, bool wanted);
	target(float x, float y, MoveAI movetype, Direction direction,bool wanted);
	target(float x, float y, MoveAI movetype, double angle,bool wanted);
	target(float x, float y, MoveAI movetype, double angle,float speed);
	target(Vector2 pos, MoveAI movetype, double angle,float speed);
	target(Vector2 pos, MoveAI movetype, double angle,float speed, Rectangle spriteRect);

	target(Vector2 pos, target_template t);
	target(Vector2 pos, Rectangle spriteRect);

	// access methods
	void setMoveType(MoveAI);

	void setPos(int, int);
	void setPos(float, float);
	void setPos(double, double);
	void setPos(Vector2);

	void setWanted(bool);
	void setAsMenuTarget();

	void setAngle(double);
	void setAngleDegrees(int);

	void setSpeed(float);

	void setSpriteRect(Rectangle);

	MoveAI getMoveType() const;

	Vector2 getCenter() const;
	Vector2 getPos() const;
	float getX() const;
	float getY() const;

	bool isWanted() const;
	bool isMenu() const;

	Rectangle getSpriteRect() const;

	// update methods
	void update(double);

	// draw methods
	//void DrawTarget();
};/*}#*/

// constructor definitions for class target/*#{*/
target::target(float x, float y, MoveAI movetype, bool wanted): sinAmplitude{BASE_SIN_AMPLITUDE} ,isMenuTarget{false}
{
	setPos(x, y);
	setMoveType(movetype);
	setWanted(wanted);
}

target::target(float x, float y, MoveAI movetype,Direction direction, bool wanted): sinAmplitude{BASE_SIN_AMPLITUDE} ,isMenuTarget{false}
{
	setPos(x, y);
	setMoveType(movetype);
	setWanted(wanted);
	dir = direction;
	speed = BASE_SPEED;

	if (movetype == DIRECTION_4)
	{
		if (dir == Down) setAngle(3 * PI / 2);
		else if (dir == Left) setAngle(PI);
		else if (dir == Up) setAngle(PI / 2);
		else setAngle(0);
	}
}

target::target(float x, float y, MoveAI movetype, double angle,bool wanted): sinAmplitude{BASE_SIN_AMPLITUDE} ,isMenuTarget{false}
{
	setPos(x,y);
	setMoveType(movetype);
	setWanted(wanted);

	setAngle(angle);

	speed = BASE_SPEED;
}

target::target(float x, float y, MoveAI movetype, double angle,float speed): sinAmplitude{BASE_SIN_AMPLITUDE} ,isMenuTarget{false}
{
	setPos(x,y);
	setMoveType(movetype);
	setWanted(false);

	setAngle(angle);
	setSpeed(speed);
}

target::target(Vector2 pos, MoveAI movetype, double angle,float speed): sinAmplitude{BASE_SIN_AMPLITUDE} ,isMenuTarget{false}
{
	setPos(pos);
	setMoveType(movetype);
	setWanted(false);

	setAngle(angle);
	setSpeed(speed);
}

target::target(Vector2 pos, MoveAI movetype, double angle,float speed, Rectangle spriteRect): sinAmplitude{BASE_SIN_AMPLITUDE} ,isMenuTarget{false}
{
	setPos(pos);
	setMoveType(movetype);
	setWanted(false);

	setAngle(angle);
	setSpeed(speed);
	setSpriteRect(spriteRect);
}

target::target(Vector2 pos, target_template t): sinAmplitude{BASE_SIN_AMPLITUDE} ,isMenuTarget{false}
{
	setPos(pos);
	setMoveType(t.movetype);
	setWanted(false);

	setAngle(t.angle);
	setSpeed(t.speed);

	setSpriteRect(t.rect);
}

target::target(Vector2 pos, Rectangle spriteRect): sinAmplitude{BASE_SIN_AMPLITUDE} ,isMenuTarget{false}
{
	setPos(pos);

	setMoveType(STATIONARY);
	setWanted(false);

	setAngle(0);
	setSpeed(0);

	setSpriteRect(spriteRect);
}
/*}#*/

// method definitions for class target/*#{*/

	// ACCESS METHODS

// SET METHODS
void target::setMoveType(MoveAI movetype)
{
	movementType = movetype;
}

void target::setPos(int x, int y)
{
	setPos(static_cast<float>(x),static_cast<float>(y));
}

void target::setPos(double x, double y)
{
	setPos(static_cast<float>(x),static_cast<float>(y));
}

void target::setPos(float x, float y)
{
	position = Vector2{x, y};
}

void target::setPos(Vector2 pos)
{
	setPos(pos.x,pos.y);
}

void target::setWanted(bool wanted)
{
	_isWanted = wanted;
}

void target::setAsMenuTarget()
{
	isMenuTarget = true;
}

void target::setAngle(double angle_radians)
{
	angle = angle_radians;

	if (angle < 0) angle = 2 * PI - std::abs(angle);

	while (angle > 2 * PI)
	{
		angle -= 2 * PI;
	}
}

void target::setAngleDegrees(int degrees)
{
	// to handle negative angles
	if (degrees < 0)
	{
		degrees = 360 - (std::abs(degrees) % 360);
	}

	// convert to radians
	angle = (degrees % 360) * PI / 180;
}

void target::setSpeed(float speed)
{
	this->speed = speed;
}

void target::setSpriteRect(Rectangle rect)
{
	this->spriteRect = rect;
}

// GET METHODS

MoveAI target::getMoveType() const
{
	return movementType;
}

Vector2 target::getCenter() const
{
	return Vector2{static_cast<float>(position.x + 0.5 * TARGET_HEIGHT), static_cast<float>(position.y + 0.5 * TARGET_HEIGHT)};
}

Vector2 target::getPos() const
{
	return position;
}

float target::getX() const
{
	return position.x;
}

float target::getY() const
{
	return position.y;
}

bool target::isWanted() const
{
	return _isWanted;
}

bool target::isMenu() const
{
	return isMenuTarget;
}

Rectangle target::getSpriteRect() const
{
	return spriteRect;
}

	// UPDATE METHODS


void target::move(double time)
{
	Vector2 v = Vector2{0,0};
	switch(movementType)
	{
		case DIRECTION_4:
			// don't break, instead continue to ANGLE code.
		case DIRECTION_ANGLE:
		case DIRECTION_ANGLE_W_BOUNCE:
			// x = r cos theta, y = r sin theta
			// r = speed, theta = angle
			position.x += speed * cos(angle);
			position.y -= speed * sin(angle);
			break;
		case DIRECTION_SIN:
			// first we build a vector that represents the displacement applied by sin (if the target was moving Right)
			v.x = speed;
			v.y = static_cast<float>(sinAmplitude * sin(4 * PI * time));

			position.x += speed *( v.x * cos(angle) - v.y * sin(angle));
			position.y -= speed *( v.x * sin(angle) + v.y * cos(angle));
			break;
		case STATIONARY:;
			// do nothing
	}
}

void target::wrap()
{
	int upperW = AREA_WIDTH, upperH = AREA_HEIGHT;
	if (isMenuTarget)
	{
		upperW = SCREEN_WIDTH;
		upperH = SCREEN_HEIGHT;
	}

	// // check edges		// 10 is used just as a safety blanky to protect from scary double rounding errors
	if (position.x > upperW 	+ TARGET_HEIGHT + 10)		// off right edge
		position.x = 0 - TARGET_HEIGHT;

	else if (position.x < 0			- TARGET_HEIGHT - 10)		// off left edge
		position.x = upperW + TARGET_HEIGHT;

	if (position.y > upperH + TARGET_HEIGHT + 10)		// off bottom edge
		position.y = 0 - TARGET_HEIGHT;

	else if (position.y < 0			- TARGET_HEIGHT - 10)		// off top edge
		position.y = upperH + TARGET_HEIGHT;
}

void target::bounce()
{
	Vector2 v = Vector2{0,0};
	bool flag = false;
	bool a = false;
	v.x = cos(angle);
	v.y = sin(angle);

	Vector2 center = getCenter();

	if ((a = center.x > AREA_WIDTH) || center.x < 0)
	{
		v.x *= -1;
		flag = true;
		if (a)
		{
			position.x -= speed * BOUNCE_OFFSET;
		}
		else
		{
			position.x += speed * BOUNCE_OFFSET;
		}
	}

	if ((a = center.y > AREA_HEIGHT) || center.y < 0)
	{
		v.y *= -1;
		flag = true;
		if (a)
		{
			position.y -= speed * BOUNCE_OFFSET;
		}
		else
		{
			position.y += speed * BOUNCE_OFFSET;
		}
	}

	if (flag)
	{
		double t = atan(v.y/v.x);
		if (t < 0)
		{
			if (v.x < 0)		// 2nd quadrant
				angle = PI + t;
			else				// 4th quadrant
				angle = 2 * PI + t;
		}
		else if (v.x < 0) 	// 3rd quadrant (bottom left)
			angle = PI + t;
		else				// 1st quadrant
			angle = t;
	}
}

void target::update(double time)
{
	// time is the seconds passed since the round started ( use as input to sin )
	move(time);
	if (movementType == DIRECTION_ANGLE_W_BOUNCE)
		bounce();
	else
		wrap();
}

bool operator==(const Rectangle& lhs, const Rectangle& rhs)
{
	return lhs.x == rhs.x; // && lhs.y == rhs.y;
}


// UTILITIES.HPP

double hamAngle(double angle)
{
	while (angle > PI / 2)	angle -= PI / 2;
	return angle;
}

int hamSqr(int a)
{
	return a*a;
}

/// returns the square distance between two points (square because sqrt slow af)
int hamelDistance(float x1, float y1, float x2, float y2)
{
	return hamSqr(x2 - x1) + hamSqr(y2 - y1);
}

int hamelDistance(Vector2 one, Vector2 two)
{
	return hamelDistance(one.x, one.y, two.x, two.y);
}

int hamelDistance(Vector2 one, float twoX, float twoY)
{
	return hamelDistance(one.x, one.y, twoX, twoY);
}

int hamelDistanceCenter(Vector2 one, Vector2 two)
{
	return hamelDistance(one.x + 0.5*TARGET_HEIGHT, one.y + 0.5*TARGET_HEIGHT, two.x + 0.5*TARGET_HEIGHT, two.y + 0.5*TARGET_HEIGHT);
}


// OPERATORS.HPP

bool operator==(target const& a, target const& b)
{
	return a.getX() == b.getX() && a.getY() == b.getY() && (a.isWanted() == b.isWanted());
}

bool operator<(const Vector2& v, const Rectangle& b)
{
	return (v.x > b.x && b.x + b.width > v.x) && (v.y > b.y && b.y + b.height > v.y);
}

// GAMEFUNCS.HPP

void updateTargets(std::vector<target>& targets, double time)
{
	auto iter = targets.begin();
	auto iEnd = targets.end();

	for(; iter != iEnd; iter++)
	{
		iter->update(time);
	}
}

float randomSpeed()
{
	return static_cast<float>(BASE_SPEED + (static_cast<float>(rand() % 100) / 100) - (BASE_SPEED * 0.25));
}

double randomAngle()
{
	return PI * static_cast<double>(rand() % 360) / 180;
}

float randomX()
{
	return ( rand() % (AREA_WIDTH) 	) - 0.5 * TARGET_HEIGHT;
}

float randomY()
{
	return ( rand() % (AREA_HEIGHT) ) - 0.5 * TARGET_HEIGHT;
}

Vector2 randomXY()
{
	return Vector2{randomX(),randomY()};
}

Vector2 randomXYWithConsideration(std::vector<Vector2>& spots)
{
	if (spots.size() < 1) return randomXY();

	bool flag = true;

	Vector2 x;

	while (flag)
	{
		x = randomXY();
		flag = false;

		for(Vector2 v : spots)
			if (hamelDistanceCenter(x, v) < TARGET_MINIMUM_DISTANCE)
				flag = true;
	}

	return x;
}

std::vector<Vector2> generateSpots(int num)
{
	std::vector<Vector2> spots;
	for(int i = 0; i < num; i++)
		spots.push_back(randomXYWithConsideration(spots));

	return spots;
}

/// assume the vector being passed is empty
void populateWithRandomTargets(std::vector<target>& targets, std::vector<Vector2>& spots, std::vector<target_template>& templates) // , int num)
{
	// spots has all the predetermined locations, and templates has the moveai, angle, speed, and sprite of each target type determined.

	// use std::shuffle from <algorithm> to shuffle the positions
	std::random_device rd;
	std::mt19937 g(rd());
	g.seed(std::time(0));

	// mix up the spots set
	std::shuffle(spots.begin(), spots.end(), g);

	// and the templates
	std::shuffle(templates.begin(), templates.end(), g);

	// size of spots
	int j = spots.size();

	// index of next template
	int l = 0;

	for(int i = 0; i < j; i++)
	{
		// old code from when random templates were chosen (instead of shuffling and then iterating through)
		// it turns out that shuffling then consistently iterating is much better since we pretty consistently get a uniform dist.
		//l = rand() % ((i == 0) ? 4 : 3);
		//l = rand() % 3; // after the first element

		//create the target
		// % 3 is okay here since l will always start from 0 so even though templates will initially have 4
		// it won't impact the rest of the loop
		targets.emplace_back(spots[i], templates[l++ % 3]);

		// make the first target also be the only one of it's kind, setWanted(true)
		if (i == 0)
		{
			templates.erase(templates.begin());
			targets[0].setWanted(true);
			l = 0;
		}
	}
}

std::vector<target> initializeLevel(std::vector<target>& targets, int level)
{
	// just gonna reset the list to test
	targets.clear();

	// start off by creating a std::vector of all the raylib Vector2s
	// we'll be using to represent the locations of the targets.
	std::vector<Vector2> spots;
	std::vector<target_template> templates;

	// populate with right number of targets
	if (level > 0 && level < 21)
	{
			// for each level, first collect the valid spots, then collect the templates before
			// breaking from the switch statement and using populateWithRandomTargets based on
			// how spots and templates were configured
		if (level == 1)
		{
				// Levels 1, 2, and 3 have prescribed locations (as do levels 6 and 10)
				spots.push_back(Vector2{3 * TARGET_HEIGHT, 2 * TARGET_HEIGHT});
				spots.push_back(Vector2{4 * TARGET_HEIGHT, 2 * TARGET_HEIGHT});
				spots.push_back(Vector2{3 * TARGET_HEIGHT, 3 * TARGET_HEIGHT});
				spots.push_back(Vector2{4 * TARGET_HEIGHT, 3 * TARGET_HEIGHT});

				// alternate constructor for stationary templates
				templates.emplace_back(criminalPosterRects[RECT_ONE]);
				templates.emplace_back(criminalPosterRects[RECT_TWO]);
				templates.emplace_back(criminalPosterRects[RECT_THREE]);
				templates.emplace_back(criminalPosterRects[RECT_FOUR]);
		}
		else if (level == 2)
		{
						// build a 4x4 grid in center
				for (int i = 2; i < 6; i++)
				{
					for (int j = 1; j < 5; j++)
					{
						spots.push_back(Vector2{static_cast<float>(i) * TARGET_HEIGHT, static_cast<float>(j) * TARGET_HEIGHT});
					}
				}

				templates.emplace_back(criminalPosterRects[RECT_ONE]);
				templates.emplace_back(criminalPosterRects[RECT_TWO]);
				templates.emplace_back(criminalPosterRects[RECT_THREE]);
				templates.emplace_back(criminalPosterRects[RECT_FOUR]);
		}
		else if (level == 3)
		{
						// build an 8x6 grid filling the screen
				for (int i = 0; i < 8; i++)
				{
					for (int j = 0; j < 6; j++)
					{
						spots.push_back(Vector2{static_cast<float>(i) * TARGET_HEIGHT, static_cast<float>(j) * TARGET_HEIGHT});
					}
				}

				templates.emplace_back(criminalPosterRects[RECT_ONE]);
				templates.emplace_back(criminalPosterRects[RECT_TWO]);
				templates.emplace_back(criminalPosterRects[RECT_THREE]);
				templates.emplace_back(criminalPosterRects[RECT_FOUR]);
		}
		// levels 4, 5, 7, 8, 9, 13, 15, 17. 18, 20
		else if (level == 4 || level == 5 || level == 7 || level == 8 || level == 9 || level == 13 || level == 15 || level == 17 || level == 18 || level == 20)
		{
				spots = generateSpots(4 * (12 + level));

				templates.emplace_back(criminalPosterRects[RECT_ONE]);
				templates.emplace_back(criminalPosterRects[RECT_TWO]);
				templates.emplace_back(criminalPosterRects[RECT_THREE]);
				templates.emplace_back(criminalPosterRects[RECT_FOUR]);
		}
		else if (level == 6)
		{
				// special stage so we will do the populate in here and then return targets

				// here's all the sprite choices
				std::vector<Rectangle> rects = { criminalPosterRects[0], criminalPosterRects[1], criminalPosterRects[2], criminalPosterRects[3] };

				// shuffle them
				std::random_device rd;
				std::mt19937 g(rd());
				g.seed(std::time(0));
				std::shuffle(rects.begin(), rects.end(), g);

				// pick one to be wanted
				Rectangle wRect = rects[0];
				rects.erase(rects.begin());

				Vector2 v;

				// now there's only 3 rects to chose from, which will be iterated over like in popWRTargets

				for (int i = 0; i < 8; i++)
				{
					for (int j = 0; j < 6; j++)
					{
						v = Vector2{static_cast<float>(i) * TARGET_HEIGHT, static_cast<float>(j) * TARGET_HEIGHT};

						//l = (i * 7) + j;
						double angle = ((i % 2) == 0) ? PI/2 : 3*PI/2;

						//(Vector2, MoveAI, 	double, int, 	Rectangle)
						//(pos, 	movetype, 	angle, 	speed, 	spriteRect)
						// i * 7 ranges from 0 to 7 (8 cols)
						// j ranges 0 to 5 			(6 rows)
						// angle is either 90 or 270 (up or down resp.)
						// speed is const across all targets
						targets.emplace_back(v, DIRECTION_ANGLE, angle, BASE_SPEED, rects[rand() % 3]);
					}
				}

				// choose a random index to become wanted
				int l = (rand() % 48);

				std::swap(targets[l], targets[0]);
				targets[0].setSpriteRect(wRect);

				return targets;
		}
		else if (level == 10)
		{
				// special stage so we will do the populate in here and then return targets

				// here's all the sprite choices
				std::vector<Rectangle> rects = { criminalPosterRects[0], criminalPosterRects[1], criminalPosterRects[2], criminalPosterRects[3] };

				// shuffle them
				std::random_device rd;
				std::mt19937 g(rd());
				g.seed(std::time(0));
				std::shuffle(rects.begin(), rects.end(), g);

				// pick one to be wanted
				Rectangle wRect = rects[0];
				rects.erase(rects.begin());

				Vector2 v;

				// now there's only 3 rects to chose from, which will be iterated over like in popWRTargets

				for (int i = 0; i < 8; i++)
				{
					v = Vector2{static_cast<float>(i) * TARGET_HEIGHT, -0.7 * TARGET_HEIGHT};

					//(Vector2, MoveAI, 	double, int, 	Rectangle)
					//(pos, 	movetype, 	angle, 	speed, 	spriteRect)
					// i * 7 ranges from 0 to 7 (8 cols)
					// j ranges 0 to 5 			(6 rows)
					// angle is either 90 or 270 (up or down resp.)
					// speed is const across all targets
					//targets.emplace_back(v, STATIONARY, 0, 0, rects[rand() % 3]);
					targets.emplace_back(v, rects[rand() % 3]);
				}

				// choose a random index to become wanted
				int l = (rand() % 8);

				std::swap(targets[l], targets[0]);
				targets[0].setSpriteRect(wRect);

				return targets;
		}
		else if (level == 11 || level == 16)	// DIRECTION_ANGLE
		{
			spots = generateSpots(4 * (12 + level));

			templates.emplace_back(DIRECTION_ANGLE, randomAngle(), randomSpeed(), criminalPosterRects[RECT_ONE]);
			templates.emplace_back(DIRECTION_ANGLE, randomAngle(), randomSpeed(), criminalPosterRects[RECT_TWO]);
			templates.emplace_back(DIRECTION_ANGLE, randomAngle(), randomSpeed(), criminalPosterRects[RECT_THREE]);
			templates.emplace_back(DIRECTION_ANGLE, randomAngle(), randomSpeed(), criminalPosterRects[RECT_FOUR]);
		}
		else if (level == 12)	// DIRECTION_SIN (up or down)
		{
			spots = generateSpots(4 * (12 + level));

			templates.emplace_back(DIRECTION_SIN, ((rand() % 2) == 0) ? PI/2 : 3*PI/2, randomSpeed(), criminalPosterRects[RECT_ONE]);
			templates.emplace_back(DIRECTION_SIN, ((rand() % 2) == 0) ? PI/2 : 3*PI/2, randomSpeed(), criminalPosterRects[RECT_TWO]);
			templates.emplace_back(DIRECTION_SIN, ((rand() % 2) == 0) ? PI/2 : 3*PI/2, randomSpeed(), criminalPosterRects[RECT_THREE]);
			templates.emplace_back(DIRECTION_SIN, ((rand() % 2) == 0) ? PI/2 : 3*PI/2, randomSpeed(), criminalPosterRects[RECT_FOUR]);
		}
		else if (level == 14)	// DIRECTION_ANGLE_W_BOUNCE
		{
			spots = generateSpots(4 * (12 + level));

			templates.emplace_back(DIRECTION_ANGLE_W_BOUNCE, randomAngle(), randomSpeed(), criminalPosterRects[RECT_ONE]);
			templates.emplace_back(DIRECTION_ANGLE_W_BOUNCE, randomAngle(), randomSpeed(), criminalPosterRects[RECT_TWO]);
			templates.emplace_back(DIRECTION_ANGLE_W_BOUNCE, randomAngle(), randomSpeed(), criminalPosterRects[RECT_THREE]);
			templates.emplace_back(DIRECTION_ANGLE_W_BOUNCE, randomAngle(), randomSpeed(), criminalPosterRects[RECT_FOUR]);
		}
		else if (level == 19)	// DIRECTION_SIN
		{
			spots = generateSpots(4 * (12 + level));

			templates.emplace_back(DIRECTION_SIN, randomAngle(), randomSpeed(), criminalPosterRects[RECT_ONE]);
			templates.emplace_back(DIRECTION_SIN, randomAngle(), randomSpeed(), criminalPosterRects[RECT_TWO]);
			templates.emplace_back(DIRECTION_SIN, randomAngle(), randomSpeed(), criminalPosterRects[RECT_THREE]);
			templates.emplace_back(DIRECTION_SIN, randomAngle(), randomSpeed(), criminalPosterRects[RECT_FOUR]);
		}
	}
	else if (level >= 21)
	{
		// randomly choose from: (possibilities listed in Round 21+ possibilities file)
		int num = 96 + (rand() % 33); // [96, 128]
		int level_type = rand() % 80; // 100

		if (level_type < 20)
		{
		// STATIONARY
			spots = generateSpots(num);

			templates.emplace_back(criminalPosterRects[RECT_ONE]);
			templates.emplace_back(criminalPosterRects[RECT_TWO]);
			templates.emplace_back(criminalPosterRects[RECT_THREE]);
			templates.emplace_back(criminalPosterRects[RECT_FOUR]);
		}
		else if (20 <= level_type && level_type < 40)
		{
		// DIRECTION_ANGLE
			spots = generateSpots(num);

			templates.emplace_back(DIRECTION_ANGLE, randomAngle(), randomSpeed(), criminalPosterRects[RECT_ONE]);
			templates.emplace_back(DIRECTION_ANGLE, randomAngle(), randomSpeed(), criminalPosterRects[RECT_TWO]);
			templates.emplace_back(DIRECTION_ANGLE, randomAngle(), randomSpeed(), criminalPosterRects[RECT_THREE]);
			templates.emplace_back(DIRECTION_ANGLE, randomAngle(), randomSpeed(), criminalPosterRects[RECT_FOUR]);
		}
		else if (40 <= level_type && level_type < 60)
		{
		// DIRECTION_SIN
			spots = generateSpots(num);

			templates.emplace_back(DIRECTION_SIN, randomAngle(), randomSpeed(), criminalPosterRects[RECT_ONE]);
			templates.emplace_back(DIRECTION_SIN, randomAngle(), randomSpeed(), criminalPosterRects[RECT_TWO]);
			templates.emplace_back(DIRECTION_SIN, randomAngle(), randomSpeed(), criminalPosterRects[RECT_THREE]);
			templates.emplace_back(DIRECTION_SIN, randomAngle(), randomSpeed(), criminalPosterRects[RECT_FOUR]);
		}
		else if (60 <= level_type && level_type < 80)
		{
		// DIRECTION_ANGLE_W_BOUNCE
			spots = generateSpots(num);

			templates.emplace_back(DIRECTION_ANGLE_W_BOUNCE, randomAngle(), randomSpeed(), criminalPosterRects[RECT_ONE]);
			templates.emplace_back(DIRECTION_ANGLE_W_BOUNCE, randomAngle(), randomSpeed(), criminalPosterRects[RECT_TWO]);
			templates.emplace_back(DIRECTION_ANGLE_W_BOUNCE, randomAngle(), randomSpeed(), criminalPosterRects[RECT_THREE]);
			templates.emplace_back(DIRECTION_ANGLE_W_BOUNCE, randomAngle(), randomSpeed(), criminalPosterRects[RECT_FOUR]);
		}
		/*
		else if (80 <= level_type && level_type < 85)
		{

		}
		else if (85 <= level_type && level_type < 90)
		{

		}
		else if (90 <= level_type && level_type < 90)
		{

		}
		else if (95 <= level_type && level_type < 100)
		{

		} */
	}

	populateWithRandomTargets(targets, spots, templates);

	std::cout << "> LEVEL : " << level << std::endl;

	// return the list just for fun
	return targets;
}

void DrawTarget(target t, Texture2D facesSmallTexture)
{
	#ifndef USE_DEV_SQUARES
	// use faces.png
	if (t.isMenu())
		DrawTextureRec(facesSmallTexture, t.getSpriteRect(), t.getPos(), BLACK);
	else
		DrawTextureRec(facesSmallTexture, t.getSpriteRect(), t.getPos(), WHITE);
	#else
	// dev textures
	Color col;
	if (t.getSpriteRect() == criminalPosterRects[0]) // RECT_ONE
		col = LIME;
	else if (t.getSpriteRect() == criminalPosterRects[1]) // RECT_TWO
		col = RED;
	else if (t.getSpriteRect() == criminalPosterRects[2]) // RECT_THREE
		col = ORANGE;
	else //if (t->getSpriteRect() == criminalPosterRects[3]) // RECT_FOUR
		col = GOLD;

	DrawRectangle(t.getX(), t.getY(), 100, 100, col);
	#endif
}

Rectangle GetDigitRect(int digit)
{
	return Rectangle{470 + (digit * 30), 100, 30, 30};
}

void tickSeconds(int& seconds, int& frames)
{
	frames--;
	if (frames <= 1)
	{
		if (seconds <= 0)
		{
			frames = 0;
		}
		else
		{
			seconds--;
			frames = FPS_TARGET;
		}
	}
}

int AlarmDuration(GAME_FLAG f)
{
	int a = FPS_TARGET;
	switch (f)
	{
		case PREROUND:
			a *= PREROUND_ALARM_DURATION;
			break;
		case DRUMROLL:
			a *= DRUMROLL_ALARM_DURATION;
			break;
		case TARGET_MISSED:
			a *= TARGET_MISSED_ALARM_DURATION;
			break;
		case TARGET_HIGHLIGHT:
			a *= TARGET_HIGHLIGHT_DURATION;
			break;
		case ROUND_BUFFER:
			a *= ROUND_BUFFER_DURATION;
			break;
		case COUNTUP:
			a *= SCORE_COUNTUP_DURATION;
			break;
		case LOSE_TIMER:
			a *= LOSE_TIMER_DURATION;
			break;
	}

	return a;
}

/// uses the digits from the atlas to display a number at a given position.
void DrawNumberAt(Texture2D atlas, int num, Vector2 center)
{
	if (num < 10)
	{
		DrawTextureRec(atlas, GetDigitRect(num), Vector2{center.x - 15, center.y + 15}, WHITE);
	}
	else if (num < 100)
	{
		DrawTextureRec(atlas, GetDigitRect(num / 10), Vector2{center.x - 30, center.y + 15}, WHITE);
		DrawTextureRec(atlas, GetDigitRect(num % 10), Vector2{center.x, center.y + 15}, WHITE);
	}
	else if (num < 1000)
	{
		DrawTextureRec(atlas, GetDigitRect(num / 100), Vector2{center.x - 45, center.y + 15}, WHITE);
		DrawTextureRec(atlas, GetDigitRect((num / 10) % 10), Vector2{center.x - 15, center.y + 15}, WHITE);
		DrawTextureRec(atlas, GetDigitRect(num % 10), Vector2{center.x + 15, center.y + 15}, WHITE);
	}
	else if (num < 10000)
	{
		DrawTextureRec(atlas, GetDigitRect(num / 1000), Vector2{center.x - 60, center.y + 15}, WHITE);
		DrawTextureRec(atlas, GetDigitRect((num / 100) % 10), Vector2{center.x - 30, center.y + 15}, WHITE);
		DrawTextureRec(atlas, GetDigitRect((num / 10) % 10), Vector2{center.x, center.y + 15}, WHITE);
		DrawTextureRec(atlas, GetDigitRect(num % 10), Vector2{center.x + 30, center.y + 15}, WHITE);
	}
}

/**
* @param atlas the sprite atlas the font will be collected from.
* @param num the number represented by the digits drawn
* @param center the center of the left-most digit
*/
void DrawNumberAtLeftJustified(Texture2D atlas, int num, Vector2 center)
{
	if (num < 10)
	{
		DrawTextureRec(atlas, GetDigitRect(num), Vector2{center.x - 15, center.y + 15}, WHITE);
	}
	else if (num < 100)
	{
		DrawTextureRec(atlas, GetDigitRect(num / 10), Vector2{center.x - 15, center.y + 15}, WHITE);
		DrawTextureRec(atlas, GetDigitRect(num % 10), Vector2{center.x + 15, center.y + 15}, WHITE);
	}
	else if (num < 1000)
	{
		DrawTextureRec(atlas, GetDigitRect(num / 100), Vector2{center.x - 15, center.y + 15}, WHITE);
		DrawTextureRec(atlas, GetDigitRect((num / 10) % 10), Vector2{center.x + 15, center.y + 15}, WHITE);
		DrawTextureRec(atlas, GetDigitRect(num % 10), Vector2{center.x + 45, center.y + 15}, WHITE);
	}
	else if (num < 10000)
	{
		DrawTextureRec(atlas, GetDigitRect(num / 1000), Vector2{center.x - 15, center.y + 15}, WHITE);
		DrawTextureRec(atlas, GetDigitRect((num / 100) % 10), Vector2{center.x + 15, center.y + 15}, WHITE);
		DrawTextureRec(atlas, GetDigitRect((num / 10) % 10), Vector2{center.x + 45, center.y + 15}, WHITE);
		DrawTextureRec(atlas, GetDigitRect(num % 10), Vector2{center.x + 60, center.y + 15}, WHITE);
	}
}

/// specializes DrawNumberAt to be especially for the round timer (turning the numbers red in the last 5 seconds)
void DrawTimerAt(Texture2D atlas, int num, Vector2 center)
{
	if (num < 6)
	{
		DrawTextureRec(atlas, GetDigitRect(num), Vector2{center.x - 15, center.y + 15}, MAROON);
	}
	else
		DrawNumberAt(atlas, num, center);
}

// void ResetGameFlags(std::array<int> indices)
// {
	// for(int i : indices)
	// {
		// alarm[i] = 0;
		// flags[i] = false;
	// }
// }

void ResetGameFlags()
{
	// removed GAME_IN_PLAY
	int indices[] = {GAME_IN_PLAY, PREROUND, DRUMROLL, TARGET_MISSED, TARGET_HIGHLIGHT, ROUND_BUFFER, COUNTUP, LOSE_TIMER, GAME_PAUSED, LOSE_SCREEN};
	for(int i : indices)
	{
		alarm[i] = 0;
		flags[i] = false;
	}
}

/**
* @brief selection sort for an array of 5 ints
* @param array pointer to an array of 5 ints
* @return true when it's successfully sorted (tautology)
*/
bool SortScores(unsigned short int* array)
{
	// array will always have 5 elements
	int max;
	int maxIndex;

	for(int i = 0; i < 5; i++)
	{
		max = array[i];
		maxIndex = i;

		// find the max between i and end of array
		for(int j = i + 1; j < 5; j++)
		{
			if (array[j] > max)
			{
				max = array[j];
				maxIndex = j;
			}
		}

		// swap max to i
		int temp = array[i];
		array[i] = array[maxIndex];
		array[maxIndex] = temp;
	}

	return (array[5] >= array[4] && array[4] >= array[3] && array[3] >= array[2] && array[2] >= array[1] && array[1] >= array[0]);
}

bool UpdateScores(unsigned short int* scores, unsigned short int score)
{
	for(int i = 0; i < 5; i++)
	{
		if (score > scores[i])
		{
			for(int j = 4; j > i; j--)
			{
				scores[j] = scores[j - 1];
			}
			scores[i] = score;

			// score made it into the high score table
			return true;
		}
	}

	return false;
}

void EggUpdate(bool* arr, int num)
{
	if (num == 1)
	{
		if (IsKeyPressed(KEY_N))
			arr[0] = true;
		else if (IsKeyPressed(KEY_I))
			arr[1] = true;
		else if (IsKeyPressed(KEY_C))
			arr[2] = true;
		else if (IsKeyPressed(KEY_K))
			arr[3] = true;
		else if (IsKeyPressed(KEY_T))
			arr[4] = true;
		else if (IsKeyPressed(KEY_O))
			arr[5] = true;
		else if (IsKeyPressed(KEY_B))
			arr[6] = true;
		else if (IsKeyPressed(KEY_E))
			arr[7] = true;
		else if (IsKeyPressed(KEY_R))
			arr[8] = true;
	}
	else if (num == 2)
	{
		if (IsKeyPressed(KEY_P))
			arr[0] = true;
		else if (IsKeyPressed(KEY_O))
			arr[1] = true;
		else if (IsKeyPressed(KEY_G) && !arr[2])
			arr[2] = true;
		else if (IsKeyPressed(KEY_G) && arr[2])
			arr[3] = true;
		else if (IsKeyPressed(KEY_E))
			arr[4] = true;
		else if (IsKeyPressed(KEY_R))
			arr[5] = true;
		else if (IsKeyPressed(KEY_S))
			arr[6] = true;
	}
	else if (num == 3)
	{
		if (IsKeyPressed(KEY_F))
			arr[0] = true;
		else if (IsKeyPressed(KEY_U))
			arr[1] = true;
		else if (IsKeyPressed(KEY_C))
			arr[2] = true;
		else if (IsKeyPressed(KEY_K))
			arr[3] = true;
		else if (IsKeyPressed(KEY_Y))
			arr[4] = true;
		else if (IsKeyPressed(KEY_O))
			arr[5] = true;
		else if (IsKeyPressed(KEY_U) && arr[1])
			arr[6] = true;
		else if (IsKeyPressed(KEY_S))
			arr[7] = true;
		else if (IsKeyPressed(KEY_I))
			arr[8] = true;
		else if (IsKeyPressed(KEY_M))
			arr[9] = true;
		else if (IsKeyPressed(KEY_O) && arr[5])
			arr[10] = true;
		else if (IsKeyPressed(KEY_N))
			arr[11] = true;
	}
}

bool EggCheck(bool* arr, int num)
{
	bool a = true;

	if (num == 1)
	{
		for(int i = 0; i < 9; i++)
			a &= arr[i];

		return a;
	}
	else if (num == 2)
	{
		for(int i = 0; i < 7; i++)
			a &= arr[i];

		return a;
	}

	return false;
}


Rectangle RECT_BUTTON[] = { Rectangle{SCREEN_WIDTH/2 - 128,	SCREEN_HEIGHT - 304 - 38, 256, 76}, Rectangle{SCREEN_WIDTH/2 - 128, AREA_HEIGHT/2 - 38, 256, 76} , Rectangle{SCREEN_WIDTH/2 - 128, SCREEN_HEIGHT - 228, 256, 76},
							Rectangle{SCREEN_WIDTH - 80 - 8, AREA_HEIGHT + 8, 80, 80}, Rectangle{SCREEN_WIDTH - 256 - 8, SCREEN_HEIGHT - 76 - 8, 256, 76}, Rectangle{8, SCREEN_HEIGHT - 76 - 8, 256, 76},
						 Rectangle{8 + 16, AREA_HEIGHT + 8, 50, 80} };

//unsigned short int scores[5] = {1, 2, 3 ,32767, 32768};

// BUTTON.HPP
/// specializes ImageButtonEx by offering the chance to include two source rects: one for what should ordinarily be shown
/// and an alternate for what should be shown when hovering.
/// @param bounds The bounds of the button {x,y,width,height} where x,y denote the top left corner
/// @param atlas The Texture2D sprite atlas the button image will come from.
/// @param source The Rectangle describing the portion of the atlas containing the sprite.
/// @param altSource Same as above, but for the alternate sprite that the button will switch to when hovered.
/// @returns True iff the button is clicked on this frame.
bool ImageButtonSpriteSwap(Rectangle bounds, Texture2D atlas, Rectangle source, Rectangle altSource)
{
	Vector2 m = Vector2{GetMouseX(), GetMouseY()};
	Color c = WHITE;
	bool q = false;

	Rectangle src = source;

	if (m < bounds)
	{
		// change atlas source for button
		src = altSource;

		if (IsMouseButtonPressed(MOUSE_LEFT_BUTTON))
		{
			c = DARKBLUE;
			q = true;
		}
	}

	// draw the button
	DrawTextureRec(atlas, src, Vector2{bounds.x,bounds.y}, c);

	return q;
}

/// A replacement for raygui::GuiImageButtonEx (which was not very good at all).
/// @param bounds The bounds of the button {x,y,width,height} where x,y denote the top left corner
/// @param atlas The Texture2D sprite atlas the button image will come from.
/// @param source The Rectangle describing the portion of the atlas containing the sprite.
/// @returns True iff the button is clicked on this frame.
bool ImageButtonEx(Rectangle bounds, Texture2D atlas, Rectangle source)
{
	Vector2 m = Vector2{GetMouseX(), GetMouseY()};
	Color c = WHITE;
	bool q = false;

	if (m < bounds)
	{
		// change render color of button
		c = SKYBLUE;

		if (IsMouseButtonPressed(MOUSE_LEFT_BUTTON))
		{
			c = DARKBLUE;
			q = true;
		}
	}

	// draw the button
	DrawTextureRec(atlas, source, Vector2{bounds.x,bounds.y}, c);

	return q;
}

/// A simplified function only useable when buttons are associated with a BUTTON_ID
/// @param atlas The Texture2D sprite atlas the button image will come from.
/// @param b_id The BUTTON_ID enum value corresponding to the button.
/// @returns True iff the button is clicked on this frame.
bool ImageButton(Texture2D atlas, BUTTON_ID b_id)
{
	Rectangle bounds;
	Rectangle src;

	if (b_id == PLAY)
	{
		bounds = RECT_BUTTON[PLAY];
		src = RECT_PLAY;
	}
	else if (b_id == PLAY_AGAIN)
	{
		bounds = RECT_BUTTON[PLAY_AGAIN];
		src = RECT_PLAY_AGAIN;
	}
	else if (b_id == QUIT)
	{
		bounds = RECT_BUTTON[QUIT];
		src = RECT_QUIT;
	}
	else if (b_id == MUTE)
	{
		if (flags[FLAG_MUTE])
			return ImageButtonSpriteSwap(RECT_BUTTON[MUTE], atlas, RECT_MUTED, RECT_UNMUTED);
		else
			return ImageButtonSpriteSwap(RECT_BUTTON[MUTE], atlas, RECT_UNMUTED, RECT_MUTED);
	}
	else if (b_id == PAUSE)
	{
		bounds = RECT_BUTTON[PAUSE];
		src = RECT_PAUSE;
	}
	else if (b_id == SKIP)
	{
		bounds = RECT_BUTTON[SKIP];
		Color c = LIME;
		bool q = false, p = false;

		if (p = (Vector2{GetMouseX(), GetMouseY()} < bounds))
		{
			// change render color of button
			c = WHITE;
			q = IsMouseButtonPressed(MOUSE_LEFT_BUTTON);
		}

		// draw the button
		DrawTextureRec(atlas, RECT_UNMUTED, Vector2{bounds.x,bounds.y}, c);
		if (p)
		{
			DrawText("SKIP", bounds.x + 11, bounds.y + 30, 8, RAYWHITE);
			DrawText("SONG", bounds.x + 9, bounds.y + 38, 8, RAYWHITE);
		}
		return q;
	}
	else
	{
		// do nothing
	}

	return ImageButtonEx(bounds, atlas, src);
}

/**
* @param bounds Defines the area of the hidden button.
* @return True iff the button is pressed this frame.
*/
bool HiddenButton(Rectangle bounds)
{
	return Vector2{GetMouseX(), GetMouseY()} < bounds && IsMouseButtonPressed(MOUSE_LEFT_BUTTON);
}


// FILES.HPP

void SaveScores(unsigned short int* scores)
{
	SaveStorageValue(HI_SCORE, scores[0]);
	SaveStorageValue(HI_SCORE2, scores[1]);
	SaveStorageValue(HI_SCORE3, scores[2]);
	SaveStorageValue(HI_SCORE4, scores[3]);
	SaveStorageValue(HI_SCORE5, scores[4]);
}

void LoadScores(unsigned short int* scores)
{
	scores[0] = LoadStorageValue(HI_SCORE);
	scores[1] = LoadStorageValue(HI_SCORE2);
	scores[2] = LoadStorageValue(HI_SCORE3);
	scores[3] = LoadStorageValue(HI_SCORE4);
	scores[4] = LoadStorageValue(HI_SCORE5);
}


// easter egg crap
bool keys1[9] = {false};
bool keys2[7] = {false};

int main(void)
{
	using namespace std;


	// Initialization
	//--------------------------------------------------------------------------------------

	LoadScores(scores);
	SortScores(scores);

	std::srand(std::time(0));

	InitWindow(SCREEN_WIDTH, SCREEN_HEIGHT, "THE MANY FACES OF NICKTOBER! (NICK GAME NICK)");
	InitAudioDevice();

	// Window Settings

	// Game Initialization
	unsigned short int level = 0; // 0 is menu, 1+ are actual levels

	// 	// Game Containers
	vector<target> allTargets;
	vector<target> menuTargets;

	//probably both to be deprecated
	target dummyTarget = target(0,0,STATIONARY,false);
	target selectedTarget = dummyTarget;

	Vector2 highlightTextOrigin;

	bool pauseFlag = false;			// TODO To Be Implemented

	// Timer variables	TODO deprecate in favor of alarms
	double highlightStart = 0;
	double timeLevelStart = 0;

	// Start menu targets
	for (int i = 0; i < 4; i++)
	{
		Vector2 v1 = Vector2{25, (static_cast<float>(i + 1) / 4) * SCREEN_HEIGHT};
		Vector2 v2 = Vector2{SCREEN_WIDTH - 125, SCREEN_HEIGHT - v1.y - 100};

		target t1 = target(v1, DIRECTION_ANGLE, 3*PI/2, BASE_SPEED/2, criminalPosterRects[i]);
		target t2 = target(v2, DIRECTION_ANGLE, PI/2, 	BASE_SPEED/2, criminalPosterRects[i]);

		t1.setAsMenuTarget();
		t2.setAsMenuTarget();

		menuTargets.push_back(t1);
		menuTargets.push_back(t2);
	}

	// Start Menu UI
	bool buttonStart 	= false;
	bool buttonQuit 	= false;

	// Lose Menu UI
	bool buttonReturnToMenu = false;

	//Rectangle buttonStartRect{SCREEN_WIDTH/2 - 128,	SCREEN_HEIGHT - 304, 256, 76};
	Rectangle buttonQuitRect{SCREEN_WIDTH/2 - 128, SCREEN_HEIGHT - 228, 256, 76};


	// Pause Menu UI
	bool buttonResume = false;
	bool buttonMenu_Pause = false;

	Rectangle buttonResumeRect{SCREEN_WIDTH/3, SCREEN_HEIGHT/3, 200, 150};
	Rectangle buttonMenu_PauseRect{SCREEN_WIDTH/3, 2*SCREEN_HEIGHT/3, 200, 150};


	// Graphics Stuff
	int textureIndex = 0;
	Texture2D atlas = LoadTexture("textures/atlas0.png");

	/// position for the vector on screen
	const Vector2 posterPos = Vector2{SCREEN_WIDTH/2 - (256/2), AREA_HEIGHT};

	/// rect for the area below the play area (bottom bar)
	const Rectangle bottomBarRect = Rectangle{0,AREA_HEIGHT,SCREEN_WIDTH,SCREEN_HEIGHT - AREA_HEIGHT};


	// Audio stuff
	Music bgm[] = { LoadMusicStream("snd/bgm-01.ogg"), LoadMusicStream("snd/bgm-02.ogg"), LoadMusicStream("snd/bgm-03.ogg"), LoadMusicStream("snd/bgm-04.ogg"), LoadMusicStream("snd/bgm-05.ogg"), LoadMusicStream("snd/bgm-06.ogg"), LoadMusicStream("snd/bgm-07.ogg") };
	float bgm_length[BGM_TRACK_COUNT];

	int currentSongIndex = rand() % BGM_TRACK_COUNT;

	for(int i = 0; i < BGM_TRACK_COUNT; i++)
	{
		SetMusicLoopCount(bgm[i], 1);

		bgm_length[i] = GetMusicTimeLength(bgm[i]);
	}

	Sound one[] = { LoadSound("snd/one1.ogg"),  LoadSound("snd/one2.ogg"),  LoadSound("snd/one3.ogg")};
	Sound two[] = { LoadSound("snd/two1.ogg"), LoadSound("snd/two2.ogg")};
	Sound three[] = { LoadSound("snd/three1.ogg"), LoadSound("snd/three2.ogg")};
	Sound four[] = { LoadSound("snd/four1.ogg"), LoadSound("snd/four2.ogg"), LoadSound("snd/four3.ogg"), LoadSound("snd/four4.ogg")};

	Sound points[] = { LoadSound("snd/points-01.ogg"), LoadSound("snd/points-02.ogg"), LoadSound("snd/points-03.ogg"), LoadSound("snd/points-04.ogg") };
	Sound miss[] = { LoadSound("snd/miss1.ogg"), LoadSound("snd/miss2.ogg"), LoadSound("snd/miss3.ogg")};
	Sound end[] = { LoadSound("snd/end1.ogg"), LoadSound("snd/end2.ogg")};

	Sound currentSound;

	SetTargetFPS(FPS_TARGET);               // Set framerate
	SetExitKey(KEY_BACKSPACE);

	PlayMusicStream(bgm[currentSongIndex]);

	//--------------------------------------------------------------------------------------

	bool TESTFLAG = true;

	// Main game loop
	while (!WindowShouldClose() && !buttonQuit)    // Detect window close button or ESC key#{
	{
		if (IsKeyPressed(KEY_P))// || IsKeyPressed(KEY_ESCAPE))
			flags[GAME_PAUSED] = !(flags[GAME_PAUSED]);

		if (flags[GAME_PAUSED]) continue;

		// each song has 5 seconds of silence at the end as a buffer
		if(GetMusicTimePlayed(bgm[currentSongIndex]) >= (bgm_length[currentSongIndex] - 5.0f))
		{
			StopMusicStream(bgm[currentSongIndex]);

			currentSongIndex++;
			if (currentSongIndex == BGM_TRACK_COUNT) currentSongIndex = 0;

			PlayMusicStream(bgm[currentSongIndex]);
		}
		else
			UpdateMusicStream(bgm[currentSongIndex]);


		// TICK ALARMS
		if (!pauseFlag)
		{
			for(int i =0; i < ALARMFLAG_COUNT; i++)
			{
				if (alarm[i] > 0) alarm[i]--;
			}
		}

		// CHECK ALARMS	#{
		if (flags[PREROUND] && alarm[1] == 0)
		{
			flags[PREROUND] = false;

			// resume play
			flags[GAME_IN_PLAY] = true;
		}
		if (flags[DRUMROLL] && alarm[2] == 0)
		{
			flags[DRUMROLL] = false;

			flags[PREROUND] = true;
			//alarm[PREROUND] = PREROUND_ALARM_DURATION;
			alarm[PREROUND] = AlarmDuration(PREROUND);
		}
		if (flags[TARGET_MISSED] && alarm[3] == 0)
		{
			flags[TARGET_MISSED] = false;
		}
		if (flags[TARGET_HIGHLIGHT] && alarm[4] == 0)
		{
			flags[TARGET_HIGHLIGHT] = false;

			// reset frame counter, initialize score count up
			counter[2] = FPS_TARGET;

			currentSound = points[rand() % POINTS_SFX_COUNT];
			PlaySound(currentSound);

			alarm[COUNTUP] = AlarmDuration(COUNTUP);
			flags[COUNTUP] = true;
		}	// }#
		if (flags[COUNTUP] && alarm[COUNTUP] == 0)
		{
			flags[COUNTUP] = false;
			counter[0] = 0;

			// increment level variable
			level++;

			flags[ROUND_BUFFER] = true;
			alarm[ROUND_BUFFER] = AlarmDuration(ROUND_BUFFER);

		}	// }#
		if (flags[ROUND_BUFFER] && alarm[ROUND_BUFFER] == 0)
		{
			flags[ROUND_BUFFER] = false;
			initializeLevel(allTargets, level);

			flags[PREROUND] = true;
			alarm[PREROUND] = AlarmDuration(PREROUND);
			// }
		}	// }#
		if (flags[LOSE_TIMER] && alarm[LOSE_TIMER] == 0)
		{
			UpdateScores(scores, level - 1);
			for(int i = 0; i < 5; i++)
			{
				std::cout << scores[i] << " ";
			}
			std::cout << std::endl;

			flags[LOSE_TIMER] = false;

			currentSound = end[rand() % 2];
			PlaySound(currentSound);
			flags[LOSE_SCREEN] = true;
		}	// }#

		// Update
		//----------------------------------------------------------------------------------
		// TODO: Update your variables here
		//----------------------------------------------------------------------------------

		// PAUSE UPDATE
		if (pauseFlag)
		{

		}
		else if (flags[COUNTUP])
		{
			if (((alarm[COUNTUP] % (FPS_TARGET/12)) == 0) && counter[0] < 5)
			{
				counter[0]++;
				counter[1]++;
			}

			if (counter[1] > 99) counter[1] = 99;
		}
		// PREROUND UPDATE
		else if (flags[PREROUND])
		{

		}
		// DRUMROLL UPDATE
		else if (flags[DRUMROLL])
		{

		}
		else if (buttonStart)
		{
			buttonStart = false;

			// initalize first level
			level = FIRST_LEVEL;

			// start the timer
			counter[1] = INITIAL_SECONDS;
			counter[2] = FPS_TARGET;

			flags[LOSE_SCREEN] = false;
			flags[GAME_IN_PLAY] = false;
			flags[ROUND_BUFFER] = true;
			alarm[ROUND_BUFFER] = AlarmDuration(ROUND_BUFFER);
		}
		else if (buttonReturnToMenu)
		{
			buttonReturnToMenu = false;

			level = 0;
			pauseFlag = false;
			for(int i = 0; i < ALARMFLAG_COUNT; i++)
			{
				alarm[i] = 0;
				flags[i] = false;
			}
			allTargets.clear();
		}
		// MAIN UPDATE
		else
		{
			// REGULAR GAME LOGIC #{
			if (level > 0)
			{
				if (flags[GAME_IN_PLAY])
				{
					updateTargets(allTargets, GetTime() - timeLevelStart);
					tickSeconds(counter[1], counter[2]);
					if (counter[1] < 0) counter[1] = 0;
					if (counter[1] == 0 && counter[2] <= 1)
					{
						// LOSE_TIMER game

						currentSound = miss[rand() % 3];
						PlaySound(currentSound);

						flags[GAME_IN_PLAY] = false;
						flags[LOSE_TIMER] = true;
						alarm[LOSE_TIMER] = AlarmDuration(LOSE_TIMER);
					}

					if(IsMouseButtonPressed(MOUSE_LEFT_BUTTON))
					{
						Vector2 mPos{static_cast<float>(GetMouseX()),static_cast<float>(GetMouseY())};

						if (!(mPos.x > AREA_WIDTH || mPos.y > AREA_HEIGHT || mPos.x < 0 || mPos.y < 0))
						{
							if (hamelDistance(allTargets[0].getCenter(), mPos) < CLICK_RANGE)
							{
								// START HIGHLIGHT ALARM
								flags[TARGET_HIGHLIGHT] = true;

								// assign an alarm duration corresponding to the sound effect duration
								//alarm[TARGET_HIGHLIGHT] = AlarmDuration(TARGET_HIGHLIGHT);

								flags[GAME_IN_PLAY] = false;

								int i;
								float j;
								if (allTargets[0].getSpriteRect() == criminalPosterRects[RECT_ONE])
								{
									i = rand() % 3;
									switch(i)
									{
										case 0: // 3
											j = 2.814f;
											break;
										case 1: // 3
											j = 2.947f;
											break;
										case 2: // 3.5
											j = 3.490f;
									}
									currentSound = one[i];
								}
								else if (allTargets[0].getSpriteRect() == criminalPosterRects[RECT_TWO])
								{
									i = rand() % 2;
									switch(i)
									{
										case 0:
											j = 1.945f;
											break;
										case 1:
											j = 1.972f;
									}
									currentSound = two[i];
								}
								else if (allTargets[0].getSpriteRect() == criminalPosterRects[RECT_THREE])
								{
									i = rand() % 2;
									switch(i)
									{
										case 0:
											j = 3.43f;
											break;
										case 1:
											j = 2.731f;
									}
									currentSound = three[i];
								}
								else
								{
									i = rand() % 4;
									switch(i)
									{
										case 0:
											j = 0.512f;
											break;
										case 1:
											j = 0.544f;
											break;
										case 2:
											j = 2.77f;
											break;
										case 3:
											j = 2.16f;
									}
									currentSound = four[i];
								}

								alarm[TARGET_HIGHLIGHT] = static_cast<int>(j * FPS_TARGET);
								std::cout << "alarm: " << alarm[TARGET_HIGHLIGHT] << std::endl;
								PlaySoundMulti(currentSound);
							}
							else	// MISSED TARGET
							{
								// find the closest target and make them flash
								auto iter = allTargets.begin() + 1;	// don't bother checking the first one (since we already did)
								auto iEnd = allTargets.end();
								int leastDistance = EXCESSIVE_DISTANCE;	// a number certainly larger than the dimensions of the window
								int d;

								// calculate square distances
								for(; iter != iEnd; iter++)
								{
									// skip element if it's outside of the click range or isn't actually a smaller distance
									if ((d = hamelDistance(iter->getCenter(), mPos.x, mPos.y)) > CLICK_RANGE || d > leastDistance) continue;

									leastDistance = d;
									highlightTextOrigin = iter->getCenter();
									selectedTarget = *iter;
								}

								// if there was an invalid target in click range, deduct time and start the TARGET_MISSED alarm
								if (leastDistance != EXCESSIVE_DISTANCE)
								{
									// remove some time
									counter[1] = (counter[1] < 10) ? (counter[2] = 0) : counter[1] - 10;

									currentSound = miss[rand() % 3];
									PlaySound(currentSound);

									flags[TARGET_MISSED] = true;
									alarm[TARGET_MISSED] = AlarmDuration(TARGET_MISSED);
								}
							}
						}
					}
				}
				else
				{
					// pause the game for some time, during which only the proper target is shown
					// and the background is changed

					// if the time has elapsed then we can move onto the next level
				}
			} //}#
			// START MENU #{
			else if (level == 0)
			{
				updateTargets(menuTargets, GetTime());

				// when the user clicks the poster it will play a sound effect and swap out the textures
				if(HiddenButton(Rectangle{AREA_WIDTH/2 - 128, AREA_HEIGHT/2 - 96 - 48, 256, 192}))
				{
					textureIndex++;
					if (textureIndex > HIGHEST_TEXTURE_INDEX) textureIndex = 0;

					char buf[30];
					std::sprintf(buf, "textures/atlas%d.png", textureIndex);
					atlas = LoadTexture(buf);
				}
			}	// }#
		}

		//----------------------------------------------------------------------------------
		// Draw
		//----------------------------------------------------------------------------------
		BeginDrawing();

			// DRAW MENU #{
			if(level == 0)
			{
				ClearBackground(NEARBLACK);
				//DrawRectangleRec(Rectangle{150,0,SCREEN_WIDTH - 300,SCREEN_HEIGHT}, BLACK);
				DrawTextureRec(atlas, posterSourceRect, Vector2{AREA_WIDTH/2 - 128, AREA_HEIGHT/2 - 96 - 48}, RAYWHITE);
				int time = static_cast<int>(GetTime()) % 12;

				// time ranges from 0-19, meaning time/5 ranges from 0-3 (so it's safe to use as an index key here)

				DrawTextureRec(atlas, posterSourceRects[time/3], Vector2{AREA_WIDTH/2 - 128 + 80, AREA_HEIGHT/2 - 96 - 48 + 19}, RAYWHITE);

				for(target t : menuTargets)
					DrawTarget(t, atlas);

				buttonStart = ImageButton(atlas, PLAY);
				buttonQuit = ImageButton(atlas, QUIT);

			}/*}#*/
			// DRAW GAME #{
			else if (level > 0)
			{
				// PAUSE DRAW
				if (pauseFlag)	// DRAW PAUSE MENU
				{
					ClearBackground(NEARBLACK);
					DrawText("PAUSED", AREA_WIDTH/2 - 100, AREA_HEIGHT/2, 30, RAYWHITE);
					if (ImageButton(atlas, PAUSE))
						pauseFlag = false;

					if (ImageButtonEx(RECT_BUTTON[PAUSE_QUIT], atlas, RECT_QUIT))
					{
						UpdateScores(scores, level - 1);
						for(int i = 0; i < 5; i++)
						{
							std::cout << scores[i] << " ";
						}
						std::cout << std::endl;

						pauseFlag = false;

						// reset any game state flags and alarms
						ResetGameFlags();
						counter[1] = counter[2] = 0;

						currentSound = end[rand() % 2];
						PlaySound(currentSound);

						flags[LOSE_SCREEN] = true;

						allTargets.clear();
					}
				}
				// LOSE DRAW
				else if (flags[LOSE_SCREEN])
				{
					ClearBackground(NEARBLACK);
					int myScore = level - 1;
					bool _p = true;

					// HIGH SCORE text
					DrawTextureRec(atlas, RECT_HIGHSCORE, Vector2{AREA_WIDTH/2 - 72, 50}, WHITE);

					for(int i = 0; i < 5; i++)
					{
						// draw stars & the times symbol
						DrawTextureRec(atlas, RECT_STAR,  Vector2{AREA_WIDTH/2 - 75, (2*i + 3)*(SCREEN_HEIGHT - 228)/15 + 15}, WHITE);
						DrawTextureRec(atlas, RECT_TIMES, Vector2{AREA_WIDTH/2 - 45,  (2*i + 3)*(SCREEN_HEIGHT - 228)/15 + 15}, WHITE);

						// draw the scores using DrawNumberAt
						DrawNumberAtLeftJustified(atlas, scores[i], Vector2{AREA_WIDTH/2, (2*i + 3)*(SCREEN_HEIGHT - 228)/15});

						if (_p && myScore == scores[i])
						{
							_p = false;
							int xOff = 20;
							if (myScore > 999) xOff += 30;
							if (myScore > 99) xOff += 30;
							if (myScore > 9) xOff += 30;
							DrawTextureRec(atlas, RECT_POG_MINI, Vector2{AREA_WIDTH/2 + xOff, (2*i + 3)*(SCREEN_HEIGHT - 228)/15 + 15}, WHITE);
						}
					}

					buttonStart = ImageButtonEx(Rectangle{SCREEN_WIDTH/2 - 128, SCREEN_HEIGHT - 76*3, 256, 76}, atlas, RECT_PLAY_AGAIN);
					buttonReturnToMenu = ImageButtonEx(Rectangle{SCREEN_WIDTH/2 - 128, SCREEN_HEIGHT - 76*2, 256, 76}, atlas, RECT_QUIT);
				}
				// PREROUND DRAW
				else if (flags[PREROUND])
				{
					ClearBackground(NEARBLACK);
					Rectangle r;

					if (level % 2 == 0)
					{
						r = posterSourceRects[0];
						r.x -= 768.0f * static_cast<float>(static_cast<float>(alarm[PREROUND]) / static_cast<float>(PREROUND_ALARM_DURATION)) / (FPS_TARGET/6);
					}
					else
					{
						r = posterSourceRects[3];
						r.x += 768.0f * static_cast<float>(static_cast<float>(alarm[PREROUND]) / static_cast<float>(PREROUND_ALARM_DURATION)) / (FPS_TARGET/6);
					}

					DrawRectangleRec(bottomBarRect, NEARBLACK);
					DrawTextureRec(atlas, posterSourceRect, posterPos, WHITE);
					DrawTextureRec(atlas, r, Vector2{posterPos.x + 80,posterPos.y + 19}, WHITE);
				}
				// BETWEEN ROUNDS, DRAW
				else if (flags[ROUND_BUFFER])	// Show name of the round
				{
					ClearBackground(NEARBLACK);
					DrawTextureRec(atlas, RECT_LEVEL, Vector2{SCREEN_WIDTH/2 - 75, AREA_HEIGHT/2 - 15}, WHITE);
					DrawNumberAt(atlas, level, Vector2{SCREEN_WIDTH/2, AREA_HEIGHT/2 + 8});
				}
				// DRUMROLL DRAW	NOT YET IMPLEMENTED AND MIGHT BE DEPRECATED, BREAKS FLOW ALOT
				else if (flags[DRUMROLL])
				{}
				// MAIN DRAW
				else
				{
					if (flags[LOSE_TIMER] || alarm[3] > 0)//0.5 * AlarmDuration(TARGET_MISSED))	// red background
						ClearBackground(MAROON);
					else if (flags[TARGET_HIGHLIGHT] || flags[COUNTUP])												// yellow
						ClearBackground(MUSTARD);
					else
						ClearBackground(NEARBLACK);

					// draw all targets
					if (flags[GAME_IN_PLAY])
					{
						auto iter = allTargets.begin();
						auto iEnd = allTargets.end();
						for(; iter != iEnd; iter++)
						{
							if (*iter == selectedTarget && flags[TARGET_MISSED]) continue;
							DrawTarget(*iter, atlas);
						}
					}
					else
					{
						DrawTarget(allTargets[0], atlas);
					}

					Rectangle r = allTargets[0].getSpriteRect();

					#ifdef USE_DS_STYLE
					// DS version (uses a faces_alt)
					// translate the source rect over to the alt side
					r.y += 316;
					#else
					// My version (uses the base faces)
					// don't change the spriterect
					#endif

					DrawRectangleRec(bottomBarRect, NEARBLACK);
					DrawTextureRec(atlas, posterSourceRect, posterPos, WHITE);
					DrawTextureRec(atlas, r, Vector2{posterPos.x + 80,posterPos.y + 19}, WHITE);

					// draw current score
					DrawTextureRec(atlas, RECT_SCORE, Vector2{posterPos.x - 32 - 70, posterPos.y + 8}, WHITE);
					DrawNumberAt(atlas, level - 1, Vector2{posterPos.x - 32 - 35, posterPos.y + 23});

					// draw time left
					DrawTextureRec(atlas, RECT_TIME, Vector2{posterPos.x + 288, posterPos.y + 8}, WHITE);
					DrawTimerAt(atlas, counter[1], Vector2{posterPos.x + 320, posterPos.y + 23});

					// draw miss text if the TARGET_MISSED still be goin
					if (flags[TARGET_MISSED])
						DrawText("-10", highlightTextOrigin.x - 32, highlightTextOrigin.y - 32, 48, RAYWHITE);

					pauseFlag = ImageButton(atlas, PAUSE);
				}

			}	// }#

			// MUTE button is always drawn on top
			if (ImageButton(atlas, MUTE))
			{
				flags[FLAG_MUTE] = !flags[FLAG_MUTE];
				if (!flags[FLAG_MUTE] && !IsMusicPlaying(bgm[currentSongIndex]))
					ResumeMusicStream(bgm[currentSongIndex]);
				else if (flags[FLAG_MUTE] && IsMusicPlaying(bgm[currentSongIndex]))
					PauseMusicStream(bgm[currentSongIndex]);
			}

			if (ImageButton(atlas, SKIP))
			{
				StopMusicStream(bgm[currentSongIndex]);

				currentSongIndex++;
				if (currentSongIndex == BGM_TRACK_COUNT) currentSongIndex = 0;

				PlayMusicStream(bgm[currentSongIndex]);

				if (flags[FLAG_MUTE])
					PauseMusicStream(bgm[currentSongIndex]);
			}

		EndDrawing();
		//----------------------------------------------------------------------------------
	}/*}#*/

	// De-Initialization
	//--------------------------------------------------------------------------------------
	CloseAudioDevice();
	CloseWindow();        // Close window and OpenGL context
	//--------------------------------------------------------------------------------------

	SaveScores(scores);

	return 0;
}
