#include "wx/wxprec.h"

#ifndef WX_PRECOMP
#include <wx/wx.h>
#endif

#include <wx/aboutdlg.h>
#include <wx/statline.h>
#include <wx/spinctrl.h>

#include "propertiesdialog.h"
#include "genesframe.h"
#include "drawpanel.h"
#include "entities.h"
#include "constants.h"

#include "properties_singleton.h"

class MyFrame: public wxFrame
{
public:
    MyFrame();
    ~MyFrame();

    void OnNew(wxCommandEvent& event);
    void OnOpen(wxCommandEvent& WXUNUSED(event));
    void OnSave(wxCommandEvent& WXUNUSED(event));
    void OnQuit(wxCommandEvent& event);
    void OnSimulation(wxCommandEvent& event);
    void OnStep(wxCommandEvent& event);
    void OnSwitchDrawWorld(wxCommandEvent& event);
    void OnSwitchAntialiasing(wxCommandEvent& event);
    void OnShowLog(wxCommandEvent& event);
    void OnProperties(wxCommandEvent& event);
    void OnDescription(wxCommandEvent& event);
    void OnAbout(wxCommandEvent& event);

    enum {
        myID_MENU_EDIT_SIMULATION,
        myID_MENU_EDIT_STEP,
        myID_MENU_EDIT_DRAW_WORLD,
        myID_MENU_EDIT_ANTIALIASING,
        myID_MENU_EDIT_SHOW_LOG,
        myID_MENU_EDIT_PROPERTIES,
        myID_MENU_HELP_SHOW_DESCRIPTION
    };

    bool IsSimulationRunning();
    void StopSimulation();
    void StartSimulation();
    void RestartSimulation();
    void SwitchSimulation();

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
    wxStaticText* lastBehaviorText {nullptr};

    wxButton* showGenesBtn {nullptr};

    wxScopedPtr<wxPreferencesEditor> propertiesEditor;

    BasicDrawPanel* worldView {nullptr};
    ProtoPuddle::World* world {nullptr};

    bool drawWorldFlag {true};

private:
    void UpdateQuickSettings();
    void UpdateInformation();
    void UpdateMemoryInformation();

    void NewWorld();
    void Step();

    void ApplySettings(wxCommandEvent& event);

    void MakeMenu();
    void MakeStatusBar();
    void MakeLayout();

    void Setting();
    void ShowPropertiesEditor(wxWindow* parent);

    //void DismissPropertiesEditor();
};

