#ifndef _GENE_H_
#define _GENE_H_

#include <wx/string.h>

namespace ProtoPuddle
{

struct Gene
{
    Gene() {}
    Gene(const wxString& _name): name(_name) {}

    Gene(const Gene& src) {
        name = src.name;
        empty = src.empty;
        other = src.other;
        same = src.same;
        meat = src.meat;
        plant = src.plant;
        wall = src.wall;
        weak = src.weak;
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

        return *this;
    }

    // todo: use enum instead wxString

    wxString name {""};
    wxString empty {"none"};
    wxString other {"none"};
    wxString same {"none"};
    wxString meat {"none"};
    wxString plant {"none"};
    wxString wall {"none"};
    wxString weak {"none"};
};

}

#endif
