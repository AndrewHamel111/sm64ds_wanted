 /*******************************************************************************************

	RAINY DAY PROTOCOL
- form a distinction (by var name) between SourceRects and DestRects. i.e. RECT_TIME -> SOURCE_RECT_TIME

	BUGS I'M ALLOWING
- since updateTargets use GetTime() - startTime, pausing doesn't effect this so sin rounds will have some weird behaviour]

	V1.02 CHANGES
~ ACTUALLY fixed the exploit for real (and I bet this is the last time you'll hear about it)
+ added bugs

********************************************************************************************/

#define WANTED_VERSION 1.02

#include "raylib.h"
#define NEARBLACK CLITERAL(Color){ 20, 20, 20, 255}
#define MUSTARD CLITERAL(Color){ 203, 182, 51, 255}

#include "jute.h"
// included from jute
// #include <iostream>
// #include <vector>
// #include <map>
// #include <string>
// #include <sstream>
// #include <fstream>
// #include <cstring>
#include "HTTPRequest.hpp"

#include <cmath>
#include <cstdio>
#include <cstdlib>
#include <ctime>
#include <utility>
#include <random>
#include <algorithm>

#include "constants.hpp"

// SPRITE SOURCE RECTANGLES, IMPORTANT TO CHANGE IF/WHEN SOURCE SPRITE CHANGES
RayRectangle criminalPosterRects[] = { RayRectangle{76,0,100,100}, RayRectangle{332,0,100,100},RayRectangle{588,0,100,100},RayRectangle{844,0,100,100}};
RayRectangle posterSourceRects[] = { RayRectangle{76,316,100,100}, RayRectangle{332,316,100,100},RayRectangle{588,316,100,100},RayRectangle{844,316,100,100}};

RayRectangle posterSourceRect = RayRectangle{0,100, 256, 192};

RayRectangle RECT_TIME 		= RayRectangle{256, 100, 64, 16};
RayRectangle RECT_LEVEL 		= RayRectangle{320, 100, 150, 30};
RayRectangle RECT_PLAY 		= RayRectangle{256, 130, 256, 76};
RayRectangle RECT_PLAY_AGAIN 	= RayRectangle{512, 130, 256, 76};
RayRectangle RECT_QUIT 		= RayRectangle{768, 130, 256, 76};
RayRectangle RECT_NONTENDO	 	= RayRectangle{256, 206, 312, 80};
RayRectangle RECT_2020 		= RayRectangle{256, 286, 112, 30};
RayRectangle RECT_UNMUTED		= RayRectangle{568, 206, 80, 80};
RayRectangle RECT_MUTED		= RayRectangle{648, 206, 80, 80};
RayRectangle RECT_PAUSE		= RayRectangle{768, 206, 256, 76};

RayRectangle RECT_STAR			= RayRectangle{770, 100, 30, 30};
RayRectangle RECT_TIMES		= RayRectangle{770 + 30, 100, 30, 30};
RayRectangle RECT_HIGHSCORE= RayRectangle{830, 			100, 133, 15};
RayRectangle RECT_BIGSCORE= RayRectangle{700, 416, 266, 30};
RayRectangle RECT_SCORE		= RayRectangle{830 + 63, 100, 70, 15};

RayRectangle RECT_POG_MINI = RayRectangle{963, 100, 30, 30};
RayRectangle RECT_OGNICK = RayRectangle{0, 416, 100, 100};
RayRectangle RECT_UPDATE_BUTTON = RayRectangle{300, 416, 312, 80};
RayRectangle RECT_CURSOR = RayRectangle{612, 416, 88, 88};

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

// placeholder to store second counter position (used to enable skipping)
int counter_second_;

//unsigned short int scores[5] = {0};

using score_pair = std::pair<std::string, unsigned short int>;
using score_vector = std::vector<score_pair>;
using string_vector = std::vector<std::string>;

std::string player_name = "[NAME]";
char player_name_placeholder[30] = "\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0";
score_vector scores;

bool nicktoberNamesOnly = false;

std::vector<std::string> nicktober_names;

Font gameboyFont;

// VIRTUAL CURSOR //
Vector2 lastCursorPosition{0,0};
Vector2 virtualCursorPos{0,0};
bool mouseOutOfBounds = false;
int cheese_keys[] = {KEY_LEFT_SUPER, KEY_RIGHT_SUPER, KEY_LEFT_ALT, KEY_RIGHT_ALT, KEY_KB_MENU, KEY_LEFT_CONTROL, KEY_RIGHT_CONTROL};

#include "enums.hpp"
#include "target.hpp"
#include "utilities.hpp"
#include "operators.hpp"
#include "gameFuncs.hpp"

