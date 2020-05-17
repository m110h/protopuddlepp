#include "propertiesdialog.h"
#include "properties_singleton.h"

PrefsPageGeneralPanel::PrefsPageGeneralPanel(wxWindow *parent) : wxPanel(parent)
{
    sortsOfCellCtrl = new wxSpinCtrl(this, wxID_ANY);
    cellEnergyCtrl = new wxSpinCtrl(this, wxID_ANY);
    maxDamageCtrl = new wxSpinCtrl(this, wxID_ANY);
    behaviorGenesCtrl = new wxSpinCtrl(this, wxID_ANY);
    minEnergyForDivisionCtrl = new wxSpinCtrl(this, wxID_ANY);
    maxEnergyForDivisionCtrl = new wxSpinCtrl(this, wxID_ANY);
    plantsCtrl = new wxSpinCtrl(this, wxID_ANY);
    plantEnergyCtrl = new wxSpinCtrl(this, wxID_ANY);
    meatEnergyCtrl = new wxSpinCtrl(this, wxID_ANY);
    maxAgeCtrl = new wxSpinCtrl(this, wxID_ANY);
    stepsPerSecondCtrl = new wxSpinCtrl(this, wxID_ANY);
    plantsPerStepCtrl = new wxSpinCtrl(this, wxID_ANY);
    worldWidthCtrl = new wxSpinCtrl(this, wxID_ANY);
    worldHeightCtrl = new wxSpinCtrl(this, wxID_ANY);

    sortsOfCellCtrl->SetRange(properties.GetMin(wxString("sortsOfCell")),properties.GetMax(wxString("sortsOfCell")));
    cellEnergyCtrl->SetRange(properties.GetMin(wxString("cellEnergy")),properties.GetMax(wxString("cellEnergy")));
    maxDamageCtrl->SetRange(properties.GetMin(wxString("maxDamage")),properties.GetMax(wxString("maxDamage")));
    behaviorGenesCtrl->SetRange(properties.GetMin(wxString("behaviorGenes")),properties.GetMax(wxString("behaviorGenes")));
    minEnergyForDivisionCtrl->SetRange(properties.GetMin(wxString("minEnergyForDivision")),properties.GetMax(wxString("minEnergyForDivision")));
    maxEnergyForDivisionCtrl->SetRange(properties.GetMin(wxString("maxEnergyForDivision")),properties.GetMax(wxString("maxEnergyForDivision")));
    plantsCtrl->SetRange(properties.GetMin(wxString("plants")),properties.GetMax(wxString("plants")));
    plantEnergyCtrl->SetRange(properties.GetMin(wxString("plantEnergy")),properties.GetMax(wxString("plantEnergy")));
    meatEnergyCtrl->SetRange(properties.GetMin(wxString("meatEnergy")),properties.GetMax(wxString("meatEnergy")));
    maxAgeCtrl->SetRange(properties.GetMin(wxString("maxAge")),properties.GetMax(wxString("maxAge")));
    stepsPerSecondCtrl->SetRange(properties.GetMin(wxString("stepsPerSecond")),properties.GetMax(wxString("stepsPerSecond")));
    plantsPerStepCtrl->SetRange(properties.GetMin(wxString("plantsPerStep")),properties.GetMax(wxString("plantsPerStep")));
    worldWidthCtrl->SetRange(properties.GetMin(wxString("worldWidth")),properties.GetMax(wxString("worldWidth")));
    worldHeightCtrl->SetRange(properties.GetMin(wxString("worldHeight")),properties.GetMax(wxString("worldHeight")));

    sortsOfCellCtrl->Bind(wxEVT_SPINCTRL, &PrefsPageGeneralPanel::ChangedSortsOfCell, this);
    cellEnergyCtrl->Bind(wxEVT_SPINCTRL, &PrefsPageGeneralPanel::ChangedCellEnergy, this);
    maxDamageCtrl->Bind(wxEVT_SPINCTRL, &PrefsPageGeneralPanel::ChangedMaxDamage, this);
    behaviorGenesCtrl->Bind(wxEVT_SPINCTRL, &PrefsPageGeneralPanel::ChangedBehaviorGenes, this);
    minEnergyForDivisionCtrl->Bind(wxEVT_SPINCTRL, &PrefsPageGeneralPanel::ChangedMinEnergyForDivision, this);
    maxEnergyForDivisionCtrl->Bind(wxEVT_SPINCTRL, &PrefsPageGeneralPanel::ChangedMaxEnergyForDivision, this);
    plantsCtrl->Bind(wxEVT_SPINCTRL, &PrefsPageGeneralPanel::ChangedPlants, this);
    plantEnergyCtrl->Bind(wxEVT_SPINCTRL, &PrefsPageGeneralPanel::ChangedPlantEnergy, this);
    meatEnergyCtrl->Bind(wxEVT_SPINCTRL, &PrefsPageGeneralPanel::ChangedMeatEnergy, this);
    maxAgeCtrl->Bind(wxEVT_SPINCTRL, &PrefsPageGeneralPanel::ChangedMaxAge, this);
    stepsPerSecondCtrl->Bind(wxEVT_SPINCTRL, &PrefsPageGeneralPanel::ChangedStepsPerSecond, this);
    plantsPerStepCtrl->Bind(wxEVT_SPINCTRL, &PrefsPageGeneralPanel::ChangedPlantsPerStep, this);
    worldWidthCtrl->Bind(wxEVT_SPINCTRL, &PrefsPageGeneralPanel::ChangedWorldWidth, this);
    worldHeightCtrl->Bind(wxEVT_SPINCTRL, &PrefsPageGeneralPanel::ChangedWorldHeight, this);

    wxGridSizer* propertiesSizer = new wxGridSizer(14, 2, 0, 0);

    propertiesSizer->Add(new wxStaticText(this, wxID_ANY, wxT("Initial quantity of sorts")), wxSizerFlags().Border());
    propertiesSizer->Add(sortsOfCellCtrl, wxSizerFlags().Border().Expand());
    propertiesSizer->Add(new wxStaticText(this, wxID_ANY, wxT("Initial cell's energy")), wxSizerFlags().Border());
    propertiesSizer->Add(cellEnergyCtrl, wxSizerFlags().Border().Expand());
    propertiesSizer->Add(new wxStaticText(this, wxID_ANY, wxT("Max damage")), wxSizerFlags().Border());
    propertiesSizer->Add(maxDamageCtrl, wxSizerFlags().Border().Expand());
    propertiesSizer->Add(new wxStaticText(this, wxID_ANY, wxT("Quantity of behavior genes")), wxSizerFlags().Border());
    propertiesSizer->Add(behaviorGenesCtrl, wxSizerFlags().Border().Expand());
    propertiesSizer->Add(new wxStaticText(this, wxID_ANY, wxT("Min energy for division")), wxSizerFlags().Border());
    propertiesSizer->Add(minEnergyForDivisionCtrl, wxSizerFlags().Border().Expand());
    propertiesSizer->Add(new wxStaticText(this, wxID_ANY, wxT("Max energy for division")), wxSizerFlags().Border());
    propertiesSizer->Add(maxEnergyForDivisionCtrl, wxSizerFlags().Border().Expand());
    propertiesSizer->Add(new wxStaticText(this, wxID_ANY, wxT("Initial quantity of plants")), wxSizerFlags().Border());
    propertiesSizer->Add(plantsCtrl, wxSizerFlags().Border().Expand());
    propertiesSizer->Add(new wxStaticText(this, wxID_ANY, wxT("Plants' energy")), wxSizerFlags().Border());
    propertiesSizer->Add(plantEnergyCtrl, wxSizerFlags().Border().Expand());
    propertiesSizer->Add(new wxStaticText(this, wxID_ANY, wxT("Meat's energy")), wxSizerFlags().Border());
    propertiesSizer->Add(meatEnergyCtrl, wxSizerFlags().Border().Expand());
    propertiesSizer->Add(new wxStaticText(this, wxID_ANY, wxT("Max cell's age")), wxSizerFlags().Border());
    propertiesSizer->Add(maxAgeCtrl, wxSizerFlags().Border().Expand());
    propertiesSizer->Add(new wxStaticText(this, wxID_ANY, wxT("Steps per second")), wxSizerFlags().Border());
    propertiesSizer->Add(stepsPerSecondCtrl, wxSizerFlags().Border().Expand());
    propertiesSizer->Add(new wxStaticText(this, wxID_ANY, wxT("Plants per step")), wxSizerFlags().Border());
    propertiesSizer->Add(plantsPerStepCtrl, wxSizerFlags().Border().Expand());
    propertiesSizer->Add(new wxStaticText(this, wxID_ANY, wxT("Width of cell's world")), wxSizerFlags().Border());
    propertiesSizer->Add(worldWidthCtrl, wxSizerFlags().Border().Expand());
    propertiesSizer->Add(new wxStaticText(this, wxID_ANY, wxT("Height of cell's world")), wxSizerFlags().Border());
    propertiesSizer->Add(worldHeightCtrl, wxSizerFlags().Border().Expand());

    SetSizerAndFit(propertiesSizer);

    //wxMessageBox(wxString::Format("id: %d", parent->GetId()), wxT("Info"), wxOK | wxICON_INFORMATION, this);
}

