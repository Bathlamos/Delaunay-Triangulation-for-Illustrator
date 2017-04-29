
#include "IllustratorSDK.h"
#include "DelaunayToolPlugin.h"
#include "SDKErrors.h"
#include <vector>


/*
*/
Plugin* AllocatePlugin(SPPluginRef pluginRef)
{
	return new DelaunayToolPlugin(pluginRef);
}

/*
*/
void FixupReload(Plugin* plugin)
{
	DelaunayToolPlugin::FixupVTable((DelaunayToolPlugin*) plugin);
}

/*
*/
DelaunayToolPlugin::DelaunayToolPlugin(SPPluginRef pluginRef)
	: Plugin(pluginRef),
    fResourceManagerHandle(NULL),
	fShutdownApplicationNotifier(NULL),
	fNotifySelectionChanged(NULL)
{
	strncpy(fPluginName, kDelaunayToolPluginName, kMaxStringLength);
}

/*
*/
ASErr DelaunayToolPlugin::Message(char* caller, char* selector, void *message)
{
	ASErr error = kNoErr;

	try
	{
		error = Plugin::Message(caller, selector, message);
	}
	catch (ai::Error& ex) {
		error = ex;
	}
	catch (...) {
		error = kCantHappenErr;
	}

	if (error)
	{
		if (error == kUnhandledMsgErr)
		{
			if (strcmp(caller, kCallerAIAnnotation) == 0)
			{
				if (strcmp(selector, kSelectorAIDrawAnnotation) == 0)
				{
					error = DrawAnnotator((AIAnnotatorMessage*)message);
					aisdk::check_ai_error(error);
				}
				else if (strcmp(selector, kSelectorAIInvalAnnotation) == 0)
				{
					error = InvalAnnotator((AIAnnotatorMessage*)message);
					aisdk::check_ai_error(error);
				}				
			}


		}
		else
		{
			Plugin::ReportError(error, caller, selector, message);
		}
	}	
	return error;
}

ASErr DelaunayToolPlugin::Notify( AINotifierMessage *message )
{
	ASErr error = kNoErr;
	if (message->notifier == fShutdownApplicationNotifier)
	{
		if(fResourceManagerHandle != NULL)
		{
			error = sAIUser->DisposeCursorResourceMgr(fResourceManagerHandle);
			fResourceManagerHandle = NULL;
		}
	}	
	else if (message->notifier == fNotifySelectionChanged) {
			// Get the bounds of the current document view.
			AIRealRect viewBounds = {0, 0, 0, 0};
			error = sAIDocumentView->GetDocumentViewBounds(NULL, &viewBounds);
			aisdk::check_ai_error(error);
			// Invalidate the entire document view bounds.
			error = InvalidateRect(viewBounds);
			aisdk::check_ai_error(error);
	}
	return error;
}
/*
*/
ASErr DelaunayToolPlugin::StartupPlugin(SPInterfaceMessage* message)
{
	ASErr error = kNoErr;
	ai::int32 pluginOptions = 0;	
	
	RETURN_ERROR(Plugin::StartupPlugin(message));
	
	RETURN_ERROR(sAIPlugin->GetPluginOptions(message->d.self, &pluginOptions));
	
	RETURN_ERROR(sAIPlugin->SetPluginOptions(message->d.self, pluginOptions | kPluginWantsResultsAutoSelectedOption ));
	
	RETURN_ERROR(AddTool(message));

	RETURN_ERROR(AddAnnotator(message));

	RETURN_ERROR(AddMenus(message));

	RETURN_ERROR(AddNotifier(message));

	return error;
}

/*
 */
ASErr DelaunayToolPlugin::SelectTool(AIToolMessage* message)
{
    sAIAnnotator->SetAnnotatorActive(fAnnotatorHandle, true);
    return kNoErr;
}

/*
 */
ASErr DelaunayToolPlugin::DeselectTool(AIToolMessage* message)
{
    sAIAnnotator->SetAnnotatorActive(fAnnotatorHandle, false);
    return kNoErr;
}