MyFrame::MyFrame(): wxFrame(nullptr, wxID_ANY, wxT("ProtoPuddle++"), wxDefaultPosition, wxSize(800,600))
{
    MakeMenu();
    MakeStatusBar();
    Setting();
    MakeLayout();

    world = new ProtoPuddle::World(PropertiesSingleton::getInstance().GetPropertiesPtr());
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

MyFrame::~MyFrame() {
    if (world)
        delete world;
}

void MyFrame::NewWorld()
{
    if (timer.IsRunning())
        timer.Stop();

    world->New();
    worldView->paintNow();

    UpdateInformation();
    UpdateMemoryInformation();

    SetStatusText(wxT("Ready"), 0);
    SetStatusText(wxT("Action 'New' has been performed"), 1);
}

void MyFrame::OnNew(wxCommandEvent& event) {
    NewWorld();
}

void MyFrame::OnOpen(wxCommandEvent& WXUNUSED(event))
{
    wxString wildCard = "JavaScript Object Notation (*.json)|*.json;*.JSON";

    //wildCard.Append("|Extensible Markup Language (*.xml)|*.xml;*.XML");
    //wildCard.Append("|Initialization file (*.ini)|*.ini;*.INI");
    //wildCard.Append("|Custom configuration file (*.cfg)|*.cfg;*.CFG");

    wxFileDialog dlg(this, "Open", wxEmptyString, wxEmptyString, wildCard, wxFD_OPEN | wxFD_FILE_MUST_EXIST);

    if (dlg.ShowModal() == wxID_OK)
    {
        auto [flag, error] = world->OpenFromFile(dlg.GetPath());

        if (flag)
        {
            NewWorld();
            UpdateQuickSettings();
            SetStatusText(wxT("Configuration has been opened"), 1);
        }
        else
        {
            wxMessageBox(error, wxT("Error"), wxOK | wxICON_INFORMATION, this);
        }
    }
}

void MyFrame::OnSave(wxCommandEvent& WXUNUSED(event))
{
    wxString wildCard = "JavaScript Object Notation (*.json)|*.json;*.JSON";

    wxFileDialog dlg(this, "Save as", wxEmptyString, wxEmptyString, wildCard, wxFD_SAVE | wxFD_OVERWRITE_PROMPT);

    if (dlg.ShowModal() == wxID_OK)
    {
        if (world->SaveToFile(dlg.GetPath()))
        {
            SetStatusText(wxT("Configuration has been saved"), 1);
        }
    }
}

void MyFrame::OnQuit(wxCommandEvent& event)
{
    Close(true);
}

void MyFrame::OnSimulation(wxCommandEvent& event)
{
    SwitchSimulation();
}

void MyFrame::OnStep(wxCommandEvent& event)
{
    if (!IsSimulationRunning())
        Step();
}

void MyFrame::OnShowLog(wxCommandEvent& event)
{
    wxMessageBox(wxT("This will be realesed in the future"), wxT("Log"), wxOK | wxICON_INFORMATION, this);
    SetStatusText(wxT("Action 'Show log' has been performed"), 1);
}

void MyFrame::OnProperties(wxCommandEvent& event)
{
    ShowPropertiesEditor(this);
}

void MyFrame::OnSwitchDrawWorld(wxCommandEvent& event)
{
    drawWorldFlag = !drawWorldFlag;

    if (drawWorldFlag)
    {
        worldView->paintNow();

        SetStatusText(wxT("Drawing is enabled"), 1);
    }
    else
    {
        SetStatusText(wxT("Drawing is disabled"), 1);
    }
}

void MyFrame::OnSwitchAntialiasing(wxCommandEvent& event)
{
    if (worldView->SwitchAntialiasingMode())
    {
        SetStatusText(wxT("Antialiasing is enabled"), 1);
    }
    else
    {
        SetStatusText(wxT("Antialiasing is disabled"), 1);
    }
}

void MyFrame::OnDescription(wxCommandEvent& event)
{
    wxMessageBox(wxT("This will be released in the future"), wxT("Description"), wxOK | wxICON_INFORMATION, this);
    SetStatusText(wxT("Action 'Description' has been performed"), 1);
}

void MyFrame::OnAbout(wxCommandEvent& event)
{
    wxAboutDialogInfo info;

    info.SetIcon(wxIcon("resources/cell_icon_64x64.png", wxBITMAP_TYPE_PNG));

    info.SetName(wxT("ProtoPuddle++"));
    info.SetVersion(wxT("0.6.0"));
    info.SetDescription(wxT(" A remake of the protopuddle cell's world simulator "));
    info.SetWebSite(wxT("https://github.com/m110h/protopuddlepp"));

    wxArrayString developers;
    developers.Add(wxT("Alexey (aka m110h) Orlov"));
    info.SetDevelopers(developers);

    wxAboutBox(info);
}

bool MyFrame::IsSimulationRunning()
{
    return timer.IsRunning();
}

void MyFrame::StopSimulation()
{
    if (timer.IsRunning())
    {
        timer.Stop();
        SetStatusText(wxT("Paused"), 0);
        SetStatusText(wxT("Simulation has been stopped"), 1);
    }
    else {
        SetStatusText(wxT("Simulation is already stopped"), 1);
    }
}

void MyFrame::StartSimulation()
{
    if (!timer.IsRunning())
    {
        ProtoPuddle::GlobalProperties properties = PropertiesSingleton::getInstance().GetProperties();

        timer.Start(1000 / properties.GetValue(wxString("stepsPerSecond")));

        SetStatusText(wxT("Simulation has been started"), 1);
    }
    else {
        SetStatusText(wxT("Simulation is already started"), 1);
    }
}

void MyFrame::RestartSimulation()
{
    if (timer.IsRunning())
    {
        StopSimulation();
        StartSimulation();
    }
}

void MyFrame::SwitchSimulation()
{
    if (timer.IsRunning())
    {
        StopSimulation();
    }
    else {
        StartSimulation();
    }
}

void MyFrame::UpdateQuickSettings()
{
    ProtoPuddle::GlobalProperties* properties = PropertiesSingleton::getInstance().GetPropertiesPtr();

    spsSpinCtrl->SetValue(properties->GetValue(wxString("stepsPerSecond")));
    ppsSpinCtrl->SetValue(properties->GetValue(wxString("plantsPerStep")));
    peSpinCtrl->SetValue(properties->GetValue(wxString("plantEnergy")));
    meSpinCtrl->SetValue(properties->GetValue(wxString("meatEnergy")));
    mdSpinCtrl->SetValue(properties->GetValue(wxString("maxDamage")));
}

void MyFrame::UpdateInformation()
{
    topIdText->SetLabel(wxString::Format(wxT("%d"), world->GetTopId()));

    auto [p, m, c] = world->GetEntitiesQuantity();

    plantsCountText->SetLabel(wxString::Format(wxT("%d"), p));
    meatCountText->SetLabel(wxString::Format(wxT("%d"), m));
    cellsCountText->SetLabel(wxString::Format(wxT("%d"), c));

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
        lastBehaviorText->SetLabel(e->Get("lastBehavior"));

        if (e->GetType() == ProtoPuddle::Entity::TYPE_CELL)
        {
            showGenesBtn->Enable();
        }
        else
        {
            showGenesBtn->Disable();
        }
    }
    else
    {
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
        lastBehaviorText->SetLabel(ProtoPuddle::unknownValueStr);

        showGenesBtn->Disable();
    }
}