bool PrefsPageGeneralPanel::TransferDataToWindow()
{
    properties = PropertiesSingleton::getInstance().GetProperties();

    sortsOfCellCtrl->SetValue(properties.GetValue(wxString("sortsOfCell")));
    cellEnergyCtrl->SetValue(properties.GetValue(wxString("cellEnergy")));
    maxDamageCtrl->SetValue(properties.GetValue(wxString("maxDamage")));
    behaviorGenesCtrl->SetValue(properties.GetValue(wxString("behaviorGenes")));
    minEnergyForDivisionCtrl->SetValue(properties.GetValue(wxString("minEnergyForDivision")));
    maxEnergyForDivisionCtrl->SetValue(properties.GetValue(wxString("maxEnergyForDivision")));
    plantsCtrl->SetValue(properties.GetValue(wxString("plants")));
    plantEnergyCtrl->SetValue(properties.GetValue(wxString("plantEnergy")));
    meatEnergyCtrl->SetValue(properties.GetValue(wxString("meatEnergy")));
    maxAgeCtrl->SetValue(properties.GetValue(wxString("maxAge")));
    stepsPerSecondCtrl->SetValue(properties.GetValue(wxString("stepsPerSecond")));
    plantsPerStepCtrl->SetValue(properties.GetValue(wxString("plantsPerStep")));
    worldWidthCtrl->SetValue(properties.GetValue(wxString("worldWidth")));
    worldHeightCtrl->SetValue(properties.GetValue(wxString("worldHeight")));

    return true;
}

