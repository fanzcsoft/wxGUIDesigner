///////////////////////////////////////////////////////////////////////////////
// Name:        wxguidesigner/gui/manager.cpp
// Purpose:     
// Author:      Andrea Zanellato
// Modified by: 
// Created:     2011/11/20
// Revision:    $Hash$
// Copyleft:    (ɔ) Andrea Zanellato
// Licence:     GNU General Public License Version 3
///////////////////////////////////////////////////////////////////////////////
#include <wx/app.h>
#include <wx/config.h>
#include <wx/dir.h>
#include <wx/event.h>
#include <wx/filefn.h>
#include <wx/frame.h>
#include <wx/fs_arc.h>
#include <wx/fs_mem.h>
#include <wx/intl.h>
#include <wx/notebook.h>
#include <wx/propgrid/propgrid.h>
#include <wx/sstream.h>
#include <wx/stdpaths.h>
#include <wx/stc/stc.h>
#include <wx/treectrl.h>
#include <wx/xml/xml.h>
#include <wx/xrc/xmlres.h>
#include <wx/xrc/xh_all.h>

#include <wx/xrc/xh_aui.h>
#include <wx/xrc/xh_stc.h>
#include <wx/xrc/xh_propgrid.h>

#include <wx/log.h>

#include "wx/xrc/gd_dialog.h"
#include "wx/xrc/gd_frame.h"
#include "wx/xrc/gd_propdlg.h"
#include "wx/xrc/gd_wizard.h"

#include "wxguidesigner/events.h"
#include "wxguidesigner/utils.h"

#include "wxguidesigner/rtti/database.h"
#include "wxguidesigner/rtti/tree.h"

#include "wxguidesigner/xrc/serializer.h"

#include "wxguidesigner/gui/artprovider.h"
#ifdef __WXDEBUG__
    #include <wx/textctrl.h>
    #include "wxguidesigner/gui/debugwindow.h"
#endif
#include "wxguidesigner/gui/dialog/about.h"
#include "wxguidesigner/gui/dialog/prefs.h"
#include "wxguidesigner/gui/editor/designer.h"
#include "wxguidesigner/gui/editor/book.h"
#include "wxguidesigner/gui/palette.h"
#include "wxguidesigner/gui/property/book.h"
#include "wxguidesigner/gui/treeview.h"
#include "wxguidesigner/gui/mainframe.h"
#include "wxguidesigner/gui/handler.h"

wxGD::Handler::Handler()
:
wxEvtHandler(),
#ifdef __WXDEBUG__
m_debug         ( NULL ),
m_logOld        ( NULL ),
#endif
m_largeImgs     ( NULL ),
m_smallImgs     ( NULL ),
m_menuBar       ( NULL ),
m_toolBar       ( NULL ),
m_frame         ( NULL ),
m_editBook      ( NULL ),
m_palette       ( NULL ),
m_propBook      ( NULL ),
m_treeView      ( NULL )
{
    InitAllXmlHandlers();

    wxFileSystem::AddHandler( new wxArchiveFSHandler );
    wxFileSystem::AddHandler( new wxMemoryFSHandler );

    wxString wxGDXRCDir        = GetResourcePath();
    wxString wxGDXRCArchive    = wxGDXRCDir     + "wxguidesigner.xrs";
    wxString wxGDImages        = wxGDXRCArchive + "#zip:images.xrc";
    wxString wxGDMainMenu      = wxGDXRCArchive + "#zip:mainmenu.xrc";
    wxString wxGDToolbar       = wxGDXRCArchive + "#zip:toolbar.xrc";

    wxXmlResource::Get()->Load( wxGDImages );
    wxXmlResource::Get()->Load( wxGDMainMenu );
    wxXmlResource::Get()->Load( wxGDToolbar );

    bool enabled = false; int selected = 0; int language = 0;

    // wxConfigBase must be initialized in the main application,
    // so we can use its configuration to load/store values
    wxConfigBase::Get()->Read( "locale/enabled",  &enabled,  false );
    wxConfigBase::Get()->Read( "locale/selected", &selected, 0 );
/*
    if(!wxImage::FindHandler( wxBITMAP_TYPE_PNG ))
        wxImage::AddHandler( new wxPNGHandler);
*/
    wxInitAllImageHandlers();

    // Load share imagelists, add a default image for missing images
    m_smallImgs  = new wxImageList( 16,16 );
    wxBitmap bmp = wxArtProvider::GetBitmap
                    ( wxART_MISSING_IMAGE, wxART_OTHER, wxSize( 16,16 ) );
    m_smallImgs->Add( bmp );

    m_largeImgs = new wxImageList( 22,22 );
    bmp = wxArtProvider::GetBitmap
                    ( wxART_MISSING_IMAGE, wxART_OTHER, wxSize( 22,22 ) );
    m_largeImgs->Add( bmp );

    ArtProvider::Load( "controls", m_smallImgs, m_largeImgs );

    if( enabled )
    {
        switch( selected )
        {
            case 0: language = wxLANGUAGE_DEFAULT; break;
            case 1: language = wxLANGUAGE_ENGLISH; break;
            case 2: language = wxLANGUAGE_ITALIAN; break;
        }

        SelectLanguage( language );
    }

    m_frame = new MainFrame(this);
    m_tree  = RTTI::Tree( new RTTI::ObjectTree() );
}

