#include "wx/wxprec.h"

#ifndef WX_PRECOMP
#include <wx/wx.h>
#endif

#include <wx/dcbuffer.h>

#include <wx/preferences.h>
#include <wx/statline.h>
#include <wx/spinctrl.h>
#include <wx/grid.h>

#include "entities.h"
#include "constants.h"

class MyFrame;

class MyApp: public wxApp
{
public:
    const ProtoPuddle::GlobalProperties& GetProperties() const;
    ProtoPuddle::GlobalProperties* GetPropertiesPtr();

    void UpdateProperties(const ProtoPuddle::GlobalProperties& properties);

    void ShowPropertiesEditor(wxWindow* parent);
    void DismissPropertiesEditor();

private:
    bool OnInit() override;
    int OnExit() override;

private:
    ProtoPuddle::GlobalProperties properties;
    MyFrame *frame;

    wxScopedPtr<wxPreferencesEditor> propertiesEditor;
};

IMPLEMENT_APP(MyApp)

class PrefsPageGeneralPanel : public wxPanel
{
public:
    PrefsPageGeneralPanel(wxWindow *parent) : wxPanel(parent)
    {
        aliveCellsCtrl = new wxSpinCtrl(this, wxID_ANY);
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

        aliveCellsCtrl->SetRange(properties.GetMin(wxString("aliveCells")),properties.GetMax(wxString("aliveCells")));
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

        aliveCellsCtrl->Bind(wxEVT_SPINCTRL, &PrefsPageGeneralPanel::ChangedAliveCells, this);
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

        propertiesSizer->Add(new wxStaticText(this, wxID_ANY, wxT("Initial quantity of alive cells")), wxSizerFlags().Border());
        propertiesSizer->Add(aliveCellsCtrl, wxSizerFlags().Border().Expand());
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
    }

    virtual bool TransferDataToWindow() override
    {
        properties = wxGetApp().GetProperties();

        aliveCellsCtrl->SetValue(properties.GetValue(wxString("aliveCells")));
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

    virtual bool TransferDataFromWindow() override
    {
        // Called on platforms with modal preferences dialog to save and apply
        // the changes.
        wxGetApp().UpdateProperties(properties);
        return true;
    }

private:
    void UpdateSettingsIfNecessary()
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

    void ChangedAliveCells(wxSpinEvent& e) {
        properties.SetValue(wxString("aliveCells"), e.GetValue());
        UpdateSettingsIfNecessary();
    }
    void ChangedCellEnergy(wxSpinEvent& e) {
        properties.SetValue(wxString("cellEnergy"), e.GetValue());
        UpdateSettingsIfNecessary();
    }
    void ChangedMaxDamage(wxSpinEvent& e) {
        properties.SetValue(wxString("maxDamage"), e.GetValue());
        UpdateSettingsIfNecessary();
    }
    void ChangedBehaviorGenes(wxSpinEvent& e) {
        properties.SetValue(wxString("behaviorGenes"), e.GetValue());
        UpdateSettingsIfNecessary();
    }
    void ChangedMinEnergyForDivision(wxSpinEvent& e) {
        properties.SetValue(wxString("minEnergyForDivision"), e.GetValue());
        UpdateSettingsIfNecessary();
    }
    void ChangedMaxEnergyForDivision(wxSpinEvent& e) {
        properties.SetValue(wxString("maxEnergyForDivision"), e.GetValue());
        UpdateSettingsIfNecessary();
    }
    void ChangedPlants(wxSpinEvent& e) {
        properties.SetValue(wxString("plants"), e.GetValue());
        UpdateSettingsIfNecessary();
    }
    void ChangedPlantEnergy(wxSpinEvent& e) {
        properties.SetValue(wxString("plantEnergy"), e.GetValue());
        UpdateSettingsIfNecessary();
    }
    void ChangedMeatEnergy(wxSpinEvent& e) {
        properties.SetValue(wxString("meatEnergy"), e.GetValue());
        UpdateSettingsIfNecessary();
    }
    void ChangedMaxAge(wxSpinEvent& e) {
        properties.SetValue(wxString("maxAge"), e.GetValue());
        UpdateSettingsIfNecessary();
    }
    void ChangedStepsPerSecond(wxSpinEvent& e) {
        properties.SetValue(wxString("stepsPerSecond"), e.GetValue());
        UpdateSettingsIfNecessary();
    }
    void ChangedPlantsPerStep(wxSpinEvent& e) {
        properties.SetValue(wxString("plantsPerStep"), e.GetValue());
        UpdateSettingsIfNecessary();
    }
    void ChangedWorldWidth(wxSpinEvent& e) {
        properties.SetValue(wxString("worldWidth"), e.GetValue());
        UpdateSettingsIfNecessary();
    }
    void ChangedWorldHeight(wxSpinEvent& e) {
        properties.SetValue(wxString("worldHeight"), e.GetValue());
        UpdateSettingsIfNecessary();
    }

private:
    wxSpinCtrl* aliveCellsCtrl {nullptr};
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

    // Settings corresponding to the current values in this dialog.
    ProtoPuddle::GlobalProperties properties;
};

class PrefsPageGeneral : public wxStockPreferencesPage
{
public:
    PrefsPageGeneral() : wxStockPreferencesPage(Kind_General) {}
    virtual wxWindow *CreateWindow(wxWindow *parent) override
        { return new PrefsPageGeneralPanel(parent); }
};


class GenesFrame : public wxFrame
{
public:
    GenesFrame(wxWindow *parent, const wxSize& size): wxFrame(parent, wxID_ANY, wxT("Genes"), wxDefaultPosition, size)
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

