#ifndef _DRAW_PANEL_H_
#define _DRAW_PANEL_H_

#include "wx/wxprec.h"

#ifndef WX_PRECOMP
#include <wx/wx.h>
#endif

#include <wx/dcbuffer.h>
#include <wx/graphics.h>

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

    DECLARE_EVENT_TABLE()

private:
    wxGraphicsRenderer* renderer {nullptr};

    double xUserScale {1.f};
    double yUserScale {1.f};

    ProtoPuddle::World* world {nullptr};
};


#endif