wxGD::Handler::~Handler()
{
    delete wxLog::SetActiveTarget( m_logOld );
    m_handlers.clear();
//  wxXmlResource::Get()->ClearHandlers(); done in wxXmlResource dtor
    ArtProvider::Unload();
//  m_tree = shared_ptr< ObjectTree >();

    delete m_smallImgs;
    delete m_largeImgs;
}

wxFrame *wxGD::Handler::GetMainFrame( wxWindow *parent )
{
    return m_frame;
}

#ifdef __WXDEBUG__
wxGD::DebugWindow *wxGD::Handler::GetDebugWindow( wxWindow *parent )
{
    if( !parent )
    {
        if( m_frame )
            parent = m_frame;
        else
            return NULL;
    }

    if( !m_debug )
    {
        m_debug  = new DebugWindow( this, parent );
        m_logOld = wxLog::SetActiveTarget(new wxLogTextCtrl(m_debug));
        wxLogMessage(_("Started") );
    }

    return m_debug;
}
#endif
wxDialog *wxGD::Handler::GetAboutDialog( wxWindow *parent )
{
    if( parent )
        return new Dialog::About( parent );

    return NULL;
}

wxDialog *wxGD::Handler::GetSettingsDialog( wxWindow *parent )
{
    if( parent )
        return new Dialog::Prefs( parent );

    return NULL;
}

wxNotebook *wxGD::Handler::GetEditorBook( wxWindow *parent )
{
    if( !m_editBook )
    {
        // Force groups to use small imagelist
        ArtProvider::Load( "languages", m_smallImgs, m_largeImgs, true );

        m_editBook = new Editor::Book( this, parent );
        m_handlers.push_back( m_editBook );
    }

    return m_editBook;
}

wxNotebook *wxGD::Handler::GetPaletteBook( wxWindow *parent )
{
    if( !m_palette )
        m_palette = new ToolPalette( this, parent );

    return m_palette;
}

wxNotebook *wxGD::Handler::GetPropertyBook( wxWindow *parent )
{
    if( !m_propBook )
    {
        m_propBook = new Property::Book( this, parent );
        m_handlers.push_back( m_propBook );
    }

    return m_propBook;
}

wxToolBar *wxGD::Handler::GetToolBar( wxWindow *parent )
{
    if( !m_toolBar )
        m_toolBar = wxXmlResource::Get()->LoadToolBar( parent, "ToolBar" );

    return m_toolBar;
}

wxTreeCtrl *wxGD::Handler::GetTreeView( wxWindow *parent )
{
    if( !m_treeView )
    {
        m_treeView = new TreeView( this, parent );
        m_handlers.push_back( m_treeView );
    }

    return m_treeView;
}
//-----------------------------------------------------------------------------
// Object operations
//-----------------------------------------------------------------------------
void wxGD::Handler::CreateObject( const wxString &className, int senderId )
{
    RTTI::Object object = m_tree->CreateObject( className );
    if( !object )
        return;

    Serialize();

    RTTI::ObjectEvent event( wxGD_EVT_OBJECT_CREATED, senderId, object );
    SendEvent( event );
}

