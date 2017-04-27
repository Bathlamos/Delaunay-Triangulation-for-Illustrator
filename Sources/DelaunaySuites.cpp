#include "DelaunaySuites.hpp"
#include "IllustratorSDK.h"

extern "C"
{
//    AIFilterSuite*				sAIFilter = nil;
    SPBlocksSuite*				sSPBlocks = nil;
//    AIArtSuite*					sAIArt = nil;
//    AIPathSuite*				sAIPath = nil;
//    AIRealMathSuite*			sAIMath = nil;
//    AIRandomSuite*				sAIRandom = nil;
//    AIDocumentViewSuite*		sAIDocumentView = nil;
    AIToolSuite*				sAITool = nil;
//    AIMatchingArtSuite*			sAIMatchingArt = nil;
//    AIActionManagerSuite*		sAIActionManager = nil;
//    AIMenuSuite*				sAIMenu = nil;
    AIUnicodeStringSuite*		sAIUnicodeString = nil;
//    AIMdMemorySuite*			sAIMDMemory = nil;
//    AIStringFormatUtilsSuite*	sAIStringFormatUtils = nil;
    AIUserSuite*               sAIUser = nil;
};

ImportSuite gImportSuites[] =
{
//    kAIFilterSuite, kAIFilterVersion, &sAIFilter,
    kSPBlocksSuite, kSPBlocksSuiteVersion, &sSPBlocks,
//    kAIArtSuite, kAIArtVersion, &sAIArt,
//    kAIPathSuite, kAIPathVersion, &sAIPath,
//    kAIRealMathSuite, kAIRealMathVersion, &sAIMath,
//    kAIRandomSuite, kAIRandomVersion, &sAIRandom,
//    kAIDocumentViewSuite, kAIDocumentViewVersion, &sAIDocumentView,
    kAIToolSuite, kAIToolVersion, &sAITool,
//    kAIMatchingArtSuite, kAIMatchingArtVersion, &sAIMatchingArt,
//    kAIActionManagerSuite, kAIActionManagerSuiteVersion, &sAIActionManager,
//    kAIMenuSuite, kAIMenuSuiteVersion, &sAIMenu,
    kAIUnicodeStringSuite, kAIUnicodeStringSuiteVersion, &sAIUnicodeString,
//    kAIMdMemorySuite, kAIMdMemorySuiteVersion, &sAIMDMemory,
//    kAIStringFormatUtilsSuite, kAIStringFormatUtilsSuiteVersion, &sAIStringFormatUtils,
    kAIUserSuite, kAIUserSuiteVersion, &sAIUser,
    nil, 0, nil
};