    bool AddGene(const ProtoPuddle::Gene& g)
    {
        genTable->AppendRows(1);

        int row = genTable->GetNumberRows() - 1;

        genTable->SetCellValue(row, 0, g.name);
        genTable->SetCellValue(row, 1, g.empty);
        genTable->SetCellValue(row, 2, g.other);
        genTable->SetCellValue(row, 3, g.same);
        genTable->SetCellValue(row, 4, g.meat);
        genTable->SetCellValue(row, 5, g.plant);
        genTable->SetCellValue(row, 6, g.wall);
        genTable->SetCellValue(row, 7, g.weak);

        return true;
    }

    ~GenesFrame()
    {
        wxFrame* parent = dynamic_cast<wxFrame*>(GetParent());
        if (parent)
        {
            parent->SetStatusText(wxT("Genes frame has been deleted"), 1);
        }
    }

    bool Show(bool show = true) override
    {
        return wxFrame::Show(show);
    }

private:
    wxGrid* genTable {nullptr};
};

enum
{
    ID_DRAW_PANEL_LEFT = 10000,
    ID_DRAW_PANEL = 10001,
    ID_MY_FRAME = 10002
};

class BasicDrawPanel : public wxPanel
{
public:
    BasicDrawPanel(wxWindow* parent, wxWindowID id, const wxSize& size): wxPanel(parent, id, wxDefaultPosition, size /*, wxBORDER_SIMPLE*/ )
    {
        SetBackgroundColour(*wxWHITE);
    }

    void onSize(wxSizeEvent& event)
    {
        //xUserScale = double(event.GetSize().GetWidth()) / double(originalSize.GetWidth());
        //yUserScale = double(event.GetSize().GetHeight()) / double(originalSize.GetHeight());

        if (world)
            world->SetPanelSize(event.GetSize());

        paintNow();

        //event.Skip();
    }

    // some useful events
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

    void mouseReleased(wxMouseEvent& event)
    {
        world->SelectEntityByPosition(world->PanelToWorld(event.GetPosition()));
        paintNow();

        wxPostEvent(GetParent(), event);

        //if (selectedId != -1)
        //    wxMessageBox(wxString::Format(wxT("X: %d; Y: %d;"), pos.x, pos.y), wxT("Mouse Click"), wxOK | wxICON_INFORMATION, this);
    }

    void paintNow()
    {
        wxClientDC dc(this);
        wxBufferedDC bdc(&dc, dc.GetSize()); // for prevent flickering
        softwareRender(&bdc);
    }

    void onPaint(wxPaintEvent & evt)
    {
        wxBufferedPaintDC dc(this);
        softwareRender(&dc);
    }

