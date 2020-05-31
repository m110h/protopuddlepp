#include "wx/wxprec.h"

#ifndef WX_PRECOMP
#include <wx/wx.h>
#endif

#include <wx/dcbuffer.h>

#include "entities.h"

#include <array>
#include <limits>
#include <iomanip>
#include <fstream>
#include <exception>

namespace ProtoPuddle
{

World::World(GlobalProperties* _properties)
{
    SetProperties(_properties);
}

World::~World()
{
    for (Entity* e: entities)
    {
        if (e)
            delete e;
    }

    entities.clear();
}

void World::New()
{
    steps = 0;
    nextId = 0;
    selectedId = -1;

    for (Entity* e: entities)
    {
        if (e)
            delete e;
    }

    entities.clear();

    worldSize.SetWidth(properties->GetValue(wxString("worldWidth")));
    worldSize.SetHeight(properties->GetValue(wxString("worldHeight")));

    GeneratePlants( properties->GetValue(wxString("plants")) );
    GenerateCells( properties->GetValue(wxString("sortsOfCell")) );
}

void World::Step()
{
    if (steps == std::numeric_limits<int>::max())
    {
        steps = 0;
    }

    GeneratePlants(properties->GetValue(wxString("plantsPerStep")));

    for (Entity* e: entities)
    {
        if (e)
            e->Step();
    }

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

Entity* World::GetEntity(const wxPoint& worldPosition)
{
    for (Entity* e: entities)
    {
        if (e)
        {
            wxPoint p = e->GetPosition();

            if ( (p.x == worldPosition.x) && (p.y == worldPosition.y) )
                return e;
        }
    }

    return nullptr;
}

Entity* World::GetEntityById(int id)
{
    for (Entity* e: entities)
    {
        if (e && (id == e->GetId()) )
            return e;
    }

    return nullptr;
}

int World::GetEntityIdByPosition(const wxPoint& worldPosition)
{
    for (Entity* e: entities)
    {
        if (e)
        {
            wxPoint p = e->GetPosition();

            if ( (p.x == worldPosition.x) && (p.y == worldPosition.y) )
                return e->GetId();
        }
    }

    return -1;
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
    if (e) entities.push_front(e);
}

bool World::OpenFromFile(const wxString& filename)
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
        return false;
    }

    in.close();

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

    return true;
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
    for (Entity* e: entities)
    {
        if (e)
            e->Draw(dc);
    }

    Entity* se = GetSelectedEntity();

    if (se)
        se->DrawSelected(dc);
}

wxPoint World::GetEmptyPoint()
{
    std::vector<wxPoint> emptyPoints;

    for (int i=0; i<worldSize.GetWidth(); i++)
    {
        for (int j=0; j<worldSize.GetHeight(); j++)
        {
            wxPoint p(i,j);

            if (nullptr == GetEntity(p))
            {
                emptyPoints.push_back(p);
            }
        }
    }

    if (emptyPoints.empty())
        return wxPoint(-1,-1);

    int index = effolkronium::random_static::get<int>(0, emptyPoints.size()-1);

    return emptyPoints[index];
}

void World::GeneratePlants(int quantity)
{
    for (int i=0; i<quantity; i++)
    {
        wxPoint p = GetEmptyPoint();

        if (p.x >= 0 && p.y >= 0)
        {
            Plant* tmp = new Plant(this);
            tmp->SetPosition(p);
            entities.push_back((Entity*)tmp);
        }
    }
}

void World::GenerateCells(int quantity)
{
    for (int i=0; i<quantity; i++)
    {
        wxPoint p = GetEmptyPoint();

        if ( (p.x >= 0) && (p.y >= 0) )
        {
            Cell* tmp = new Cell(this);
            tmp->SetPosition(p);
            entities.push_back((Entity*)tmp);
        }
    }
}

void World::DeathHandle()
{
    auto iter = entities.begin();

    while (iter != entities.end())
    {
        Entity* e = *iter;

        if ( e && e->IsDead() )
        {
            if ( e->GetType() == Entity::TYPE_PLANT || e->GetType() == Entity::TYPE_MEAT )
            {
                delete e;
                entities.erase(iter++);  // alternatively, iter = entities.erase(iter);
            }
            else if ( e->GetType() == Entity::TYPE_CELL )
            {
                // don't forget about selected cells!

                wxPoint p = e->GetPosition();
                delete e;

                e = new Meat(this);
                e->SetPosition(p);

                *iter = e;
            }
        }
        else
        {
            ++iter;
        }
    }
}