void MyFrame::UpdateMemoryInformation()
{
    auto [total, used, peak] = world->GetAllocatedMemoryInfo();
    SetStatusText(
        wxString::Format("%s%llu%s%llu%s%llu", " [Memory in Bytes] -> Total: ", total, " | Used: ", used, " | Peak: ", peak),
        2
    );
}

void MyFrame::Step()
{
    world->Step();

    if (drawWorldFlag)
    {
        worldView->paintNow();

        /*
        if (world->GetSteps() % 10 == 0)
            worldView->paintNow();
        */
    }

    UpdateInformation();
    UpdateMemoryInformation();

    SetStatusText(wxString::Format("%s%d", "Steps: ", world->GetSteps()), 0);
}

void MyFrame::ApplySettings(wxCommandEvent& event)
{
    ProtoPuddle::GlobalProperties properties = PropertiesSingleton::getInstance().GetProperties();

    properties.SetValue(wxString("stepsPerSecond"), spsSpinCtrl->GetValue());
    properties.SetValue(wxString("plantsPerStep"), ppsSpinCtrl->GetValue());
    properties.SetValue(wxString("plantEnergy"), peSpinCtrl->GetValue());
    properties.SetValue(wxString("meatEnergy"), meSpinCtrl->GetValue());
    properties.SetValue(wxString("maxDamage"), mdSpinCtrl->GetValue());

    PropertiesSingleton::getInstance().UpdateProperties(properties);

    RestartSimulation();

    SetStatusText(wxT("Settings have been applied"), 1);
}

void MyFrame::MakeMenu()
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
    menuEdit->AppendCheckItem(myID_MENU_EDIT_DRAW_WORLD, wxT("Enable &Drawing\tCtrl+d"));
    menuEdit->AppendCheckItem(myID_MENU_EDIT_ANTIALIASING, wxT("Enable &Antialiasing\tCtrl+a"));
    //menuEdit->Append(myID_MENU_EDIT_SHOW_LOG, wxT("Show &Log\tCtrl+l"));
    menuEdit->Append(myID_MENU_EDIT_PROPERTIES, wxT("&Properties\tCtrl+p"));

    wxMenu* menuHelp = new wxMenu;

    menuHelp->Append(myID_MENU_HELP_SHOW_DESCRIPTION, wxT("&Description\tCtrl+d"));
    menuHelp->Append(wxID_ABOUT, wxT("&About"));

    menuBar->Append(menuFile, wxT("&File"));
    menuBar->Append(menuEdit, wxT("&Edit"));
    menuBar->Append(menuHelp, wxT("&Help"));

    menuBar->Check(myID_MENU_EDIT_DRAW_WORLD, true);
    menuBar->Check(myID_MENU_EDIT_ANTIALIASING, true);

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
        case myID_MENU_EDIT_DRAW_WORLD:
            OnSwitchDrawWorld(event);
            break;
        case myID_MENU_EDIT_ANTIALIASING:
            OnSwitchAntialiasing(event);
            break;
        //case myID_MENU_EDIT_SHOW_LOG:
        //    OnShowLog(event);
        //    break;
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

void MyFrame::MakeStatusBar()
{
    CreateStatusBar(3);

    SetStatusText(wxT("Ready"), 0);
    SetStatusText(wxT("No actions performed"), 1);

    UpdateMemoryInformation();
}

