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
#define NEARBLACK CLITERAL(Color){ 30, 30, 30, 255}

#include <iostream>

bool operator<(const Vector2& v, const Rectangle& b)
{
	return (v.x > b.x && b.x + b.width > v.x) && (v.y > b.y && b.y + b.height > v.y);
}

bool Button(Rectangle bounds)
{
	Color c = WHITE;
	bool q = false;

	if (Vector2{GetMouseX(), GetMouseY()} < bounds)
	{
		// change render color of button
		c = PINK;

		if (IsMouseButtonPressed(MOUSE_LEFT_BUTTON))
		{
			c = RED;
			q = true;
		}
	}

	// draw the button
	DrawRectangleRec(bounds, c);

	return q;
}

int main(void)
{
	using namespace std;
	
	InitWindow(800, 600, "fuck!");
	InitAudioDevice();
	
	Sound currentSound = LoadSound("snd/three1.ogg");

	SetTargetFPS(144);               // Set framerate
	SetExitKey(KEY_BACKSPACE);
	
	// Main game loop
	while (!WindowShouldClose())    // Detect window close button or ESC key#{
	{
		// Update
		//----------------------------------------------------------------------------------
		// TODO: Update your variables here
		//----------------------------------------------------------------------------------


		//----------------------------------------------------------------------------------
		// Draw
		//----------------------------------------------------------------------------------
		BeginDrawing();

			ClearBackground(NEARBLACK);
			
			if (Button(Rectangle{100,100,100,75}))
			{
				PlaySound(currentSound);
			}			
			
			if (Button(Rectangle{300, 100, 100, 75}))
			{
				std::cout << IsSoundPlaying(currentSound) << std::endl;	
			}

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
