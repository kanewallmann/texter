#pragma once
#include <pebble.h>
#include "constants.h"
		
typedef struct
{
	char title[24];
	char value[24];
}
SelectItem;

typedef void(*SelectHandler)( SelectItem* item, unsigned short index );

typedef struct
{
	Window* window;
	
	SelectHandler handler;
		
	SelectItem item_up;
	SelectItem item_select;
	SelectItem item_down;
	
	SelectItem item_long_up;
	SelectItem item_long_select;
	SelectItem item_long_down;
	
	TextLayer* text_item_up;
	TextLayer* text_item_select;
	TextLayer* text_item_down;
	
	TextLayer* text_item_long_up;
	TextLayer* text_item_long_select;
	TextLayer* text_item_long_down;
}
SelectWindow;
	
SelectWindow* select_window_create( );
void select_window_refresh( SelectWindow* );