/////////////////////////////////////////////////////////////////////////////
// Name:               genesframe.h
// Description:        ...
// Author:             Alexey Orlov (https://github.com/m110h)
// Last modification:  18/08/2020
// Licence:            MIT licence
/////////////////////////////////////////////////////////////////////////////

#ifndef _GENES_FRAME_H_
#define _GENES_FRAME_H_

#include "wx/wxprec.h"

#ifndef WX_PRECOMP
#include <wx/wx.h>
#endif

#include <wx/grid.h>

#include "gene.h"

class GenesFrame : public wxFrame
{
public:
    GenesFrame(wxWindow *parent, const wxSize& size);
    ~GenesFrame();

    bool AddGene(const ProtoPuddle::Gene& g);
    bool Show(bool show = true) override;

private:
    wxString ActionToString(int action);

private:
    wxGrid* genTable {nullptr};
};


#endif
