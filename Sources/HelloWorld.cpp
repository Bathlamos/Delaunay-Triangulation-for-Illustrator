#include "IllustratorSDK.h"
#include "DelaunaySuites.hpp"

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


// Should go in the H file
#define RETURN_ERROR(CALL) error = CALL; \
if (error) return error;

#define kDelaunayToolIconLightResourceID	16649
#define kDelaunayToolIconDarkResourceID     16650
#define kDelaunayTool                       "Delaunay Triangulation Tool"


// Declaration
AIErr AddTool(void* message);
ASErr acquireSuites(ImportSuite *suites);
ASErr acquireSuite(ImportSuite *suite);
ASErr releaseSuites(ImportSuite *suites);
ASErr releaseSuite(ImportSuite *suite);

extern "C"
{
    AIToolHandle* fToolHandle = NULL;
    SPBasicSuite* sSPBasic = NULL;
    AIErr error = NULL;
}

extern "C" ASAPI ASErr PluginMain(char* caller, char* selector, void* message)
{
    ASErr error = kNoErr;
    sSPBasic = ((SPMessageData*)message)->basic;
    
    
    if (sSPBasic->IsEqual(caller, kSPInterfaceCaller)) {

        if(sSPBasic->IsEqual(selector, kSPInterfaceStartupSelector)){
            // Register all Suites
            RETURN_ERROR(acquireSuites(gImportSuites));
            
            sAIUser->MessageAlert(ai::UnicodeString("Hello World!"));
            
            
            error = AddTool(message);
        }
        else if(sSPBasic->IsEqual(selector, kSPInterfaceShutdownSelector)){
            sAIUser->MessageAlert(ai::UnicodeString("Goodbye World!"));
            
            // Unregister all Suites
            RETURN_ERROR(releaseSuites(gImportSuites));
            
        }

    }
    return error;
}


ASErr acquireSuites(ImportSuite *suites) {
    for (int i = 0; suites[i].name != NULL; i++) {
        RETURN_ERROR(acquireSuite(&suites[i]));
    }
    return kNoErr;
}

ASErr releaseSuites(ImportSuite *suites) {
    for (int i = 0; suites[i].name != NULL; i++) {
        RETURN_ERROR(releaseSuite(&suites[i]));
    }
    return kNoErr;
}

ASErr releaseSuite(ImportSuite *suite) {
    ASErr error = kNoErr;
    
    if (suite->suite != NULL) {
        void **s = (void **)suite->suite;
        if (*s != NULL) {
            error = sSPBasic->ReleaseSuite(suite->name, suite->version);
            *s = NULL;
        }
    }
    return error;
}

ASErr acquireSuite(ImportSuite *suite) {
    ASErr error = kNoErr;
    char message[256];
    
    if (suite->suite != NULL) {
        error = sSPBasic->AcquireSuite(suite->name, suite->version, (const void **) suite->suite);
        
        if (error && sAIUser != NULL) {
            sprintf(message, "Error: %d, suite: %s, version: %d!", error,
                    suite->name, suite->version);
            sAIUser->MessageAlert(ai::UnicodeString(message));
        }
    }
    return error;
}


/* Adds this plug-in's tool to Illustrator.
 */
AIErr AddTool(void* message)
{
    AIErr error = kNoErr;
    SPBasicSuite* sSPBasic = ((SPMessageData*)message)->basic;

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
    
    return kNoErr;
}
