///////////////////////////////////////////////////////////////////////////////
// Name:        wxguidesigner/gui/property/props.h
// Purpose:     
// Author:      Andrea Zanellato
// Modified by:
// Created:     2011/11/20
// Revision:    $Hash$
// Copyleft:    (ɔ) Andrea Zanellato
// Licence:     GNU General Public License Version 3
///////////////////////////////////////////////////////////////////////////////
#ifndef __WXGUIDESIGNER_GUI_PROPERTY_PROPS_H__
#define __WXGUIDESIGNER_GUI_PROPERTY_PROPS_H__

namespace wxGD
{
namespace Property
{
//=============================================================================
// wxGD::Property::Colour
//=============================================================================
class Colour : public wxSystemColourProperty
{
    WX_PG_DECLARE_PROPERTY_CLASS( wxGD::Property::Colour )

public:
    Colour( const wxString              &label  = wxPG_LABEL,
                    const wxString              &name   = wxPG_LABEL,
                    const wxColourPropertyValue &value  =
                                                wxColourPropertyValue() );
    virtual ~Colour();

    virtual wxString ColourToString( const wxColour &colour,
                                    int index, int flags = 0 ) const;
};
//=============================================================================
// wxGD::Property::Flags
//=============================================================================
class Flags : public wxPGProperty
{
    WX_PG_DECLARE_PROPERTY_CLASS( wxGD::Property::Flags )

public:
    Flags ( const wxString         &label  = wxPG_LABEL,
            const wxString         &name   = wxPG_LABEL,
            const wxArrayString    &labels = wxArrayString(),
            const wxArrayInt       &values = wxArrayInt(),
            int                    value   = 0 );

    virtual ~Flags();

    virtual void        OnSetValue();
    virtual wxString    ValueToString ( wxVariant &value, int flags = 0 ) const;
    virtual bool        StringToValue ( wxVariant &variant,
                                        const wxString &text, int flags = 0 ) const;
    virtual wxVariant   ChildChanged  ( wxVariant &thisValue, int childIndex,
                                        wxVariant &childValue ) const;
    virtual void        RefreshChildren();
    virtual bool        DoSetAttribute( const wxString &name, wxVariant &value );

    // GetChoiceSelection needs to overridden since m_choices is
    // used and value is integer, but it is not index.
    virtual int         GetChoiceSelection() const { return wxNOT_FOUND; }

    // helpers
    size_t              GetItemCount() const { return m_choices.GetCount(); }
    const wxString      &GetLabel( size_t index ) const
                        { return m_choices.GetLabel( static_cast<int>(index) ); }
protected:
    // Creates children and sets value.
    void                Init();

    // Converts string id to a relevant bit.
    long                IdToBit( const wxString &id ) const;

    // Used to detect if choices have been changed
    wxPGChoicesData     *m_oldChoicesData;

    // Needed to properly mark changed sub-properties
    long                m_oldValue;
};
//=============================================================================
// wxGD::Property::Font
//=============================================================================
class Font : public wxPGProperty
{
    WX_PG_DECLARE_PROPERTY_CLASS( wxGD::Property::Font )

public:
    Font  ( const wxString          &label = wxPG_LABEL,
            const wxString          &name  = wxPG_LABEL,
            const wxFontContainer   &value = *wxNORMAL_FONT );

    virtual ~Font();

    virtual void        OnSetValue();
    virtual wxString    ValueToString ( wxVariant       &value,
                                        int             flags = 0 ) const;

    virtual bool        StringToValue ( wxVariant       &variant,
                                        const wxString  &text,
                                        int             flags = 0 ) const;

    virtual bool        OnEvent       ( wxPropertyGrid *propgrid,
                                        wxWindow *primary, wxEvent &event );

    virtual wxVariant   ChildChanged  ( wxVariant &thisValue, int childIndex,
                                        wxVariant &childValue ) const;
    virtual void        RefreshChildren();
};
//=============================================================================
// wxGD::Property::Point
//=============================================================================
class Point : public wxPGProperty
{
    WX_PG_DECLARE_PROPERTY_CLASS( wxGD::Property::Point )

public:
    Point ( const wxString &label = wxPG_LABEL,
            const wxString &name  = wxPG_LABEL,
            const wxPoint  &value = wxPoint() );

    virtual ~Point();

    virtual wxString    ValueToString ( wxVariant       &value,
                                        int             flags = 0 ) const;

    virtual bool        StringToValue ( wxVariant       &variant,
                                        const wxString  &text,
                                        int             flags = 0 ) const;

    virtual wxVariant   ChildChanged  ( wxVariant       &thisValue,
                                        int             childIndex,
                                        wxVariant       &childValue ) const;

    virtual void        RefreshChildren();

protected:
    void DoSetValue( const wxPoint &value ) { m_value = WXVARIANT( value ); }
};
//=============================================================================
// wxGD::Property::Size
//=============================================================================
class Size : public wxPGProperty
{
    WX_PG_DECLARE_PROPERTY_CLASS( wxGD::Property::Size )

public:
    Size  ( const wxString &label = wxPG_LABEL,
            const wxString &name  = wxPG_LABEL,
            const wxSize   &value = wxSize() );

    virtual ~Size();

    virtual wxString    ValueToString ( wxVariant       &value,
                                        int             flags = 0 ) const;

    virtual bool        StringToValue ( wxVariant       &variant,
                                        const wxString  &text,
                                        int             flags = 0 ) const;

    virtual wxVariant   ChildChanged  ( wxVariant       &thisValue,
                                        int             childIndex,
                                        wxVariant       &childValue ) const;

    virtual void        RefreshChildren();

protected:
    void DoSetValue( const wxSize &value ) { m_value = WXVARIANT( value ); }
};
}; // namespace Property
}; // namespace wxGD

#endif //__WXGUIDESIGNER_GUI_PROPERTY_PROPS_H__
