/***************************************************************
 * Name:      interfaceApp.cpp
 * Purpose:   Code for Application Class
 * Author:     ()
 * Created:   2015-10-25
 * Copyright:  ()
 * License:
 **************************************************************/

#include "wx_pch.h"
#include "interfaceApp.h"

//(*AppHeaders
#include "interfaceMain.h"
#include <wx/image.h>
//*)

IMPLEMENT_APP(interfaceApp);

bool interfaceApp::OnInit()
{
    //(*AppInitialize
    bool wxsOK = true;
    wxInitAllImageHandlers();
    if ( wxsOK )
    {
    	interfaceFrame* Frame = new interfaceFrame(0);
    	Frame->Show();
    	SetTopWindow(Frame);
    }
    //*)
    return wxsOK;

}
