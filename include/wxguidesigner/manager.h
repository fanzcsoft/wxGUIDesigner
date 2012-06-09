///////////////////////////////////////////////////////////////////////////////
// Name:        wxguidesigner/gui/manager.h
// Purpose:     
// Author:      Andrea Zanellato
// Modified by: 
// Created:     2012/01/13
// Revision:    $Hash$
// Copyright:   (c) Andrea Zanellato
// Licence:     wxWindows licence
///////////////////////////////////////////////////////////////////////////////
#ifndef __WXGUIDESIGNER_MANAGER_H__
#define __WXGUIDESIGNER_MANAGER_H__

#include "wxguidesigner/dllimpexp.h"

#include <boost/tr1/memory.hpp>

#include "wxguidesigner/ipc.h"
#include "wxguidesigner/interfaces/iobject.h"

class wxDialog;
class wxMenuBar;
class wxNotebook;
class wxFrame;
class wxPanel;
class wxPropertyGrid;
class wxStyledTextCtrl;
class wxToolBar;
class wxTreeCtrl;
class wxGDHandler;
//class IGUIDesigner;

using namespace std::tr1;

class DLLIMPEXP_WXGUIDESIGNER wxGUIDesigner : public IGUIDesigner
{
public:
    static wxGUIDesigner *Get();
    static void Free();

    // wxGDHandler
    wxFrame         *GetMainFrame        ( wxWindow *parent = NULL );
    wxDialog        *GetAboutDialog      ( wxWindow *parent );
    wxPanel         *GetDesignPanel();
    wxNotebook      *GetEditorBook       ( wxWindow *parent );
    wxNotebook      *GetPropertyBook     ( wxWindow *parent );
    wxNotebook      *GetPaletteBook      ( wxWindow *parent );
    wxTreeCtrl      *GetTreeView         ( wxWindow *parent );
    wxToolBar       *GetToolBar          ( wxWindow *parent );
    wxPropertyGrid  *GetPropertiesGrid();
    wxPropertyGrid  *GetEventsGrid();
    wxStyledTextCtrl *GetEditor( wxWindow *parent, const wxString &name );

    // Project
    void        NewProject();
    bool        LoadProject( const wxString &filePath, bool check = true );
    bool        SaveProject( const wxString &filePath );
    wxString    GetProjectFileName() const  { return m_currPrj; }
    wxString    GetProjectFilePath() const  { return m_currDir; }
    bool        IsProjectModified()         { return m_isChanged; }
    bool        CheckSingleInstance( const wxString &filePath,
                                        bool switchTo = true );
    void        GenerateCode();

private:
    wxGUIDesigner();
    ~wxGUIDesigner();

    static wxGUIDesigner    *ms_instance;

    wxGDHandler             *m_handler;
    shared_ptr< IPCFile >   m_ipcFile;
    wxString                m_currPrj;
    wxString                m_currDir;
    bool                    m_isChanged;
};

#endif //__WXGUIDESIGNER_MANAGER_H__