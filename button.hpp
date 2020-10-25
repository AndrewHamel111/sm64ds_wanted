#ifndef BUTTON_H_
#define BUTTON_H_

//GuiImageButtonEx(RECT_BUTTON[PAUSE], "", atlas, RECT_PAUSE);

/// specializes ImageButtonEx by offering the chance to include two source rects: one for what should ordinarily be shown
/// and an alternate for what should be shown when hovering.
/// @param bounds The bounds of the button {x,y,width,height} where x,y denote the top left corner
/// @param atlas The Texture2D sprite atlas the button image will come from.
/// @param source The RayRectangle describing the portion of the atlas containing the sprite.
/// @param altSource Same as above, but for the alternate sprite that the button will switch to when hovered.
/// @returns True iff the button is clicked on this frame.
bool ImageButtonSpriteSwap(RayRectangle bounds, Texture2D atlas, RayRectangle source, RayRectangle altSource)
{
	Vector2 m = virtualCursorPos;
	Color c = WHITE;
	bool q = false;

	RayRectangle src = source;

	if (m < bounds)
	{
		// change atlas source for button
		src = altSource;

		if (IsMouseButtonPressed(MOUSE_LEFT_BUTTON))
		{
			c = GRAY;
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
/// @param source The RayRectangle describing the portion of the atlas containing the sprite.
/// @returns True iff the button is clicked on this frame.
bool ImageButtonEx(RayRectangle bounds, Texture2D atlas, RayRectangle source)
{
	Vector2 m = virtualCursorPos;
	Color c = WHITE;
	bool q = false;

	if (m < bounds)
	{
		// change render color of button
		c = LIGHTGRAY;

		if (IsMouseButtonPressed(MOUSE_LEFT_BUTTON))
		{
			c = GRAY;
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
	RayRectangle bounds;
	RayRectangle src;

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

		if (p = (Vector2{virtualCursorPos.x, virtualCursorPos.y} < bounds))
		{
			// change render color of button
			c = WHITE;
			q = IsMouseButtonPressed(MOUSE_LEFT_BUTTON);
		}

		// draw the button
		DrawTextureRec(atlas, RECT_UNMUTED, Vector2{bounds.x,bounds.y}, c);
		if (p)
		{
			RayDrawText("SKIP", bounds.x + 11, bounds.y + 30, 8, RAYWHITE);
			RayDrawText("SONG", bounds.x + 9, bounds.y + 38, 8, RAYWHITE);
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
bool HiddenButton(RayRectangle bounds)
{
	return Vector2{virtualCursorPos.x, virtualCursorPos.y} < bounds && IsMouseButtonPressed(MOUSE_LEFT_BUTTON);
}

/** ordinarily returns a value between 0 and 1 OR, if scale is defined, a number between 0 and scale
* @param start Left side of slider bar
* @param end Right side of slider bar
* @param scale multiplier applied to the regular percentage
*/
float SliderBar(Vector2 start, float length, float* percent, float scale = 1.0)
{	
	bool isHovering = virtualCursorPos < RayRectangle{start.x - 20, start.y - 30, length + 40, 60};
	Color col;

	  ////////////
	//// UPDATE ////
	  ////////////
	
	if (isHovering && IsMouseButtonDown(MOUSE_LEFT_BUTTON))
	{
		col = DARKBLUE;
		
		// use mousepos to find the new percent
		*percent = ((virtualCursorPos.x - start.x) / length);
		
		// clamp percent to [0,1]
		if (*percent < 0) *percent = 0;
		else if (*percent > 1.0) *percent = 1.0;
	}
	else if (isHovering)
		col = BLUE;
	else
		col = WHITE;
	
	  //////////
	//// DRAW ////
	  //////////
	
	Vector2 handlePosition = start;
	// offset
	handlePosition.x -= SLIDER_HANDLE_HEIGHT/2;
	handlePosition.y -= SLIDER_HANDLE_HEIGHT/2;
	
	// adjust X pos to the current percentage
	handlePosition.x += (*percent) * length;
	
	// draw bar and handle
	DrawRectangleRec(RayRectangle{start.x, start.y - SLIDER_HEIGHT/2, length, SLIDER_HEIGHT}, GRAY);
	DrawRectangleRec(RayRectangle{handlePosition.x, handlePosition.y, SLIDER_HANDLE_HEIGHT, SLIDER_HANDLE_HEIGHT}, col);
	
	return (*percent) * scale;
}

/** specialization of SliderBar that takes the center of the bar instead, to take some of the weight off UI design.
* @param start Left side of slider bar
* @param end Right side of slider bar
* @param scale multiplier applied to the regular percentage
*/
float SliderBarCenter(Vector2 center, float length, float* percent, float scale = 1.0)
{	
	Vector2 v = center;
	v.x -= length/2;
	v.y -= SLIDER_HEIGHT/2;

	return SliderBar(v, length, percent, scale);
}

#endif
