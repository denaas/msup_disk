/***************************************************************
 * Name:      interfaceMain.h
 * Purpose:   Defines Application Frame
 * Author:     ()
 * Created:   2015-10-25
 * Copyright:  ()
 * License:
 **************************************************************/

#ifndef INTERFACEMAIN_H
#define INTERFACEMAIN_H

//(*Headers(interfaceFrame)
#include <wx/sizer.h>
#include <wx/textctrl.h>
#include <wx/button.h>
#include <wx/frame.h>
//*)

class interfaceFrame: public wxFrame
{
    public:

        interfaceFrame(wxWindow* parent,wxWindowID id = -1);
        virtual ~interfaceFrame();

    private:

        //(*Handlers(interfaceFrame)
        void OnQuit(wxCommandEvent& event);
        void OnAbout(wxCommandEvent& event);
        void OnComboBox1Selected(wxCommandEvent& event);
        void OnSearchCtrl1Text(wxCommandEvent& event);
        void OnTextCtrl1Text(wxCommandEvent& event);
        void OnButton1Click(wxCommandEvent& event);
        void OnButton2Click(wxCommandEvent& event);
        void OnButton3Click(wxCommandEvent& event);
        void OnButton4Click(wxCommandEvent& event);
        //*)

        //(*Identifiers(interfaceFrame)
        static const long ID_TEXTCTRL1;
        static const long ID_BUTTON4;
        static const long ID_BUTTON1;
        static const long ID_BUTTON2;
        static const long ID_BUTTON3;
        //*)

        //(*Declarations(interfaceFrame)
        wxButton* Button4;
        wxButton* Button1;
        wxButton* Button2;
        wxButton* Button3;
        wxTextCtrl* TextCtrl1;
        //*)

        DECLARE_EVENT_TABLE()
};

#endif // INTERFACEMAIN_H