void World::GetEntitiesQuantity(int& plants, int& meat, int& cells)
{
    auto iter = entities.begin();

    while (iter != entities.end())
    {
        Entity* e = *iter;

        if (e && !e->IsDead())
        {
            switch (e->GetType())
            {
            case Entity::TYPE_PLANT:
                plants++;
                break;
            case Entity::TYPE_MEAT:
                meat++;
                break;
            case Entity::TYPE_CELL:
                cells++;
                break;
            default:
                break;
            }
        }

        ++iter;
    }
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
const std::array<int, 4> emptyActions { Gene::ACTION_TURN_L, Gene::ACTION_TURN_R, Gene::ACTION_MOVE, Gene::ACTION_NONE };
const std::array<int, 3> otherActions { Gene::ACTION_TURN_L, Gene::ACTION_TURN_R, Gene::ACTION_ATTACK };
const std::array<int, 3> sameActions { Gene::ACTION_TURN_L, Gene::ACTION_TURN_R, Gene::ACTION_ATTACK };
const std::array<int, 4> meatActions { Gene::ACTION_TURN_L, Gene::ACTION_TURN_R, Gene::ACTION_NONE, Gene::ACTION_EAT };
const std::array<int, 4> plantActions { Gene::ACTION_TURN_L, Gene::ACTION_TURN_R, Gene::ACTION_NONE, Gene::ACTION_EAT };
const std::array<int, 2> wallActions { Gene::ACTION_TURN_L, Gene::ACTION_TURN_R };
const std::array<int, 2> weakActions { Gene::ACTION_TURN_L, Gene::ACTION_TURN_R };

Cell::Cell(World* _world): Entity(_world)
{
    type = Entity::TYPE_CELL;

    lifeTime = effolkronium::random_static::get<int>(
        1,
        world->GetProperties()->GetValue(wxString("maxAge"))
    );

    energy = world->GetProperties()->GetValue(wxString("cellEnergy"));

    divEnergy = effolkronium::random_static::get<int>(
        world->GetProperties()->GetValue(wxString("minEnergyForDivision")),
        world->GetProperties()->GetValue(wxString("maxEnergyForDivision"))
    );

    damage = effolkronium::random_static::get<int>(
        1,
        world->GetProperties()->GetValue(wxString("maxDamage"))
    );

    mutationProbability = effolkronium::random_static::get<int>(
        0,
        world->GetProperties()->GetValue(wxString("maxMutationProbability"))
    );

    std::array<wxPoint, 8> directions { wxPoint(1,0), wxPoint(1,1), wxPoint(0,1), wxPoint(-1,1), wxPoint(-1,0), wxPoint(-1,-1), wxPoint(0,-1), wxPoint(1,-1) };
    direction = directions[effolkronium::random_static::get<int>(0, directions.size()-1)];

    gen1 = GenerateGene("gene1");
    color = GenerateColor();
}

Cell::Cell(World* _world, int _divEnergy, int _damage, int _mutationProbability, const wxColor& _color, const Gene& _gene): Entity(_world)
{
    type = TYPE_CELL;

    divEnergy = _divEnergy;
    damage = _damage;
    mutationProbability = _mutationProbability;
    energy = 0;

    lifeTime = effolkronium::random_static::get<int>(1, world->GetProperties()->GetValue(wxString("maxAge")));

    std::array<wxPoint, 8> directions { wxPoint(1,0), wxPoint(1,1), wxPoint(0,1), wxPoint(-1,1), wxPoint(-1,0), wxPoint(-1,-1), wxPoint(0,-1), wxPoint(1,-1) };
    direction = directions[effolkronium::random_static::get<int>(0, directions.size()-1)];

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

Cell::~Cell()
{
    //
}

void Cell::Step()
{
    Entity::Step();

    if (attacked)
    {
        // ?
        attacked = false;
    }
    else if (CanDivide())
    {
        Clone();
    }
    else // genetic behavior
    {
        wxPoint p = position;

        p += direction;

        if (!world->IsInside(p))
        {
            Execute(gen1.wall);
            return;
        }

        Entity* e = world->GetEntity(p);

        if (e == nullptr)
        {
            Execute(gen1.empty);
            return;
        }

        if (e->IsDead())
        {
            Execute(gen1.wall);
            return;
        }

        switch (e->GetType())
        {
        case TYPE_PLANT:
            Execute(gen1.plant);
            break;
        case TYPE_MEAT:
            Execute(gen1.meat);
            break;
        case TYPE_CELL:
            if (e->GetColor() == color)
            {
                Execute(gen1.same);
            }
            else {
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
    Cell* child = new Cell(world, divEnergy, damage, mutationProbability, color, gen1);

    child->SetPosition(position);

    Execute(Gene::ACTION_MOVE);

    energy /= 2;
    child->SetEnergy(energy);

    world->AddEntity(static_cast<Entity*>(child));

    childrenCounter++;
}

int Cell::NormalizeCoord(int x)
{
    if (x > 0) return 1;
    if (x < 0) return -1;

    return 0;
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
        wxPoint p = position;

        p += direction;

        SetPosition(p);

        energy -= world->GetProperties()->GetValue(wxString("movementEnergy"));

        return;
    }

    if (cmd == Gene::ACTION_NONE)
    {
        return;
    }

    if (cmd == Gene::ACTION_ATTACK)
    {
        wxPoint p = position;

        p += direction;

        Entity* e = world->GetEntity(p);

        if (e && e->GetType() == TYPE_CELL)
        {
            Cell* c = dynamic_cast<Cell*>(e);

            if (!Attack(c))
                Execute(gen1.weak);
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
        wxPoint p = position;

        p += direction;

        Entity* e = world->GetEntity(p);

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
        WEAK
    };

    switch (effolkronium::random_static::get<int>(EMPTY, WEAK))
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

    // if an energy enough
    // if movement to forward field is possible
    // if forward field is empty
    return ( (energy >= divEnergy) && world->IsInside(p) && (world->GetEntity(p) == nullptr) );
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

    return unknownValueStr;
}

}