/*
*/
ASErr DelaunayToolPlugin::GoMenuItem(AIMenuMessage* message)
{
	ASErr error = kNoErr;
	if (message->menuItem == this->fAboutPluginMenu) {
		// Pop this plug-in's about box.
		SDKAboutPluginsHelper aboutPluginsHelper;
		aboutPluginsHelper.PopAboutBox(message, "About the Delaunay Triangulation Tool", kDelaunayToolPluginsAlertString);
	}	
	return error;
}

/*
*/
ASErr DelaunayToolPlugin::AddMenus(SPInterfaceMessage* message)
{
	ASErr error = kNoErr;
    
    // Add menu item for this plug-in under the about group.
    AIPlatformAddMenuItemDataUS aboutPluginMenuData;
    aboutPluginMenuData.groupName = kAboutMenuGroup;
    aboutPluginMenuData.itemText = ai::UnicodeString("About Delaunay Triangulation...");
    error = sAIMenu->AddMenuItem(message->d.self, NULL, &aboutPluginMenuData, kMenuItemNoOptions, &fAboutPluginMenu);

    return error;
}

/*
*/
ASErr DelaunayToolPlugin::AddTool(SPInterfaceMessage* message)
{
    ASErr error = kNoErr;
    AIAddToolData toolData;
    toolData.title = "Delaunay Triangulation Tool";
    toolData.tooltip = "Delaunay Triangulation Tool";
    toolData.sameGroupAs = kNoTool;
    toolData.sameToolsetAs = kNoTool;
    toolData.normalIconResID = kDelaunayToolIconResID;
    toolData.darkIconResID = kDelaunayToolIconDarkResID;
    
    ai::int32	options = kToolWantsToTrackCursorOption;
    
    RETURN_ERROR(sAITool->GetToolNumberFromName(kDelaunayToolPluginName, &toolData.sameGroupAs));
    
    RETURN_ERROR(sAITool->GetToolNumberFromName(kDelaunayToolPluginName, &toolData.sameToolsetAs));
    
    RETURN_ERROR(sAITool->AddTool(message->d.self, kDelaunayToolPluginName, &toolData, options, &fToolHandle));
    
    return kNoErr;
    
}

ASErr DelaunayToolPlugin::AddNotifier(SPInterfaceMessage *message)
{
	ASErr result = kNoErr;
	try {
		result = sAINotifier->AddNotifier(fPluginRef, "DelaunayToolPlugin", kAIApplicationShutdownNotifier, &fShutdownApplicationNotifier);
		aisdk::check_ai_error(result);
		result = sAINotifier->AddNotifier(fPluginRef, "DelaunayToolPlugin", kAIArtSelectionChangedNotifier, &fNotifySelectionChanged);

		aisdk::check_ai_error(result);
	}
	catch (ai::Error& ex) {
		result = ex;
	}
	catch(...)
	{
		result = kCantHappenErr;
	}
	return result;
}

/*
*/
ASErr DelaunayToolPlugin::TrackToolCursor(AIToolMessage* message) 
{
    return sAIUser->SetCursor(kCursorID, fResourceManagerHandle);
}

/*
*/
ASErr DelaunayToolPlugin::ToolMouseDown(AIToolMessage* message)
{
	ASErr error = kNoErr;

	// we want our initial mouse down to base the drag on later	
	this->fStartingPoint = message->cursor;
    
    AIPoint point;
    RETURN_ERROR(sAIDocumentView->ArtworkPointToViewPoint(NULL, &message->cursor, &point));
    delaunayPoints.push_back(point);
	
	// Activate annotator.
	RETURN_ERROR(sAIAnnotator->SetAnnotatorActive(fAnnotatorHandle, true));
	
	return error;
}

/*
*/
ASErr DelaunayToolPlugin::ToolMouseUp(AIToolMessage* message)
{
	ASErr error = kNoErr;

	// Deactivates annotator.
//	error = sAIAnnotator->SetAnnotatorActive(fAnnotatorHandle, false);
	
	return error;
}

