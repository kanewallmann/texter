#include "select_window.h"
#include "loading_window.h"

void select_window_destroy( SelectWindow* mw )
{
	text_layer_destroy( mw->text_item_up );
	text_layer_destroy( mw->text_item_select );
	text_layer_destroy( mw->text_item_down );
	
	window_stack_remove( mw->window, true );	
	free( mw );
}

void select_window_back_click(ClickRecognizerRef recognizer, void *context)
{
	SelectWindow* m = (SelectWindow*)context;
	select_window_destroy( m );
	
	if( current_path > path )
	{
		current_path--;
		*current_path = 0;
	}
	
	APP_LOG(APP_LOG_LEVEL_INFO, "Select window destroyed, current path %s", path );
}

void select_window_select(ClickRecognizerRef recognizer, void *context)
{		
	SelectWindow* mw = (SelectWindow*)context;
	mw->handler( &mw->item_select, 1 );
	
	APP_LOG( APP_LOG_LEVEL_ERROR,  "Send selection select" );
}

void select_window_up(ClickRecognizerRef recognizer, void *context)
{
		SelectWindow* mw = (SelectWindow*)context;
		mw->handler( &mw->item_up, 0 );

		APP_LOG( APP_LOG_LEVEL_ERROR, "Send selection up" );
}

void select_window_down(ClickRecognizerRef recognizer, void *context)
{
		SelectWindow* mw = (SelectWindow*)context;
		mw->handler( &mw->item_down, 2 );
	
	APP_LOG( APP_LOG_LEVEL_ERROR, "Send selection down" );
}

void select_window_long_select(ClickRecognizerRef recognizer, void *context)
{		
	SelectWindow* mw = (SelectWindow*)context;
	mw->handler( &mw->item_long_select, 4 );
	
	APP_LOG( APP_LOG_LEVEL_ERROR,  "Send selection long select" );
}

void select_window_long_up(ClickRecognizerRef recognizer, void *context)
{
		SelectWindow* mw = (SelectWindow*)context;
		mw->handler( &mw->item_long_up, 3 );

		APP_LOG( APP_LOG_LEVEL_ERROR, "Send selection long up" );
}

void select_window_long_down(ClickRecognizerRef recognizer, void *context)
{
		SelectWindow* mw = (SelectWindow*)context;
		mw->handler( &mw->item_long_down, 5 );
	
	APP_LOG( APP_LOG_LEVEL_ERROR, "Send selection long down" );
}

void select_window_click_config( SelectWindow* mw )
{	
	window_single_click_subscribe( BUTTON_ID_UP, select_window_up );
	window_single_click_subscribe( BUTTON_ID_DOWN, select_window_down );
	window_single_click_subscribe( BUTTON_ID_SELECT, select_window_select );
	
	window_long_click_subscribe( BUTTON_ID_UP, 0, select_window_long_up, NULL );
	window_long_click_subscribe( BUTTON_ID_DOWN, 0, select_window_long_down, NULL );
	window_long_click_subscribe( BUTTON_ID_SELECT, 0, select_window_long_select, NULL );
	
	// Intercept back button to clean up properly
	window_single_click_subscribe( BUTTON_ID_BACK, select_window_back_click );
}

void select_window_refresh( SelectWindow* mw )
{
	text_layer_set_text( mw->text_item_up, mw->item_up.title );
	text_layer_set_text( mw->text_item_select, mw->item_select.title );
	text_layer_set_text( mw->text_item_down, mw->item_down.title );
	
	text_layer_set_text( mw->text_item_long_up, mw->item_long_up.title );
	text_layer_set_text( mw->text_item_long_select, mw->item_long_select.title );
	text_layer_set_text( mw->text_item_long_down, mw->item_long_down.title );
}

