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

- build the high scores table

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

#include "constants.hpp"

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

#include "enums.hpp"
#include "target.hpp"
#include "utilities.hpp"
#include "operators.hpp"
#include "gameFuncs.hpp"

Rectangle RECT_BUTTON[] = { Rectangle{SCREEN_WIDTH/2 - 128,	SCREEN_HEIGHT - 304 - 38, 256, 76}, Rectangle{SCREEN_WIDTH/2 - 128, AREA_HEIGHT/2 - 38, 256, 76} , Rectangle{SCREEN_WIDTH/2 - 128, SCREEN_HEIGHT - 228, 256, 76},
							Rectangle{SCREEN_WIDTH - 80 - 8, AREA_HEIGHT + 8, 80, 80}, Rectangle{SCREEN_WIDTH - 256 - 8, SCREEN_HEIGHT - 76 - 8, 256, 76}, Rectangle{8, SCREEN_HEIGHT - 76 - 8, 256, 76},
						 Rectangle{8 + 16, AREA_HEIGHT + 8, 50, 80} };

//unsigned short int scores[5] = {1, 2, 3 ,32767, 32768};

#include "button.hpp"
#include "files.hpp"

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