/*
*/
ASErr DelaunayToolPlugin::ToolMouseDrag(AIToolMessage* message)
{
	ASErr error = kNoErr;
//	AIArtHandle path;
//	AIPathSegment segments[8];
//	AIReal pathAngle, arrowAngle;
//	AIRealPoint arrowPt1, arrowPt2;
//	AIPathStyle pathStyle;
//	AIPathStyleMap pathStyleMap;
//	AIDictionaryRef advStrokeParams = NULL;
//	
//	error = sAIUndo->UndoChanges( );
//
//	this->fEndPoint = message->cursor;
//
//	// Invalidate the old Annotation
//	error = InvalidateRect(oldAnnotatorRect);
//	aisdk::check_ai_error(error);
//	
//	// Create new art, we will fill it with points below.
//	error = sAIArt->NewArt( kPathArt, kPlaceAboveAll, NULL, &path );
//	if ( error )
//		goto error;
//		
//	if (message->event->modifiers & aiEventModifiers_shiftKey)
//	{
//		short angle = (short)(abs((int)(sAIRealMath->RadianToDegree(sAIRealMath->AIRealPointAngle(&this->fStartingPoint, &this->fEndPoint)))));
//
//		if (angle < 45 || angle > 135)
//		{
//			this->fEndPoint.v = this->fStartingPoint.v;
//		}
//		else
//		{
//			this->fEndPoint.h = this->fStartingPoint.h;
//		}
//	}
//
//	
//	if ( message->tool == this->fToolHandle ) {
//		//	HEAD ARROW
//		// head arrow has 5 points	
//		error = sAIPath->SetPathSegmentCount( path, 5 );		
//
//		// beginning (and end) point. This is butt end of arrow
//		segments[0].p.h = this->fStartingPoint.h;
//		segments[0].p.v = this->fStartingPoint.v;
//		segments[0].in = segments[0].out = segments[0].p;
//		segments[0].corner = true;
//
//		// arrow head point
//		segments[1].p.h = this->fEndPoint.h;
//		segments[1].p.v = this->fEndPoint.v;
//		segments[1].in = segments[1].out = segments[1].p;
//		segments[1].corner = true;
//
//		// angle created by line segment
//		pathAngle = sAIRealMath->AIRealPointAngle( &this->fEndPoint, &this->fStartingPoint );
//		arrowAngle = sAIRealMath->DegreeToRadian( _ShortToAIReal(20) );
//
//		sAIRealMath->AIRealPointLengthAngle( _ShortToAIReal(10), pathAngle + arrowAngle, &arrowPt1 );
//		sAIRealMath->AIRealPointLengthAngle( _ShortToAIReal(10), pathAngle - arrowAngle, &arrowPt2 );
//
//		segments[2].p.h = this->fEndPoint.h + arrowPt1.h;
//		segments[2].p.v = this->fEndPoint.v + arrowPt1.v;
//		segments[2].in = segments[2].out = segments[2].p;
//		segments[2].corner = true;
//
//		segments[3].p.h = this->fEndPoint.h + arrowPt2.h;
//		segments[3].p.v = this->fEndPoint.v + arrowPt2.v;
//		segments[3].in = segments[3].out = segments[3].p;
//		segments[3].corner = true;		
//
//		segments[4].p.h = this->fEndPoint.h;
//		segments[4].p.v = this->fEndPoint.v;
//		segments[4].in = segments[4].out = segments[4].p;
//		segments[4].corner = true;		
//
//		error = sAIPath->SetPathSegments( path, 0, 5, segments );
//		if ( error )
//			goto error;
//			
//    }
//
//	error = sAIPath->SetPathClosed( path, true );
//	if ( error )
//		goto error;
//		
//	// fill and stroke with black; 1 point line
//	error = sAIPathStyle->GetCurrentPathStyle( &pathStyle, &pathStyleMap, &advStrokeParams );
//	pathStyle.fillPaint = true;
//	pathStyle.fill.color.kind = kGrayColor;
//	pathStyle.fill.color.c.g.gray = kAIRealOne;
//	
//	pathStyle.strokePaint = true;
//	pathStyle.stroke.color.kind = kGrayColor;
//	pathStyle.stroke.color.c.g.gray = kAIRealOne;
//	pathStyle.stroke.width = kAIRealOne;
//	error = sAIPathStyle->SetPathStyle( path, &pathStyle );
//
//error:
	return error;
}