    void SetWorld(ProtoPuddle::World* _world)
    {
        world = _world;
    }

private:
    void softwareRender(wxDC* dc)
    {
        dc->Clear();

        if (world)
            world->Draw(dc);
    }

/*
    void drawSizeText(wxDC* dc)
    {
        dc->DrawText(wxString::Format("Current size: %i %i", currentSize.GetX(), currentSize.GetY()), 10, 3);
        dc->DrawText(wxString::Format("Original size: %i %i", originalSize.GetX(), originalSize.GetY()), 10, 15);
    }
*/

    DECLARE_EVENT_TABLE()

private:
    double xUserScale {1.f};
    double yUserScale {1.f};

    ProtoPuddle::World* world {nullptr};
};

class MyFrame: public wxFrame
{
public:
    MyFrame(): wxFrame(nullptr, ID_MY_FRAME, wxT("ProtoPuddle++"), wxDefaultPosition, wxSize(800,600))
    {
        MakeMenu();
        MakeStatusBar();
        Setting();
        MakeLayout();

        world = new ProtoPuddle::World(wxGetApp().GetPropertiesPtr());
        world->New();

        if (worldView)
            worldView->SetWorld(world);

        UpdateInformation();

        // handle left click on draw panel
        this->Bind(wxEVT_LEFT_UP, [&](wxMouseEvent& event) {
            UpdateInformation();
        });

        // handle timer event
        timer.Bind(wxEVT_TIMER, [&](wxTimerEvent& event) {
            Step();
        });
    }

    ~MyFrame() {
        if (world)
            delete world;
    }

    void OnNew(wxCommandEvent& event) {
        if (timer.IsRunning())
            timer.Stop();

        world->New();
        worldView->paintNow();

        UpdateInformation();

        SetStatusText(wxString::Format("%s%d", "Steps: ", world->GetSteps()), 2);
        SetStatusText(wxT("Ready"), 0);
        SetStatusText(wxT("New action performed"), 1);
    }

    void OnOpen(wxCommandEvent& event) {
        wxMessageBox(wxT("This will be realesed in the future"), wxT("Open"), wxOK | wxICON_INFORMATION, this);
        SetStatusText(wxT("Open action performed"), 1);
    }
    void OnSave(wxCommandEvent& event) {
        wxMessageBox(wxT("This will be realesed in the future"), wxT("Save"), wxOK | wxICON_INFORMATION, this);
        SetStatusText(wxT("Save action performed"), 1);
    }
    void OnQuit(wxCommandEvent& event) {
        Close(true);
    }

    void OnSimulation(wxCommandEvent& event)
    {
        SwitchSimulation();
    }

    void OnStep(wxCommandEvent& event)
    {
        Step();
    }

    void OnShowLog(wxCommandEvent& event)
    {
        wxMessageBox(wxT("This will be realesed in the future"), wxT("Log"), wxOK | wxICON_INFORMATION, this);
        SetStatusText(wxT("Show log action performed"), 1);
    }
    void OnProperties(wxCommandEvent& event)
    {
        wxGetApp().ShowPropertiesEditor(this);
    }

    void OnDescription(wxCommandEvent& event)
    {
        wxMessageBox(wxT("This will be realesed in the future"), wxT("Description"), wxOK | wxICON_INFORMATION, this);
        SetStatusText(wxT("Description action performed"), 1);
    }
    void OnAbout(wxCommandEvent& event)
    {
        wxMessageBox(wxT("ProtoPuddle++, Version 1.0, developed by Alexey Orlov, 2020"), wxT("About"), wxOK | wxICON_INFORMATION, this);
    }

    enum {
        myID_MENU_EDIT_SIMULATION,
        myID_MENU_EDIT_STEP,
        myID_MENU_EDIT_SHOW_LOG,
        myID_MENU_EDIT_PROPERTIES,
        myID_MENU_HELP_SHOW_DESCRIPTION
    };

    void UpdateSettingsCtrls()
    {
        ProtoPuddle::GlobalProperties properties = wxGetApp().GetProperties();

        spsSpinCtrl->SetValue(properties.GetValue(wxString("stepsPerSecond")));
        ppsSpinCtrl->SetValue(properties.GetValue(wxString("plantsPerStep")));
        peSpinCtrl->SetValue(properties.GetValue(wxString("plantEnergy")));
        meSpinCtrl->SetValue(properties.GetValue(wxString("meatEnergy")));
        mdSpinCtrl->SetValue(properties.GetValue(wxString("maxDamage")));
    }

