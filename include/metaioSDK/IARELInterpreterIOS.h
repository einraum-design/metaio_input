#ifndef _AS_IARELINTERPRETERIOS_
#define _AS_IARELINTERPRETERIOS_

#include "IARELInterpreter.h"

// forward declaration
@class UIWebView;

namespace metaio
{
	
	/**
	* Create a IARELInterpreter for IOS
	*	
	*	This functions should be only called on iOS platforms
	*
	* \param webView provide a UIWebView whre AREL is attached to
	* \return IARELInterpreter an pointer to the created IARELInterpreter
	*/
	IARELInterpreter* CreateARELInterpreterIOS(UIWebView* webView);
}

#endif
