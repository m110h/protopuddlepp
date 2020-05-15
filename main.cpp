#include "wx/wxprec.h"

#ifndef WX_PRECOMP
#include <wx/wx.h>
#endif

#include <wx/statline.h>
#include <wx/spinctrl.h>

#include "propertiesdialog.h"
#include "properties_singleton.h"
#include "genesframe.h"
#include "drawpanel.h"
#include "entities.h"
#include "constants.h"

enum
{
    ID_DRAW_PANEL = 10001,
    ID_MY_FRAME = 10002
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
        ShowPropertiesEditor(this);
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
            ProtoPuddle::GlobalProperties properties = PropertiesSingleton::getInstance().GetProperties();

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

    wxScopedPtr<wxPreferencesEditor> propertiesEditor;

    BasicDrawPanel* worldView {nullptr};
    ProtoPuddle::World* world {nullptr};

private:
    void UpdateQuickSettings()
    {
        ProtoPuddle::GlobalProperties* properties = PropertiesSingleton::getInstance().GetPropertiesPtr();

        spsSpinCtrl->SetValue(properties->GetValue(wxString("stepsPerSecond")));
        ppsSpinCtrl->SetValue(properties->GetValue(wxString("plantsPerStep")));
        peSpinCtrl->SetValue(properties->GetValue(wxString("plantEnergy")));
        meSpinCtrl->SetValue(properties->GetValue(wxString("meatEnergy")));
        mdSpinCtrl->SetValue(properties->GetValue(wxString("maxDamage")));
    }

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
            showGenesBtn->Enable();
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
            showGenesBtn->Disable();
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

        worldView = new BasicDrawPanel(this, ID_DRAW_PANEL, wxSize(500, 500));
        topSizer->Add(worldView, 2, wxEXPAND, 0);

        this->SetSizer(topSizer);
        this->SetAutoLayout(true);
    }

    void Setting()
    {
        SetMinSize(wxSize(800, 600));
    }

    void ShowPropertiesEditor(wxWindow* parent)
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
        }
    }

    /*
    void DismissPropertiesEditor()
    {
        if ( propertiesEditor )
        {
            propertiesEditor->Dismiss();
        }
    }
    */
};

// ***
class MyApp: public wxApp
{
private:
    bool OnInit() override
    {
        if (!wxApp::OnInit()) return false;

        MyFrame* frame = new MyFrame;
        frame->Show();

        return true;
    }

    int OnExit() override
    {
        return 0;
    }
};

IMPLEMENT_APP(MyApp)
