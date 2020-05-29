#ifndef _ENTITIES_H_
#define _ENTITIES_H_

#include "wx/wxprec.h"

#ifndef WX_PRECOMP
#include <wx/wx.h>
#endif

#include <list>

#include "gene.h"
#include "properties.h"
#include "constants.h"
#include "random.h"
#include "config.h"

namespace ProtoPuddle
{

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

    bool OpenFromFile(const wxString& filename);
    bool SaveToFile(const wxString& filename);

private:
    void DrawBoard(wxDC* dc);
    void DrawEntities(wxDC* dc);

    void GeneratePlants(int quantity);
    void GenerateCells(int quantity);

    void DeathHandle();

private:
    std::list<Entity*> entities;

    int nextId {0};
    int selectedId {-1};
    int steps {0};

    int paddingX {0};
    int paddingY {0};

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

    //virtual nlohmann::json GetConfig() = 0;
    //virtual bool SetConfig(const nlohmann::json& config) = 0;

    bool IsDead();

    void Die();

    int GetId();

    void SetColor(const wxColor& _color);
    const wxColor& GetColor() const;

    void SetPosition(const wxPoint& _position);
    const wxPoint& GetPosition() const;

    int GetType();

    void SetEnergy(int _energy);
    int GetEnergy();

    virtual wxString Get(const wxString& name);

    // types for entities
    enum
    {
        TYPE_PLANT = 1,
        TYPE_MEAT,
        TYPE_CELL,
        TYPE_WALL
    };

protected:
    void SetSelectedBrushAndPen(wxDC* dc);
    void SetNormalBrushAndPen(wxDC* dc);
    void DrawCircle(wxDC* dc);
    void DrawRectangle(wxDC* dc);

protected:
    int type {0};
    int id {-1};
    int age {0};
    int lifeTime {0};
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



class Cell: public Entity
{
public:
    Cell(World* _world);
    Cell(World* _world, int _divEnergy, int _damage, int _mutationProbability, const wxColor& _color, const Gene& _gene);

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
    void Execute(int cmd);

    Gene GenerateGene(const wxString& name);
    Gene MutateGene(const Gene& _gene);

    wxColor GenerateColor();

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
