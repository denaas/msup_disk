/***************************************************************
 * Name:      interfaceMain.cpp
 * Purpose:   Code for Application Frame
 * Author:     ()
 * Created:   2015-10-25
 * Copyright:  ()
 * License:
 **************************************************************/

#include "wx_pch.h"
#include "interfaceMain.h"
#include <wx/msgdlg.h>
#include <iostream>

using std::cout;
using std::endl;

//(*InternalHeaders(interfaceFrame)
#include <wx/intl.h>
#include <wx/string.h>
//*)

//helper functions
enum wxbuildinfoformat {
    short_f, long_f };

wxString wxbuildinfo(wxbuildinfoformat format)
{
    wxString wxbuild(wxVERSION_STRING);

    if (format == long_f )
    {
#if defined(__WXMSW__)
        wxbuild << _T("-Windows");
#elif defined(__UNIX__)
        wxbuild << _T("-Linux");
#endif

#if wxUSE_UNICODE
        wxbuild << _T("-Unicode build");
#else
        wxbuild << _T("-ANSI build");
#endif // wxUSE_UNICODE
    }

    return wxbuild;
}

//(*IdInit(interfaceFrame)
const long interfaceFrame::ID_TEXTCTRL1 = wxNewId();
const long interfaceFrame::ID_BUTTON4 = wxNewId();
const long interfaceFrame::ID_BUTTON1 = wxNewId();
const long interfaceFrame::ID_BUTTON2 = wxNewId();
const long interfaceFrame::ID_BUTTON3 = wxNewId();
//*)

BEGIN_EVENT_TABLE(interfaceFrame,wxFrame)
    //(*EventTable(interfaceFrame)
    //*)
END_EVENT_TABLE()

interfaceFrame::interfaceFrame(wxWindow* parent,wxWindowID id)
{
    //(*Initialize(interfaceFrame)
    wxBoxSizer* BoxSizer4;
    wxBoxSizer* BoxSizer2;
    wxBoxSizer* BoxSizer1;
    wxBoxSizer* BoxSizer3;

    Create(parent, wxID_ANY, wxEmptyString, wxDefaultPosition, wxDefaultSize, wxDEFAULT_FRAME_STYLE, _T("wxID_ANY"));
    SetClientSize(wxSize(1000,1000));
    BoxSizer1 = new wxBoxSizer(wxVERTICAL);
    TextCtrl1 = new wxTextCtrl(this, ID_TEXTCTRL1, _("Write name of the file"), wxDefaultPosition, wxSize(350,65), 0, wxDefaultValidator, _T("ID_TEXTCTRL1"));
    BoxSizer1->Add(TextCtrl1, 0, wxALL|wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL, 5);
    BoxSizer2 = new wxBoxSizer(wxHORIZONTAL);
    Button4 = new wxButton(this, ID_BUTTON4, _("Generate masterkey"), wxDefaultPosition, wxSize(350,65), 0, wxDefaultValidator, _T("ID_BUTTON4"));
    BoxSizer2->Add(Button4, 0, wxALL|wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL, 5);
    BoxSizer1->Add(BoxSizer2, 0, wxALL|wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL, 5);
    BoxSizer3 = new wxBoxSizer(wxHORIZONTAL);
    Button1 = new wxButton(this, ID_BUTTON1, _("Encode"), wxDefaultPosition, wxSize(170,65), 0, wxDefaultValidator, _T("ID_BUTTON1"));
    BoxSizer3->Add(Button1, 0, wxALL|wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL, 5);
    Button2 = new wxButton(this, ID_BUTTON2, _("Decode"), wxDefaultPosition, wxSize(170,65), 0, wxDefaultValidator, _T("ID_BUTTON2"));
    BoxSizer3->Add(Button2, 0, wxALL|wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL, 5);
    BoxSizer1->Add(BoxSizer3, 0, wxALL|wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL, 5);
    BoxSizer4 = new wxBoxSizer(wxHORIZONTAL);
    Button3 = new wxButton(this, ID_BUTTON3, _("Delete encoded data"), wxDefaultPosition, wxSize(350,65), 0, wxDefaultValidator, _T("ID_BUTTON3"));
    BoxSizer4->Add(Button3, 0, wxALL|wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL, 5);
    BoxSizer1->Add(BoxSizer4, 0, wxALL|wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL, 5);
    SetSizer(BoxSizer1);
    SetSizer(BoxSizer1);
    Layout();

    Connect(ID_TEXTCTRL1,wxEVT_COMMAND_TEXT_UPDATED,(wxObjectEventFunction)&interfaceFrame::OnTextCtrl1Text);
    Connect(ID_BUTTON4,wxEVT_COMMAND_BUTTON_CLICKED,(wxObjectEventFunction)&interfaceFrame::OnButton4Click);
    Connect(ID_BUTTON1,wxEVT_COMMAND_BUTTON_CLICKED,(wxObjectEventFunction)&interfaceFrame::OnButton1Click);
    Connect(ID_BUTTON2,wxEVT_COMMAND_BUTTON_CLICKED,(wxObjectEventFunction)&interfaceFrame::OnButton2Click);
    Connect(ID_BUTTON3,wxEVT_COMMAND_BUTTON_CLICKED,(wxObjectEventFunction)&interfaceFrame::OnButton3Click);
    //*)
}

interfaceFrame::~interfaceFrame()
{
    //(*Destroy(interfaceFrame)
    //*)
}

void interfaceFrame::OnQuit(wxCommandEvent& event)
{
    Close();
}

void interfaceFrame::OnAbout(wxCommandEvent& event)
{
    wxString msg = wxbuildinfo(long_f);
    wxMessageBox(msg, _("Welcome to..."));
}

void interfaceFrame::OnTextCtrl1Text(wxCommandEvent& event)
{
}

void interfaceFrame::OnButton1Click(wxCommandEvent& event)
{
    wxString tx = TextCtrl1->GetLineText(0);
    //here should be code of encoding files


    TextCtrl1->Clear();
    const wxString& name = _T("File was successfully encoded");
    TextCtrl1->AppendText(name);
}

void interfaceFrame::OnButton2Click(wxCommandEvent& event)
{
    wxString tx = TextCtrl1->GetLineText(0);
    //here should be code of decoding files


    TextCtrl1->Clear();
    const wxString& name = _T("File was successfully decoded");
    TextCtrl1->AppendText(name);
}

void interfaceFrame::OnButton3Click(wxCommandEvent& event)
{
    TextCtrl1->Clear();
    const wxString& name = _T("File was successfully deleted");
    TextCtrl1->AppendText(name);
}

void interfaceFrame::OnButton4Click(wxCommandEvent& event)
{
    TextCtrl1->Clear();
    const wxString& name = _T("Masterkey was successfully created");
    TextCtrl1->AppendText(name);
}

