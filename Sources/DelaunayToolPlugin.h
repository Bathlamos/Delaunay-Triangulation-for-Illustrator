
#ifndef __DelaunayToolPlugin_H__
#define __DelaunayToolPlugin_H__

#include "Plugin.hpp"
#include "SDKDef.h"
#include "SDKAboutPluginsHelper.h"
#include "DelaunayToolID.h"
#include "DelaunayToolSuites.h"
#include <vector>

#define RETURN_ERROR(CALL) error = CALL; \
if (error) return error;

#define kDelaunayToolPluginsAlertString "A Delaunay Triangulation plugin inspired from Jonathan Puckey\n\nhttp://http://legault.cc/\n\nCopyright 2017 Philippe Legault. All rights reserved."

/**	Creates a new DelaunayToolPlugin.
	@param pluginRef IN unique reference to this plugin.
	@return pointer to new DelaunayToolPlugin.
*/
Plugin* AllocatePlugin(SPPluginRef pluginRef);

/**	Reloads the DelaunayToolPlugin class state when the plugin is 
	reloaded by the application.
	@param plugin IN pointer to plugin being reloaded.
*/
void FixupReload(Plugin* plugin);

/**	Provides a plugin which demonstrates adding a group of tools to the toolbar.
*/
class DelaunayToolPlugin : public Plugin
{
public:

	/**	Constructor.
		@param pluginRef IN reference to this plugin.
	*/
	DelaunayToolPlugin(SPPluginRef pluginRef);

	/**	Destructor.
	*/
	~DelaunayToolPlugin(){}

	/**	Restores state of DelaunayToolPlugin during reload.
	*/
	FIXUP_VTABLE_EX(DelaunayToolPlugin, Plugin);

	virtual ASErr Notify( AINotifierMessage * message );

protected:

	/** Calls Plugin::Message and handles any errors returned.
		@param caller IN sender of the message.
		@param selector IN nature of the message.
		@param message IN pointer to plugin and call information.
		@return kNoErr on success, other ASErr otherwise.
	*/
	virtual ASErr Message(char* caller, char* selector, void *message);

	/**	Calls Plugin::Startup and initialisation functions, such as 
		AddTool and AddMenus.
		@param message IN pointer to plugin and call information.
		@return kNoErr on success, other ASErr otherwise.
	*/
	virtual ASErr StartupPlugin(SPInterfaceMessage* message);

	/**	Performs actions required for menu item selected.
		@param message IN pointer to plugin and call information.
		@return kNoErr on success, other ASErr otherwise.
	*/
	virtual ASErr GoMenuItem(AIMenuMessage* message);

	/**	Sets the platform cursor to the icon for the current tool selected.
		@param message IN pointer to plugin and call information.
		@return kNoErr on success, other ASErr otherwise.
	*/
	virtual ASErr TrackToolCursor(AIToolMessage* message);

	/**	Sets fStartingPoint to location of pointer during mouse down event.
		@param message IN pointer to plugin and call information.
		@return kNoErr on success, other ASErr otherwise.
	*/
	virtual ASErr ToolMouseDown(AIToolMessage* message);

	/**	Deactivates annotator during mouse up event.
		@param message IN pointer to plugin and call information.
		@return kNoErr on success, other ASErr otherwise.
	*/
	virtual ASErr ToolMouseUp(AIToolMessage* message);

	/**	Draws the art on screen during mouse drag event.
		@param message IN pointer to plugin and call information.
		@return kNoErr on success, other ASErr otherwise.
	*/
	virtual ASErr ToolMouseDrag(AIToolMessage* message);
	
private:

	/**	Menu item handle for about plugin dialog.
	*/
	AIMenuItemHandle fAboutPluginMenu;

	/** Stores handles for the tool.
	*/
	AIToolHandle fToolHandle;

	/** Stores h,v location of line/arrow start point.
	*/
	AIRealPoint fStartingPoint;

	/** Stores h,v location of line/arrow end point.
	*/
	AIRealPoint fEndPoint;

	/** Stores old annotator's rect.
	*/
	AIRect oldAnnotatorRect;

	/** Handle for the annotator added by this plug-in.
	*/
	AIAnnotatorHandle fAnnotatorHandle;

	/** Handle for illustrator shutdown notifier.
	*/
	AINotifierHandle	fShutdownApplicationNotifier;

	/** Handle for the selection changed notifier.
	*/
	AINotifierHandle	fNotifySelectionChanged;

    /** Handle for the resource manager added by this plug-in used for setting cursor
     */
    AIResourceManagerHandle fResourceManagerHandle;
    
    std::vector<AIRealPoint> delaunayPoints;

	/**	Adds the tools for this plugin to the application toolbar.
		@param message IN pointer to plugin and call information.
		@return kNoErr on success, other ASErr otherwise.
	*/
	ASErr AddTool(SPInterfaceMessage* message);

	/**	Adds the menu items for this plugin to the application UI.
		@param message IN pointer to plugin and call information.
		@return kNoErr on success, other ASErr otherwise.
	*/
	ASErr AddMenus(SPInterfaceMessage* message);

	/**	Adds an annotator.
		@param message IN message data.
		@return kNoErr on success, other ASErr otherwise.
	*/
	ASErr AddAnnotator(SPInterfaceMessage* message);

	/**	Registers this plug-in to receive the selection changed
		notifier.
		@param message IN message data.
		@return kNoErr on success, other ASErr otherwise.
	*/
	ASErr AddNotifier(SPInterfaceMessage* message);

	/**	Draws the cursor location annotation.
		@param message IN message data.
		@return kNoErr on success, other ASErr otherwise.
	*/
	ASErr DrawAnnotator(AIAnnotatorMessage* message);

	/**	Invalidates the annotations within an area of the artwork when
		prompted by the application.
		@param message IN message data.
		@return kNoErr on success, other ASErr otherwise.
	*/
	ASErr InvalAnnotator(AIAnnotatorMessage* message);

	/**	Converts a point coordinate to a unicode string.
		@param point IN the point to convert.
		@param pointStr OUT the unicode string representing the point coordinate.
		@return kNoErr on success, other ASErr otherwise.
	*/
	ASErr GetPointString(const AIRealPoint& point, ai::UnicodeString& pointStr);
    
    ASErr PostStartupPlugin();

	/**	Converts an AIRealRect in artwork coordinates to an AIRect in view coordinates.
		@param artworkBounds IN the rectangle bounds in artwork coordinates.
		@param viewBounds OUT the rectangle bounds in view coordinates.
		@return kNoErr on success, other ASErr otherwise.
	*/
	ASErr ArtworkBoundsToViewBounds(const AIRealRect& artworkBounds, AIRect& viewBounds);

	/**	Invalidates an area on the artboard so affected annotations will be redrawn.
		@param invalRealRect IN area to invalidate as an AIRealRect.
		@return kNoErr on success, other ASErr otherwise.
	*/
	ASErr InvalidateRect(const AIRealRect& invalRealRect);

	/**	Invalidates an area on the artboard so affected annotations will be redrawn
		@param invalRectIN area to invalidate as an AIRect.
		@return kNoErr on success, other ASErr otherwise.
	*/
	ASErr InvalidateRect(const AIRect& invalRect);
    
    ASErr SelectTool(AIToolMessage* message);
    ASErr DeselectTool(AIToolMessage* message);

};

#endif // End DelaunayToolPlugin.h
