/////////////////////////////////////////////////////////////////////////////
// Name:               entities.h
// Description:        ...
// Author:             Alexey Orlov (https://github.com/m110h)
// Last modification:  18/08/2020
// Licence:            MIT licence
/////////////////////////////////////////////////////////////////////////////

#ifndef _ENTITIES_H_
#define _ENTITIES_H_

#include "wx/wxprec.h"

#ifndef WX_PRECOMP
#include <wx/wx.h>
#endif

#include <vector>
#include <tuple>

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
    World(wxFrame* _parentFrame, GlobalProperties* _properties);
    ~World();

    void Step();
    void Draw(wxDC* dc);

    void SetProperties(GlobalProperties* _properties);
    GlobalProperties* GetProperties();

    Entity* GetEntityByPosition(const wxPoint& worldPosition);
    Entity* GetEntityById(int id);

    int GetEntityIdByPosition(const wxPoint& worldPosition);

    void SelectEntityByPosition(const wxPoint& worldPosition);
    Entity* GetSelectedEntity();

    // returns plants, meat, cells
    std::tuple<int, int, int> GetEntitiesQuantity();

    // returns total, used, peak
    std::tuple<std::size_t, std::size_t, std::size_t> GetMemoryInfo();

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

    void AddEntity(Entity* e);
    bool MoveEntity(Entity* e, const wxPoint& newPosition);

    std::tuple<bool, wxString> OpenFromFile(const wxString& filename);
    bool SaveToFile(const wxString& filename);

    bool LeaseEmptyPoint(const wxPoint& point);
    wxPoint LeaseRandomEmptyPoint();
    void ReleasePoint(const wxPoint& point);

private:
    void StepEntities();

    void DrawBoard(wxDC* dc);
    void DrawEntities(wxDC* dc);

    void GenerateEmptyPoints();

    void GenerateEntities(int type, int quantity);

    void DeathHandle();

    void ClearEntitiesTable();
    void ClearEmptyPoints();

private:
    std::vector<wxPoint> emptyPoints;

    int nextId {0};
    int selectedId {-1};
    int steps {0};

    int plantsCounter {0};
    int meatCounter {0};
    int cellsCounter {0};

    int paddingX {0};
    int paddingY {0};

    GlobalProperties* properties {nullptr};

    wxSize worldSize {wxSize(0,0)};
    wxSize panelSize {wxSize(0,0)};

    wxFrame* parentFrame {nullptr};
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
    virtual ~Plant();

    void Draw(wxDC* dc) final;
    void DrawSelected(wxDC* dc) final;
};

class Meat: public Entity
{
public:
    Meat(World* _world);
    virtual ~Meat();

    void Draw(wxDC* dc) final;
    void DrawSelected(wxDC* dc) final;
};



class Cell: public Entity
{
public:
    Cell(World* _world, const wxString& geneName);
    Cell(World* _world, int _divEnergy, int _damage, int _mutationProbability, const wxColor& _color, const Gene& _gene);

    virtual ~Cell();

    void Step() final;
    void Draw(wxDC* dc) final;
    void DrawSelected(wxDC* dc) final;

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
    wxPoint GenerateDirection();

    bool CanDivide();
    void DrawDirection(wxDC* dc);

    int NormalizeCoord(int x);

    void SetLastBehavior(const wxString& behavior);

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

    wxString lastBehavior {""};
};

}

#endif