    void StopSimulation()
    {
        if (timer.IsRunning())
        {
            timer.Stop();
            SetStatusText(wxT("Paused"), 0);
            SetStatusText(wxT("Simulation has been stopped"), 1);
        }
    }

    void StartSimulation()
    {
        if (!timer.IsRunning())
        {
            ProtoPuddle::GlobalProperties properties = wxGetApp().GetProperties();

            timer.Start(1000 / properties.GetValue(wxString("stepsPerSecond")));

            SetStatusText(wxT("Running"), 0);
            SetStatusText(wxT("Simulation has been started"), 1);
        }
    }

    void RestartSimulation()
    {
        if (timer.IsRunning())
        {
            StopSimulation();
            StartSimulation();
        }
    }

    void SwitchSimulation()
    {
        if (timer.IsRunning())
        {
            StopSimulation();
        }
        else {
            StartSimulation();
        }
    }

    ProtoPuddle::World* GetWorld()
    {
        return world;
    }

private:
    wxTimer timer;

    wxSpinCtrl* spsSpinCtrl {nullptr};
    wxSpinCtrl* ppsSpinCtrl {nullptr};
    wxSpinCtrl* peSpinCtrl {nullptr};
    wxSpinCtrl* meSpinCtrl {nullptr};
    wxSpinCtrl* mdSpinCtrl {nullptr};

    wxStaticText* topIdText {nullptr};
    wxStaticText* cellsCountText {nullptr};
    wxStaticText* plantsCountText {nullptr};
    wxStaticText* meatCountText {nullptr};

    wxStaticText* idText {nullptr};
    wxStaticText* ageText {nullptr};
    wxStaticText* maxAgeText {nullptr};
    wxStaticText* energyText {nullptr};
    wxStaticText* divEnergyText {nullptr};
    wxStaticText* mutationText {nullptr};
    wxStaticText* damageText {nullptr};
    wxStaticText* killsText {nullptr};
    wxStaticText* childrensText {nullptr};
    wxStaticText* eatenPlantsText {nullptr};
    wxStaticText* eatenMeatText {nullptr};

    wxButton* showGenesBtn {nullptr};

    BasicDrawPanel* worldView {nullptr};
    ProtoPuddle::World* world {nullptr};

private:
    void UpdateInformation()
    {
        topIdText->SetLabel(wxString::Format(wxT("%d"), world->GetTopId()));

        int pc {0}; int mc {0}; int cc {0};

        world->GetEntitiesQuantity(pc, mc, cc);

        cellsCountText->SetLabel(wxString::Format(wxT("%d"), cc));
        plantsCountText->SetLabel(wxString::Format(wxT("%d"), pc));
        meatCountText->SetLabel(wxString::Format(wxT("%d"), mc));

        ProtoPuddle::Entity* e = world->GetSelectedEntity();

        if (e)
        {
            idText->SetLabel(e->Get("id"));
            ageText->SetLabel(e->Get("age"));
            maxAgeText->SetLabel(e->Get("maxAge"));
            energyText->SetLabel(e->Get("energy"));
            divEnergyText->SetLabel(e->Get("divEnergy"));
            mutationText->SetLabel(e->Get("mutation"));
            damageText->SetLabel(e->Get("damage"));
            killsText->SetLabel(e->Get("kills"));
            childrensText->SetLabel(e->Get("childrens"));
            eatenPlantsText->SetLabel(e->Get("eatenPlants"));
            eatenMeatText->SetLabel(e->Get("eatenMeat"));

            // enable showGenesBtn if disabled
        }
        else {
            idText->SetLabel(ProtoPuddle::unknownValueStr);
            ageText->SetLabel(ProtoPuddle::unknownValueStr);
            maxAgeText->SetLabel(ProtoPuddle::unknownValueStr);
            energyText->SetLabel(ProtoPuddle::unknownValueStr);
            divEnergyText->SetLabel(ProtoPuddle::unknownValueStr);
            mutationText->SetLabel(ProtoPuddle::unknownValueStr);
            damageText->SetLabel(ProtoPuddle::unknownValueStr);
            killsText->SetLabel(ProtoPuddle::unknownValueStr);
            childrensText->SetLabel(ProtoPuddle::unknownValueStr);
            eatenPlantsText->SetLabel(ProtoPuddle::unknownValueStr);
            eatenMeatText->SetLabel(ProtoPuddle::unknownValueStr);

            // disable showGenesBtn if enabled
        }
    }

