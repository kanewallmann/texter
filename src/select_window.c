#include "select_window.h"
#include "loading_window.h"

#define TOP_PADDING 14
#define BUTTON_STRIDE 52
#define LONG_STRIDE 21
	
static GBitmap* bmp_bg = NULL;
	
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
}

void select_window_refresh( SelectWindow* mw )
{
	text_layer_set_text( mw->text_item_up, mw->item_up.title );
	text_layer_set_text( mw->text_item_select, mw->item_select.title );
	text_layer_set_text( mw->text_item_down, mw->item_down.title );
	
	text_layer_set_text( mw->text_item_long_up, mw->item_long_up.title );
	text_layer_set_text( mw->text_item_long_select, mw->item_long_select.title );
	text_layer_set_text( mw->text_item_long_down, mw->item_long_down.title );
	
	if( strcmp( mw->item_long_up.title, "" ) == 0 )
	{
			layer_set_frame( text_layer_get_layer( mw->text_item_up ), GRect(0, TOP_PADDING + 10, 140, 30)	);
	}
	else
	{
		layer_set_frame( text_layer_get_layer( mw->text_item_up ), GRect(0, TOP_PADDING, 140, 30)	);	
	}
	
	if( strcmp( mw->item_long_select.title, "" ) == 0 )
	{
			layer_set_frame( text_layer_get_layer( mw->text_item_select ), GRect(0, TOP_PADDING + BUTTON_STRIDE + 10, 140, 30)	);
	}
	else
	{
			layer_set_frame( text_layer_get_layer( mw->text_item_select ), GRect(0, TOP_PADDING + BUTTON_STRIDE, 140, 30)	);
	}
	
	if( strcmp( mw->item_long_down.title, "" ) == 0 )
	{
			layer_set_frame( text_layer_get_layer( mw->text_item_down ), GRect(0, TOP_PADDING + BUTTON_STRIDE*2 + 10, 140, 30)	);
	}
	else
	{
			layer_set_frame( text_layer_get_layer( mw->text_item_down ), GRect(0, TOP_PADDING + BUTTON_STRIDE*2, 140, 30)	);
	}
	
	text_layer_set_text( mw->text_status, mw->status );
}

void select_window_destroy( SelectWindow* mw )
{	
	text_layer_destroy( mw->text_item_up );
	text_layer_destroy( mw->text_item_select );
	text_layer_destroy( mw->text_item_down );
	text_layer_destroy( mw->text_item_long_up );
	text_layer_destroy( mw->text_item_long_select );
	text_layer_destroy( mw->text_item_long_down );
	
	bitmap_layer_destroy( mw->drawing_layer );
	
	window_destroy( mw->window );
	
	free( mw );
	
	if( current_path > path )
	{
		current_path--;
		*current_path = 0;
	}
}

static void handle_window_unload(Window* window)
{	
  SelectWindow* mw = (SelectWindow*)window_get_user_data( window );
	select_window_destroy( mw );
}

