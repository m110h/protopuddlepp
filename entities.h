#ifndef _ENTITIES_H_
#define _ENTITIES_H_

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
    int GetSteps();

    void New();

    void SetPanelSize(const wxSize& size);

    wxPoint WorldToPanel(const wxPoint& position);
    wxPoint PanelToWorld(const wxPoint& position);
    wxSize GetFieldSize(const wxRect& board);
    wxRect GetBoardBoundingBox();
    wxPoint GetEmptyPoint();

    void AddEntity(Entity* e);

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
    Entity(World* _world);

    virtual ~Entity();

    virtual void Step();
    virtual void Draw(wxDC* dc) = 0;
    virtual void DrawSelected(wxDC* dc) = 0;

    bool IsDead();

    void Die();

    int GetId();

    void SetColor(const wxColor& _color);
    const wxColor& GetColor() const;

    void SetPosition(const wxPoint& _position);
    const wxPoint& GetPosition() const;

    EntityType GetType();

    void SetEnergy(int _energy);
    int GetEnergy();

    virtual wxString Get(const wxString& name);

protected:
    void SetSelectedBrushAndPen(wxDC* dc);
    void SetNormalBrushAndPen(wxDC* dc);
    void DrawCircle(wxDC* dc);
    void DrawRectangle(wxDC* dc);

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
    Plant(World* _world);
    ~Plant();

    void Draw(wxDC* dc) override;
    void DrawSelected(wxDC* dc) override;
};

class Meat: public Entity
{
public:
    Meat(World* _world);
    ~Meat();

    void Draw(wxDC* dc) override;
    void DrawSelected(wxDC* dc) override;
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