    void Step()
    {
        world->Step();
        worldView->paintNow();

        UpdateInformation();

        SetStatusText(wxString::Format("%s%d", "Steps: ", world->GetSteps()), 2);
    }

    void ApplySettings(wxCommandEvent& event)
    {
        ProtoPuddle::GlobalProperties properties = wxGetApp().GetProperties();

        properties.SetValue(wxString("stepsPerSecond"), spsSpinCtrl->GetValue());
        properties.SetValue(wxString("plantsPerStep"), ppsSpinCtrl->GetValue());
        properties.SetValue(wxString("plantEnergy"), peSpinCtrl->GetValue());
        properties.SetValue(wxString("meatEnergy"), meSpinCtrl->GetValue());
        properties.SetValue(wxString("maxDamage"), mdSpinCtrl->GetValue());

        wxGetApp().UpdateProperties(properties);

        RestartSimulation();

        SetStatusText(wxT("Settings have been applied"), 1);
    }

    void MakeMenu()
    {
        wxMenuBar* menuBar = new wxMenuBar;

        wxMenu* menuFile = new wxMenu;

        menuFile->Append(wxID_NEW, wxT("&New\tCtrl+n"));
        menuFile->Append(wxID_OPEN, wxT("&Open\tCtrl+o"));
        menuFile->Append(wxID_SAVE, wxT("&Save\tCtrl+s"));
        menuFile->AppendSeparator();
        menuFile->Append(wxID_EXIT, wxT("&Quit\tCtrl+q"));

        wxMenu* menuEdit = new wxMenu;

        menuEdit->Append(myID_MENU_EDIT_SIMULATION, wxT("&Start/Stop Simulation\tCtrl+r"));
        menuEdit->Append(myID_MENU_EDIT_STEP, wxT("&One Step\tCtrl+x"));
        menuEdit->Append(myID_MENU_EDIT_SHOW_LOG, wxT("Show &Log\tCtrl+l"));
        menuEdit->Append(myID_MENU_EDIT_PROPERTIES, wxT("&Properties\tCtrl+p"));

        wxMenu* menuHelp = new wxMenu;

        menuHelp->Append(myID_MENU_HELP_SHOW_DESCRIPTION, wxT("&Description\tCtrl+d"));
        menuHelp->Append(wxID_ABOUT, wxT("&About"));

        menuBar->Append(menuFile, wxT("&File"));
        menuBar->Append(menuEdit, wxT("&Edit"));
        menuBar->Append(menuHelp, wxT("&Help"));

        this->SetMenuBar(menuBar);

        menuBar->Bind(wxEVT_MENU, [&](wxCommandEvent& event) {
            switch (event.GetId())
            {
            case wxID_NEW:
                OnNew(event);
                break;
            case wxID_OPEN:
                OnOpen(event);
                break;
            case wxID_SAVE:
                OnSave(event);
                break;
            case wxID_EXIT:
                OnQuit(event);
                break;
            case myID_MENU_EDIT_SIMULATION:
                OnSimulation(event);
                break;
            case myID_MENU_EDIT_STEP:
                OnStep(event);
                break;
            case myID_MENU_EDIT_SHOW_LOG:
                OnShowLog(event);
                break;
            case myID_MENU_EDIT_PROPERTIES:
                OnProperties(event);
                break;
            case myID_MENU_HELP_SHOW_DESCRIPTION:
                OnDescription(event);
                break;
            case wxID_ABOUT:
                OnAbout(event);
                break;
            default:
                SetStatusText(wxT("Menu: unknown action!"), 1);
                break;
            }
        });
    }