void wxGD::Handler::SelectObject( RTTI::Object object, int senderId )
{
    if( !object )
        return;

    m_tree->SelectObject( object );

    Serialize();

    RTTI::ObjectEvent event( wxGD_EVT_OBJECT_SELECTED, senderId, object );
    SendEvent( event );
}

wxGD::RTTI::Object wxGD::Handler::GetSelectedObject() const
{
    return m_tree->GetSelectedObject();
}
//-----------------------------------------------------------------------------
// Serialize
//-----------------------------------------------------------------------------
bool wxGD::Handler::Load( const wxString &filePath )
{
    return XRCSerializer::Load( m_tree, filePath );
}

bool wxGD::Handler::Save( const wxString &filePath )
{
    return XRCSerializer::Save( m_tree, filePath );
}

void wxGD::Handler::Serialize()
{
    wxXmlNode *xrcRoot = XRCSerializer::Serialize( m_tree );

// Reload the XRC project in memory
    wxXmlResource::Get()->Unload("memory:xrc.xrc");
    wxMemoryFSHandler::RemoveFile("xrc.xrc");

    wxStringOutputStream sout;
    wxXmlDocument        doc;
    doc.SetRoot( xrcRoot );
    doc.Save( sout, 4 );
    wxString xrcText = sout.GetString();

    wxMemoryFSHandler::AddFile("xrc.xrc", xrcText );
    wxXmlResource::Get()->Load("memory:xrc.xrc");

// Display the new XRC project
    wxStyledTextCtrl *xrcEditor =
                    wxDynamicCast( m_editBook->GetPage(1), wxStyledTextCtrl );
    if( xrcEditor )
        xrcEditor->SetText( xrcText );
}

void wxGD::Handler::SerializeObject( RTTI::Object object, wxXmlNode *rootNode )
{
    XRCSerializer::SerializeObject( object, rootNode );
}

void wxGD::Handler::SendEvent( wxEvent &event, bool delayed )
{
    std::vector< wxEvtHandler * >::iterator it;
    // Process the event immediatly or delay it using
    // QueueEvent to be thread safe to all wxEvtHandlers
    for( it = m_handlers.begin(); it != m_handlers.end(); ++it )
    {
        wxEvtHandler *handler = (*it);
        if( handler == event.GetEventObject() )
            continue; // Skip the sender

        if( delayed )
            handler->QueueEvent( event.Clone() );
        else
            handler->ProcessEvent( event );
    }
}

void wxGD::Handler::SelectLanguage( int language )
{
    if ( !m_locale.Init( language ) )
    {
        wxLogDebug("This language is not supported by the system.");
        return;
    } 

#ifdef __WXGTK__

    wxLocale::AddCatalogLookupPathPrefix("/usr/share/locale");
    wxLocale::AddCatalogLookupPathPrefix("/usr/local/share/locale");

#elif defined(__WXMSW__)

    wxLocale::AddCatalogLookupPathPrefix( wxStandardPaths::Get().GetDataDir() +
                                        "\\locale" );
#endif
    wxLocale::AddCatalogLookupPathPrefix("locale");

    m_locale.AddCatalog("wxguidesigner");

#ifdef __LINUX__
    wxLogNull noLog;
    m_locale.AddCatalog("fileutils");
#endif
}

