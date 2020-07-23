#include "drawpanel.h"

BasicDrawPanel::BasicDrawPanel(wxWindow* parent, wxWindowID id, const wxSize& size): wxPanel(parent, id, wxDefaultPosition, size /*, wxBORDER_SIMPLE*/ )
{
    SetBackgroundColour(*wxWHITE);
    SetBackgroundStyle(wxBG_STYLE_PAINT);
/*
// direct2d will enable only for ms compiler, why?
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
*/
    this->Bind(wxEVT_SIZE, [&](wxSizeEvent& event){
        onSize(event);
    });

    this->Bind(wxEVT_LEFT_UP, [&](wxMouseEvent& event){
        mouseReleased(event);
    });

    this->Bind(wxEVT_PAINT, [&](wxPaintEvent& event){
        onPaint(event);
    });

}

void BasicDrawPanel::onSize(wxSizeEvent& event)
{
    if (world)
        world->SetPanelSize(event.GetSize());

#ifdef __WXMSW__
    paintNow();
#endif

}

void BasicDrawPanel::mouseReleased(wxMouseEvent& event)
{
	if (world)
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
    //softwareRender(&dc);
}

void BasicDrawPanel::onPaint(wxPaintEvent & evt)
{
    wxAutoBufferedPaintDC dc(this);
    //wxPaintDC dc(this);
    softwareRender(&dc);
}

void BasicDrawPanel::SetWorld(ProtoPuddle::World* _world)
{
    world = _world;
}

bool BasicDrawPanel::SwitchAntialiasingMode()
{
    return (antialiasingFlag = !antialiasingFlag);
}

void BasicDrawPanel::softwareRender(wxDC* dc)
{
	if (dc)
	{
		dc->Clear();

		if (world)
		    world->Draw(dc);
	}
/*
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

        if (antialiasingFlag)
        {
            context->SetAntialiasMode(wxANTIALIAS_DEFAULT);
        }
        else
        {
            context->SetAntialiasMode(wxANTIALIAS_NONE);
        }

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
*/
}