// Called on platforms with modal preferences dialog to save and apply
// the changes.
bool PrefsPageGeneralPanel::TransferDataFromWindow()
{
    PropertiesSingleton::getInstance().UpdateProperties(properties);
    return true;
}

void PrefsPageGeneralPanel::UpdateSettingsIfNecessary()
{
    return;
    // On some platforms (OS X, GNOME), changes to preferences are applied
    // immediately rather than after the OK or Apply button is pressed, so
    // we need to take them into account as soon as they happen. On others
    // (MSW), we need to wait until the changes are accepted by the user by
    // pressing the "OK" button. To reuse the same code for both cases, we
    // always update m_settingsCurrent object under all platforms, but only
    // update the real application settings if necessary here.
    //if ( wxPreferencesEditor::ShouldApplyChangesImmediately() )
    //{
        //wxGetApp().UpdateProperties(properties);
        //wxMessageBox(wxT("UpdateSettingsIfNecessary"), wxT("Hook"), wxOK | wxICON_INFORMATION, this);
    //}
}

void PrefsPageGeneralPanel::ChangedSortsOfCell(wxSpinEvent& e) {
    properties.SetValue(wxString("sortsOfCell"), e.GetValue());
    UpdateSettingsIfNecessary();
}
void PrefsPageGeneralPanel::ChangedCellEnergy(wxSpinEvent& e) {
    properties.SetValue(wxString("cellEnergy"), e.GetValue());
    UpdateSettingsIfNecessary();
}
void PrefsPageGeneralPanel::ChangedMaxDamage(wxSpinEvent& e) {
    properties.SetValue(wxString("maxDamage"), e.GetValue());
    UpdateSettingsIfNecessary();
}
void PrefsPageGeneralPanel::ChangedBehaviorGenes(wxSpinEvent& e) {
    properties.SetValue(wxString("behaviorGenes"), e.GetValue());
    UpdateSettingsIfNecessary();
}
void PrefsPageGeneralPanel::ChangedMinEnergyForDivision(wxSpinEvent& e) {
    properties.SetValue(wxString("minEnergyForDivision"), e.GetValue());
    UpdateSettingsIfNecessary();
}
void PrefsPageGeneralPanel::ChangedMaxEnergyForDivision(wxSpinEvent& e) {
    properties.SetValue(wxString("maxEnergyForDivision"), e.GetValue());
    UpdateSettingsIfNecessary();
}
void PrefsPageGeneralPanel::ChangedPlants(wxSpinEvent& e) {
    properties.SetValue(wxString("plants"), e.GetValue());
    UpdateSettingsIfNecessary();
}
void PrefsPageGeneralPanel::ChangedPlantEnergy(wxSpinEvent& e) {
    properties.SetValue(wxString("plantEnergy"), e.GetValue());
    UpdateSettingsIfNecessary();
}
void PrefsPageGeneralPanel::ChangedMeatEnergy(wxSpinEvent& e) {
    properties.SetValue(wxString("meatEnergy"), e.GetValue());
    UpdateSettingsIfNecessary();
}
void PrefsPageGeneralPanel::ChangedMaxAge(wxSpinEvent& e) {
    properties.SetValue(wxString("maxAge"), e.GetValue());
    UpdateSettingsIfNecessary();
}
void PrefsPageGeneralPanel::ChangedStepsPerSecond(wxSpinEvent& e) {
    properties.SetValue(wxString("stepsPerSecond"), e.GetValue());
    UpdateSettingsIfNecessary();
}
void PrefsPageGeneralPanel::ChangedPlantsPerStep(wxSpinEvent& e) {
    properties.SetValue(wxString("plantsPerStep"), e.GetValue());
    UpdateSettingsIfNecessary();
}
void PrefsPageGeneralPanel::ChangedWorldWidth(wxSpinEvent& e) {
    properties.SetValue(wxString("worldWidth"), e.GetValue());
    UpdateSettingsIfNecessary();
}
void PrefsPageGeneralPanel::ChangedWorldHeight(wxSpinEvent& e) {
    properties.SetValue(wxString("worldHeight"), e.GetValue());
    UpdateSettingsIfNecessary();
}

//

PrefsPageGeneral::PrefsPageGeneral(): wxStockPreferencesPage(Kind_General) {}

wxWindow* PrefsPageGeneral::CreateWindow(wxWindow *parent)
{
    return new PrefsPageGeneralPanel(parent);
}