RayRectangle RECT_BUTTON[] = { RayRectangle{SCREEN_WIDTH/2 - 128,	SCREEN_HEIGHT - 304 - 38, 256, 76}, RayRectangle{SCREEN_WIDTH/2 - 128, AREA_HEIGHT/2 - 38, 256, 76} , RayRectangle{SCREEN_WIDTH/2 - 128, SCREEN_HEIGHT - 228, 256, 76},
							RayRectangle{SCREEN_WIDTH - 80 - 8, AREA_HEIGHT + 8, 80, 80}, RayRectangle{SCREEN_WIDTH - 256 - 8, SCREEN_HEIGHT - 76 - 8, 256, 76}, RayRectangle{8, SCREEN_HEIGHT - 76 - 8, 256, 76},
						 RayRectangle{8 + 16, AREA_HEIGHT + 8, 50, 80} };

#include "button.hpp"
#include "files.hpp"

// easter egg crap
bool keys1[9] = {false};
bool keys2[7] = {false};

// EASTER EGG STUFF //
// 0 - click counter, 1 - how long to show the text (ticks down), 2 - stage of easter egg
// stages of easter egg include [1] "please stop" [2] "i said stop please" [3] "last warning" [4] "fuck you simon" (game closes)
int EEcounter1[3] = {0};
bool EEbool2 = false;
Vector2 EEvector2;

int letterKeys[] = {KEY_A, KEY_B, KEY_C, KEY_D, KEY_E, KEY_F, KEY_G, KEY_H, KEY_I, KEY_J, KEY_K, KEY_L, KEY_M, KEY_N, KEY_O, KEY_P, KEY_Q, KEY_R, KEY_S, KEY_T, KEY_U, KEY_V, KEY_W, KEY_X, KEY_Y, KEY_Z} ;

bool GuiTextBoxUpdate(RayRectangle r, char* c, int fontSize, int max_field_length)
{
	static int hold_counter = 0;

	if(IsKeyDown(KEY_BACKSPACE))
	{
		if (++hold_counter > 30)
		{
			hold_counter = 0;
			strcpy(c, "\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0");
			return false;
		}
	}
	else
		hold_counter = 0;

	int field_length = strlen(c);

	// if it was actually pressed this frame
	for(int key : letterKeys)
	{
		if (IsKeyPressed(key) && field_length < max_field_length)
		{
			c[field_length] = key;
			c[field_length + 1] = '\0';
			field_length++;
		}
	}

	if (IsKeyPressed(KEY_BACKSPACE) && field_length > 0)
	{
		c[field_length - 1] = '\0';
		field_length--;
	}
	else if (IsKeyPressed(KEY_ENTER)) return true;

	return false;
}

bool GuiTextBoxDraw(RayRectangle r, char* c, int fontSize, int max_field_length)
{
	// draw button
	DrawRectangleRec(r, RAYWHITE);

	// draw TEXT
	RayDrawText(c, r.x + 10, r.y + 10, fontSize, NEARBLACK);

	return false;
}

