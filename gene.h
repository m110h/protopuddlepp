/////////////////////////////////////////////////////////////////////////////
// Name:               gene.h
// Description:        ...
// Author:             Alexey Orlov (https://github.com/m110h)
// Last modification:  18/08/2020
// Licence:            MIT licence
/////////////////////////////////////////////////////////////////////////////

#ifndef _GENE_H_
#define _GENE_H_

#include <wx/string.h>

namespace ProtoPuddle
{

struct Gene
{
    explicit Gene(const wxString& _name = ""): name(_name) {}

    Gene(const Gene& src) {
        name = src.name;
        empty = src.empty;
        other = src.other;
        same = src.same;
        meat = src.meat;
        plant = src.plant;
        wall = src.wall;
        weak = src.weak;
        dead = src.dead;
    }

    ~Gene() {}

    Gene& operator=(const Gene& r) {
        if (this == &r) {
            return *this;
        }

        name = r.name;
        empty = r.empty;
        other = r.other;
        same = r.same;
        meat = r.meat;
        plant = r.plant;
        wall = r.wall;
        weak = r.weak;
        dead = r.dead;

        return *this;
    }

    // a type of cell's action
    enum
    {
        ACTION_NONE,
        ACTION_TURN_L,
        ACTION_TURN_R,
        ACTION_MOVE,
        ACTION_ATTACK,
        ACTION_EAT
    };

    wxString name {""};

    int empty {ACTION_NONE};
    int other {ACTION_NONE};
    int same {ACTION_NONE};
    int meat {ACTION_NONE};
    int plant {ACTION_NONE};
    int wall {ACTION_NONE};
    int weak {ACTION_NONE};
    int dead {ACTION_NONE};
};

}

#endif
