#pragma once 
	
// Incoming actions
#define ACTION_HOME_DATA 0 
#define ACTION_MESSAGE_BRANCH 1
#define ACTION_MESSAGE_SENT 2
#define ACTION_CONTACTS 3
#define ACTION_CONTACTS_EXTRA 4

// Outgoing actions
#define ACTION_SHOW_HOME 0
#define ACTION_SELECT_CONTACT 1
#define ACTION_NAVIGATE 2
#define ACTION_RECENT 3
#define ACTION_FAVOURITE 4

char path[10];
char address[12];
char* current_path;

char person[24];
char message[36];