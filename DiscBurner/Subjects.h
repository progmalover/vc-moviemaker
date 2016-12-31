#pragma once

enum
{
	//Profile
	SUB_PROFILE_CHANGED,			// Current profile changed
    //File List
	SUB_FILE_SEL_CHANGED,			//CFileList::OnUlnSelChanged()
    SUB_FILE_LIST_CHANGED,          //Add,Remove,Check,UnCheck;
	//Settings(Simple pane)
	SUB_DISC_TYPE_CHANGED, 
	SUB_DISC_LABEL_CHANGED, 
	SUB_VIDEO_FRAME_RATE_CHANGED,	// Simple pane
	SUB_VIDEO_DIMENSION_CHANGED,	// Simple pane
	SUB_VIDEO_STANDARD_CHANGED,		// Simple pane
	SUB_VIDEO_QUALITY_CHANGED, 		// Simple pane
	SUB_AUDIO_QUALITY_CHANGED, 		// Simple pane
	SUB_VIDEO_ZOOM_MODE, 			// Simple pane
    SUB_VIDEO_TRIMMED, 
	SUB_VIDEO_ASPECT_CHANGED,
	//Advanced
	SUB_PROPERTY_GRID_CHANGED,      //CMFCPropertyGridCtrl item changed
    //DVD Menu(Menu pane)
	SUB_MENU_TEMPLATE_CHANGED,
	SUB_CHANGE_ACTIVE_TAB_SETTINGS,
	SUB_BUTTON_TEXT_CHANDED,           //Button Text change
	SUB_MENU_TEMPLATE_PARAM_CHANGED,   //Not include SUB_MENU_TEMPLATE_CHANGED AND SUB_BUTTON_TEXT_CHANDED
};
