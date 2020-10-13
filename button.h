#ifndef BUTTON_H_
#define BUTTON_H_

//GuiImageButtonEx(RECT_BUTTON[PAUSE], "", atlas, RECT_PAUSE);

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
	
	return ImageButtonEx(bounds, atlas, src);
}
#endif
