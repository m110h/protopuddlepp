#ifndef _PROPERTIES_DIALOG_H_
#define _PROPERTIES_DIALOG_H_

#include "wx/wxprec.h"

#ifndef WX_PRECOMP
#include <wx/wx.h>
#endif

#include <wx/preferences.h>
#include <wx/spinctrl.h>

#include "properties.h"

class PrefsPageGeneralPanel : public wxPanel
{
public:
    PrefsPageGeneralPanel(wxWindow* parent);

    virtual bool TransferDataToWindow() override;
    virtual bool TransferDataFromWindow() override;

private:
    void UpdateSettingsIfNecessary();
    void NotifyMainFrame();

    void ChangedSortsOfCell(wxSpinEvent& e);
    void ChangedCellEnergy(wxSpinEvent& e);
    void ChangedMaxDamage(wxSpinEvent& e);
    void ChangedBehaviorGenes(wxSpinEvent& e);
    void ChangedMinEnergyForDivision(wxSpinEvent& e);
    void ChangedMaxEnergyForDivision(wxSpinEvent& e);
    void ChangedPlants(wxSpinEvent& e);
    void ChangedPlantEnergy(wxSpinEvent& e);
    void ChangedMeatEnergy(wxSpinEvent& e);
    void ChangedMaxAge(wxSpinEvent& e);
    void ChangedStepsPerSecond(wxSpinEvent& e);
    void ChangedPlantsPerStep(wxSpinEvent& e);
    void ChangedWorldWidth(wxSpinEvent& e);
    void ChangedWorldHeight(wxSpinEvent& e);

private:
    wxSpinCtrl* sortsOfCellCtrl {nullptr};
    wxSpinCtrl* cellEnergyCtrl {nullptr};
    wxSpinCtrl* maxDamageCtrl {nullptr};
    wxSpinCtrl* behaviorGenesCtrl {nullptr};
    wxSpinCtrl* minEnergyForDivisionCtrl {nullptr};
    wxSpinCtrl* maxEnergyForDivisionCtrl {nullptr};
    wxSpinCtrl* plantsCtrl {nullptr};
    wxSpinCtrl* plantEnergyCtrl {nullptr};
    wxSpinCtrl* meatEnergyCtrl {nullptr};
    wxSpinCtrl* maxAgeCtrl {nullptr};
    wxSpinCtrl* stepsPerSecondCtrl {nullptr};
    wxSpinCtrl* plantsPerStepCtrl {nullptr};
    wxSpinCtrl* worldWidthCtrl {nullptr};
    wxSpinCtrl* worldHeightCtrl {nullptr};

    bool showAdvice {false};

    // Settings corresponding to the current values in this dialog.
    ProtoPuddle::GlobalProperties properties;
};

class PrefsPageGeneral : public wxStockPreferencesPage
{
public:
    PrefsPageGeneral();
    virtual wxWindow* CreateWindow(wxWindow* parent) override;
};

#endif
