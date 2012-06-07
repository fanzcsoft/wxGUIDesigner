/////////////////////////////////////////////////////////////////////////////
// Name:        wx/xrc/gd_frame.cpp
// Purpose:     XML resource handler for Frame (fake toplevel for wxGD)
//              (from the original wx/xrc/xh_frame.cpp)
// Author:      Andrea Zanellato
// Modified by: 
// Created:     2012/06/05
// Revision:    $Hash$
// Licence:     wxWindows licence
/////////////////////////////////////////////////////////////////////////////

// For compilers that support precompilation, includes "wx.h".
#include <wx/wxprec.h>

#ifdef __BORLANDC__
    #pragma hdrstop
#endif

#if wxUSE_XRC

#include "wx/xrc/gd_frame.h"

#ifndef WX_PRECOMP
    #include <wx/intl.h>
    #include <wx/log.h>
    #include <wx/frame.h>
    #include <wx/dialog.h> // to get wxDEFAULT_DIALOG_STYLE
#endif

#include "core/gui/toplevel/frame.h"

IMPLEMENT_DYNAMIC_CLASS(FrameXmlHandler, wxXmlResourceHandler)

FrameXmlHandler::FrameXmlHandler() : wxXmlResourceHandler()
{
    XRC_ADD_STYLE(wxSTAY_ON_TOP);
    XRC_ADD_STYLE(wxCAPTION);
    XRC_ADD_STYLE(wxDEFAULT_DIALOG_STYLE);
    XRC_ADD_STYLE(wxDEFAULT_FRAME_STYLE);
#if WXWIN_COMPATIBILITY_2_6
    XRC_ADD_STYLE(wxTHICK_FRAME);
#endif // WXWIN_COMPATIBILITY_2_6
    XRC_ADD_STYLE(wxSYSTEM_MENU);
    XRC_ADD_STYLE(wxRESIZE_BORDER);
#if WXWIN_COMPATIBILITY_2_6
    XRC_ADD_STYLE(wxRESIZE_BOX);
#endif // WXWIN_COMPATIBILITY_2_6
    XRC_ADD_STYLE(wxCLOSE_BOX);

    XRC_ADD_STYLE(wxFRAME_NO_TASKBAR);
    XRC_ADD_STYLE(wxFRAME_SHAPED);
    XRC_ADD_STYLE(wxFRAME_TOOL_WINDOW);
    XRC_ADD_STYLE(wxFRAME_FLOAT_ON_PARENT);
    XRC_ADD_STYLE(wxMAXIMIZE_BOX);
    XRC_ADD_STYLE(wxMINIMIZE_BOX);
    XRC_ADD_STYLE(wxSTAY_ON_TOP);

#if WXWIN_COMPATIBILITY_2_6
    XRC_ADD_STYLE(wxNO_3D);
#endif // WXWIN_COMPATIBILITY_2_6
    XRC_ADD_STYLE(wxTAB_TRAVERSAL);
    XRC_ADD_STYLE(wxWS_EX_VALIDATE_RECURSIVELY);
    XRC_ADD_STYLE(wxFRAME_EX_METAL);
    XRC_ADD_STYLE(wxFRAME_EX_CONTEXTHELP);

    AddWindowStyles();
}

wxObject *FrameXmlHandler::DoCreateResource()
{
    XRC_MAKE_INSTANCE(frame, Frame);

    frame->Create(m_parentAsWindow,
                  GetID(),
                  GetText(wxT("title")),
                  wxDefaultPosition, wxDefaultSize,
                  GetStyle(wxT("style"), wxDEFAULT_FRAME_STYLE),
                  GetName());

    if (HasParam(wxT("size")))
        frame->SetClientSize(GetSize(wxT("size"), frame));
    if (HasParam(wxT("pos")))
        frame->Move(GetPosition());
    if (HasParam(wxT("icon")))
        frame->SetIcons(GetIconBundle(wxT("icon"), wxART_FRAME_ICON));

    SetupWindow(frame);

    CreateChildren(frame);

    if (GetBool(wxT("centered"), false))
        frame->Centre();

    return frame;
}

bool FrameXmlHandler::CanHandle(wxXmlNode *node)
{
    return IsOfClass(node, wxT("Frame"));
}

#endif // wxUSE_XRC