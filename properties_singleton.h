#ifndef _PROPERTIES_SINGLETON_H_
#define _PROPERTIES_SINGLETON_H_

#include "properties.h"

class PropertiesSingleton
{
public:
    PropertiesSingleton(const PropertiesSingleton& src) = delete;

    ~PropertiesSingleton() {
        std::cout << "~Singleton()" << std::endl;
    }

    const ProtoPuddle::GlobalProperties& GetProperties() const
    {
        return properties;
    }

    ProtoPuddle::GlobalProperties* GetPropertiesPtr()
    {
        return &properties;
    }

    void UpdateProperties(const ProtoPuddle::GlobalProperties& _properties)
    {
        properties = _properties;
        updated = true;
    }

    bool IsPropertiesUpdated()
    {
        return updated;
    }

    void ResetUpdateFlag()
    {
        updated = false;
    }

    static PropertiesSingleton& getInstance() {
        static PropertiesSingleton _instance;
        return _instance;
    }

private:
    PropertiesSingleton() {}

private:
    ProtoPuddle::GlobalProperties properties;

    bool updated {false};
};


#endif