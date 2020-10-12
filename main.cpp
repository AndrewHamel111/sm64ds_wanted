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

********************************************************************************************/

#include "raylib.h"
#define NEARBLACK CLITERAL(Color){ 20, 20, 20, 255}

#define RAYGUI_IMPLEMENTATION
#include "raygui.h"

#include <iostream>
#include <cmath>
#include <cstdlib>
#include <ctime>
#include <vector>
#include <utility>

//Rectangle spriteRects[] = { Rectangle{0,0,25,32}, Rectangle{25,0,28,32}, Rectangle{53,0,23,32}, Rectangle{76,0,30,32} };
// SPRITE SOURCE RECTANGLES, IMPORTANT TO CHANGE IF/WHEN SOURCE SPRITE CHANGES
Rectangle spriteRects[] = { Rectangle{0,0,100,100}, Rectangle{100,0,100,100},Rectangle{200,0,100,100},Rectangle{300,0,100,100}};
//Rectangle posterRects[] = { Rectangle{0,0,82,67}, Rectangle{82,0,82,67},Rectangle{164,0,82,67},Rectangle{246,0,82,67}};

Rectangle RECT_TIME 	= Rectangle{0, 0, 64, 16};
Rectangle RECT_LEVEL 	= Rectangle{64, 0, 150, 30};
Rectangle RECT_PLAY 	= Rectangle{0, 30, 256, 76};
Rectangle RECT_PLAY_AGAIN = Rectangle{256, 30, 256, 76};
Rectangle RECT_QUIT 	= Rectangle{512, 30, 256, 76};
Rectangle RECT_NONTENDO = Rectangle{0, 106, 312, 80};
Rectangle RECT_2020 	= Rectangle{0, 186, 112, 30};

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
	const int screenWidth = SCREEN_WIDTH;
	const int screenHeight = SCREEN_HEIGHT;

	std::srand(std::time(0));

	InitWindow(screenWidth, screenHeight, "applesauce");
	InitAudioDevice();

	// Window Settings
	//HideCursor();		// prevents the cursor from being displayed

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
	Texture2D faces 	= LoadTexture("textures/faces.png");
	Texture2D faces_alt 	= LoadTexture("textures/faces_alt.png");
	Texture2D uiTexture		= LoadTexture("textures/ui.png");
	
	Texture2D poster = LoadTexture("textures/wanted.png");
	Rectangle posterSourceRect = Rectangle{0,0, 256, 192};
	const Vector2 posterPos = Vector2{272,600};
	
	const Rectangle bottomBarRect = Rectangle{0,600,800,192};


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
	alarm[5] -	
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
	
	int secondCounter = 10;
	int frameCounter = FPS_TARGET;

	SetTargetFPS(FPS_TARGET);               // Set framerate
	SetExitKey(KEY_BACKSPACE);

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
			
			// initialize the next level
			level++;
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
			//if (alarm)
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
					level = 1;
					
					flags[GAME_IN_PLAY] = false;
					flags[DRUMROLL] = true;
					alarm[DRUMROLL] = AlarmDuration(DRUMROLL);
					initializeLevel(allTargets, level);
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
				
				buttonStart = GuiImageButtonEx(buttonStartRect, "", uiTexture, RECT_PLAY);
				//buttonOptions = GuiImageButtonEx(buttonOptionsRect, "", uiTexture, RECT_OPTIONS);
				buttonQuit = GuiImageButtonEx(buttonQuitRect, "", uiTexture, RECT_QUIT);
				
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
					// TODO implement this whole section.
					// play the animation of the slow reveal of the target
				}
				// DRUMROLL DRAW
				else if (flags[DRUMROLL])	// DON'T EXPOSE THE LOCATIONS OF THE TARGETS YET
				{

				}
				// MAIN DRAW
				else
				{
					// TODO change how this works probably
					if (alarm[3] > 0.5 * AlarmDuration(TARGET_MISSED))	// red background
						ClearBackground(MAROON);
					else												// white background
						ClearBackground(NEARBLACK);

					// draw all targets
					if (flags[GAME_IN_PLAY])
					{
						auto iter = allTargets.begin();
						auto iEnd = allTargets.end();
						for(; iter != iEnd; iter++)
						{
							DrawTarget(*iter, faces);
						}
					}
					else
					{
						DrawTarget(allTargets[0], faces);
					}
					
					Texture2D _tex;
					
					#ifdef USE_DS_STYLE
						// DS version (uses a faces_alt)
					_tex = faces_alt;
					#else
						// My version (uses the base faces)
					_tex = faces;
					#endif
					
					Rectangle r = allTargets[0].getSpriteRect();
					DrawRectangleRec(bottomBarRect, NEARBLACK);
					DrawTextureRec(poster, posterSourceRect, posterPos, WHITE); 
					DrawTextureRec(_tex, r, Vector2{posterPos.x + 80,posterPos.y + 19}, WHITE);
					
					if (flags[9])
					DrawRectangleRec(bottomBarRect, MAROON);
					
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
