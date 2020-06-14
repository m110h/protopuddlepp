#include "wx/wxprec.h"

#ifndef WX_PRECOMP
#include <wx/wx.h>
#endif

#include <wx/dcbuffer.h>

#include "entities.h"

#include "thirdparty/allocator/freelistallocator.h"

#include <array>
#include <limits>
#include <iomanip>
#include <fstream>
#include <cassert>
#include <algorithm>

namespace ProtoPuddle
{

Entity* entitiesTable[maxWorldWidth][maxWorldHeight] = {nullptr};

mtrebi::FreeListAllocator _allocator(sizeof(Cell)*(maxWorldWidth*maxWorldHeight), mtrebi::FreeListAllocator::FIND_FIRST);

World::World(GlobalProperties* _properties)
{
    SetProperties(_properties);
    _allocator.Init();
}

World::~World()
{
    ClearEntitiesTable();
    ClearEmptyPoints();
}

void World::New()
{
    steps = 0;
    nextId = 0;
    selectedId = -1;

    plantsCounter = 0;
    meatCounter = 0;
    cellsCounter = 0;

    ClearEntitiesTable();

    _allocator.Reset();

    worldSize.SetWidth(properties->GetValue(wxString("worldWidth")));
    worldSize.SetHeight(properties->GetValue(wxString("worldHeight")));

    GenerateEmptyPoints();

    GenerateEntities(Entity::TYPE_PLANT, properties->GetValue(wxString("plants")));
    GenerateEntities(Entity::TYPE_CELL, properties->GetValue(wxString("sortsOfCell")));
}

void World::StepEntities()
{
    Entity* entities[worldSize.GetWidth()*worldSize.GetHeight()];

    size_t count = 0;

    for (int i=0; i<worldSize.GetWidth(); i++)
    {
        for (int j=0; j<worldSize.GetHeight(); j++)
        {
            if (entitiesTable[i][j])
            {
                entities[count] = entitiesTable[i][j];
                count++;
            }
        }
    }

    std::random_shuffle(entities, entities + count);

    for (size_t i=0; i<count; i++)
    {
        entities[i]->Step();
    }
}

void World::Step()
{
    if (steps == std::numeric_limits<int>::max())
    {
        steps = 0;
    }

    GenerateEntities(Entity::TYPE_PLANT, properties->GetValue(wxString("plantsPerStep")));
    StepEntities();
    DeathHandle();

    steps++;
}

void World::Draw(wxDC* dc)
{
    DrawBoard(dc);
    DrawEntities(dc);
}

void World::SetProperties(GlobalProperties* _properties)
{
    properties = _properties;
}

GlobalProperties* World::GetProperties()
{
    return properties;
}

bool World::IsInside(const wxPoint& worldPosition)
{
    return (worldPosition.x >= 0) && (worldPosition.x < worldSize.GetWidth()) && (worldPosition.y >= 0) && (worldPosition.y < worldSize.GetHeight());
}

Entity* World::GetEntityByPosition(const wxPoint& worldPosition)
{
    return entitiesTable[worldPosition.x][worldPosition.y];
}

int World::GetEntityIdByPosition(const wxPoint& worldPosition)
{
    Entity* e = GetEntityByPosition(worldPosition);

    if (nullptr == e)
        return -1;

    return e->GetId();
}

Entity* World::GetEntityById(int id)
{
    for (int i=0; i<worldSize.GetWidth(); i++)
    {
        for (int j=0; j<worldSize.GetHeight(); j++)
        {
            if ( (entitiesTable[i][j]) && (id == entitiesTable[i][j]->GetId()) )
                return entitiesTable[i][j];
        }
    }

    return nullptr;
}

void World::SelectEntityByPosition(const wxPoint& worldPosition)
{
    selectedId = GetEntityIdByPosition(worldPosition);
}

Entity* World::GetSelectedEntity()
{
    return GetEntityById(selectedId);
}

int World::GetNextId()
{
    if (nextId+1 == std::numeric_limits<int>::max())
        nextId = 0;

    return ++nextId;
}

int World::GetTopId()
{
    return nextId;
}

int World::GetSteps()
{
   return steps;
}

void World::SetPanelSize(const wxSize& size)
{
    panelSize = size;
}

wxPoint World::WorldToPanel(const wxPoint& position)
{
    wxRect bbb = GetBoardBoundingBox();
    wxSize field = GetFieldSize(bbb);

    return wxPoint(paddingX+field.GetWidth()*position.x, paddingY+field.GetHeight()*position.y);
}

wxPoint World::PanelToWorld(const wxPoint& position)
{
    wxRect bbb = GetBoardBoundingBox();

    if ( !bbb.Contains(position) )
        return wxPoint(-1, -1);

    wxSize field = GetFieldSize(bbb);

    return wxPoint((position.x-paddingX)/field.GetWidth(), (position.y-paddingY)/field.GetHeight());
}

wxSize World::GetFieldSize(const wxRect& board)
{
    return wxSize(board.GetWidth()/worldSize.GetWidth(), board.GetHeight()/worldSize.GetHeight());
}

wxRect World::GetBoardBoundingBox()
{
    const int padding = 10;

    wxSize tmp = panelSize - wxSize(padding*2,padding*2);

    int px = tmp.GetWidth() % worldSize.GetWidth();
    int py = tmp.GetHeight() % worldSize.GetHeight();

    int w = tmp.GetWidth() - px;
    int h = tmp.GetHeight() - py;

    paddingX = padding + px/2;
    paddingY = padding + py/2;

    return wxRect(paddingX, paddingY, w, h);
}

void World::AddEntity(Entity* e)
{
    wxPoint p = e->GetPosition();

    if (e->GetType() == Entity::TYPE_PLANT)
    {
        plantsCounter++;
    }
    else
    {
        cellsCounter++;
    }

    entitiesTable[p.x][p.y] = e;
}

bool World::MoveEntity(Entity* e, const wxPoint& newPosition)
{
    if (e && LeaseEmptyPoint(newPosition))
    {
        wxPoint position = e->GetPosition();

        entitiesTable[position.x][position.y] = nullptr;
        ReleasePoint(position);

        e->SetPosition(newPosition);
        entitiesTable[newPosition.x][newPosition.y] = e;

        return true;
    }

    return false;
}

std::tuple<bool, wxString> World::OpenFromFile(const wxString& filename)
{
    std::ifstream in(filename.c_str().AsChar());

    nlohmann::json config;

    try
    {
        in >> config;
    }
    catch (const std::exception& e)
    {
        in.close();
        return { false, wxT("Configuration file is invalid. Please, check its syntax.") };
    }

    in.close();

    int _min = 0;
    int _max = 0;

    if (!properties->CheckValue(wxString("sortsOfCell"), config["world"]["sortsOfCell"]))
    {
        _min = properties->GetMin(wxString("sortsOfCell"));
        _max = properties->GetMin(wxString("sortsOfCell"));

        return { false, wxString::Format(wxT("Value of 'sortsOfCell' must be between %d and %d."), _min, _max) };
    }

    if (!properties->CheckValue(wxString("cellEnergy"), config["world"]["cellEnergy"]))
    {
        _min = properties->GetMin(wxString("cellEnergy"));
        _max = properties->GetMin(wxString("cellEnergy"));

        return { false, wxString::Format(wxT("Value of 'cellEnergy' must be between %d and %d."), _min, _max) };
    }

    if (!properties->CheckValue(wxString("maxDamage"), config["world"]["maxDamage"]))
    {
        _min = properties->GetMin(wxString("maxDamage"));
        _max = properties->GetMin(wxString("maxDamage"));

        return { false, wxString::Format(wxT("Value of 'maxDamage' must be between %d and %d."), _min, _max) };
    }

    if (!properties->CheckValue(wxString("behaviorGenes"), config["world"]["behaviorGenes"]))
    {
        _min = properties->GetMin(wxString("behaviorGenes"));
        _max = properties->GetMin(wxString("behaviorGenes"));

        return { false, wxString::Format(wxT("Value of 'behaviorGenes' must be between %d and %d."), _min, _max) };
    }

    if (!properties->CheckValue(wxString("minEnergyForDivision"), config["world"]["minEnergyForDivision"]))
    {
        _min = properties->GetMin(wxString("minEnergyForDivision"));
        _max = properties->GetMin(wxString("minEnergyForDivision"));

        return { false, wxString::Format(wxT("Value of 'minEnergyForDivision' must be between %d and %d."), _min, _max) };
    }

    if (!properties->CheckValue(wxString("maxEnergyForDivision"), config["world"]["maxEnergyForDivision"]))
    {
        _min = properties->GetMin(wxString("maxEnergyForDivision"));
        _max = properties->GetMin(wxString("maxEnergyForDivision"));

        return { false, wxString::Format(wxT("Value of 'maxEnergyForDivision' must be between %d and %d."), _min, _max) };
    }

    if (!properties->CheckValue(wxString("plants"), config["world"]["plants"]))
    {
        _min = properties->GetMin(wxString("plants"));
        _max = properties->GetMin(wxString("plants"));

        return { false, wxString::Format(wxT("Value of 'plants' must be between %d and %d."), _min, _max) };
    }

    if (!properties->CheckValue(wxString("plantEnergy"), config["world"]["plantEnergy"]))
    {
        _min = properties->GetMin(wxString("plantEnergy"));
        _max = properties->GetMin(wxString("plantEnergy"));

        return { false, wxString::Format(wxT("Value of 'plantEnergy' must be between %d and %d."), _min, _max) };
    }

    if (!properties->CheckValue(wxString("meatEnergy"), config["world"]["meatEnergy"]))
    {
        _min = properties->GetMin(wxString("meatEnergy"));
        _max = properties->GetMin(wxString("meatEnergy"));

        return { false, wxString::Format(wxT("Value of 'meatEnergy' must be between %d and %d."), _min, _max) };
    }

    if (!properties->CheckValue(wxString("maxAge"), config["world"]["maxAge"]))
    {
        _min = properties->GetMin(wxString("maxAge"));
        _max = properties->GetMin(wxString("maxAge"));

        return { false, wxString::Format(wxT("Value of 'maxAge' must be between %d and %d."), _min, _max) };
    }

    if (!properties->CheckValue(wxString("stepsPerSecond"), config["world"]["stepsPerSecond"]))
    {
        _min = properties->GetMin(wxString("stepsPerSecond"));
        _max = properties->GetMin(wxString("stepsPerSecond"));

        return { false, wxString::Format(wxT("Value of 'stepsPerSecond' must be between %d and %d."), _min, _max) };
    }

    if (!properties->CheckValue(wxString("plantsPerStep"), config["world"]["plantsPerStep"]))
    {
        _min = properties->GetMin(wxString("plantsPerStep"));
        _max = properties->GetMin(wxString("plantsPerStep"));

        return { false, wxString::Format(wxT("Value of 'plantsPerStep' must be between %d and %d."), _min, _max) };
    }

    if (!properties->CheckValue(wxString("worldWidth"), config["world"]["worldWidth"]))
    {
        _min = properties->GetMin(wxString("worldWidth"));
        _max = properties->GetMin(wxString("worldWidth"));

        return { false, wxString::Format(wxT("Value of 'worldWidth' must be between %d and %d."), _min, _max) };
    }

    if (!properties->CheckValue(wxString("worldHeight"), config["world"]["worldHeight"]))
    {
        _min = properties->GetMin(wxString("worldHeight"));
        _max = properties->GetMin(wxString("worldHeight"));

        return { false, wxString::Format(wxT("Value of 'worldHeight' must be between %d and %d."), _min, _max) };
    }

    if (!properties->CheckValue(wxString("plantLifeTime"), config["world"]["plantLifeTime"]))
    {
        _min = properties->GetMin(wxString("plantLifeTime"));
        _max = properties->GetMin(wxString("plantLifeTime"));

        return { false, wxString::Format(wxT("Value of 'plantLifeTime' must be between %d and %d."), _min, _max) };
    }

    if (!properties->CheckValue(wxString("meatLifeTime"), config["world"]["meatLifeTime"]))
    {
        _min = properties->GetMin(wxString("meatLifeTime"));
        _max = properties->GetMin(wxString("meatLifeTime"));

        return { false, wxString::Format(wxT("Value of 'meatLifeTime' must be between %d and %d."), _min, _max) };
    }

    if (!properties->CheckValue(wxString("movementEnergy"), config["world"]["movementEnergy"]))
    {
        _min = properties->GetMin(wxString("movementEnergy"));
        _max = properties->GetMin(wxString("movementEnergy"));

        return { false, wxString::Format(wxT("Value of 'movementEnergy' must be between %d and %d."), _min, _max) };
    }

    if (!properties->CheckValue(wxString("attackEnergy"), config["world"]["attackEnergy"]))
    {
        _min = properties->GetMin(wxString("attackEnergy"));
        _max = properties->GetMin(wxString("attackEnergy"));

        return { false, wxString::Format(wxT("Value of 'attackEnergy' must be between %d and %d."), _min, _max) };
    }

    if (!properties->CheckValue(wxString("attackCondition"), config["world"]["attackCondition"]))
    {
        _min = properties->GetMin(wxString("attackCondition"));
        _max = properties->GetMin(wxString("attackCondition"));

        return { false, wxString::Format(wxT("Value of 'attackCondition' must be between %d and %d."), _min, _max) };
    }

    if (!properties->CheckValue(wxString("maxMutationProbability"), config["world"]["maxMutationProbability"]))
    {
        _min = properties->GetMin(wxString("maxMutationProbability"));
        _max = properties->GetMin(wxString("maxMutationProbability"));

        return { false, wxString::Format(wxT("Value of 'maxMutationProbability' must be between %d and %d."), _min, _max) };
    }

    properties->SetValue(wxString("sortsOfCell"), config["world"]["sortsOfCell"]);
    properties->SetValue(wxString("cellEnergy"), config["world"]["cellEnergy"]);
    properties->SetValue(wxString("maxDamage"), config["world"]["maxDamage"]);
    properties->SetValue(wxString("behaviorGenes"), config["world"]["behaviorGenes"]);
    properties->SetValue(wxString("minEnergyForDivision"), config["world"]["minEnergyForDivision"]);
    properties->SetValue(wxString("maxEnergyForDivision"), config["world"]["maxEnergyForDivision"]);
    properties->SetValue(wxString("plants"), config["world"]["plants"]);
    properties->SetValue(wxString("plantEnergy"), config["world"]["plantEnergy"]);
    properties->SetValue(wxString("meatEnergy"), config["world"]["meatEnergy"]);
    properties->SetValue(wxString("maxAge"), config["world"]["maxAge"]);
    properties->SetValue(wxString("stepsPerSecond"), config["world"]["stepsPerSecond"]);
    properties->SetValue(wxString("plantsPerStep"), config["world"]["plantsPerStep"]);
    properties->SetValue(wxString("worldWidth"), config["world"]["worldWidth"]);
    properties->SetValue(wxString("worldHeight"), config["world"]["worldHeight"]);
    properties->SetValue(wxString("plantLifeTime"), config["world"]["plantLifeTime"]);
    properties->SetValue(wxString("meatLifeTime"), config["world"]["meatLifeTime"]);
    properties->SetValue(wxString("movementEnergy"), config["world"]["movementEnergy"]);
    properties->SetValue(wxString("attackEnergy"), config["world"]["attackEnergy"]);
    properties->SetValue(wxString("attackCondition"), config["world"]["attackCondition"]);
    properties->SetValue(wxString("maxMutationProbability"), config["world"]["maxMutationProbability"]);

    return { true, wxT("") };
}

bool World::SaveToFile(const wxString& filename)
{
    nlohmann::json config;

    config["world"]["sortsOfCell"] = properties->GetValue(wxString("sortsOfCell"));
    config["world"]["cellEnergy"] = properties->GetValue(wxString("cellEnergy"));
    config["world"]["maxDamage"] = properties->GetValue(wxString("maxDamage"));
    config["world"]["behaviorGenes"] = properties->GetValue(wxString("behaviorGenes"));
    config["world"]["minEnergyForDivision"] = properties->GetValue(wxString("minEnergyForDivision"));
    config["world"]["maxEnergyForDivision"] = properties->GetValue(wxString("maxEnergyForDivision"));
    config["world"]["plants"] = properties->GetValue(wxString("plants"));
    config["world"]["plantEnergy"] = properties->GetValue(wxString("plantEnergy"));
    config["world"]["meatEnergy"] = properties->GetValue(wxString("meatEnergy"));
    config["world"]["maxAge"] = properties->GetValue(wxString("maxAge"));
    config["world"]["stepsPerSecond"] = properties->GetValue(wxString("stepsPerSecond"));
    config["world"]["plantsPerStep"] = properties->GetValue(wxString("plantsPerStep"));
    config["world"]["worldWidth"] = properties->GetValue(wxString("worldWidth"));
    config["world"]["worldHeight"] = properties->GetValue(wxString("worldHeight"));
    config["world"]["plantLifeTime"] = properties->GetValue(wxString("plantLifeTime"));
    config["world"]["meatLifeTime"] = properties->GetValue(wxString("meatLifeTime"));
    config["world"]["movementEnergy"] = properties->GetValue(wxString("movementEnergy"));
    config["world"]["attackEnergy"] = properties->GetValue(wxString("attackEnergy"));
    config["world"]["attackCondition"] = properties->GetValue(wxString("attackCondition"));
    config["world"]["maxMutationProbability"] = properties->GetValue(wxString("maxMutationProbability"));

    std::ofstream out(filename.c_str().AsChar());

    out << std::setw(4) << config << std::endl;
    out.close();

    return true;
}


void World::DrawBoard(wxDC* dc)
{
    wxBrush brush;
    wxPen pen;

    brush.SetStyle(wxBRUSHSTYLE_TRANSPARENT);

    pen.SetColour(wxColor(0,0,0));
    pen.SetWidth(1);

    dc->SetBrush(brush);
    dc->SetPen(pen);

    wxRect bbb = GetBoardBoundingBox();
    wxSize field = GetFieldSize(bbb);

    // draw vertical lines
    for (int i=bbb.GetX()+field.GetWidth(); i<(bbb.GetX()+bbb.GetWidth()); i+=field.GetWidth())
    {
        dc->DrawLine( i, bbb.GetY(), i, bbb.GetY()+bbb.GetHeight() );
    }

    // draw horizontal lines
    for (int i=bbb.GetY()+field.GetHeight(); i<(bbb.GetY()+bbb.GetHeight()); i+=field.GetHeight())
    {
        dc->DrawLine( bbb.GetX(), i, bbb.GetX()+bbb.GetWidth(), i );
    }

    // draw border
    dc->SetPen( wxPen( wxColor(0,0,0), 2 ) );
    dc->DrawRectangle( bbb );
}

void World::DrawEntities(wxDC* dc)
{
    for (int i=0; i<worldSize.GetWidth(); i++)
    {
        for (int j=0; j<worldSize.GetHeight(); j++)
        {
            if (entitiesTable[i][j])
                entitiesTable[i][j]->Draw(dc);
        }
    }

    Entity* se = GetSelectedEntity();

    if (se)
        se->DrawSelected(dc);
}

void World::GenerateEmptyPoints()
{
    emptyPoints.clear();

    for (int i=0; i<worldSize.GetWidth(); i++)
    {
        for (int j=0; j<worldSize.GetHeight(); j++)
        {
            wxPoint p(i,j);

            if (nullptr == GetEntityByPosition(p))
            {
                emptyPoints.push_back(p);
            }
        }
    }
}

bool World::LeaseEmptyPoint(const wxPoint& point)
{
    auto it (std::find(emptyPoints.begin(), emptyPoints.end(), point));

    if (it != emptyPoints.end())
    {
        *it = std::move(emptyPoints.back());
        emptyPoints.pop_back();

        return true;
    }

    return false;
}

wxPoint World::LeaseRandomEmptyPoint()
{
    wxPoint point(-1,-1);

    if (emptyPoints.size()>0)
    {
        auto index = effolkronium::random_static::get<int>(0, emptyPoints.size()-1);
        point = emptyPoints[index];

        emptyPoints[index] = std::move(emptyPoints.back());
        emptyPoints.pop_back();
    }

    return point;
}

void World::ReleasePoint(const wxPoint& point)
{
    emptyPoints.push_back(point);
}

void World::GenerateEntities(int type, int quantity)
{
    for (int i=0; i<quantity; i++)
    {
        wxPoint point = LeaseRandomEmptyPoint();

        if (point == wxPoint(-1,-1))
            break;

        Entity* tmp = nullptr;

        switch (type)
        {
        case Entity::TYPE_PLANT:
            {
                Plant* plt = (Plant*)_allocator.Allocate(sizeof(Plant),8);
                if (plt)
                {
                    new(plt) Plant(this);
                    tmp = plt;
                }
            }
            break;
        case Entity::TYPE_CELL:
            {
                Cell* cl = (Cell*)_allocator.Allocate(sizeof(Cell),8);
                if (cl)
                {
                    new(cl) Cell(this, wxString::Format(wxT("gene%d"), i+1));
                    tmp = cl;
                }
            }
            break;
        default:
            break;
        }

        assert("Critacal error: can't allocate memory." && tmp);

        if (tmp)
        {
            tmp->SetPosition(point);
            AddEntity(tmp);
        }
        else
        {
            ReleasePoint(point);
        }
    }
}

void World::DeathHandle()
{
    /*
    auto iter = entitiesList.begin();

    while (iter != entitiesList.end())
    {
        Entity* e = *iter;

        if ( e && e->IsDead() )
        {
            if ( e->GetType() == Entity::TYPE_PLANT || e->GetType() == Entity::TYPE_MEAT )
            {
                switch (e->GetType())
                {
                case Entity::TYPE_PLANT:
                    plantsCounter--;
                    break;
                case Entity::TYPE_MEAT:
                    meatCounter--;
                    break;
                default:
                    break;
                }

                wxPoint p = e->GetPosition();
                entitiesTable[p.x][p.y] = nullptr;

                ReleasePoint(p);
                delete e;
                entitiesList.erase(iter++);  // alternatively, iter = entities.erase(iter);
            }
            else if ( e->GetType() == Entity::TYPE_CELL )
            {
                wxPoint p = e->GetPosition();
                delete e;

                e = new Meat(this);
                e->SetPosition(p);

                entitiesTable[p.x][p.y] = e;

                *iter = e;

                cellsCounter--;
                meatCounter++;
            }
        }
        else
        {
            ++iter;
        }
    }
    */

    for (int i=0; i<worldSize.GetWidth(); i++)
    {
        for (int j=0; j<worldSize.GetHeight(); j++)
        {
            Entity* e = entitiesTable[i][j];

            if ( e && e->IsDead() )
            {
                if ( e->GetType() == Entity::TYPE_PLANT || e->GetType() == Entity::TYPE_MEAT )
                {
                    wxPoint p = e->GetPosition();

                    entitiesTable[p.x][p.y] = nullptr;
                    ReleasePoint(p);

                    if (e->GetType() == Entity::TYPE_PLANT)
                    {
                        plantsCounter--;
                    }
                    else
                    {
                        meatCounter--;
                    }

                    {
                        e->~Entity();
                        _allocator.Free(e);
                    }
                }
                else if ( e->GetType() == Entity::TYPE_CELL )
                {
                    wxPoint p = e->GetPosition();

                    {
                        e->~Entity();
                        _allocator.Free(e);
                    }

                    entitiesTable[p.x][p.y] = nullptr;
                    e = nullptr;

                    cellsCounter--;

                    {
                        Meat* mt = (Meat*)_allocator.Allocate(sizeof(Meat),8);
                        if (mt)
                        {
                            new(mt) Meat(this);
                            e = mt;
                        }
                        else
                        {
                            assert("Critacal error: can't allocate memory." && 0);
                        }
                    }

                    if (e)
                    {
                        e->SetPosition(p);
                        entitiesTable[p.x][p.y] = e;
                        meatCounter++;
                    }
                    else
                    {
                        ReleasePoint(p);
                    }
                }
            }
        }
    }
}

void World::ClearEntitiesTable()
{
    for (int i=0; i<worldSize.GetWidth(); i++)
        for (int j=0; j<worldSize.GetHeight(); j++)
        {
            if (entitiesTable[i][j])
            {
                {
                    Entity* e = entitiesTable[i][j];
                    e->~Entity();
                    _allocator.Free(e);
                }

                entitiesTable[i][j] = nullptr;
            }
        }
}

void World::ClearEmptyPoints()
{
    emptyPoints.clear();
}

std::tuple<int, int, int> World::GetEntitiesQuantity()
{
    return { plantsCounter, meatCounter, cellsCounter };
}

std::tuple<std::size_t, std::size_t, std::size_t> World::GetAllocationMemory()
{
    return { _allocator.GetTotal(), _allocator.GetUsed(), _allocator.GetPeak() };
}

// ENTITY CLASS
Entity::Entity(World* _world): world(_world)
{
    id = world->GetNextId();
}

Entity::~Entity() {}

void Entity::Step()
{
    if (IsDead()) return;
    age++;
}

bool Entity::IsDead()
{
    return (age >= lifeTime) || (energy <= 0);
}

void Entity::Die() { age = lifeTime; }
int Entity::GetId() { return id; }

void Entity::SetColor(const wxColor& _color) { color = _color; }
const wxColor& Entity::GetColor() const { return color; }

void Entity::SetPosition(const wxPoint& _position) { position = _position; }
const wxPoint& Entity::GetPosition() const { return position; }

int Entity::GetType() { return type; }

void Entity::SetEnergy(int _energy) { energy = _energy; }
int Entity::GetEnergy() { return energy; }

wxString Entity::Get(const wxString& name)
{
    if (name == wxString("id"))
        return wxString::Format(wxT("%d"), id);
    if (name == wxString("age"))
        return wxString::Format(wxT("%d"), age);
    if (name == wxString("maxAge"))
        return wxString::Format(wxT("%d"), lifeTime);
    if (name == wxString("energy"))
        return wxString::Format(wxT("%d"), energy);

    return unknownValueStr;
}

void Entity::SetSelectedBrushAndPen(wxDC* dc)
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

void Entity::SetNormalBrushAndPen(wxDC* dc)
{
    wxBrush brush;
    wxPen pen;

    brush.SetStyle(wxBRUSHSTYLE_SOLID);
    brush.SetColour(color);

    pen.SetColour(color);

    dc->SetBrush(brush);
    dc->SetPen(pen);
}

void Entity::DrawCircle(wxDC* dc)
{
    wxRect bbb = world->GetBoardBoundingBox();
    wxSize field = world->GetFieldSize(bbb);

    int radius = field.GetHeight()/3;

    wxPoint p = world->WorldToPanel(position);
    dc->DrawCircle(p.x+field.GetWidth()/2, p.y+field.GetHeight()/2, radius);
}

void Entity::DrawRectangle(wxDC* dc)
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

// PLANT CLASS

Plant::Plant(World* _world): Entity(_world)
{
    type = Entity::TYPE_PLANT;
    color = wxColor(43,168,74);
    lifeTime = world->GetProperties()->GetValue(wxString("plantLifeTime"));
}

Plant::~Plant() {}

void Plant::Draw(wxDC* dc)
{
    SetNormalBrushAndPen(dc);
    DrawCircle(dc);
}

void Plant::DrawSelected(wxDC* dc)
{
    SetSelectedBrushAndPen(dc);
    DrawCircle(dc);
}

// MEAT CLASS

Meat::Meat(World* _world): Entity(_world) {
    type = Entity::TYPE_MEAT;
    color = wxColor(205,83,59);
    lifeTime = world->GetProperties()->GetValue(wxString("meatLifeTime"));
}

Meat::~Meat() {}

void Meat::Draw(wxDC* dc)
{
    SetNormalBrushAndPen(dc);
    DrawCircle(dc);
}

void Meat::DrawSelected(wxDC* dc)
{
    SetSelectedBrushAndPen(dc);
    DrawCircle(dc);
}

// CELL CLASS

// sets of behavior
static const std::array<int, 4> emptyActions { Gene::ACTION_TURN_L, Gene::ACTION_TURN_R, Gene::ACTION_MOVE, Gene::ACTION_NONE };
static const std::array<int, 3> otherActions { Gene::ACTION_TURN_L, Gene::ACTION_TURN_R, Gene::ACTION_ATTACK };
static const std::array<int, 3> sameActions { Gene::ACTION_TURN_L, Gene::ACTION_TURN_R, Gene::ACTION_ATTACK };
static const std::array<int, 4> meatActions { Gene::ACTION_TURN_L, Gene::ACTION_TURN_R, Gene::ACTION_NONE, Gene::ACTION_EAT };
static const std::array<int, 4> plantActions { Gene::ACTION_TURN_L, Gene::ACTION_TURN_R, Gene::ACTION_NONE, Gene::ACTION_EAT };
static const std::array<int, 2> wallActions { Gene::ACTION_TURN_L, Gene::ACTION_TURN_R };
static const std::array<int, 2> weakActions { Gene::ACTION_TURN_L, Gene::ACTION_TURN_R };
static const std::array<int, 3> deadActions { Gene::ACTION_TURN_L, Gene::ACTION_TURN_R, Gene::ACTION_NONE };

Cell::Cell(World* _world, const wxString& geneName): Entity(_world)
{
    type = Entity::TYPE_CELL;

    lifeTime = effolkronium::random_static::get<int>(
        0,
        world->GetProperties()->GetValue(wxString("maxAge"))
    );

    energy = world->GetProperties()->GetValue(wxString("cellEnergy"));

    divEnergy = effolkronium::random_static::get<int>(
        world->GetProperties()->GetValue(wxString("minEnergyForDivision")),
        world->GetProperties()->GetValue(wxString("maxEnergyForDivision"))
    );

    damage = effolkronium::random_static::get<int>(
        0,
        world->GetProperties()->GetValue(wxString("maxDamage"))
    );

    mutationProbability = effolkronium::random_static::get<int>(
        0,
        world->GetProperties()->GetValue(wxString("maxMutationProbability"))
    );

    direction = GenerateDirection();

    gen1 = GenerateGene(geneName);
    color = GenerateColor();
}

Cell::Cell(World* _world, int _divEnergy, int _damage, int _mutationProbability, const wxColor& _color, const Gene& _gene): Entity(_world)
{
    type = Entity::TYPE_CELL;

    divEnergy = _divEnergy;
    damage = _damage;
    mutationProbability = _mutationProbability;
    energy = 0;

    lifeTime = effolkronium::random_static::get<int>(1, world->GetProperties()->GetValue(wxString("maxAge")));

    direction = GenerateDirection();

    auto mutation = effolkronium::random_static::get<bool>(mutationProbability/100.f);

    if (mutation)
    {
        gen1 = MutateGene(_gene);
        color = GenerateColor();
    }
    else
    {
        gen1 = _gene;
        color = _color;
    }
}

Cell::~Cell() {}

wxPoint Cell::GenerateDirection()
{
    static const std::array<wxPoint,8> directions {
        wxPoint(1,0),
        wxPoint(1,1),
        wxPoint(0,1),
        wxPoint(-1,1),
        wxPoint(-1,0),
        wxPoint(-1,-1),
        wxPoint(0,-1),
        wxPoint(1,-1)
    };

    return directions[effolkronium::random_static::get<int>(0, directions.size()-1)];
}

void Cell::Step()
{
    if (IsDead())
    {
        return;
    }

    age++;

    if (attacked)
    {
        SetLastBehavior(wxT("attacked"));
        attacked = false;
    }
    else if (CanDivide())
    {
        SetLastBehavior(wxT("division"));
        Clone();
    }
    else // genetic behavior
    {
        wxPoint p = position + direction;

        if (!world->IsInside(p))
        {
            SetLastBehavior(wxT("wall"));
            Execute(gen1.wall);
            return;
        }

        Entity* e = world->GetEntityByPosition(p);

        if (e == nullptr)
        {
            SetLastBehavior(wxT("empty"));
            Execute(gen1.empty);
            return;
        }

        if (e->IsDead())
        {
            SetLastBehavior(wxT("dead"));
            Execute(gen1.dead);
            return;
        }

        switch (e->GetType())
        {
        case TYPE_PLANT:
            SetLastBehavior(wxT("plant"));
            Execute(gen1.plant);
            break;
        case TYPE_MEAT:
            SetLastBehavior(wxT("meat"));
            Execute(gen1.meat);
            break;
        case TYPE_CELL:
            if (e->GetColor() == color)
            {
                SetLastBehavior(wxT("same cell"));
                Execute(gen1.same);
            }
            else {
                SetLastBehavior(wxT("other cell"));
                Execute(gen1.other);
            }
            break;
        default:
            break;
        }
    }
}

void Cell::Draw(wxDC* dc)
{
    SetNormalBrushAndPen(dc);

    DrawRectangle(dc);
    DrawDirection(dc);
}

void Cell::DrawSelected(wxDC* dc)
{
    SetSelectedBrushAndPen(dc);
    DrawRectangle(dc);
}

void Cell::DrawDirection(wxDC* dc)
{
    wxRect bbb = world->GetBoardBoundingBox();
    wxSize field = world->GetFieldSize(bbb);

    wxPoint a = world->WorldToPanel(position);

    a.x += field.GetWidth() / 2;
    a.y += field.GetHeight() / 2;

    wxPoint b(a.x+direction.x*field.GetWidth(), a.y+direction.y*field.GetHeight());

    dc->DrawLine(a, b);
}

bool Cell::Attack(Cell* victim)
{
    if (energy - victim->GetEnergy() < world->GetProperties()->GetValue(wxString("attackCondition")) )
        return false;

    victim->SetEnergy(victim->GetEnergy() - damage);
    victim->SetAttacked(true);

    return true;
}

void Cell::SetAttacked(bool _attacked)
{
    attacked = _attacked;
}

const Gene& Cell::GetGene()
{
    return gen1;
}

void Cell::SetGene(const Gene& _gene)
{
    gen1 = _gene;
}


void Cell::Clone()
{
    wxPoint oldPosition = position;
    wxPoint newPosition = position + direction;

    Entity* child = nullptr;
    {
        Cell* cl = (Cell*)_allocator.Allocate(sizeof(Cell),8);
        if (cl)
        {
            new(cl) Cell(world, divEnergy, damage, mutationProbability, color, gen1);
            child = cl;
        }
        else
        {
            assert("Critacal error: can't allocate memory." && 0);
        }
    }

    if (world->MoveEntity(this, newPosition))
    {
        if (world->LeaseEmptyPoint(oldPosition))
        {
            energy = (energy - world->GetProperties()->GetValue(wxString("movementEnergy"))) / 2;

            child->SetPosition(oldPosition);
            child->SetEnergy(energy);

            world->AddEntity(child);
            childrenCounter++;
        }
        else
        {
            assert("Logical error: can't lease an empty point for a child." && 0);
        }
    }
    else
    {
        assert("Logical error: can't move a parent cell to new position." && 0);
    }
}

int Cell::NormalizeCoord(int x)
{
    if (x > 0) return 1;
    if (x < 0) return -1;

    return 0;
}

void Cell::SetLastBehavior(const wxString& behavior)
{
    lastBehavior = behavior;
}

void Cell::Execute(int cmd)
{
    if (cmd == Gene::ACTION_TURN_L)
    {
        int x = direction.x + direction.y;
        int y = direction.y - direction.x;

        direction.x = NormalizeCoord(x);
        direction.y = NormalizeCoord(y);

        energy -= world->GetProperties()->GetValue(wxString("movementEnergy"));

        return;
    }

    if (cmd == Gene::ACTION_TURN_R)
    {
        int x = direction.x - direction.y;
        int y = direction.x + direction.y;

        direction.x = NormalizeCoord(x);
        direction.y = NormalizeCoord(y);

        energy -= world->GetProperties()->GetValue(wxString("movementEnergy"));

        return;
    }

    if (cmd == Gene::ACTION_MOVE)
    {
        wxPoint p = position + direction;

        if (world->MoveEntity(this, p))
        {
            energy -= world->GetProperties()->GetValue(wxString("movementEnergy"));
        }

        return;
    }

    if (cmd == Gene::ACTION_NONE)
    {
        return;
    }

    if (cmd == Gene::ACTION_ATTACK)
    {
        wxPoint p = position + direction;
        Entity* e = world->GetEntityByPosition(p);

        if (e && e->GetType() == TYPE_CELL)
        {
            Cell* c = dynamic_cast<Cell*>(e);

            if (!Attack(c))
            {
                SetLastBehavior(wxT("weak"));
                Execute(gen1.weak);
            }
            else
            {
                energy -= world->GetProperties()->GetValue(wxString("attackEnergy"));

                if (c->IsDead())
                    killsCounter++;
            }
        }

        return;
    }

    if (cmd == Gene::ACTION_EAT)
    {
        wxPoint p = position + direction;
        Entity* e = world->GetEntityByPosition(p);

        if (e)
        {
            switch(e->GetType())
            {
            case TYPE_PLANT:
                energy += world->GetProperties()->GetValue(wxString("plantEnergy"));
                eatenPlantsCounter++;
                break;
            case TYPE_MEAT:
                energy += world->GetProperties()->GetValue(wxString("meatEnergy"));
                eatenMeatCounter++;
                break;
            default:
                break;
            }

            e->Die();
        }
    }
}

Gene Cell::GenerateGene(const wxString& name)
{
    Gene newGene(name);

    newGene.empty = emptyActions[effolkronium::random_static::get<int>(0, emptyActions.size()-1)];
    newGene.other = otherActions[effolkronium::random_static::get<int>(0, otherActions.size()-1)];
    newGene.same = sameActions[effolkronium::random_static::get<int>(0, sameActions.size()-1)];
    newGene.meat = meatActions[effolkronium::random_static::get<int>(0, meatActions.size()-1)];
    newGene.plant = plantActions[effolkronium::random_static::get<int>(0, plantActions.size()-1)];
    newGene.wall = wallActions[effolkronium::random_static::get<int>(0, wallActions.size()-1)];
    newGene.weak = weakActions[effolkronium::random_static::get<int>(0, weakActions.size()-1)];
    newGene.dead = deadActions[effolkronium::random_static::get<int>(0, deadActions.size()-1)];

    return newGene;
}

Gene Cell::MutateGene(const Gene& _gene)
{
    Gene newGene = _gene;

    enum
    {
        EMPTY = 0,
        OTHER,
        SAME,
        MEAT,
        PLANT,
        WALL,
        WEAK,
        DEAD
    };

    switch (effolkronium::random_static::get<int>(EMPTY, DEAD))
    {
    case EMPTY:
        newGene.empty = emptyActions[effolkronium::random_static::get<int>(0, emptyActions.size()-1)];
        break;
    case OTHER:
        newGene.other = otherActions[effolkronium::random_static::get<int>(0, otherActions.size()-1)];
        break;
    case SAME:
        newGene.same = sameActions[effolkronium::random_static::get<int>(0, sameActions.size()-1)];
        break;
    case MEAT:
        newGene.meat = meatActions[effolkronium::random_static::get<int>(0, meatActions.size()-1)];
        break;
    case PLANT:
        newGene.plant = plantActions[effolkronium::random_static::get<int>(0, plantActions.size()-1)];
        break;
    case WALL:
        newGene.wall = wallActions[effolkronium::random_static::get<int>(0, wallActions.size()-1)];
        break;
    case WEAK:
        newGene.weak = weakActions[effolkronium::random_static::get<int>(0, weakActions.size()-1)];
        break;
    case DEAD:
        newGene.dead = deadActions[effolkronium::random_static::get<int>(0, deadActions.size()-1)];
        break;
    default:
        break;
    }

    return newGene;
}

wxColor Cell::GenerateColor()
{
    int r = effolkronium::random_static::get<int>(0, 128);
    int g = effolkronium::random_static::get<int>(0, 128);
    int b = effolkronium::random_static::get<int>(0, 128);

    return wxColor(r,g,b);
}

bool Cell::CanDivide()
{
    wxPoint p = position;
    p += direction;
    return ( (energy >= divEnergy) && world->IsInside(p) && (world->GetEntityByPosition(p) == nullptr) );
}

wxString Cell::Get(const wxString& name)
{
    if (name == wxString("id"))
        return wxString::Format(wxT("%d"), id);
    if (name == wxString("age"))
        return wxString::Format(wxT("%d"), age);
    if (name == wxString("maxAge"))
        return wxString::Format(wxT("%d"), lifeTime);
    if (name == wxString("energy"))
        return wxString::Format(wxT("%d"), energy);
    if (name == wxString("divEnergy"))
        return wxString::Format(wxT("%d"), divEnergy);
    if (name == wxString("mutation"))
        return wxString::Format(wxT("%d"), mutationProbability);
    if (name == wxString("damage"))
        return wxString::Format(wxT("%d"), damage);
    if (name == wxString("kills"))
        return wxString::Format(wxT("%d"), killsCounter);
    if (name == wxString("childrens"))
        return wxString::Format(wxT("%d"), childrenCounter);
    if (name == wxString("eatenPlants"))
        return wxString::Format(wxT("%d"), eatenPlantsCounter);
    if (name == wxString("eatenMeat"))
        return wxString::Format(wxT("%d"), eatenMeatCounter);
    if (name == wxString("lastBehavior"))
        return lastBehavior;

    return unknownValueStr;
}

}