    void MakeStatusBar()
    {
        CreateStatusBar(3);

        SetStatusText(wxT("Ready"), 0);
        SetStatusText(wxT("No actions performed"), 1);
        SetStatusText(wxString::Format("%s%d", "Steps: ", 0), 2);
    }

    void MakeLayout()
    {
        wxBoxSizer* topSizer = new wxBoxSizer(wxHORIZONTAL);

        // wxDefaultPosition, wxDefaultSize
        wxScrolledWindow* sw = new wxScrolledWindow(this, wxID_ANY, wxDefaultPosition, wxSize(200,600));
        wxBoxSizer* vswSizer = new wxBoxSizer(wxVERTICAL);

        // Settings Grop

        ProtoPuddle::GlobalProperties properties = wxGetApp().GetProperties();

        wxStaticBox* settingsGroupBox = new wxStaticBox(sw, wxID_ANY, "Settings");
        wxStaticBoxSizer * vSettingsSizer = new wxStaticBoxSizer (settingsGroupBox, wxVERTICAL);

        wxGridSizer* settingsSizer = new wxGridSizer(5, 2, 3, 0);
            settingsSizer->Add(new wxStaticText(settingsGroupBox, wxID_ANY, wxT("Steps per second")), 0, wxEXPAND);
                spsSpinCtrl = new wxSpinCtrl(settingsGroupBox, wxID_ANY);
                spsSpinCtrl->SetRange(properties.GetMin(wxString("stepsPerSecond")) ,properties.GetMax(wxString("stepsPerSecond")));
                settingsSizer->Add(spsSpinCtrl, 1, wxEXPAND);
            settingsSizer->Add(new wxStaticText(settingsGroupBox, wxID_ANY, wxT("Plants per step")), 0, wxEXPAND);
                ppsSpinCtrl = new wxSpinCtrl(settingsGroupBox, wxID_ANY);
                ppsSpinCtrl->SetRange(properties.GetMin(wxString("plantsPerStep")) ,properties.GetMax(wxString("plantsPerStep")));
                settingsSizer->Add(ppsSpinCtrl, 1, wxEXPAND);
            settingsSizer->Add(new wxStaticText(settingsGroupBox, wxID_ANY, wxT("Plant energy")), 0, wxEXPAND);
                peSpinCtrl = new wxSpinCtrl(settingsGroupBox, wxID_ANY);
                peSpinCtrl->SetRange(properties.GetMin(wxString("plantEnergy")) ,properties.GetMax(wxString("plantEnergy")));
                settingsSizer->Add(peSpinCtrl, 1, wxEXPAND);
            settingsSizer->Add(new wxStaticText(settingsGroupBox, wxID_ANY, wxT("Meat energy")), 0, wxEXPAND);
                meSpinCtrl = new wxSpinCtrl(settingsGroupBox, wxID_ANY);
                meSpinCtrl->SetRange(properties.GetMin(wxString("meatEnergy")) ,properties.GetMax(wxString("meatEnergy")));
                settingsSizer->Add(meSpinCtrl, 1, wxEXPAND);
            settingsSizer->Add(new wxStaticText(settingsGroupBox, wxID_ANY, wxT("Max damage")), 0, wxEXPAND);
                mdSpinCtrl = new wxSpinCtrl(settingsGroupBox, wxID_ANY);
                mdSpinCtrl->SetRange(properties.GetMin(wxString("maxDamage")) ,properties.GetMax(wxString("maxDamage")));
                settingsSizer->Add(mdSpinCtrl, 1, wxEXPAND);

            UpdateSettingsCtrls();

        vSettingsSizer->Add(settingsSizer, 0, wxEXPAND);
        vSettingsSizer->Add(new wxStaticLine(settingsGroupBox, wxID_STATIC, wxDefaultPosition, wxDefaultSize, wxLI_HORIZONTAL), 0, wxGROW|wxTOP|wxBOTTOM, 10);

        wxButton* applySettingsBtn = new wxButton(settingsGroupBox, wxID_ANY, wxT("Apply"));
        applySettingsBtn->Bind(wxEVT_BUTTON, &MyFrame::ApplySettings, this);
        vSettingsSizer->Add(applySettingsBtn, 0, wxALIGN_RIGHT|wxALL, 5);

        vswSizer->Add(vSettingsSizer, 0, wxEXPAND | wxALL, 10);

        // Information Group

        wxStaticBox* infoGroupBox = new wxStaticBox(sw, wxID_ANY, "Common Information");
        wxStaticBoxSizer * vInfoSizer = new wxStaticBoxSizer (infoGroupBox, wxVERTICAL);

        wxGridSizer* infoSizer = new wxGridSizer(4, 2, 3, 0);
            infoSizer->Add(new wxStaticText(infoGroupBox, wxID_ANY, wxT("Top Id")));
                topIdText = new wxStaticText(infoGroupBox, wxID_ANY, wxT("0"));
                infoSizer->Add(topIdText);
            infoSizer->Add(new wxStaticText(infoGroupBox, wxID_ANY, wxT("Alive cells")));
                cellsCountText = new wxStaticText(infoGroupBox, wxID_ANY, ProtoPuddle::unknownValueStr);
                infoSizer->Add(cellsCountText);
            infoSizer->Add(new wxStaticText(infoGroupBox, wxID_ANY, wxT("Plants count")));
                plantsCountText = new wxStaticText(infoGroupBox, wxID_ANY, ProtoPuddle::unknownValueStr);
                infoSizer->Add(plantsCountText);
            infoSizer->Add(new wxStaticText(infoGroupBox, wxID_ANY, wxT("Meat count")));
                meatCountText = new wxStaticText(infoGroupBox, wxID_ANY, ProtoPuddle::unknownValueStr);
                infoSizer->Add(meatCountText);

        vInfoSizer->Add(infoSizer, 0, wxEXPAND);
        vswSizer->Add(vInfoSizer, 0, wxEXPAND | wxALL, 10);

        // Selected Cell Group

        wxStaticBox* selectedGroupBox = new wxStaticBox(sw, wxID_ANY, "Cell Information");
        wxStaticBoxSizer * vSelectedSizer = new wxStaticBoxSizer (selectedGroupBox, wxVERTICAL);

        wxGridSizer* selectedSizer = new wxGridSizer(11, 2, 3, 0);
            selectedSizer->Add(new wxStaticText(selectedGroupBox, wxID_ANY, wxT("Id number")));
                idText = new wxStaticText(selectedGroupBox, wxID_ANY, ProtoPuddle::unknownValueStr);
                selectedSizer->Add(idText);
            selectedSizer->Add(new wxStaticText(selectedGroupBox, wxID_ANY, wxT("Age")));
                ageText = new wxStaticText(selectedGroupBox, wxID_ANY, ProtoPuddle::unknownValueStr);
                selectedSizer->Add(ageText);
            selectedSizer->Add(new wxStaticText(selectedGroupBox, wxID_ANY, wxT("Max age")));
                maxAgeText = new wxStaticText(selectedGroupBox, wxID_ANY, ProtoPuddle::unknownValueStr);
                selectedSizer->Add(maxAgeText);
            selectedSizer->Add(new wxStaticText(selectedGroupBox, wxID_ANY, wxT("Energy")));
                energyText = new wxStaticText(selectedGroupBox, wxID_ANY, ProtoPuddle::unknownValueStr);
                selectedSizer->Add(energyText);
            selectedSizer->Add(new wxStaticText(selectedGroupBox, wxID_ANY, wxT("Energy for div")));
                divEnergyText = new wxStaticText(selectedGroupBox, wxID_ANY, ProtoPuddle::unknownValueStr);
                selectedSizer->Add(divEnergyText);
            selectedSizer->Add(new wxStaticText(selectedGroupBox, wxID_ANY, wxT("Mutation (%)")));
                mutationText = new wxStaticText(selectedGroupBox, wxID_ANY, ProtoPuddle::unknownValueStr);
                selectedSizer->Add(mutationText);
            selectedSizer->Add(new wxStaticText(selectedGroupBox, wxID_ANY, wxT("Damage")));
                damageText = new wxStaticText(selectedGroupBox, wxID_ANY, ProtoPuddle::unknownValueStr);
                selectedSizer->Add(damageText);
            selectedSizer->Add(new wxStaticText(selectedGroupBox, wxID_ANY, wxT("Kills")));
                killsText = new wxStaticText(selectedGroupBox, wxID_ANY, ProtoPuddle::unknownValueStr);
                selectedSizer->Add(killsText);
            selectedSizer->Add(new wxStaticText(selectedGroupBox, wxID_ANY, wxT("Childrens")));
                childrensText = new wxStaticText(selectedGroupBox, wxID_ANY, ProtoPuddle::unknownValueStr);
                selectedSizer->Add(childrensText);
            selectedSizer->Add(new wxStaticText(selectedGroupBox, wxID_ANY, wxT("Eaten plants")));
                eatenPlantsText = new wxStaticText(selectedGroupBox, wxID_ANY, ProtoPuddle::unknownValueStr);
                selectedSizer->Add(eatenPlantsText);
            selectedSizer->Add(new wxStaticText(selectedGroupBox, wxID_ANY, wxT("Eaten meat")));
                eatenMeatText = new wxStaticText(selectedGroupBox, wxID_ANY, ProtoPuddle::unknownValueStr);
                selectedSizer->Add(eatenMeatText);

        vSelectedSizer->Add(selectedSizer, 0, wxEXPAND);
        vSelectedSizer->Add(new wxStaticLine(selectedGroupBox, wxID_STATIC, wxDefaultPosition, wxDefaultSize, wxLI_HORIZONTAL), 0, wxGROW|wxTOP|wxBOTTOM, 10);

        showGenesBtn = new wxButton(selectedGroupBox, wxID_ANY, wxT("Show Genes"));
        showGenesBtn->Bind(wxEVT_BUTTON, [this](wxCommandEvent& event) {
            ProtoPuddle::Entity* e = world->GetSelectedEntity();

            if (e && e->GetType() == ProtoPuddle::EntityType::TYPE_CELL)
            {
                ProtoPuddle::Cell* c = dynamic_cast<ProtoPuddle::Cell*>(e);
                GenesFrame* table = new GenesFrame(this, wxSize(500,400));
                    table->AddGene(c->GetGene());
                table->Show();
            }
        });

        vSelectedSizer->Add(showGenesBtn, 0, wxALIGN_RIGHT|wxALL, 5);

        vswSizer->Add(vSelectedSizer, 0, wxEXPAND | wxALL, 10);

        sw->SetSizer(vswSizer);
        sw->SetScrollbars(10, 10, sw->GetSize().GetWidth()/10, sw->GetSize().GetHeight()/10);

        topSizer->Add(sw, 1, wxEXPAND);

        topSizer->Add(new wxStaticLine(this, wxID_STATIC, wxDefaultPosition, wxDefaultSize, wxLI_VERTICAL), 0, wxGROW);

        worldView = new BasicDrawPanel(this, ID_DRAW_PANEL, wxSize(500, 500));
        topSizer->Add(worldView, 2, wxEXPAND, 0);

        this->SetSizer(topSizer);
        this->SetAutoLayout(true);
    }

    void Setting()
    {
        SetMinSize(wxSize(800, 600));
    }
};

bool MyApp::OnInit()
{
    if (!wxApp::OnInit()) return false;

    frame = new MyFrame;
    frame->Show();

    return true;
}

int MyApp::OnExit()
{
    return 0;
}

const ProtoPuddle::GlobalProperties& MyApp::GetProperties() const
{
    return properties;
}

ProtoPuddle::GlobalProperties* MyApp::GetPropertiesPtr()
{
    return &properties;
}

void MyApp::UpdateProperties(const ProtoPuddle::GlobalProperties& _properties)
{
    properties = _properties;

    frame->UpdateSettingsCtrls();
}

void MyApp::ShowPropertiesEditor(wxWindow* parent)
{
    if ( !propertiesEditor )
    {
        propertiesEditor.reset(new wxPreferencesEditor(wxT("Properties")));
        propertiesEditor->AddPage(new PrefsPageGeneral);
    }

    propertiesEditor->Show(parent);
}

void MyApp::DismissPropertiesEditor()
{
    if ( propertiesEditor )
        propertiesEditor->Dismiss();
}

BEGIN_EVENT_TABLE(BasicDrawPanel, wxPanel)
    // some useful events
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