/*
*/
ASErr DelaunayToolPlugin::AddAnnotator(SPInterfaceMessage *message)
{
	ASErr result = kNoErr;
	try
	{
		result = sAIAnnotator->AddAnnotator(message->d.self, "DelaunayTool Annotator", &fAnnotatorHandle);
		aisdk::check_ai_error(result);

//		result = sAIAnnotator->SetAnnotatorActive(fAnnotatorHandle, false);
		aisdk::check_ai_error(result);
	}
	catch (ai::Error& ex)
	{
		result = ex;
	}
	catch(...)
	{
		result = kCantHappenErr;
	}
	return result;
}

/*
*/
ASErr DelaunayToolPlugin::DrawAnnotator(AIAnnotatorMessage* message)
{
	ASErr error = kNoErr;
    short radius = 3;
	try
	{
        
        for(AIPoint const& point: delaunayPoints) {
            
            // Define bounding rectangle
            AIRect bbox;
            bbox.left = point.h + radius;
            bbox.right = point.h - radius;
            bbox.top = point.v + radius;
            bbox.bottom = point.v - radius;
            
            // Define the color
            unsigned short white = 65000;
            unsigned short black = 0;
            AIRGBColor whiteFill = {white, white, white};
            AIRGBColor blackFill = {black, black, black};
            
            sAIAnnotatorDrawer->SetColor(message->drawer, blackFill);
            RETURN_ERROR(sAIAnnotatorDrawer->DrawEllipse(message->drawer, bbox, true));
            
            sAIAnnotatorDrawer->SetColor(message->drawer, whiteFill);
            sAIAnnotatorDrawer->SetLineWidth(message->drawer, 2);
            RETURN_ERROR(sAIAnnotatorDrawer->DrawEllipse(message->drawer, bbox, false));
            InvalidateRect(bbox);
        }
        
        
//		// Get the string to display in annotator.
//		ai::UnicodeString pointStr;
//		error = this->GetPointString(fEndPoint, pointStr);
//		aisdk::check_ai_error(error);
//
//		AIPoint annotatorPoint;
//		error = sAIDocumentView->ArtworkPointToViewPoint(NULL, &fEndPoint, &annotatorPoint);
//
//		// Move 5 points right and 5 points up.
//		annotatorPoint.h += 5;
//		annotatorPoint.v -= 5;
//        
//        annotatorPoint.h = 500;
//        annotatorPoint.v = 500;
//
//		// Find cursor bound rect.
//		AIRect annotatorRect;
//		error = sAIAnnotatorDrawer->GetTextBounds(message->drawer, pointStr, &annotatorPoint,false, annotatorRect);
//		aisdk::check_ai_error(error);
//
//		// Draw a filled rectangle, the following R, G and B values combined makes light yellow.
//		unsigned short red = 65000;
//		unsigned short green = 65000;
//		unsigned short blue = 40000;
//		AIRGBColor yellowFill = {red, green, blue};
//		sAIAnnotatorDrawer->SetColor(message->drawer, yellowFill);
//        sAIAnnotatorDrawer->DrawEllipse(message->drawer, annotatorRect, true);
////		error = sAIAnnotatorDrawer->DrawRect(message->drawer, annotatorRect, true);
//		aisdk::check_ai_error(error);
//
//		// Draw black outline, 0 for R, G and B makes black.
//		unsigned short black = 0;
//		AIRGBColor blackFill = {black, black, black};
//		sAIAnnotatorDrawer->SetColor(message->drawer, blackFill);
//		sAIAnnotatorDrawer->SetLineWidth(message->drawer, 0.5);
//		error = sAIAnnotatorDrawer->DrawRect(message->drawer, annotatorRect, false);
//		aisdk::check_ai_error(error);
//
//		// Draw cursor text.
//		error = sAIAnnotatorDrawer->SetFontPreset(message->drawer, kAIAFSmall);
//		aisdk::check_ai_error(error);
//
//		error = sAIAnnotatorDrawer->DrawTextAligned(message->drawer, pointStr, kAICenter, kAIMiddle, annotatorRect);
//		aisdk::check_ai_error(error);
//
//		// Save old rect
//		oldAnnotatorRect = annotatorRect;
	}
	catch (ai::Error& ex)
	{
		error = ex;
	}
	return error;
}

