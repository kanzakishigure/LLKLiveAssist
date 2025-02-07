#include "ModuleManager.h"

namespace NAssist
{
   
    std::variant<bool, std::error_code> ModuleManager::addPlugin(const std::shared_ptr<PluginBase> plugin)
    {
        if (plugin == nullptr)
        {
        return std::make_error_code(std::errc::invalid_argument);
        }

        //error will appear here
        plugin->init();
        //error will appear here

        m_plugins.push_back(plugin);
        return true;
    }
    void ModuleManager::startAllModule()
    {
        for (const auto &plugin : m_plugins)
        {
            plugin->start();
        }
    }
    void ModuleManager::update()
    {
        for (const auto &plugin : m_plugins)
        {
            plugin->drawUI();
        }
    }
    bool ModuleManager::startModule(PluginType type)
    {
        for (const auto &plugin : m_plugins)
        {
            if (plugin->getStaticType() == type)
            {
                plugin->start();
                return true;
            }
        }
        return false;
    }

    
    
  

    void ModuleManager::shutdown()
    {
        for (const auto &plugin : m_plugins)
        {
            plugin->shutdown();
        }
    }
}