int main(void)
{
	using namespace std;


	// Initialization
	//--------------------------------------------------------------------------------------

	ifstream is("player.data", ios::binary);
	string s;
	bool _q = false;
	if (is >> s) _q = true;

	if (!_q || s == "REDACTED")
	{
		// player name hasn't been set, so have a separate screen asking for the name before the game starts.
		// a button can bring the user back to this screen at any time
		flags[SET_NAME] = true;
	}
	else
	{
		player_name = s;
		strcpy(player_name_placeholder, s.c_str());
	}

	for(int i = 0; i < 26; i++) letterKeys[i] = KEY_A + i;

// retrieve scoreboard from server
	DownloadScores(scores);
	std::cout << scores.size() << std::endl;
// retrieve valid nicktober_names
	GetNames(nicktober_names);

// check version number w/ server
	bool needUpdate = false;
	bool drawUpdateButton = false;
	RayRectangle updateButtonRect;
	std::string downloadURL;
	CheckVersionJSON(&needUpdate, &drawUpdateButton, &updateButtonRect, &downloadURL);
	
	std::srand(std::time(0));

// create window
	InitWindow(SCREEN_WIDTH, SCREEN_HEIGHT, "THE MANY FACES OF NICKTOBER! (NICK GAME NICK)");
	
	// lock cursor to window
	DisableCursor();

// set window icon
	Image windowIcon = RayLoadImage("resources/icon.png");
	SetWindowIcon(windowIcon);

// prep audio device
	InitAudioDevice();

	// Window Settings

	// Game Initialization
	unsigned short int level = 0; // 0 is menu, 1+ are actual levels

	// 	// Game Containers
	vector<target> allTargets;
	vector<target> menuTargets;

	//probably both to be deprecated
	target selectedTarget(0,0,STATIONARY,false);

	// where to draw the -10 when a miss occurs (needed for levels with moving targets)
	Vector2 highlightTextOrigin;

	// should've fucking moved this to flags[] fuck
	bool pauseFlag = false;			
	
	// Used for UpdateTargets (time elapsed during round)
	double timeLevelStart = 0;
	
	gameboyFont = LoadFont("resources/gameboy.ttf");

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

	//RayRectangle buttonStartRect{SCREEN_WIDTH/2 - 128,	SCREEN_HEIGHT - 304, 256, 76};
	RayRectangle buttonQuitRect{SCREEN_WIDTH/2 - 128, SCREEN_HEIGHT - 228, 256, 76};


	// Pause Menu UI
	bool buttonResume = false;
	bool buttonMenu_Pause = false;

	RayRectangle buttonResumeRect{SCREEN_WIDTH/3, SCREEN_HEIGHT/3, 200, 150};
	RayRectangle buttonMenu_PauseRect{SCREEN_WIDTH/3, 2*SCREEN_HEIGHT/3, 200, 150};


	// Graphics Stuff
	int textureIndex = 0;
	Texture2D atlas = LoadTexture("resources/textures/atlas0.png");

	/// position for the vector on screen
	const Vector2 posterPos = Vector2{SCREEN_WIDTH/2 - (256/2), AREA_HEIGHT};

	/// rect for the area below the play area (bottom bar)
	const RayRectangle bottomBarRect = RayRectangle{0,AREA_HEIGHT,SCREEN_WIDTH,SCREEN_HEIGHT - AREA_HEIGHT};


	// Audio stuff
	
	std::vector<std::vector<Sound>> character_sounds;
	std::vector<std::vector<Sound>> sound_effects;
	std::vector<Music> bgm;
	Sound currentSound;
	
	std::cout << "cum" << std::endl;
	// load all the sounds from files
	LoadSounds(&character_sounds, &sound_effects, &bgm);
	
	
	// use std::shuffle from <algorithm> to shuffle the playlist
	std::random_device rd;
	std::mt19937 g(rd());
	g.seed(std::time(0));
	std::shuffle(bgm.begin(), bgm.end(), g);
	
	float bgm_length[BGM_TRACK_COUNT];
	float volumePercent = 0.33;

	int currentSongIndex = rand() % BGM_TRACK_COUNT;

	for(int i = 0; i < BGM_TRACK_COUNT; i++)
	{
		SetMusicLoopCount(bgm[i], 1);

		bgm_length[i] = GetMusicTimeLength(bgm[i]);
	}

	SetTargetFPS(FPS_TARGET);               // Set framerate
	SetExitKey(KEY_ESCAPE);

	PlayMusicStream(bgm[currentSongIndex]);

	//--------------------------------------------------------------------------------------

	bool TESTFLAG = true;
	bool name_error[3] = {false};

	// Main game loop
	while (!WindowShouldClose() && !buttonQuit)    // Detect window close button or ESC key#{
	{		
		virtualCursorPos = virtualCursorPos + (GetMousePosition() - lastCursorPosition);
		lastCursorPosition = GetMousePosition();
		mouseOutOfBounds = !(virtualCursorPos < RayRectangle{0,0,SCREEN_WIDTH, SCREEN_HEIGHT});
		
		// check for attempts to break the game
		for(int i : cheese_keys)
			if (IsKeyDown(i))
				pauseFlag = true;
		
		// CTRL centers cursor
		if(IsKeyPressed(KEY_LEFT_SHIFT) || IsKeyPressed(KEY_RIGHT_SHIFT))
			virtualCursorPos = Vector2{AREA_WIDTH/2, AREA_HEIGHT/2};
		
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
	
		SetMasterVolume(volumePercent);

		// TICK ALARMS
		if (!pauseFlag && !mouseOutOfBounds)
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

			// play points sound effect
			currentSound = sound_effects[POINTS_SND][rand() % sound_effects[POINTS_SND].size()];
			PlaySound(currentSound);

			// increment level variable
			level++;

			counter_second_ = counter[1];
			alarm[COUNTUP] = AlarmDuration(COUNTUP);
			flags[COUNTUP] = true;
		}	// }#
		if (flags[COUNTUP] && alarm[COUNTUP] == 0)
		{
			flags[COUNTUP] = false;
			counter[0] = 0;

			flags[ROUND_BUFFER] = true;
			alarm[ROUND_BUFFER] = AlarmDuration(ROUND_BUFFER);
			
			EEvector2 = Vector2{0,0};
			EEbool2 = false;
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
			UpdateScores(scores, make_pair(player_name ,level - 1));

			flags[LOSE_TIMER] = false;

			currentSound = sound_effects[LOSE_SND][rand() % sound_effects[LOSE_SND].size()];
			PlaySound(currentSound);
			flags[LOSE_SCREEN] = true;
		}	// }#

		// Update
		//----------------------------------------------------------------------------------
		// TODO: Update your variables here
		//----------------------------------------------------------------------------------

		// PAUSE UPDATE
		if (flags[SET_NAME])
		{
			if(GuiTextBoxUpdate(RayRectangle{SCREEN_WIDTH/2 - 100, 5*SCREEN_HEIGHT/6, 200, 100}, player_name_placeholder, 30, 10))
			{
				player_name = player_name_placeholder;
				if (player_name.length() > 0 && player_name.length() <= 10 && player_name != "REDACTED" && player_name != "[NAME]")
				{
					// set name
					flags[SET_NAME] = false;

					// save name to file
					ofstream os;
					os.open("player.data", ios::binary);
					os << player_name;
					os.close();
				}
				else
				{
					// tell the player what's wrong
					if (player_name.length() > 10)
						name_error[0] = true;
					else if(player_name == "REDACTED" || player_name == "[NAME]")
						name_error[1] = true;
					else
					{
						name_error[0] = name_error[1] = name_error[2] = false;
					}
				}
			}
		}
		else if (pauseFlag)
		{
			
		}
		else if (flags[COUNTUP])
		{
			if (IsMouseButtonPressed(MOUSE_LEFT_BUTTON) || IsMouseButtonPressed(MOUSE_RIGHT_BUTTON))
			{
				counter[1] = counter_second_ + 5;
				alarm[COUNTUP] = 0;
				if (counter[1] > TIME_COUNTER_MAX) counter[1] = TIME_COUNTER_MAX;
			}
			else if (((alarm[COUNTUP] % (FPS_TARGET/12)) == 0) && counter[0] < 5)
			{
				counter[0]++;
				counter[1]++;
				if (counter[1] > TIME_COUNTER_MAX) counter[1] = TIME_COUNTER_MAX;
			}
		}
		// PREROUND UPDATE
		else if (flags[PREROUND])
		{
			if (IsMouseButtonPressed(MOUSE_LEFT_BUTTON) || IsMouseButtonPressed(MOUSE_RIGHT_BUTTON))
					alarm[PREROUND] = 0;
				
				//alarm[PREROUND] = (alarm[PREROUND] <= CLICK_ALARM_ADVANCE) ? 0 : alarm[PREROUND] - CLICK_ALARM_ADVANCE;
		}
		// HIGHLIGHT
		else if (flags[TARGET_HIGHLIGHT])
		{
			if (IsMouseButtonPressed(MOUSE_LEFT_BUTTON) || IsMouseButtonPressed(MOUSE_RIGHT_BUTTON))
					alarm[TARGET_HIGHLIGHT] = 0;
				
				//alarm[PREROUND] = (alarm[PREROUND] <= CLICK_ALARM_ADVANCE) ? 0 : alarm[PREROUND] - CLICK_ALARM_ADVANCE;
		}
		// BUFFER UPDATE
		else if (flags[ROUND_BUFFER])
		{
			if (IsMouseButtonPressed(MOUSE_LEFT_BUTTON) || IsMouseButtonPressed(MOUSE_RIGHT_BUTTON))
					alarm[ROUND_BUFFER] = 0;
				
				//alarm[ROUND_BUFFER] = (alarm[ROUND_BUFFER] <= CLICK_ALARM_ADVANCE) ? 0 : alarm[ROUND_BUFFER] - CLICK_ALARM_ADVANCE;
		}
		// DRUMROLL UPDATE
		else if (flags[DRUMROLL])
		{
			if (IsMouseButtonPressed(MOUSE_LEFT_BUTTON) || IsMouseButtonPressed(MOUSE_RIGHT_BUTTON))
				alarm[DRUMROLL] = 0;
				//alarm[DRUMROLL] = (alarm[DRUMROLL] <= CLICK_ALARM_ADVANCE) ? 0 : alarm[DRUMROLL] - CLICK_ALARM_ADVANCE;
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
			ResetGameFlags();
			// for(int i = 0; i < ALARMFLAG_COUNT; i++)
			// {
				// alarm[i] = 0;
				// flags[i] = false;
			// }
			allTargets.clear();
		}
		// MAIN UPDATE
		else
		{
			// REGULAR GAME LOGIC #{
			if (level > 0)
			{
				if (flags[GAME_IN_PLAY] && !mouseOutOfBounds)
				{
					updateTargets(allTargets, GetTime() - timeLevelStart);
					tickSeconds(counter[1], counter[2], level);
					if (counter[1] < 0) counter[1] = 0;
					if (counter[1] == 0 && counter[2] <= 1)
					{
						// LOSE_TIMER game

						currentSound = sound_effects[MISS_SND][rand() % sound_effects[MISS_SND].size()];
						PlaySound(currentSound);

						flags[GAME_IN_PLAY] = false;
						flags[LOSE_TIMER] = true;
						alarm[LOSE_TIMER] = AlarmDuration(LOSE_TIMER);
					}

					if(IsMouseButtonPressed(MOUSE_LEFT_BUTTON))
					{
						Vector2 mPos{static_cast<float>(virtualCursorPos.x),static_cast<float>(virtualCursorPos.y)};

						if (!(mPos.x > AREA_WIDTH || mPos.y > AREA_HEIGHT || mPos.x < 0 || mPos.y < 0))
						{
							if (hamelDistance(allTargets[0].getCenter(), mPos) < CLICK_RANGE)
							{
								// START HIGHLIGHT ALARM
								flags[TARGET_HIGHLIGHT] = true;
								
								// determine if we're going to tell simon to fuck himself
								if (rand() % EE_FUCK_SIMON_CHANCE == 0)
								{
									EEbool2 = true;
									EEvector2 = randomXYStrict();
								}

								flags[GAME_IN_PLAY] = false;

								int i;
								float j;
								if (allTargets[0].getSpriteRect() == criminalPosterRects[RECT_ONE])
								{
									i = rand() % character_sounds[0].size();
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
									currentSound = character_sounds[0][i];
								}
								else if (allTargets[0].getSpriteRect() == criminalPosterRects[RECT_TWO])
								{
									i = rand() % character_sounds[1].size();
									switch(i)
									{
										case 0:
											j = 1.945f;
											break;
										case 1:
											j = 1.972f;
									}
									currentSound = character_sounds[1][i];
								}
								else if (allTargets[0].getSpriteRect() == criminalPosterRects[RECT_THREE])
								{
									i = rand() % character_sounds[2].size();
									switch(i)
									{
										case 0:
											j = 3.43f;
											break;
										case 1:
											j = 2.731f;
									}
									currentSound = character_sounds[2][i];
								}
								else
								{
									i = rand() % character_sounds[3].size();
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
									currentSound = character_sounds[3][i];
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

									currentSound = sound_effects[MISS_SND][rand() % sound_effects[MISS_SND].size()];
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
				pauseFlag = false;
				
				updateTargets(menuTargets, GetTime());

				Color col = RAYWHITE;

				// when the user clicks the name it brings them back to the name screen
				if(HiddenButton(RayRectangle{SCREEN_WIDTH/2 - 135, 50, 200, 30}))
				{
					strcpy(player_name_placeholder,"\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0");
					player_name = "";
					flags[SET_NAME] = true;
				}

				if (virtualCursorPos < RayRectangle{SCREEN_WIDTH/2 - 135, 50, 200, 30})
				{
					col = RED;
					RayDrawText("CLICK to change name! (please don't abuse this)", SCREEN_WIDTH/2 - 240, 90, 20, col);
				}

				DrawTextCentered(player_name_placeholder, SCREEN_WIDTH/2, 50, 40, col);

				// when the user clicks the poster it will play a sound effect
				if(HiddenButton(RayRectangle{AREA_WIDTH/2 - 128, AREA_HEIGHT/2 - 96 - 48, 256, 192}))
				{
					// play a random nick sound
					int i = rand() % 4;
					int j = rand() % character_sounds[i].size();
					currentSound = character_sounds[i][j];
					PlaySound(currentSound);
					
					EEcounter1[0]++;
					if (EEcounter1[0] > EE_POSTER_CLICK_COUNT)
					{
						EEcounter1[0] = 0;
						EEcounter1[1] = EE_POSTER_DURATION * FPS_TARGET;
						EEcounter1[2]++;
					}
				}
			}	// }#
		}

		//----------------------------------------------------------------------------------
		// Draw
		//----------------------------------------------------------------------------------
		BeginDrawing();

			// DRAW MENU #{

			if (flags[SET_NAME])
			{
				ClearBackground(NEARBLACK);
				RayDrawText("Welcome to\n\"The Many Faces of Nicktober\"\nAKA \"NICK GAME!\n\n\nTo get started you need to choose a name\nthat your scores will be listed under.\n\nPlease put a name the NICKTOBER admins can Identify you by\nand one that you'll be okay with other people seeing :eyes:", 80, 100, 20, RAYWHITE);

				GuiTextBoxDraw(RayRectangle{140, 5*SCREEN_HEIGHT/6, 520, 80}, player_name_placeholder, 63, 10);

				if (name_error[0])
					RayDrawText("Please keep the name at 10 characters or less :cowboy:", SCREEN_WIDTH/2 - 100, 5*SCREEN_HEIGHT/6 + 100, 10, PINK);
				else if (name_error[1])
					RayDrawText("I can pretty much guarantee that's not your name :fax:", SCREEN_WIDTH/2 - 100, 5*SCREEN_HEIGHT/6 + 100, 10, PINK);
				else if (name_error[2])
					RayDrawText("Placeholder error :nduronPog:", SCREEN_WIDTH/2 - 100, 5*SCREEN_HEIGHT/6 + 100, 10, PINK);
			}
			else if(level == 0)
			{
				
				if(EEcounter1[1] > 0) EEcounter1[1]--;
				if (EEcounter1[1] == 0 && EEcounter1[2] >= 4)
				{
					buttonQuit = true;
					continue;
				}
				
				ClearBackground(NEARBLACK);
				//DrawRectangleRec(RayRectangle{150,0,SCREEN_WIDTH - 300,SCREEN_HEIGHT}, BLACK);
				DrawTextureRec(atlas, posterSourceRect, Vector2{AREA_WIDTH/2 - 128, AREA_HEIGHT/2 - 96 - 48}, RAYWHITE);
				int time = static_cast<int>(GetTime()) % 12;

				// time ranges from 0-19, meaning time/5 ranges from 0-3 (so it's safe to use as an index key here)

				DrawTextureRec(atlas, posterSourceRects[time/3], Vector2{AREA_WIDTH/2 - 128 + 80, AREA_HEIGHT/2 - 96 - 48 + 19}, RAYWHITE);

				for(target t : menuTargets)
					DrawTarget(t, atlas);

				buttonStart = ImageButton(atlas, PLAY);
				if (buttonStart)
				{
					// reset poster easter egg
					EEcounter1[0] = EEcounter1[1] = EEcounter1[2] = 0;
				}
				
				if(EEcounter1[2] > 0 && EEcounter1[1] > 0)
				{
					switch(EEcounter1[2])
					{
						case 1:
							DrawTextCentered( "please stop", Vector2{100,220}, 15, RAYWHITE);
							break;
						case 2:
							DrawTextCentered( "i said stop please", Vector2{650,240}, 15, RAYWHITE);
							break;
						case 3:
							DrawTextCentered( "last warning", Vector2{100, 250}, 15, RAYWHITE);
							break;
						case 4:
							DrawTextCentered("fuck you simon", Vector2{SCREEN_WIDTH/2,350}, 50, RAYWHITE);
					}
				}
				
				buttonQuit = ImageButton(atlas, QUIT);
				
				if (drawUpdateButton && ImageButtonEx(RayRectangle{SCREEN_WIDTH/2 - 154,360, 312,80}, atlas, RECT_UPDATE_BUTTON))
				{
					OpenURL(downloadURL.c_str());
				}
				
				if(virtualCursorPos < RayRectangle{SCREEN_WIDTH/2 - 120, SCREEN_HEIGHT - 85, 200 + 40, 60})
					RayDrawText("MASTER VOLUME", SCREEN_WIDTH/2 - 90, SCREEN_HEIGHT - 105, 20, BLUE);
				SliderBarCenter(Vector2{SCREEN_WIDTH/2, SCREEN_HEIGHT - 55}, 200, &volumePercent);

			}/*}#*/
			// DRAW GAME #{
			else if (level > 0)
			{
				// PAUSE DRAW
				if (pauseFlag)	// DRAW PAUSE MENU
				{
					ClearBackground(NEARBLACK);
					DrawTextCentered("PAUSED", AREA_WIDTH/2, AREA_HEIGHT/2, 30, RAYWHITE);
					if (ImageButton(atlas, PAUSE))
						pauseFlag = false;

					if (ImageButtonEx(RECT_BUTTON[PAUSE_QUIT], atlas, RECT_QUIT))
					{
						UpdateScores(scores, make_pair(player_name, level - 1));

						pauseFlag = false;

						// reset any game state flags and alarms
						ResetGameFlags();
						counter[1] = counter[2] = 0;

						currentSound = sound_effects[LOSE_SND][rand() % sound_effects[LOSE_SND].size()];
						PlaySound(currentSound);

						flags[LOSE_SCREEN] = true;

						allTargets.clear();
					}
					
					if(virtualCursorPos < RayRectangle{SCREEN_WIDTH/2 - 120, SCREEN_HEIGHT - 85, 200 + 40, 60})
						RayDrawText("MASTER VOLUME", SCREEN_WIDTH/2 - 90, SCREEN_HEIGHT - 105, 20, BLUE);
					SliderBarCenter(Vector2{SCREEN_WIDTH/2, SCREEN_HEIGHT - 55}, 200, &volumePercent);
				}
				// LOSE DRAW
				else if (flags[LOSE_SCREEN])
				{
					ClearBackground(NEARBLACK);
					score_pair myScore = make_pair(player_name, (level > 0) ? level - 1 : 0);
					bool _p = true;

					// HIGH SCORE text
					DrawTextureRec(atlas, RECT_BIGSCORE, Vector2{AREA_WIDTH/2 - 133, 30}, WHITE);

					// display scores
					auto iter = scores.begin(), iEnd = scores.end();
					short scoreCounter = 0;
					score_pair lowestHighScore;
					bool myScoreIsInTheTopFiveFuckYes = false;

					while(iter != iEnd && scoreCounter < 5)
					{
						// use nicktober filter as needed
						if (nicktoberNamesOnly && not_in(nicktober_names, iter->first) )
						{
							iter++;
							continue;
						}

						Color col;
						if (scoreCounter == 0) col = GOLD;
						else if (scoreCounter == 1) col = Color{ 217, 238, 255, 255};	// SILVER
						else if (scoreCounter == 2) col = Color{ 153, 109, 38, 255 };	// BRONZE
						else col = LIGHTGRAY;

						// draw stars & the times symbol
						DrawTextureRec(atlas, RECT_STAR,  Vector2{AREA_WIDTH/2 - 75, (2*scoreCounter + 3)*(SCREEN_HEIGHT - 228)/18 + 15}, WHITE);
						DrawTextureRec(atlas, RECT_TIMES, Vector2{AREA_WIDTH/2 - 45,  (2*scoreCounter + 3)*(SCREEN_HEIGHT - 228)/18 + 15}, WHITE);
	
						// draw player's name
						//RayDrawText((std::to_string(myLeaderboardPosition) + ".").c_str(), 10, 14*(SCREEN_HEIGHT - 228)/18 + 15, 35, LIGHTGRAY);
						//RayDrawText(myScore.first.c_str(), 40, 14*(SCREEN_HEIGHT - 228)/18 + 15, 35, LIGHTGRAY);
						
						RayDrawText((std::to_string(scoreCounter + 1) + ".").c_str(), 10, (2*scoreCounter + 3)*(SCREEN_HEIGHT - 228)/18 + 15, 35, col);
						RayDrawText(iter->first.c_str(), 40, (2*scoreCounter + 3)*(SCREEN_HEIGHT - 228)/18 + 15, 35, col);

						// draw the scores using DrawNumberAt
						DrawNumberAtLeftJustified(atlas, iter->second, Vector2{AREA_WIDTH/2, (2*scoreCounter + 3)*(SCREEN_HEIGHT - 228)/18});

						// when it's "your" score it renders a special icon on the right
						if (myScore.first == iter->first)
						{
							myScoreIsInTheTopFiveFuckYes = true;
							
							_p = false;
							int xOff = 20;
							if (iter->second > 999) xOff += 30;
							if (iter->second > 99) xOff += 30;
							if (iter->second > 9) xOff += 30;
							DrawTextureRec(atlas, RECT_POG_MINI, Vector2{AREA_WIDTH/2 + xOff, (2*scoreCounter + 3)*(SCREEN_HEIGHT - 228)/18 + 15}, WHITE);
						}

						lowestHighScore = *iter;
						scoreCounter++;
						iter++;
					}
	
					iter = scores.begin();
					score_pair myBestScore;
					bool _q = false;
					int myLeaderboardPosition = 0;
					while(iter != iEnd && !_q)
					{
						myLeaderboardPosition++;
						if (iter->first == myScore.first)
						{
							_q = true;
							myBestScore = *iter;
						}
						iter++;
					}
					if (!_q) myBestScore = myScore;

					// if the score the player attained this round is not a top 5 draw it below the table including it's position on the LB
					if (myScore.second < lowestHighScore.second && !myScoreIsInTheTopFiveFuckYes)
					{						
						// draw stars & the times symbol (FOR BEST ROUND)
						DrawTextureRec(atlas, RECT_STAR,  Vector2{AREA_WIDTH/2 - 75, 14*(SCREEN_HEIGHT - 228)/18 + 15}, WHITE);
						DrawTextureRec(atlas, RECT_TIMES, Vector2{AREA_WIDTH/2 - 45,  14*(SCREEN_HEIGHT - 228)/18 + 15}, WHITE);

						// draw the scores using DrawNumberAt (FOR BEST ROUND)
						DrawNumberAtLeftJustified(atlas, myBestScore.second, Vector2{AREA_WIDTH/2, 14*(SCREEN_HEIGHT - 228)/18});
						
						// BEST ROUND label
						RayDrawText("BEST ROUND", 550, 14*(SCREEN_HEIGHT - 228)/18 + 20, 20, LIGHTGRAY);
					}
					
					int _xOff = (myLeaderboardPosition > 20) ? 20 : ((myLeaderboardPosition > 11) ? 15 : 0);
					int _yOff = (myScoreIsInTheTopFiveFuckYes) ? 2 : 0;
					
					// draw your name
					RayDrawText((std::to_string(myLeaderboardPosition) + ".").c_str(), 10, (14 + _yOff)*(SCREEN_HEIGHT - 228)/18 + 15, 35, LIGHTGRAY);
					RayDrawText(myScore.first.c_str(), 40 + _xOff, (14 + _yOff)*(SCREEN_HEIGHT - 228)/18 + 15, 35, LIGHTGRAY);
					
					
					// draw stars & the times symbol (for THIS ROUND)
					DrawTextureRec(atlas, RECT_STAR,  Vector2{AREA_WIDTH/2 - 75, 16*(SCREEN_HEIGHT - 228)/18 + 15}, WHITE);
					DrawTextureRec(atlas, RECT_TIMES, Vector2{AREA_WIDTH/2 - 45,  16*(SCREEN_HEIGHT - 228)/18 + 15}, WHITE);

					// draw the scores using DrawNumberAt (for THIS ROUND)
					DrawNumberAtLeftJustified(atlas, myScore.second, Vector2{AREA_WIDTH/2, 16*(SCREEN_HEIGHT - 228)/18});
					
					// THIS ROUND label;
					RayDrawText("THIS ROUND", 550, 16*(SCREEN_HEIGHT - 228)/18 + 20, 20, LIGHTGRAY);

					buttonStart = ImageButtonEx(RayRectangle{SCREEN_WIDTH/2 - 128, SCREEN_HEIGHT - 76*3, 256, 76}, atlas, RECT_PLAY_AGAIN);
					buttonReturnToMenu = ImageButtonEx(RayRectangle{SCREEN_WIDTH/2 - 128, SCREEN_HEIGHT - 76*2, 256, 76}, atlas, RECT_QUIT);

					// show full leaderboard button
					Color _col = RAYWHITE;
					if (virtualCursorPos < RayRectangle{SCREEN_WIDTH/2 - 200, SCREEN_HEIGHT - 50, 400, 40}) _col = SKYBLUE;
					RayDrawTextEx(gameboyFont, "SEE FULL LEADERBOARD", Vector2{SCREEN_WIDTH/2 - 200, SCREEN_HEIGHT - 50}, 25, 1.0, _col);
					if (HiddenButton(RayRectangle{SCREEN_WIDTH/2 - 200, SCREEN_HEIGHT - 50, 400, 40}))
						OpenURL("http://hamel111.myweb.cs.uwindsor.ca/sm64games/wanted/leaderboard.php");

					// flavor text
					if (nicktoberNamesOnly && virtualCursorPos < RayRectangle{0,0,100,100})
						RayDrawText("SHOW ALL SCORES", 105, 80, 12, RAYWHITE);
					else if (!nicktoberNamesOnly && virtualCursorPos < RayRectangle{0,0,100,100})
						RayDrawText("SHOW NICKTOBER SCORES", 105, 80, 12, RAYWHITE);

					// toggle nicktober button
					if (nicktoberNamesOnly && ImageButtonEx(RayRectangle{0,0,100,100}, atlas, RECT_OGNICK))
						nicktoberNamesOnly = false;
					else if (!nicktoberNamesOnly && ImageButtonEx(RayRectangle{0,0,100,100}, atlas, criminalPosterRects[2]))
						nicktoberNamesOnly = true;
				}
				// PREROUND DRAW
				else if (flags[PREROUND])
				{
					ClearBackground(NEARBLACK);
					RayRectangle r;

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
				else if (mouseOutOfBounds)
				{
					ClearBackground(NEARBLACK);
					DrawTextCentered("PLEASE RETURN TO THE WINDOW", AREA_WIDTH/2, AREA_HEIGHT/2, 30, RAYWHITE);
				}
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

					RayRectangle r = allTargets[0].getSpriteRect();

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
						RayDrawText("-10", highlightTextOrigin.x - 32, highlightTextOrigin.y - 32, 48, RAYWHITE);

					if (EEbool2 && (flags[TARGET_HIGHLIGHT] || flags[COUNTUP]))
						DrawTextCentered("fuck you simon", EEvector2.x, EEvector2.y, 15, NEARBLACK);

					if(ImageButton(atlas, PAUSE))
						pauseFlag = true;
				}

			}	// }#

			// MUTE button is always drawn on top
			if (ImageButton(atlas, MUTE))
			{
				flags[FLAG_MUTE] = !flags[FLAG_MUTE];
				if (!flags[FLAG_MUTE])// && !IsMusicPlaying(bgm[currentSongIndex]))
					ResumeMusicStream(bgm[currentSongIndex]);
				else if (flags[FLAG_MUTE])// && IsMusicPlaying(bgm[currentSongIndex]))
					PauseMusicStream(bgm[currentSongIndex]);
			}
			
			// as is skip button
			if (ImageButton(atlas, SKIP))
			{
				StopMusicStream(bgm[currentSongIndex]);

				currentSongIndex++;
				if (currentSongIndex == BGM_TRACK_COUNT) currentSongIndex = 0;

				PlayMusicStream(bgm[currentSongIndex]);

				if (flags[FLAG_MUTE])
					PauseMusicStream(bgm[currentSongIndex]);
			}
			
			// Draw Virtual Cursor ALWAYS
			DrawTextureRec(atlas, RECT_CURSOR, virtualCursorPos, RAYWHITE);
			
			// If cursor is off screen, remind the player they can return it to the screen.
			if (mouseOutOfBounds)
				DrawTextCentered("PRESS SHIFT TO CENTER CURSOR", SCREEN_WIDTH/2, 40, 15, RAYWHITE);
			
			// RayDrawText((std::to_string(virtualCursorPos.x) + ", " + std::to_string(virtualCursorPos.y)).c_str(), 0, 0, 20, RAYWHITE);
			// RayDrawText(std::to_string(virtualCursorPos.x).c_str(), 0, 0, 10, RAYWHITE);
			// RayDrawText((", " + std::to_string(virtualCursorPos.y)).c_str(), 75, 0, 10, RAYWHITE);

		EndDrawing();
		//----------------------------------------------------------------------------------
	}/*}#*/

	// De-Initialization
	//--------------------------------------------------------------------------------------
	CloseAudioDevice();
	RayCloseWindow();        // Close window and OpenGL context
	//--------------------------------------------------------------------------------------

	return 0;
}
