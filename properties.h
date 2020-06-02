#ifndef _PROPERTIES_H_
#define _PROPERTIES_H_

#include <map>
#include <list>
#include <wx/string.h>

namespace ProtoPuddle
{

struct Property
{
    Property() {}
    Property(const Property& src) {
        value = src.value;
        min = src.min;
        max = src.max;
    }
    Property(int _value, int _min, int _max): value(_value), min(_min), max(_max) {}

    Property& operator=(const Property& r) {
        if (this == &r) {
            return *this;
        }

        value = r.value;
        min = r.min;
        max = r.max;

        return *this;
    }

    int value {0};
    int min {0};
    int max {0};
};

class GlobalProperties
{
public:
    GlobalProperties() {}

    GlobalProperties(const GlobalProperties& src) {
        _properties = src._properties;
    }

    ~GlobalProperties()
    {
        _properties.clear();
    }

    GlobalProperties& operator=(const GlobalProperties& r) {
        if (this == &r) {
            return *this;
        }

        _properties = r._properties;
        return *this;
    }

    void GetNamesAsList(std::list<wxString> *names)
    {
        for (auto it=_properties.begin(); it!=_properties.end(); ++it)
        {
            names->push_back(it->first);
        }
    }

    bool IsExist(const wxString& name)
    {
        if (_properties.find(name) == _properties.end())
            return false;

        return true;
    }

    void SetProperty(const wxString& name, const Property& property)
    {
        if (IsExist(name))
        {
            _properties[name] = property;
        }

        _properties.insert( std::pair<wxString,Property>(name,property) );
    }

    void SetValue(const wxString& name, int value)
    {
        if (IsExist(name))
        {
            //_properties.erase(_properties.find(name));

            if (value >= _properties[name].min && value <= _properties[name].max)
                _properties[name].value = value;
        }
    }

    int GetValue(const wxString& name)
    {
        if (IsExist(name))
        {
            return _properties[name].value;
        }

        return 0;
    }

    void SetRange(const wxString& name, int min, int max)
    {
        if (IsExist(name))
        {
            _properties[name].min = min;
            _properties[name].max = max;
        }
    }

    int GetMin(const wxString& name)
    {
        if (IsExist(name))
        {
            return _properties[name].min;
        }

        return 0;
    }

    int GetMax(const wxString& name)
    {
        if (IsExist(name))
        {
            return _properties[name].max;
        }

        return 0;
    }

    int GetQuantity()
    {
        return static_cast<int>(_properties.size());
    }

private:
    std::map<wxString,Property> _properties {
        { wxT("sortsOfCell"), Property(5,1,100) },
        { wxT("cellEnergy"), Property(100,1,1000) },
        { wxT("maxDamage"), Property(30,1,500) },
        { wxT("behaviorGenes"), Property(1,1,10) },
        { wxT("minEnergyForDivision"), Property(50,1,1000) },
        { wxT("maxEnergyForDivision"), Property(100,1,1000) },
        { wxT("plants"), Property(0,0,100) },
        { wxT("plantEnergy"), Property(30,1,500) },
        { wxT("meatEnergy"), Property(50,1,500) },
        { wxT("maxAge"), Property(300,1,1000) },
        { wxT("stepsPerSecond"), Property(5,1,60) },
        { wxT("plantsPerStep"), Property(2,0,100) },
        { wxT("worldWidth"), Property(20,4,80) },
        { wxT("worldHeight"), Property(20,4,80) },
        { wxT("plantLifeTime"), Property(20,1,1000) },
        { wxT("meatLifeTime"), Property(20,1,1000) },
        { wxT("movementEnergy"), Property(1,1,100) },
        { wxT("attackEnergy"), Property(2,1,100) },
        { wxT("attackCondition"), Property(10,1,500) },
        { wxT("maxMutationProbability"), Property(50,0,100) }
    };
};

}

#endif
