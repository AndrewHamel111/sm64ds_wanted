#ifndef GAMEFUNCS_H_
#define GAMEFUNCS_H_

#include <vector>
#include <utility>
#include <cstdlib>
#include <iterator>
#include <iostream>

#include <random>
#include <algorithm>

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
		DrawTextureRec(atlas, GetDigitRect(num / 10), Vector2{center.x - 31, center.y + 15}, WHITE);
		DrawTextureRec(atlas, GetDigitRect(num % 10), Vector2{center.x + 1, center.y + 15}, WHITE);
	}
	else if (num < 1000)
	{
		DrawTextureRec(atlas, GetDigitRect(num / 100), Vector2{center.x - 47, center.y + 15}, WHITE);
		DrawTextureRec(atlas, GetDigitRect((num / 10) % 10), Vector2{center.x - 15, center.y + 15}, WHITE);
		DrawTextureRec(atlas, GetDigitRect(num % 10), Vector2{center.x + 17, center.y + 15}, WHITE);
	}
	else if (num < 10000)
	{
		DrawTextureRec(atlas, GetDigitRect(num / 1000), Vector2{center.x - 63, center.y + 15}, WHITE);
		DrawTextureRec(atlas, GetDigitRect((num / 100) % 10), Vector2{center.x - 31, center.y + 15}, WHITE);
		DrawTextureRec(atlas, GetDigitRect((num / 10) % 10), Vector2{center.x + 1, center.y + 15}, WHITE);
		DrawTextureRec(atlas, GetDigitRect(num % 10), Vector2{center.x + 33, center.y + 15}, WHITE);
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

void UpdateScores(int score)
{
	std::vector<int> v{6};
	for(int i = 0; i < 5; i++)
		v[i] = scores[i];
	
	v[5] = score;
	
	// sorts using <, so v[5] > v[4] > v[3] > v[2] > v[1] > v[0]
	// v[0] is excluded since we are only concerned with the top 5
	std::sort(v.begin(), v.end());
	for(int i = 0; i < 5; i++)
		scores[i] = v[5 - i];
}

#endif
