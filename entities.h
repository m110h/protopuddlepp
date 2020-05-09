#ifndef _ENTITIES_H_
#define _ENTITIES_H_

#include <vector>
#include <list>

#include "properties.h"
#include "constants.h"
#include "random.hpp"

namespace ProtoPuddle
{

enum EntityType
{
    TYPE_NULL = 0,
    TYPE_PLANT,
    TYPE_MEAT,
    TYPE_CELL,
    TYPE_WALL
};

class Entity;

class World
{
public:
    World(GlobalProperties* _properties);
    ~World();

    void Step();
    void Draw(wxDC* dc);

    void SetProperties(GlobalProperties* _properties);
    GlobalProperties* GetProperties();

    Entity* GetEntity(const wxPoint& worldPosition);
    Entity* GetEntityById(int id);

    int GetEntityIdByPosition(const wxPoint& worldPosition);

    void SelectEntityByPosition(const wxPoint& worldPosition);
    Entity* GetSelectedEntity();

    void GetEntitiesQuantity(int& plants, int& meat, int& cells);

    bool IsInside(const wxPoint& worldPosition);

    int GetNextId();
    int GetTopId();

    int GetSteps() {
        return steps;
    }

    void New();

    void SetPanelSize(const wxSize& size)
    {
        panelSize = size;
    }

    wxPoint WorldToPanel(const wxPoint& position)
    {
        wxRect bbb = GetBoardBoundingBox();
        wxSize field = GetFieldSize(bbb);

        return wxPoint(padding+field.GetWidth()*position.x, padding+field.GetHeight()*position.y);
    }

    wxPoint PanelToWorld(const wxPoint& position)
    {
        wxRect bbb = GetBoardBoundingBox();

        if ( !bbb.Contains(position) )
            return wxPoint(-1, -1);

        wxSize field = GetFieldSize(bbb);

        return wxPoint((position.x-padding)/field.GetWidth(), (position.y-padding)/field.GetHeight());
    }

    wxSize GetFieldSize(const wxRect& board)
    {
        return wxSize(board.GetWidth()/worldSize.GetWidth(), board.GetHeight()/worldSize.GetHeight());
    }

    wxRect GetBoardBoundingBox()
    {
        int w = panelSize.GetWidth() - padding*2 - (panelSize.GetWidth() % worldSize.GetWidth());
        int h = panelSize.GetHeight() - padding*2 - (panelSize.GetHeight() % worldSize.GetHeight());

        return wxRect(padding, padding, w, h);
    }

    wxPoint GetEmptyPoint();

    void AddEntity(Entity* e) { if (e) entities.push_front(e); }

private:
    void DrawBoard(wxDC* dc);
    void DrawEntities(wxDC* dc);

    void GeneratePlants(int quantity);
    void GenerateCells();

    void DeathHandle();

private:
    std::list<Entity*> entities;

    int nextId {0};
    int selectedId {-1};
    int steps {0};

    int padding {30};

    GlobalProperties* properties {nullptr};

    wxSize worldSize {wxSize(0,0)};
    wxSize panelSize {wxSize(0,0)};
};

class Entity
{
public:
    Entity(World* _world): world(_world) {
        id = world->GetNextId();
    }

    virtual ~Entity() {}

    virtual void Step() = 0;
    virtual void Draw(wxDC* dc) = 0;
    virtual void DrawSelected(wxDC* dc) = 0;

    bool IsDead()
    {
        return (age >= liveTime) || (energy <= 0);
    }

    void Die() { age = liveTime; }

    int GetId() { return id; }

    void SetColor(const wxColor& _color) { color = _color; }
    const wxColor& GetColor() const { return color; }

    void SetPosition(const wxPoint& _position) { position = _position; }
    const wxPoint& GetPosition() const { return position; }

    EntityType GetType() { return type; }

    void SetEnergy(int _energy) { energy = _energy; }
    int GetEnergy() { return energy; }

    virtual wxString Get(const wxString& name)
    {
        if (name == wxString("id"))
            return wxString::Format(wxT("%d"), id);
        if (name == wxString("age"))
            return wxString::Format(wxT("%d"), age);
        if (name == wxString("maxAge"))
            return wxString::Format(wxT("%d"), liveTime);
        if (name == wxString("energy"))
            return wxString::Format(wxT("%d"), energy);

        return unknownValueStr;
    }

protected:
    void SetSelectedBrushAndPen(wxDC* dc)
    {
        wxBrush brush;
        wxPen pen;

        brush.SetStyle(wxBRUSHSTYLE_CROSSDIAG_HATCH);
        brush.SetColour(wxColor(255,255,255));

        pen.SetColour(color);
        pen.SetWidth(2);

        dc->SetBrush(brush);
        dc->SetPen(pen);
    }

