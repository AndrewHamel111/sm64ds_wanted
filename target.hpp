#ifndef TARGET_H_
#define TARGET_H_

// helper struct target_template used to create a consistent relationship between targets sharing a like sprite
struct target_template
{
	MoveAI movetype;
	double angle;
	float speed;
	int rect;

	target_template(int r): movetype{STATIONARY}, angle{0}, speed{0}, rect{r}
	{}

	target_template(MoveAI m, double a, float s, int r): movetype{m}, angle{a}, speed{s}, rect{r}
	{}
};

// declaration of class target/*#{*/
class target
{
	// standard attributes
	Vector2 position;		/// starting position of the target
	MoveAI movementType;
	
	Direction dir;
	double angle;
	float speed;

	bool _isWanted;			/// determines if this particular target is the win condition on the level.
	
	float sinAmplitude;
	bool isMenuTarget;		/// targets created only for display purposes, they will wrap on screen boundaries instead of area boundaries

	//float sinPeriod;
	
	// TOOD replace with SPRITE_RECT_ENUM values
	int spriteRect;
	//Rectangle spriteRect;

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
	target(Vector2 pos, MoveAI movetype, double angle,float speed, int spriteRect);

	target(Vector2 pos, target_template t);
	target(Vector2 pos, int spriteRect);

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

	void setSpriteRect(int);

	MoveAI getMoveType() const;

	Vector2 getCenter() const;
	Vector2 getPos() const;
	float getX() const;
	float getY() const;

	bool isWanted() const;
	bool isMenu() const;

	int getSpriteRect() const;
	//Rectangle getSpriteRect() const;

	// update methods
	void update(double);
};/*}#*/

// constructor definitions for class target/*#{*/
target::target(float x, float y, MoveAI movetype, bool wanted):
position{Vector2{x,y}},movementType{movetype}, _isWanted{wanted},
sinAmplitude{BASE_SIN_AMPLITUDE} ,isMenuTarget{false} {}

/**
*
*	@param x starting x position of target.
*	@param y starting y position of target.
*/
target::target(float x, float y, MoveAI movetype,Direction direction, bool wanted):
position{Vector2{x,y}}, movementType{movetype}, dir{direction}, speed{BASE_SPEED},
_isWanted{wanted}, sinAmplitude{BASE_SIN_AMPLITUDE} ,isMenuTarget{false}
{
	if (movetype == DIRECTION_4)
	{
		if (dir == Down) setAngle(3 * PI / 2);
		else if (dir == Left) setAngle(PI);
		else if (dir == Up) setAngle(PI / 2);
		else setAngle(0);
	}
}

target::target(float x, float y, MoveAI movetype, double angle,bool wanted):
sinAmplitude{BASE_SIN_AMPLITUDE} ,isMenuTarget{false}
{
	setPos(x,y);
	setMoveType(movetype);
	setWanted(wanted);

	setAngle(angle);

	speed = BASE_SPEED;
}

target::target(float x, float y, MoveAI movetype, double angle,float speed):
sinAmplitude{BASE_SIN_AMPLITUDE} ,isMenuTarget{false}
{
	setPos(x,y);
	setMoveType(movetype);
	setWanted(false);

	setAngle(angle);
	setSpeed(speed);
}

target::target(Vector2 pos, MoveAI movetype, double angle,float speed):
sinAmplitude{BASE_SIN_AMPLITUDE} ,isMenuTarget{false}
{
	setPos(pos);
	setMoveType(movetype);
	setWanted(false);

	setAngle(angle);
	setSpeed(speed);
}

target::target(Vector2 pos, MoveAI movetype, double angle,float speed, int spriteRect):
sinAmplitude{BASE_SIN_AMPLITUDE} ,isMenuTarget{false}
{
	setPos(pos);
	setMoveType(movetype);
	setWanted(false);

	setAngle(angle);
	setSpeed(speed);
	setSpriteRect(spriteRect);
}

target::target(Vector2 pos, target_template t): sinAmplitude{BASE_SIN_AMPLITUDE}
,isMenuTarget{false}
{
	setPos(pos);
	setMoveType(t.movetype);
	setWanted(false);

	setAngle(t.angle);
	setSpeed(t.speed);

	setSpriteRect(t.rect);
}

target::target(Vector2 pos, int spriteRect): sinAmplitude{BASE_SIN_AMPLITUDE}
,isMenuTarget{false}
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

void target::setSpriteRect(int rect)
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

int target::getSpriteRect() const
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

/*}#*/

#endif
