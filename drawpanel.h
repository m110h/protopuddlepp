#ifndef _DRAW_PANEL_H_
#define _DRAW_PANEL_H_

#include "wx/wxprec.h"

#ifndef WX_PRECOMP
#include <wx/wx.h>
#endif

#include <wx/dcbuffer.h>

#include "entities.h"

class BasicDrawPanel : public wxPanel
{
public:
    BasicDrawPanel(wxWindow* parent, wxWindowID id, const wxSize& size);

    void onSize(wxSizeEvent& event);
    void onPaint(wxPaintEvent & evt);
    void mouseReleased(wxMouseEvent& event);

    /*
     void mouseMoved(wxMouseEvent& event) {}
     void mouseDown(wxMouseEvent& event) {}
     void mouseWheelMoved(wxMouseEvent& event) {}
     void mouseReleased(wxMouseEvent& event) {}
     void rightClick(wxMouseEvent& event) {}
     void mouseLeftWindow(wxMouseEvent& event) {}
     void keyPressed(wxKeyEvent& event) {}
     void keyReleased(wxKeyEvent& event) {}
     */

    void paintNow();
    void SetWorld(ProtoPuddle::World* _world);

private:
    void softwareRender(wxDC* dc);

/*
    void drawSizeText(wxDC* dc)
    {
        dc->DrawText(wxString::Format("Current size: %i %i", currentSize.GetX(), currentSize.GetY()), 10, 3);
        dc->DrawText(wxString::Format("Original size: %i %i", originalSize.GetX(), originalSize.GetY()), 10, 15);
    }
*/

    DECLARE_EVENT_TABLE()

private:
    double xUserScale {1.f};
    double yUserScale {1.f};

    ProtoPuddle::World* world {nullptr};
};


#endif