    void SetNormalBrushAndPen(wxDC* dc)
    {
        wxBrush brush;
        wxPen pen;

        brush.SetStyle(wxBRUSHSTYLE_SOLID);
        brush.SetColour(color);

        pen.SetColour(color);

        dc->SetBrush(brush);
        dc->SetPen(pen);
    }

    void DrawCircle(wxDC* dc)
    {
        wxRect bbb = world->GetBoardBoundingBox();
        wxSize field = world->GetFieldSize(bbb);

        int radius = field.GetHeight()/3;

        wxPoint p = world->WorldToPanel(position);
        dc->DrawCircle(p.x+field.GetWidth()/2, p.y+field.GetHeight()/2, radius);
    }

    void DrawRectangle(wxDC* dc)
    {
        wxRect bbb = world->GetBoardBoundingBox();
        wxSize field = world->GetFieldSize(bbb);

        int w = int(field.GetWidth()*0.8f);
        int h = int(field.GetHeight()*0.8f);

        wxPoint p = world->WorldToPanel(position);

        p.x += field.GetWidth()/2 - w/2;
        p.y += field.GetHeight()/2 - h/2;

        dc->DrawRectangle(p.x, p.y, w, h);
    }

protected:
    EntityType type {TYPE_NULL};

    int id {-1};
    int age {0};
    int liveTime {0};
    int energy {1};

    World* world {nullptr};

    wxColor color {wxColor(0,0,0)};
    wxPoint position {wxPoint(-1,-1)};
};

class Plant: public Entity
{
public:
    Plant(World* _world): Entity(_world) {
        type = TYPE_PLANT;
        color = wxColor(43,168,74);
        liveTime = world->GetProperties()->GetValue(wxString("plantLiveTime"));
    }

    ~Plant() {}

    void Step() override
    {
        if (IsDead()) return;
        age++;
    }

    void Draw(wxDC* dc) override
    {
        SetNormalBrushAndPen(dc);
        DrawCircle(dc);
    }

    void DrawSelected(wxDC* dc) override
    {
        SetSelectedBrushAndPen(dc);
        DrawCircle(dc);
    }
};

class Meat: public Entity
{
public:
    Meat(World* _world): Entity(_world) {
        type = TYPE_MEAT;
        color = wxColor(205,83,59);
        liveTime = world->GetProperties()->GetValue(wxString("meatLiveTime"));
    }

    ~Meat() {}

    void Step() override
    {
        if (IsDead()) return;
        age++;
    }

    void Draw(wxDC* dc) override
    {
        SetNormalBrushAndPen(dc);
        DrawCircle(dc);
    }

    void DrawSelected(wxDC* dc) override
    {
        SetSelectedBrushAndPen(dc);
        DrawCircle(dc);
    }
};


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

    wxString name {""};
    wxString empty {"none"};
    wxString other {"none"};
    wxString same {"none"};
    wxString meat {"none"};
    wxString plant {"none"};
    wxString wall {"none"};
    wxString weak {"none"};
};

class Cell: public Entity
{
public:
    Cell(World* _world);
    Cell(World* _world, int _divEnergy, int _damage, int _mutationProbability);

    ~Cell();

    void Step() override;
    void Draw(wxDC* dc) override;
    void DrawSelected(wxDC* dc) override;

    bool Attack(Cell* victim);
    void SetAttacked(bool _attacked);

    const Gene& GetGene();
    void SetGene(const Gene& _gene);

    wxString Get(const wxString& name) override;

private:
    void Clone();
    void Execute(const wxString& cmd);
    Gene GenerateGene(const wxString& name);

    bool CanDivide();
    void DrawDirection(wxDC* dc);

    int NormalizeCoord(int x);

private:
    wxPoint direction {wxPoint(1,0)};

    Gene gen1;

    int divEnergy {0};
    int damage {0};
    int childrenCounter {0};
    int killsCounter {0};
    int mutationProbability {0};
    int eatenPlantsCounter {0};
    int eatenMeatCounter {0};

    bool attacked {false};
};

}

#endif