SelectWindow* select_window_create()
{
	SelectWindow* mw = malloc( sizeof( SelectWindow ) );
	
	// Create the window
	mw->window = window_create();

	APP_LOG(APP_LOG_LEVEL_INFO, "Window created" );
		
	window_set_click_config_provider_with_context( mw->window, (ClickConfigProvider) select_window_click_config, mw );

	// Up action
	mw->text_item_up = text_layer_create( GRect(0, 0, 140, 50) );
	text_layer_set_background_color( mw->text_item_up, GColorClear);
	text_layer_set_text_color( mw->text_item_up, GColorBlack);
  text_layer_set_text( mw->text_item_up, "" );
  text_layer_set_font( mw->text_item_up, fonts_get_system_font(FONT_KEY_GOTHIC_24_BOLD) );
  text_layer_set_text_alignment( mw->text_item_up, GTextAlignmentRight );
	layer_add_child(window_get_root_layer( mw->window ), text_layer_get_layer( mw->text_item_up ));
  
	// Select action
	mw->text_item_select = text_layer_create( GRect(0, 55, 140, 50) );
	text_layer_set_background_color( mw->text_item_select, GColorClear);
	text_layer_set_text_color( mw->text_item_select, GColorBlack);
  text_layer_set_text( mw->text_item_select, "Loading..." );
  text_layer_set_font( mw->text_item_select, fonts_get_system_font(FONT_KEY_GOTHIC_24_BOLD) );
	text_layer_set_text_alignment( mw->text_item_select, GTextAlignmentRight );
	layer_add_child(window_get_root_layer( mw->window ), text_layer_get_layer( mw->text_item_select ));
	
	// Down action
	mw->text_item_down = text_layer_create( GRect(0, 105, 140, 50) );
	text_layer_set_background_color( mw->text_item_down, GColorClear);
	text_layer_set_text_color( mw->text_item_down, GColorBlack);
  text_layer_set_text( mw->text_item_down, "" );
  text_layer_set_font( mw->text_item_down, fonts_get_system_font(FONT_KEY_GOTHIC_24_BOLD) );
	text_layer_set_text_alignment( mw->text_item_down, GTextAlignmentRight );
	layer_add_child(window_get_root_layer( mw->window ), text_layer_get_layer( mw->text_item_down ));
	
	// Long Up action
	mw->text_item_long_up = text_layer_create( GRect(0, 20, 140, 50) );
	text_layer_set_background_color( mw->text_item_long_up, GColorClear);
	text_layer_set_text_color( mw->text_item_long_up, GColorBlack);
  text_layer_set_text( mw->text_item_long_up, "" );
  text_layer_set_font( mw->text_item_long_up, fonts_get_system_font(FONT_KEY_GOTHIC_24) );
  text_layer_set_text_alignment( mw->text_item_long_up, GTextAlignmentRight );
	layer_add_child(window_get_root_layer( mw->window ), text_layer_get_layer( mw->text_item_long_up ));
  
	// Long Select action
	mw->text_item_long_select = text_layer_create( GRect(0, 75, 140, 50) );
	text_layer_set_background_color( mw->text_item_long_select, GColorClear);
	text_layer_set_text_color( mw->text_item_long_select, GColorBlack);
  text_layer_set_text( mw->text_item_long_select, "" );
  text_layer_set_font( mw->text_item_long_select, fonts_get_system_font(FONT_KEY_GOTHIC_24) );
	text_layer_set_text_alignment( mw->text_item_long_select, GTextAlignmentRight );
	layer_add_child(window_get_root_layer( mw->window ), text_layer_get_layer( mw->text_item_long_select ));
	
	// Long Down action
	mw->text_item_long_down = text_layer_create( GRect(0, 125, 140, 50) );
	text_layer_set_background_color( mw->text_item_long_down, GColorClear);
	text_layer_set_text_color( mw->text_item_long_down, GColorBlack);
  text_layer_set_text( mw->text_item_long_down, "" );
  text_layer_set_font( mw->text_item_long_down, fonts_get_system_font(FONT_KEY_GOTHIC_24) );
	text_layer_set_text_alignment( mw->text_item_long_down, GTextAlignmentRight );
	layer_add_child(window_get_root_layer( mw->window ), text_layer_get_layer( mw->text_item_long_down ));
	
	// Push window onto stack
  window_stack_push( mw->window, false );
	
	APP_LOG(APP_LOG_LEVEL_INFO, "Window pused to stack" );
	
	return mw;
}	