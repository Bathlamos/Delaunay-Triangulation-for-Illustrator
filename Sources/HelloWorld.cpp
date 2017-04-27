#include "IllustratorSDK.h"

// Tell Xcode to export the following symbols
#if defined(__GNUC__)
#pragma GCC visibility push(default)
#endif

// Plug-in entry point
extern "C" ASAPI ASErr PluginMain(char * caller, char* selector, void* message);

// Tell Xcode to return to default visibility for symbols
#if defined(__GNUC__)
#pragma GCC visibility pop
#endif

#define kDelaunayToolIconLightResourceID		16649
#define kDelaunayToolIconDarkResourceID         16650
#define kDelaunayTool                           "Delaunay Triangulation Tool"


// Declaration
AIErr AddTool(void* message);

extern "C"
{
    AIUnicodeStringSuite* sAIUnicodeString = NULL;
    SPBlocksSuite* sSPBlocks = NULL;
    AIToolSuite* sAITool = NULL;
    AIToolHandle* fToolHandle = NULL;
}

extern "C" ASAPI ASErr PluginMain(char* caller, char* selector, void* message)
{
    ASErr error = kNoErr;
    SPBasicSuite* sSPBasic = ((SPMessageData*)message)->basic;
    if (sSPBasic->IsEqual(caller, kSPInterfaceCaller)) {
        AIUserSuite *sAIUser = NULL;
        error = sSPBasic->AcquireSuite(kAIUserSuite, kAIUserSuiteVersion, (const
                                                                           void**) &sAIUser);
        error = sSPBasic->AcquireSuite(kAIUnicodeStringSuite,
                                       kAIUnicodeStringSuiteVersion, (const void**) &sAIUnicodeString);
        error = sSPBasic->AcquireSuite(kSPBlocksSuite, kSPBlocksSuiteVersion, (const
                                                                               void**) &sSPBlocks);
        if(sSPBasic->IsEqual(selector, kSPInterfaceStartupSelector)){
            sAIUser->MessageAlert(ai::UnicodeString("Hello World!"));
            
            
            error = AddTool(message);
        }
        else if(sSPBasic->IsEqual(selector, kSPInterfaceShutdownSelector)){
//            sAIUser->MessageAlert(ai::UnicodeString("Goodbye World!"));
        }
        error = sSPBasic->ReleaseSuite(kAIUserSuite, kAIUserSuiteVersion);
        error = sSPBasic->ReleaseSuite(kAIUnicodeStringSuite,
                                       kAIUnicodeStringSuiteVersion);
    }
    return error;
}

//static ASErr StartupPlugin(SPInterfaceMessage* message)
//{
//    ASErr error = kNoErr;
////    error = Plugin::StartupPlugin(message);
//    
//    if (!error) {
//        error = AddTool(message);
//    }
//    return error;
//}

/* Adds this plug-in's tool to Illustrator.
 */
AIErr AddTool(void* message)
{
    AIErr error = kNoErr;
    SPBasicSuite* sSPBasic = ((SPMessageData*)message)->basic;
    
    AIUserSuite *sAIUser = NULL;
    error = sSPBasic->AcquireSuite(kAIUserSuite, kAIUserSuiteVersion, (const
                                                                       void**) &sAIUser);
    error = sSPBasic->AcquireSuite(kAIUnicodeStringSuite,
                                   kAIUnicodeStringSuiteVersion, (const void**) &sAIUnicodeString);
    error = sSPBasic->AcquireSuite(kSPBlocksSuite, kSPBlocksSuiteVersion, (const
                                                                           void**) &sSPBlocks);
    
    error = sSPBasic->AcquireSuite(kAIToolSuite, kAIToolVersion, (const void**) &sAITool);

    
    
    
    AIAddToolData toolData;
    toolData.title = "Delaunay Triangulation Tool";
    toolData.tooltip = "Delaunay Triangulation Tool";
    toolData.sameGroupAs = kNoTool;
    toolData.sameToolsetAs = kNoTool;
    toolData.normalIconResID = kDelaunayToolIconDarkResourceID;
    toolData.darkIconResID = kDelaunayToolIconLightResourceID;

    ai::int32	options = kToolWantsToTrackCursorOption;
    
    error = sAITool->GetToolNumberFromName(kDelaunayTool, &toolData.sameGroupAs); // the Crop Area tool is now the first in its group
    if (error) return error;
    
    error = sAITool->GetToolNumberFromName(kDelaunayTool, &toolData.sameToolsetAs); // and at the top of its tool stack
    if (error) return error;
    
    
    if (!error) {
        error = sAITool->AddTool(((SPInterfaceMessage*)message)->d.self, kDelaunayTool, &toolData,
                                 options, fToolHandle);
    }
    
    error = sSPBasic->ReleaseSuite(kAIToolSuite,
                                   kAIToolVersion);
    error = sSPBasic->ReleaseSuite(kAIUserSuite, kAIUserSuiteVersion);
    error = sSPBasic->ReleaseSuite(kAIUnicodeStringSuite,
                                   kAIUnicodeStringSuiteVersion);
    
    return error;
}
