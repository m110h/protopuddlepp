#include "drawpanel.h"

/**/
#include <wx/dcgraph.h>

BasicDrawPanel::BasicDrawPanel(wxWindow* parent, wxWindowID id, const wxSize& size): wxPanel(parent, id, wxDefaultPosition, size /*, wxBORDER_SIMPLE*/ )
{
    SetBackgroundColour(*wxWHITE);

#if wxUSE_GRAPHICS_CONTEXT
#ifdef __WXMSW__
#if wxUSE_GRAPHICS_DIRECT2D
    renderer = wxGraphicsRenderer::GetDirect2DRenderer();
    //wxMessageBox(wxT("DIRECT2D"), wxT("Renderer"), wxOK | wxICON_INFORMATION, this);
#elif wxUSE_GRAPHICS_GDIPLUS
    renderer = wxGraphicsRenderer::GetGDIPlusRenderer();
    //wxMessageBox(wxT("GDIPLUS"), wxT("Renderer"), wxOK | wxICON_INFORMATION, this);
#endif
#endif // __WXMSW__
#endif // wxUSE_GRAPHICS_CONTEXT

}

void BasicDrawPanel::onSize(wxSizeEvent& event)
{
    if (world)
        world->SetPanelSize(event.GetSize());

    paintNow();
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
    if (renderer)
    {
        wxGraphicsContext* context {nullptr};

        if ( wxBufferedPaintDC* bufferedpaintdc = wxDynamicCast(dc, wxBufferedPaintDC) )
        {
            context = renderer->CreateContext(*bufferedpaintdc);
        }
        else if ( wxBufferedDC* buffereddc = wxDynamicCast(dc, wxBufferedDC) )
        {
            context = renderer->CreateContext(*buffereddc);
        }
        else
        {
            return;
        }

        context->SetAntialiasMode(wxANTIALIAS_DEFAULT);
        //context->SetAntialiasMode(wxANTIALIAS_NONE);

        wxGCDC gdc;

        gdc.SetGraphicsContext(context);
        gdc.Clear();

        if (world)
            world->Draw(static_cast<wxDC*>(&gdc));
    }
    else
    {
        dc->Clear();

        if (world)
            world->Draw(dc);
    }
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
