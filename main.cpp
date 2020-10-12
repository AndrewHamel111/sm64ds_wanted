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
- implement a frames variable and a seconds variable that track time.
	right answers are extra extra seconds, and at the end of each level the frames counter resets
	(so we're always round up for the player)
- form a distinction (by var name) between SourceRects and DestRects. i.e. RECT_TIME -> SOURCE_RECT_TIME

********************************************************************************************/

#include "raylib.h"
#define NEARBLACK CLITERAL(Color){ 20, 20, 20, 255}
#define MUSTARD CLITERAL(Color){ 203, 182, 51, 255}

#define RAYGUI_IMPLEMENTATION
#include "raygui.h"

#include <iostream>
#include <cmath>
#include <cstdio>
#include <cstdlib>
#include <ctime>
#include <vector>
#include <utility>

//Rectangle spriteRects[] = { Rectangle{0,0,25,32}, Rectangle{25,0,28,32}, Rectangle{53,0,23,32}, Rectangle{76,0,30,32} };
// SPRITE SOURCE RECTANGLES, IMPORTANT TO CHANGE IF/WHEN SOURCE SPRITE CHANGES
Rectangle spriteRects[] = { Rectangle{76,0,100,100}, Rectangle{332,0,100,100},Rectangle{588,0,100,100},Rectangle{844,0,100,100}};
Rectangle posterRects[] = { Rectangle{76,316,100,100}, Rectangle{332,316,100,100},Rectangle{588,316,100,100},Rectangle{844,316,100,100}};

Rectangle posterSourceRect = Rectangle{0,100, 256, 192};

Rectangle RECT_TIME 		= Rectangle{256, 100, 64, 16};
Rectangle RECT_LEVEL 		= Rectangle{320, 100, 150, 30};
//Rectangle RECT_DIGIT_ZERO 	= Rectangle{470, 100, 30, 30};
Rectangle RECT_PLAY 		= Rectangle{256, 130, 256, 76};
Rectangle RECT_PLAY_AGAIN 	= Rectangle{512, 130, 256, 76};
Rectangle RECT_QUIT 		= Rectangle{768, 130, 256, 76};
Rectangle RECT_NONTENDO	 	= Rectangle{256, 206, 312, 80};
Rectangle RECT_2020 		= Rectangle{256, 286, 112, 30};
Rectangle RECT_UNMUTED		= Rectangle{568, 206, 80, 80};
Rectangle RECT_MUTED		= Rectangle{648, 206, 80, 80};

#include "constants.h"
#include "enums.h"
#include "target.h"
#include "utilities.h"
#include "operators.h"
#include "const_strings.h"
#include "gameFuncs.h"

int main(void)
{
	using namespace std;

	// Initialization
	//--------------------------------------------------------------------------------------

	std::srand(std::time(0));

	InitWindow(SCREEN_WIDTH, SCREEN_HEIGHT, "WANTED!");
	InitAudioDevice();

	// Window Settings

	// Game Initialization
	int level = 0; // 0 is menu, -1 is settings

	// 	// Game Containers
	vector<target> allTargets;
	
	target dummyTarget = target(0,0,STATIONARY,false);
	target selectedTarget = dummyTarget;

	bool pauseFlag = false;			// TODO To Be Implemented


	// Timer variables	TODO deprecate in favor of alarms
	double highlightStart = 0;
	double timeLevelStart = 0;


	// Start Menu UI
	bool buttonStart 	= false;
	bool buttonOptions 	= false;
	bool buttonQuit 	= false;

	Rectangle buttonStartRect{SCREEN_WIDTH/2 - 128,	SCREEN_HEIGHT - 304, 256, 76};
	Rectangle buttonOptionsRect{SCREEN_WIDTH/2 - 100, 2*SCREEN_HEIGHT/4 - 75, 200, 150};
	Rectangle buttonQuitRect{SCREEN_WIDTH/2 - 128, SCREEN_HEIGHT - 228, 256, 76};

	const char * buttonStartLabel 	= "Start";
	const char * buttonOptionsLabel = "Options";
	const char * buttonQuitLabel 	= "Exit";


	// Options Menu UI
	bool buttonMenu_Options = false;

	Rectangle buttonMenu_OptionsRect{SCREEN_WIDTH/3, 3*SCREEN_HEIGHT/4, 200, 150};


	// Pause Menu UI
	bool buttonResume = false;
	bool buttonMenu_Pause = false;

	Rectangle buttonResumeRect{SCREEN_WIDTH/3, SCREEN_HEIGHT/3, 200, 150};
	Rectangle buttonMenu_PauseRect{SCREEN_WIDTH/3, 2*SCREEN_HEIGHT/3, 200, 150};


	// Graphics Stuff
	Texture2D atlas = LoadTexture("textures/atlas.png");
	
	/// position for the vector on screen
	const Vector2 posterPos = Vector2{SCREEN_WIDTH/2 - (256/2), AREA_HEIGHT};
	
	/// rect for the area below the play area (bottom bar)
	const Rectangle bottomBarRect = Rectangle{0,AREA_HEIGHT,SCREEN_WIDTH,SCREEN_HEIGHT - AREA_HEIGHT};


	// Audio stuff
	Music bgm = LoadMusicStream("snd/bgm.ogg");
	
	Sound one[] = { LoadSound("snd/one.wav")};
	Sound two[] = { LoadSound("snd/two.wav"), LoadSound("snd/two2.wav"), LoadSound("snd/two3.wav"), LoadSound("snd/two4.wav")};
	Sound three[] = { LoadSound("snd/three.wav"), LoadSound("snd/three2.wav")};
	Sound four[] = { LoadSound("snd/four.wav"), LoadSound("snd/four2.wav"), LoadSound("snd/four3.wav")};
	
	Sound points = LoadSound("snd/points.wav");

	// Alarm variables #{
	/*										flag						constant
	alarm[0] -	EMPTY ALARM
	alarm[1] -	preRoundAlarm				|	preRoundFlag			|	PREROUND_ALARM_DURATION
	alarm[2] -	drumrollAlarm				|	drumrollFlag			|	DRUMROLL_ALARM_DURATION
	alarm[3] - 	targetMissedAlarm			|	targetMissedFlag		|	TARGET_MISSED_ALARM_DURATION
	alarm[4] -	targetHighlightAlarm		|	targetHighlightFlag		|	TARGET_HIGHLIGHT_DURATION
	alarm[5] -	roundBuffer
	alarm[6] -	scoreCountupAlarm
	alarm[7] -
	alarm[8] -
	alarm[9] -
	}#		*/
	int alarm[10] = {0};	// linked with flags by enum GAME_FLAG
	
	// flags
	// preRoundFlag = false;
	// drumrollFlag = false;		// TODO To Be Implemented
	// targetMissedFlag = false;	// TODO To Be Implemented
	// targetHighlightFlag = false;
	// roundTimeDepletedFlag = false;
	// scoreCountupFlag
	bool flags[10] = {false};// linked with alarms by enum GAME_FLAG
	
	// counter[0] - used for countup
	int counter[2] = {0};
	
	int secondCounter = 10;
	int frameCounter = FPS_TARGET;

	SetTargetFPS(FPS_TARGET);               // Set framerate
	SetExitKey(KEY_BACKSPACE);
	//SetTextureWrap(atlas, WRAP_MIRROR_CLAMP);

	//PlaySound(bgm);
	PlayMusicStream(bgm);

	//--------------------------------------------------------------------------------------

	// Main game loop
	while (!WindowShouldClose() && !buttonQuit)    // Detect window close button or ESC key#{
	{
		if (IsKeyPressed(KEY_P))// || IsKeyPressed(KEY_ESCAPE))
			flags[GAME_PAUSED] = !(flags[GAME_PAUSED]);
		
		if (flags[GAME_PAUSED]) continue;
		
		
		//if (!IsSoundPlaying(bgm))	PlaySound(bgm);
		UpdateMusicStream(bgm);
		
		// TICK ALARMS
		for(int i =0; i < 10; i++)
		{
			if (alarm[i] > 0) alarm[i]--;
		}

		// CHECK ALARMS	#{
		if (flags[PREROUND] && alarm[1] == 0)
		{
			flags[PREROUND] = false;
			
			// resume play
			flags[GAME_IN_PLAY] = true;
			cout << "allTargets.size() : " << allTargets.size() << endl;
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
			// TODO puppeteer the -10 text either here or directly in draw
			flags[TARGET_MISSED] = false;
		}
		if (flags[TARGET_HIGHLIGHT] && alarm[4] == 0)
		{
			flags[TARGET_HIGHLIGHT] = false;
			
			// reset frame counter, initialize score count up
			frameCounter = FPS_TARGET;
			
			PlaySound(points);
			
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
			
			// initialize the next level
			initializeLevel(allTargets, level);
			
			// assign pre-round based on level number
			if (level % 5 == 1) 	// 5n + 1 levels get a drumroll
			{
				flags[DRUMROLL] = true;
				alarm[DRUMROLL] = AlarmDuration(DRUMROLL);
			}
			else 			// other levels get a normal buildup
			{
				flags[PREROUND] = true;
				alarm[PREROUND] = AlarmDuration(PREROUND);
			}
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
			// update score as alarm ticks down
			//int _a = static_cast<int>(SCORE_COUNTUP_DURATION * FPS_TARGET);
			//int _a = static_cast<int>(FPS_TARGET/3);
			//_a -= (_a % 5);	// now 5 | _a
			
			if (((alarm[COUNTUP] % (FPS_TARGET/12)) == 0) && counter[0] < 5)// && alarm[COUNTUP] > SCORE_COUNTUP_DURATION * 0.5)
			{
				counter[0]++;
				secondCounter++;
			}
			
			if (secondCounter > 99) secondCounter = 99;
		}
		// PREROUND UPDATE
		else if (flags[PREROUND])
		{

		}
		// DRUMROLL UPDATE
		else if (flags[DRUMROLL])
		{

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
					tickSeconds(secondCounter, frameCounter);

					if(IsMouseButtonPressed(MOUSE_LEFT_BUTTON))
					{
						
						float mx = static_cast<float>(GetMouseX());
						float my = static_cast<float>(GetMouseY());
						Vector2 mPos{mx,my};
						
						if (!(mPos.x > AREA_WIDTH || mPos.y > AREA_HEIGHT || mPos.x < 0 || mPos.y < 0))
						{
							if (hamelDistance(allTargets[0].getCenter(), mPos) < CLICK_RANGE)
							{
								// START HIGHLIGHT ALARM
								flags[TARGET_HIGHLIGHT] = true;
								alarm[TARGET_HIGHLIGHT] = AlarmDuration(TARGET_HIGHLIGHT);
								
								flags[GAME_IN_PLAY] = false;
								
								if (allTargets[0].getSpriteRect() == spriteRects[RECT_ONE])
									PlaySound(one[0]);
								else if (allTargets[0].getSpriteRect() == spriteRects[RECT_TWO])
									PlaySound(two[rand() % 4]);
								else if (allTargets[0].getSpriteRect() == spriteRects[RECT_THREE])
									PlaySound(three[rand() % 2]);
								else
									PlaySound(four[rand() % 3]);
							}
							else	// MISSED TARGET
							{
								// find the closest target and make them flash
								auto iter = allTargets.begin() + 1;	// don't bother checking the first one (since we already did)
								auto iEnd = allTargets.end();
								int leastDistance = EXCESSIVE_DISTANCE;	// a number certainly larger than the dimensions of the window
								int d;

								for(; iter != iEnd; iter++)
								{
									// skip element if it's outside of the click range or isn't actually a smaller distance
									if ((d = hamelDistance(iter->getCenter(), mx, my)) < CLICK_RANGE && d < leastDistance) continue;

									leastDistance = d;
									selectedTarget = *iter;
								}

								if (leastDistance != EXCESSIVE_DISTANCE)
								{
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
				if (buttonStart)
				{
					buttonStart = false;

					// initalize first level
					level = FIRST_LEVEL;
					
					flags[GAME_IN_PLAY] = false;
					flags[ROUND_BUFFER] = true;
					alarm[ROUND_BUFFER] = AlarmDuration(ROUND_BUFFER);
					//initializeLevel(allTargets, level);
				}
				else if (buttonOptions)
				{
					buttonOptions = false;

					level = -1;	// choose options menu
				}
				else if (buttonQuit)
				{
					buttonQuit = false;

					// TODO replace with a clean shutdown function?
					CloseWindow();
				}
			}	// }#
			else	// OPTIONS MENU #{
			{
				if (buttonMenu_Options)
				{
					buttonMenu_Options = false;
					// TODO save whatever's been changed to a file or something

					// switch back to main menu
					level = 0;
				}
			} // }#
		}

		//----------------------------------------------------------------------------------
		// Draw
		//----------------------------------------------------------------------------------
		BeginDrawing();

			// DRAW MENU #{
			if(level == 0)
			{
				ClearBackground(WHITE);
				
				//GuiSetStyle(LABEL, TEXT_ALIGNMENT, GUI_TEXT_ALIGN_CENTER);
				// buttonStart 	= GuiButton(buttonStartRect, 	buttonStartLabel);
				// buttonOptions 	= GuiButton(buttonOptionsRect, 	buttonOptionsLabel);
				// buttonQuit 		= GuiButton(buttonQuitRect, 	buttonQuitLabel);
				
				buttonStart = GuiImageButtonEx(buttonStartRect, "", atlas, RECT_PLAY);
				//buttonOptions = GuiImageButtonEx(buttonOptionsRect, "", uiTexture, RECT_OPTIONS);
				buttonQuit = GuiImageButtonEx(buttonQuitRect, "", atlas, RECT_QUIT);
				
			}/*}#*/
			/* DRAW OPTIONS MENU  #{*/	
			else if (level == -1)
			{

			}/*}#*/
			// DRAW GAME #{
			else if (level > 0)	
			{
				// PAUSE DRAW
				if (pauseFlag)	// DRAW PAUSE MENU
				{

				}
				// PREROUND DRAW
				else if (flags[PREROUND])
				{
					ClearBackground(NEARBLACK);
					Rectangle r; 
					
					if (level % 2 == 0)
					{
						r = posterRects[0];
						r.x -= 768.0f * static_cast<float>(static_cast<float>(alarm[PREROUND]) / static_cast<float>(PREROUND_ALARM_DURATION)) / (FPS_TARGET/6);
					}
					else
					{
						r = posterRects[3];
						r.x += 768.0f * static_cast<float>(static_cast<float>(alarm[PREROUND]) / static_cast<float>(PREROUND_ALARM_DURATION)) / (FPS_TARGET/6);
					}
					
					DrawRectangleRec(bottomBarRect, NEARBLACK);
					DrawTextureRec(atlas, posterSourceRect, posterPos, WHITE); 
					DrawTextureRec(atlas, r, Vector2{posterPos.x + 80,posterPos.y + 19}, WHITE);
					// TODO implement this whole section.
					// play the animation of the slow reveal of the target
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
					// TODO change how this works probably
					if (alarm[3] > 0)//0.5 * AlarmDuration(TARGET_MISSED))	// red background
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
					
					// draw time left
					DrawTextureRec(atlas, RECT_TIME, Vector2{posterPos.x + 288, posterPos.y + 8}, WHITE);
					DrawTimerAt(atlas, secondCounter, Vector2{posterPos.x + 320, posterPos.y + 23});
					
					//		secondCounter / 10 -> digit[0]
					//    	secondCounter % 10 -> digit[1]
					// DrawTextureRec(atlas, GetDigitRect(secondCounter / 10), Vector2{posterPos.x + 288, posterPos.y + 38}, WHITE);
					// DrawTextureRec(atlas, GetDigitRect(secondCounter % 10), Vector2{posterPos.x + 320, posterPos.y + 38}, WHITE);					
				}	
				
			}	// }#

		EndDrawing();
		//----------------------------------------------------------------------------------
	}/*}#*/

	// De-Initialization
	//--------------------------------------------------------------------------------------
	CloseAudioDevice();
	CloseWindow();        // Close window and OpenGL context
	//--------------------------------------------------------------------------------------

	return 0;
}