SelectWindow* select_window_create()
{
	SelectWindow* mw = malloc( sizeof( SelectWindow ) );
	
	memset( mw, 0, sizeof( SelectWindow ) );
	
	// Create the window
	mw->window = window_create();
	
	window_set_user_data( mw->window, mw );
	
	window_set_fullscreen( mw->window, true );
	
	window_set_window_handlers(mw->window, (WindowHandlers) {
    .unload = handle_window_unload,
  });

	APP_LOG(APP_LOG_LEVEL_INFO, "Window created" );
		
	window_set_click_config_provider_with_context( mw->window, (ClickConfigProvider) select_window_click_config, mw );

	if( bmp_bg == NULL )
	{
		bmp_bg = gbitmap_create_with_resource(RESOURCE_ID_IMAGE_SELECT_BG);
	}
	
	mw->drawing_layer = bitmap_layer_create(GRect(0, 0, 144, 168));
	bitmap_layer_set_bitmap( mw->drawing_layer, bmp_bg );
	layer_add_child(window_get_root_layer( mw->window ), bitmap_layer_get_layer( mw->drawing_layer ) );
		
	// Up action
	mw->text_item_up = text_layer_create( GRect(0, TOP_PADDING, 140, 30) );
	text_layer_set_background_color( mw->text_item_up, GColorClear);
	text_layer_set_text_color( mw->text_item_up, GColorBlack);
  text_layer_set_text( mw->text_item_up, "" );
  text_layer_set_font( mw->text_item_up, fonts_get_system_font(FONT_KEY_GOTHIC_24_BOLD) );
  text_layer_set_text_alignment( mw->text_item_up, GTextAlignmentRight );
	text_layer_set_overflow_mode( mw->text_item_up, GTextOverflowModeTrailingEllipsis );
	layer_add_child(window_get_root_layer( mw->window ), text_layer_get_layer( mw->text_item_up ));
  
	// Long Up action
	mw->text_item_long_up = text_layer_create( GRect(0, TOP_PADDING + LONG_STRIDE, 140, 30) );
	text_layer_set_background_color( mw->text_item_long_up, GColorClear);
	text_layer_set_text_color( mw->text_item_long_up, GColorBlack);
  text_layer_set_text( mw->text_item_long_up, "" );
  text_layer_set_font( mw->text_item_long_up, fonts_get_system_font(FONT_KEY_GOTHIC_24) );
  text_layer_set_text_alignment( mw->text_item_long_up, GTextAlignmentRight );
	text_layer_set_overflow_mode( mw->text_item_long_up, GTextOverflowModeTrailingEllipsis );
	layer_add_child(window_get_root_layer( mw->window ), text_layer_get_layer( mw->text_item_long_up ));
	
	// Select action
	mw->text_item_select = text_layer_create( GRect(0, TOP_PADDING + BUTTON_STRIDE, 140, 30) );
	text_layer_set_background_color( mw->text_item_select, GColorClear);
	text_layer_set_text_color( mw->text_item_select, GColorBlack);
  text_layer_set_text( mw->text_item_select, "Loading..." );
  text_layer_set_font( mw->text_item_select, fonts_get_system_font(FONT_KEY_GOTHIC_24_BOLD) );
	text_layer_set_text_alignment( mw->text_item_select, GTextAlignmentRight );
	text_layer_set_overflow_mode( mw->text_item_select, GTextOverflowModeTrailingEllipsis );
	layer_add_child(window_get_root_layer( mw->window ), text_layer_get_layer( mw->text_item_select ));
	
	// Long Select action
	mw->text_item_long_select = text_layer_create( GRect(0, TOP_PADDING + BUTTON_STRIDE + LONG_STRIDE, 140, 30) );
	text_layer_set_background_color( mw->text_item_long_select, GColorClear);
	text_layer_set_text_color( mw->text_item_long_select, GColorBlack);
  text_layer_set_text( mw->text_item_long_select, "" );
  text_layer_set_font( mw->text_item_long_select, fonts_get_system_font(FONT_KEY_GOTHIC_24) );
	text_layer_set_text_alignment( mw->text_item_long_select, GTextAlignmentRight );
	text_layer_set_overflow_mode( mw->text_item_long_select, GTextOverflowModeTrailingEllipsis );
	layer_add_child(window_get_root_layer( mw->window ), text_layer_get_layer( mw->text_item_long_select ));
	
	// Down action
	mw->text_item_down = text_layer_create( GRect(0, TOP_PADDING + BUTTON_STRIDE*2, 140, 30) );
	text_layer_set_background_color( mw->text_item_down, GColorClear);
	text_layer_set_text_color( mw->text_item_down, GColorBlack);
  text_layer_set_text( mw->text_item_down, "" );
  text_layer_set_font( mw->text_item_down, fonts_get_system_font(FONT_KEY_GOTHIC_24_BOLD) );
	text_layer_set_text_alignment( mw->text_item_down, GTextAlignmentRight );
	text_layer_set_overflow_mode( mw->text_item_down, GTextOverflowModeTrailingEllipsis );
	layer_add_child(window_get_root_layer( mw->window ), text_layer_get_layer( mw->text_item_down ));

	// Long Down action
	mw->text_item_long_down = text_layer_create( GRect(0, TOP_PADDING + BUTTON_STRIDE*2 + LONG_STRIDE, 140, 30) );
	text_layer_set_background_color( mw->text_item_long_down, GColorClear);
	text_layer_set_text_color( mw->text_item_long_down, GColorBlack);
  text_layer_set_text( mw->text_item_long_down, "" );
  text_layer_set_font( mw->text_item_long_down, fonts_get_system_font(FONT_KEY_GOTHIC_24) );
	text_layer_set_text_alignment( mw->text_item_long_down, GTextAlignmentRight );
	text_layer_set_overflow_mode( mw->text_item_long_down, GTextOverflowModeTrailingEllipsis );
	layer_add_child(window_get_root_layer( mw->window ), text_layer_get_layer( mw->text_item_long_down ));
	
	// Status layer
	mw->text_status = text_layer_create( GRect( 0, 0, 144, 17 ) );
	text_layer_set_background_color( mw->text_status, GColorBlack );
	text_layer_set_text_color( mw->text_status, GColorWhite );
	text_layer_set_text( mw->text_status, "" );
	text_layer_set_font( mw->text_status, fonts_get_system_font(FONT_KEY_GOTHIC_14) );
	text_layer_set_text_alignment( mw->text_status, GTextAlignmentCenter );
	text_layer_set_overflow_mode( mw->text_status, GTextOverflowModeTrailingEllipsis );
	layer_add_child(window_get_root_layer( mw->window ), text_layer_get_layer( mw->text_status ));
		
	// Push window onto stack
  window_stack_push( mw->window, false );
	
	APP_LOG(APP_LOG_LEVEL_INFO, "Window pused to stack" );
	
	return mw;
}	