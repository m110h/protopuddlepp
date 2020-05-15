#include "drawpanel.h"

BasicDrawPanel::BasicDrawPanel(wxWindow* parent, wxWindowID id, const wxSize& size): wxPanel(parent, id, wxDefaultPosition, size /*, wxBORDER_SIMPLE*/ )
{
    SetBackgroundColour(*wxWHITE);
}

void BasicDrawPanel::onSize(wxSizeEvent& event)
{
    //xUserScale = double(event.GetSize().GetWidth()) / double(originalSize.GetWidth());
    //yUserScale = double(event.GetSize().GetHeight()) / double(originalSize.GetHeight());

    if (world)
        world->SetPanelSize(event.GetSize());

    paintNow();

    //event.Skip();
}

void BasicDrawPanel::mouseReleased(wxMouseEvent& event)
{
    world->SelectEntityByPosition(world->PanelToWorld(event.GetPosition()));
    paintNow();

    wxPostEvent(GetParent(), event);

    //if (selectedId != -1)
    //    wxMessageBox(wxString::Format(wxT("X: %d; Y: %d;"), pos.x, pos.y), wxT("Mouse Click"), wxOK | wxICON_INFORMATION, this);
}

void BasicDrawPanel::paintNow()
{
    wxClientDC dc(this);
    wxBufferedDC bdc(&dc, dc.GetSize()); // for prevent flickering
    softwareRender(&bdc);
}

void BasicDrawPanel::onPaint(wxPaintEvent & evt)
{
    wxBufferedPaintDC dc(this);
    softwareRender(&dc);
}

void BasicDrawPanel::SetWorld(ProtoPuddle::World* _world)
{
    world = _world;
}

void BasicDrawPanel::softwareRender(wxDC* dc)
{
    dc->Clear();

    if (world)
        world->Draw(dc);
}

BEGIN_EVENT_TABLE(BasicDrawPanel, wxPanel)
    /*
    EVT_MOTION(BasicDrawPanel::mouseMoved)
    EVT_LEFT_DOWN(BasicDrawPanel::mouseDown)
    EVT_LEFT_UP(BasicDrawPanel::mouseReleased)
    EVT_RIGHT_DOWN(BasicDrawPanel::rightClick)
    EVT_LEAVE_WINDOW(BasicDrawPanel::mouseLeftWindow)
    EVT_KEY_DOWN(BasicDrawPanel::keyPressed)
    EVT_KEY_UP(BasicDrawPanel::keyReleased)
    EVT_MOUSEWHEEL(BasicDrawPanel::mouseWheelMoved)
    */

    EVT_LEFT_UP(BasicDrawPanel::mouseReleased)

    // catch paint events
    EVT_PAINT(BasicDrawPanel::onPaint)

    // catch resize events
    EVT_SIZE(BasicDrawPanel::onSize)

END_EVENT_TABLE()