void wxGD::Handler::InitAllXmlHandlers()
{
    wxXmlResource::Get()->AddHandler(new wxUnknownWidgetXmlHandler);
    wxXmlResource::Get()->AddHandler(new wxBitmapXmlHandler);
    wxXmlResource::Get()->AddHandler(new wxIconXmlHandler);
    wxXmlResource::Get()->AddHandler(new DialogXmlHandler);
    wxXmlResource::Get()->AddHandler(new wxPanelXmlHandler);
    wxXmlResource::Get()->AddHandler(new wxSizerXmlHandler);
    wxXmlResource::Get()->AddHandler(new FrameXmlHandler);
    wxXmlResource::Get()->AddHandler(new wxScrolledWindowXmlHandler);

#if wxUSE_AUI
    wxXmlResource::Get()->AddHandler(new wxAuiXmlHandler);
#endif
#if wxUSE_ANIMATIONCTRL
    wxXmlResource::Get()->AddHandler(new wxAnimationCtrlXmlHandler);
#endif
#if wxUSE_BANNERWINDOW
    wxXmlResource::Get()->AddHandler(new wxBannerWindowXmlHandler);
#endif
#if wxUSE_BITMAPCOMBOBOX
    wxXmlResource::Get()->AddHandler(new wxBitmapComboBoxXmlHandler);
#endif
#if wxUSE_BMPBUTTON
    wxXmlResource::Get()->AddHandler(new wxBitmapButtonXmlHandler);
#endif
#if wxUSE_BOOKCTRL
    wxXmlResource::Get()->AddHandler(new PropertySheetDialogXmlHandler);
#endif
#if wxUSE_BUTTON
    wxXmlResource::Get()->AddHandler(new wxStdDialogButtonSizerXmlHandler);
    wxXmlResource::Get()->AddHandler(new wxButtonXmlHandler);
#endif
#if wxUSE_CALENDARCTRL
    wxXmlResource::Get()->AddHandler(new wxCalendarCtrlXmlHandler);
#endif
#if wxUSE_CHECKBOX
    wxXmlResource::Get()->AddHandler(new wxCheckBoxXmlHandler);
#endif
#if wxUSE_CHECKLISTBOX
    wxXmlResource::Get()->AddHandler(new wxCheckListBoxXmlHandler);
#endif
#if wxUSE_CHOICE
    wxXmlResource::Get()->AddHandler(new wxChoiceXmlHandler);
#endif
#if wxUSE_CHOICEBOOK
    wxXmlResource::Get()->AddHandler(new wxChoicebookXmlHandler);
#endif
#if wxUSE_COLLPANE
    wxXmlResource::Get()->AddHandler(new wxCollapsiblePaneXmlHandler);
#endif
#if wxUSE_COLOURPICKERCTRL
    wxXmlResource::Get()->AddHandler(new wxColourPickerCtrlXmlHandler);
#endif
#if wxUSE_COMBOBOX
    wxXmlResource::Get()->AddHandler(new wxComboBoxXmlHandler);
#endif
#if wxUSE_COMBOCTRL
    wxXmlResource::Get()->AddHandler(new wxComboCtrlXmlHandler);
#endif
#if wxUSE_COMMANDLINKBUTTON
    wxXmlResource::Get()->AddHandler(new wxCommandLinkButtonXmlHandler);
#endif
#if wxUSE_DATEPICKCTRL
    wxXmlResource::Get()->AddHandler(new wxDateCtrlXmlHandler);
#endif
#if wxUSE_DIRDLG
    wxXmlResource::Get()->AddHandler(new wxGenericDirCtrlXmlHandler);
#endif
#if wxUSE_DIRPICKERCTRL
    wxXmlResource::Get()->AddHandler(new wxDirPickerCtrlXmlHandler);
#endif
#if wxUSE_EDITABLELISTBOX
    wxXmlResource::Get()->AddHandler(new wxEditableListBoxXmlHandler);
#endif
#if wxUSE_FILECTRL
    wxXmlResource::Get()->AddHandler(new wxFileCtrlXmlHandler);
#endif
#if wxUSE_FILEPICKERCTRL
    wxXmlResource::Get()->AddHandler(new wxFilePickerCtrlXmlHandler);
#endif
#if wxUSE_FONTPICKERCTRL
    wxXmlResource::Get()->AddHandler(new wxFontPickerCtrlXmlHandler);
#endif
#if wxUSE_GAUGE
    wxXmlResource::Get()->AddHandler(new wxGaugeXmlHandler);
#endif
#if wxUSE_GRID
    wxXmlResource::Get()->AddHandler(new wxGridXmlHandler);
#endif
#if wxUSE_HTML
    wxXmlResource::Get()->AddHandler(new wxHtmlWindowXmlHandler);
    wxXmlResource::Get()->AddHandler(new wxSimpleHtmlListBoxXmlHandler);
#endif
#if wxUSE_HYPERLINKCTRL
    wxXmlResource::Get()->AddHandler(new wxHyperlinkCtrlXmlHandler);
#endif
#if wxUSE_LISTBOOK
    wxXmlResource::Get()->AddHandler(new wxListbookXmlHandler);
#endif
#if wxUSE_LISTBOX
    wxXmlResource::Get()->AddHandler(new wxListBoxXmlHandler);
#endif
#if wxUSE_LISTCTRL
    wxXmlResource::Get()->AddHandler(new wxListCtrlXmlHandler);
#endif
#if wxUSE_MDI
    wxXmlResource::Get()->AddHandler(new wxMdiXmlHandler);
#endif
#if wxUSE_MENUS
    wxXmlResource::Get()->AddHandler(new wxMenuXmlHandler);
    wxXmlResource::Get()->AddHandler(new wxMenuBarXmlHandler);
#endif
#if wxUSE_NOTEBOOK
    wxXmlResource::Get()->AddHandler(new wxNotebookXmlHandler);
#endif
#if wxUSE_ODCOMBOBOX
    wxXmlResource::Get()->AddHandler(new wxOwnerDrawnComboBoxXmlHandler);
#endif
#if wxUSE_RADIOBOX
    wxXmlResource::Get()->AddHandler(new wxRadioBoxXmlHandler);
#endif
#if wxUSE_RADIOBTN
    wxXmlResource::Get()->AddHandler(new wxRadioButtonXmlHandler);
#endif
#if 0 && wxUSE_RICHTEXT
    wxXmlResource::Get()->AddHandler(new wxRichTextCtrlXmlHandler);
#endif
#if wxUSE_SCROLLBAR
    wxXmlResource::Get()->AddHandler(new wxScrollBarXmlHandler);
#endif
#if wxUSE_SEARCHCTRL
    wxXmlResource::Get()->AddHandler(new wxSearchCtrlXmlHandler);
#endif
#if wxUSE_SLIDER
    wxXmlResource::Get()->AddHandler(new wxSliderXmlHandler);
#endif
#if wxUSE_SPINBTN
    wxXmlResource::Get()->AddHandler(new wxSpinButtonXmlHandler);
#endif
#if wxUSE_SPINCTRL
    wxXmlResource::Get()->AddHandler(new wxSpinCtrlXmlHandler);
#endif
#if wxUSE_SPLITTER
    wxXmlResource::Get()->AddHandler(new wxSplitterWindowXmlHandler);
#endif
#if wxUSE_STATBMP
    wxXmlResource::Get()->AddHandler(new wxStaticBitmapXmlHandler);
#endif
#if wxUSE_STATBOX
    wxXmlResource::Get()->AddHandler(new wxStaticBoxXmlHandler);
#endif
#if wxUSE_STATLINE
    wxXmlResource::Get()->AddHandler(new wxStaticLineXmlHandler);
#endif
#if wxUSE_STATTEXT
    wxXmlResource::Get()->AddHandler(new wxStaticTextXmlHandler);
#endif
#if wxUSE_STATUSBAR
    wxXmlResource::Get()->AddHandler(new wxStatusBarXmlHandler);
#endif
    wxXmlResource::Get()->AddHandler(new wxStyledTextCtrlXmlHandler);
#if wxUSE_TEXTCTRL
    wxXmlResource::Get()->AddHandler(new wxTextCtrlXmlHandler);
#endif
#if wxUSE_TOGGLEBTN
    wxXmlResource::Get()->AddHandler(new wxToggleButtonXmlHandler);
#endif
#if wxUSE_TIMEPICKCTRL
    wxXmlResource::Get()->AddHandler(new wxTimeCtrlXmlHandler);
#endif
#if wxUSE_TOOLBAR
    wxXmlResource::Get()->AddHandler(new wxToolBarXmlHandler);
#endif
#if wxUSE_TOOLBOOK
    wxXmlResource::Get()->AddHandler(new wxToolbookXmlHandler);
#endif
#if wxUSE_TREEBOOK
    wxXmlResource::Get()->AddHandler(new wxTreebookXmlHandler);
#endif
#if wxUSE_TREECTRL
    wxXmlResource::Get()->AddHandler(new wxTreeCtrlXmlHandler);
#endif
#if wxUSE_WIZARDDLG
    wxXmlResource::Get()->AddHandler(new WizardXmlHandler);
#endif
}