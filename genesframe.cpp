#include "genesframe.h"

GenesFrame::GenesFrame(wxWindow *parent, const wxSize& size): wxFrame(parent, wxID_ANY, wxT("Genes"), wxDefaultPosition, size)
{
    genTable = new wxGrid(this, wxID_ANY);

    genTable->CreateGrid(0, 8);
    genTable->SetRowLabelSize(0);
    genTable->EnableEditing(false);
    genTable->DisableDragGridSize();

    genTable->SetDefaultCellAlignment(wxALIGN_CENTRE,wxALIGN_CENTRE);

    genTable->SetColLabelValue(0, wxT("Name"));
    genTable->SetColLabelValue(1, wxT("Empty"));
    genTable->SetColLabelValue(2, wxT("Other"));
    genTable->SetColLabelValue(3, wxT("Same"));
    genTable->SetColLabelValue(4, wxT("Meat"));
    genTable->SetColLabelValue(5, wxT("Plant"));
    genTable->SetColLabelValue(6, wxT("Wall"));
    genTable->SetColLabelValue(7, wxT("Weak"));

    Layout();
    // Fit(); //?

    SetBackgroundColour( *wxWHITE );

    /*
    if (CanSetTransparent())
    {
        SetTransparent(wxByte(200));
    }
    */

    //ToggleWindowStyle(wxRESIZE_BORDER);
    ToggleWindowStyle(wxMAXIMIZE_BOX);
}

GenesFrame::~GenesFrame()
{
    wxFrame* parent = dynamic_cast<wxFrame*>(GetParent());
    if (parent)
    {
        parent->SetStatusText(wxT("Genes frame has been deleted"), 1);
    }
}

bool GenesFrame::AddGene(const ProtoPuddle::Gene& g)
{
    genTable->AppendRows(1);

    int row = genTable->GetNumberRows() - 1;

    genTable->SetCellValue(row, 0, g.name);

    genTable->SetCellValue(row, 1, ActionToString(g.empty));
    genTable->SetCellValue(row, 2, ActionToString(g.other));
    genTable->SetCellValue(row, 3, ActionToString(g.same));
    genTable->SetCellValue(row, 4, ActionToString(g.meat));
    genTable->SetCellValue(row, 5, ActionToString(g.plant));
    genTable->SetCellValue(row, 6, ActionToString(g.wall));
    genTable->SetCellValue(row, 7, ActionToString(g.weak));

    return true;
}

bool GenesFrame::Show(bool show)
{
    return wxFrame::Show(show);
}

wxString GenesFrame::ActionToString(int action)
{
    wxString str = "unknown";

    switch (action)
    {
    case ProtoPuddle::Gene::ACTION_TURN_L:
        str = "turnl";
        break;
    case ProtoPuddle::Gene::ACTION_TURN_R:
        str = "turnr";
        break;
    case ProtoPuddle::Gene::ACTION_MOVE:
        str = "move";
        break;
    case ProtoPuddle::Gene::ACTION_NONE:
        str = "none";
        break;
    case ProtoPuddle::Gene::ACTION_ATTACK:
        str = "attack";
        break;
    case ProtoPuddle::Gene::ACTION_EAT:
        str = "eat";
        break;
    default:
        break;
    }

    return str;
}