ASErr DelaunayToolPlugin::InvalAnnotator(AIAnnotatorMessage *message)
{
//	ASErr result = sAIAnnotator->InvalAnnotationRect(NULL, &oldAnnotatorRect);

	return kNoErr;
}

ASErr DelaunayToolPlugin::InvalidateRect(const AIRect& invalRect)
{
	ASErr result = kNoErr;
	try {		
		// Invalidate the rect bounds so it is redrawn.
//		result = sAIAnnotator->InvalAnnotationRect(NULL, &invalRect);
		aisdk::check_ai_error(result);
	}
	catch (ai::Error& ex) {
		result = ex;
	}
	return result;
}

/*
*/
ASErr DelaunayToolPlugin::InvalidateRect(const AIRealRect& invalRealRect)
{
	ASErr result = kNoErr;
	try {		
		// invalRealRect is in artwork coordinates, convert to view 
		// coordinates for AIAnnotatorSuite::InvalAnnotationRect.
		AIRect invalRect;
		result = this->ArtworkBoundsToViewBounds(invalRealRect, invalRect);
		aisdk::check_ai_error(result);

		// Invalidate the rect bounds so it is redrawn.
//		result = sAIAnnotator->InvalAnnotationRect(NULL, &invalRect);
		aisdk::check_ai_error(result);
	}
	catch (ai::Error& ex) {
		result = ex;
	}
	return result;
}

/*
*/
ASErr DelaunayToolPlugin::ArtworkBoundsToViewBounds(const AIRealRect& artworkBounds, AIRect& viewBounds)
{
	ASErr result = kNoErr;
	try {		
		AIRealPoint tlArt, brArt;
		tlArt.h = artworkBounds.left;
		tlArt.v = artworkBounds.top;
		brArt.h = artworkBounds.right;
		brArt.v = artworkBounds.bottom;

		// Convert artwork coordinates to view coordinates.
		AIPoint tlView, brView;
		result = sAIDocumentView->ArtworkPointToViewPoint(NULL, &tlArt, &tlView);
		aisdk::check_ai_error(result);
		result = sAIDocumentView->ArtworkPointToViewPoint(NULL, &brArt, &brView);
		aisdk::check_ai_error(result);

		viewBounds.left = tlView.h;
		viewBounds.top = tlView.v;
		viewBounds.right = brView.h;
		viewBounds.bottom = brView.v;
	}
	catch (ai::Error& ex) {
		result = ex;
	}
	return result;
}

/*
*/
ASErr DelaunayToolPlugin::GetPointString(const AIRealPoint& point, ai::UnicodeString& pointStr)
{
	ASErr result = kNoErr;
	try
	{	
		ASInt32 precision = 2;
		ai::UnicodeString horiz, vert;
		ai::NumberFormat numFormat;

		horiz = numFormat.toString((float) point.h, precision, horiz);
		vert = numFormat.toString((float) -point.v, precision, vert);

		pointStr.append(ai::UnicodeString("h: ")
				.append(horiz)
				.append(ai::UnicodeString(", v: ")
				.append(vert)));
	}
	catch (ai::Error& ex)
	{
		result = ex;
	}
	return result;
}

ASErr DelaunayToolPlugin::PostStartupPlugin()
{
    AIErr result = kNoErr;
    result = sAIUser->CreateCursorResourceMgr(fPluginRef,&fResourceManagerHandle);
    return result;
}

// End DelaunayToolPlugin.cpp