void MyFrame::MakeLayout()
{
    wxBoxSizer* topSizer = new wxBoxSizer(wxHORIZONTAL);

    wxScrolledWindow* sw = new wxScrolledWindow(this, wxID_ANY, wxDefaultPosition, wxSize(200,600));
    wxBoxSizer* vswSizer = new wxBoxSizer(wxVERTICAL);

    // Settings Grop

    ProtoPuddle::GlobalProperties properties = PropertiesSingleton::getInstance().GetProperties();

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

        UpdateQuickSettings();

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

    // Selected Entity Group

    wxStaticBox* selectedGroupBox = new wxStaticBox(sw, wxID_ANY, "Entity Information");
    wxStaticBoxSizer * vSelectedSizer = new wxStaticBoxSizer (selectedGroupBox, wxVERTICAL);

    wxGridSizer* selectedSizer = new wxGridSizer(12, 2, 3, 0);
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
        selectedSizer->Add(new wxStaticText(selectedGroupBox, wxID_ANY, wxT("Children")));
            childrensText = new wxStaticText(selectedGroupBox, wxID_ANY, ProtoPuddle::unknownValueStr);
            selectedSizer->Add(childrensText);
        selectedSizer->Add(new wxStaticText(selectedGroupBox, wxID_ANY, wxT("Eaten plants")));
            eatenPlantsText = new wxStaticText(selectedGroupBox, wxID_ANY, ProtoPuddle::unknownValueStr);
            selectedSizer->Add(eatenPlantsText);
        selectedSizer->Add(new wxStaticText(selectedGroupBox, wxID_ANY, wxT("Eaten meat")));
            eatenMeatText = new wxStaticText(selectedGroupBox, wxID_ANY, ProtoPuddle::unknownValueStr);
            selectedSizer->Add(eatenMeatText);
        selectedSizer->Add(new wxStaticText(selectedGroupBox, wxID_ANY, wxT("Last behavior")));
            lastBehaviorText = new wxStaticText(selectedGroupBox, wxID_ANY, ProtoPuddle::unknownValueStr);
            selectedSizer->Add(lastBehaviorText);

    vSelectedSizer->Add(selectedSizer, 0, wxEXPAND);
    vSelectedSizer->Add(new wxStaticLine(selectedGroupBox, wxID_STATIC, wxDefaultPosition, wxDefaultSize, wxLI_HORIZONTAL), 0, wxGROW|wxTOP|wxBOTTOM, 10);

    showGenesBtn = new wxButton(selectedGroupBox, wxID_ANY, wxT("Show Genes"));
    showGenesBtn->Bind(wxEVT_BUTTON, [this](wxCommandEvent& event) {
        ProtoPuddle::Entity* e = world->GetSelectedEntity();

        if (e && e->GetType() == ProtoPuddle::Entity::TYPE_CELL)
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

    worldView = new BasicDrawPanel(this, wxID_ANY, wxSize(500, 500));
    topSizer->Add(worldView, 2, wxEXPAND, 0);

    this->SetSizer(topSizer);
    this->SetAutoLayout(true);
}

void MyFrame::Setting()
{
    SetMinSize(wxSize(800, 600));
}

void MyFrame::ShowPropertiesEditor(wxWindow* parent)
{
    if ( !propertiesEditor )
    {
        propertiesEditor.reset(new wxPreferencesEditor(wxT("Properties")));
        propertiesEditor->AddPage(new PrefsPageGeneral);
    }

    propertiesEditor->Show(parent);

    if (PropertiesSingleton::getInstance().IsPropertiesUpdated())
    {
        UpdateQuickSettings();
        PropertiesSingleton::getInstance().ResetUpdateFlag();
        RestartSimulation();
    }
}

/*
void MyFrame::DismissPropertiesEditor()
{
    if ( propertiesEditor )
    {
        propertiesEditor->Dismiss();
    }
}
*/


class MyApp: public wxApp
{
private:
    bool OnInit() override
    {
        if (!wxApp::OnInit()) return false;

        MyFrame* frame = new MyFrame;

        wxImage::AddHandler(new wxPNGHandler());
        frame->SetIcon(wxIcon("resources/cell_icon.png", wxBITMAP_TYPE_PNG));

        frame->Show();

        return true;
    }

    int OnExit() override
    {
        return 0;
    }
};

IMPLEMENT_APP(MyApp)
