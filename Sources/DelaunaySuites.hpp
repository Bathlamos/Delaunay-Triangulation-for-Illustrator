#ifndef DelaunaySuites_hpp
#define DelaunaySuites_hpp

#include "IllustratorSDK.h"
#include "Suites.hpp"
#include "AITransformArt.h"
#include "AIStringFormatUtils.h"
#include "AIRandom.h"
#include "AIDocumentView.h"

extern  "C" ImportSuite                 gImportSuites[];
extern	"C" AIFilterSuite*				sAIFilter;
extern	"C" SPBlocksSuite*				sSPBlocks;
extern	"C" AIArtSuite*					sAIArt;
extern	"C" AIPathSuite*				sAIPath;
extern	"C" AIRealMathSuite*			sAIMath;
extern	"C" AIRandomSuite*				sAIRandom;
extern	"C" AIDocumentViewSuite*		sAIDocumentView;
extern	"C" AIToolSuite*				sAITool;
extern	"C" AIMatchingArtSuite*			sAIMatchingArt;
extern	"C" AIActionManagerSuite*		sAIActionManager;
extern	"C" AIMenuSuite*				sAIMenu;
extern	"C" AIUnicodeStringSuite*		sAIUnicodeString;
extern	"C" AIMdMemorySuite*			sAIMDMemory;
extern	"C" AIStringFormatUtilsSuite*	sAIStringFormatUtils;
extern  "C" AIUserSuite*                sAIUser;


#endif /* DelaunaySuites_hpp */
