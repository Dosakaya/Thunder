#include "MetaData.h"
#include "IStateControl.h"
#include "ISubSystem.h"

namespace WPEFramework {

ENUM_CONVERSION_BEGIN(PluginHost::MetaData::Channel::state)

    { PluginHost::MetaData::Channel::WEBSERVER, _TXT("WebServer") },
    { PluginHost::MetaData::Channel::WEBSOCKET, _TXT("WebSocket") },
    { PluginHost::MetaData::Channel::RAWSOCKET, _TXT("RawSocket") },
    { PluginHost::MetaData::Channel::CLOSED, _TXT("Closed") },
    { PluginHost::MetaData::Channel::SUSPENDED, _TXT("Suspended") },

    ENUM_CONVERSION_END(PluginHost::MetaData::Channel::state)

        ENUM_CONVERSION_BEGIN(PluginHost::MetaData::Service::state)

            { PluginHost::MetaData::Service::DEACTIVATED, _TXT("deactivated") },
    { PluginHost::MetaData::Service::DEACTIVATION, _TXT("deactivation") },
    { PluginHost::MetaData::Service::ACTIVATED, _TXT("activated") },
    { PluginHost::MetaData::Service::ACTIVATION, _TXT("activation") },
    { PluginHost::MetaData::Service::SUSPENDED, _TXT("suspended") },
    { PluginHost::MetaData::Service::RESUMED, _TXT("resumed") },
    { PluginHost::MetaData::Service::PRECONDITION, _TXT("precondition") },

    ENUM_CONVERSION_END(PluginHost::MetaData::Service::state)

        ENUM_CONVERSION_BEGIN(PluginHost::ISubSystem::IInternet::network_type)

            { PluginHost::ISubSystem::IInternet::UNKNOWN, _TXT("Unknown") },
    { PluginHost::ISubSystem::IInternet::IPV4, _TXT("IPv4") },
    { PluginHost::ISubSystem::IInternet::IPV6, _TXT("IPv6") },

    ENUM_CONVERSION_END(PluginHost::ISubSystem::IInternet::network_type)

        namespace PluginHost
{

    /* static */ const TCHAR* ISubSystem::IInternet::ToString(const network_type value)
    {
        return (Core::EnumerateType<network_type>(value).Data());
    }

    MetaData::Service::State& MetaData::Service::State::operator=(const PluginHost::IShell* RHS)
    {
        Core::JSON::EnumType<state>::operator=(static_cast<state>(RHS->State()));

        if (RHS->State() == PluginHost::IShell::ACTIVATED) {
            // See if there is a state interface, if so, we are suspended or resumed :-)
            PluginHost::IStateControl* mode = const_cast<PluginHost::IShell*>(RHS)->QueryInterface<PluginHost::IStateControl>();

            if (mode != nullptr) {
                Core::JSON::EnumType<state>::operator=(mode->State() == PluginHost::IStateControl::RESUMED ? RESUMED : SUSPENDED);
                mode->Release();
            }
        }

        return (*this);
    }

    string MetaData::Service::State::Data() const
    {
        return (Core::JSON::EnumType<state>::Data());
    }

    MetaData::Channel::State& MetaData::Channel::State::operator=(const MetaData::Channel::state RHS)
    {
        Core::JSON::EnumType<state>::operator=(RHS);

        return (*this);
    }

    string MetaData::Channel::State::Data() const
    {
        return (Core::JSON::EnumType<state>::Data());
    }

    MetaData::Service::Service()
        : Plugin::Config()
    {
        Add(_T("state"), &JSONState);
#ifdef RUNTIME_STATISTICS
        Add(_T("processedrequests"), &ProcessedRequests);
        Add(_T("processedobjects"), &ProcessedObjects);
#endif
#ifdef RESTFULL_API
        Add(_T("observers"), &Observers);
#endif
        Add(_T("module"), &Module);
        Add(_T("hash"), &Hash);
    }
    MetaData::Service::Service(const MetaData::Service& copy)
        : Plugin::Config(copy)
        , JSONState(copy.JSONState)
#ifdef RUNTIME_STATISTICS
        , ProcessedRequests(copy.ProcessedRequests)
        , ProcessedObjects(copy.ProcessedObjects)
#endif
#ifdef RESTFULL_API
        , Observers(copy.Observers)
#endif
        , Module(copy.Module)
        , Hash(copy.Hash)
    {
        Add(_T("state"), &JSONState);
#ifdef RUNTIME_STATISTICS
        Add(_T("processedrequests"), &ProcessedRequests);
        Add(_T("processedobjects"), &ProcessedObjects);
#endif
#ifdef RESTFULL_API
        Add(_T("observers"), &Observers);
#endif
        Add(_T("module"), &Module);
        Add(_T("hash"), &Hash);
    }
    MetaData::Service::~Service()
    {
    }

    MetaData::Service& MetaData::Service::operator=(const Plugin::Config& RHS)
    {
        Plugin::Config::operator=(RHS);
        return (*this);
    }

    MetaData::Channel::Channel()
        : Core::JSON::Container()
    {
        Core::JSON::Container::Add(_T("remote"), &Remote);
        Core::JSON::Container::Add(_T("state"), &JSONState);
        Core::JSON::Container::Add(_T("activity"), &Activity);
        Core::JSON::Container::Add(_T("id"), &ID);
        Core::JSON::Container::Add(_T("name"), &Name);
    }
    MetaData::Channel::Channel(const MetaData::Channel& copy)
        : Core::JSON::Container()
        , Remote(copy.Remote)
        , JSONState(copy.JSONState)
        , Activity(copy.Activity)
        , ID(copy.ID)
        , Name(copy.Name)
    {
        Core::JSON::Container::Add(_T("remote"), &Remote);
        Core::JSON::Container::Add(_T("state"), &JSONState);
        Core::JSON::Container::Add(_T("activity"), &Activity);
        Core::JSON::Container::Add(_T("id"), &ID);
        Core::JSON::Container::Add(_T("name"), &Name);
    }
    MetaData::Channel::~Channel()
    {
    }

    MetaData::Channel& MetaData::Channel::operator=(const MetaData::Channel& RHS)
    {
        Remote = RHS.Remote;
        JSONState = RHS.JSONState;
        Activity = RHS.Activity;
        ID = RHS.ID;
        Name = RHS.Name;

        return (*this);
    }

    MetaData::Bridge::Bridge()
        : Core::JSON::Container()
    {
        Add(_T("locator"), &Locator);
        Add(_T("latency"), &Latency);
        Add(_T("model"), &Model);
        Add(_T("secure"), &Secure);
    }
    MetaData::Bridge::Bridge(const string& text, const uint32_t latency, const string& model, const bool secure)
        : Core::JSON::Container()
    {
        Add(_T("locator"), &Locator);
        Add(_T("latency"), &Latency);
        Add(_T("model"), &Model);
        Add(_T("secure"), &Secure);

        Locator = text;
        Latency = latency;
        Secure = secure;
        if (model.empty() == false) {
            Model = model;
        }
    }
    MetaData::Bridge::Bridge(const Bridge& copy)
        : Core::JSON::Container()
        , Locator(copy.Locator)
        , Latency(copy.Latency)
        , Model(copy.Model)
        , Secure(copy.Secure)
    {
        Add(_T("locator"), &Locator);
        Add(_T("latency"), &Latency);
        Add(_T("model"), &Model);
        Add(_T("secure"), &Secure);
    }
    MetaData::Bridge::~Bridge()
    {
    }

    MetaData::Server::Server()
    {
        Core::JSON::Container::Add(_T("threads"), &ThreadPoolRuns);
        Core::JSON::Container::Add(_T("pending"), &PendingRequests);
        Core::JSON::Container::Add(_T("occupation"), &PoolOccupation);
    }
    MetaData::Server::~Server()
    {
    }

    MetaData::MetaData()
    {
        Core::JSON::Container::Add(_T("plugins"), &Plugins);
        Core::JSON::Container::Add(_T("channel"), &Channels);
        Core::JSON::Container::Add(_T("server"), &Process);
        Core::JSON::Container::Add(_T("bridges"), &Bridges);
        Core::JSON::Container::Add(_T("value"), &Value);
        Core::JSON::Container::Add(_T("subsystems"), &SubSystems);
    }
    MetaData::~MetaData()
    {
    }

    MetaData::SubSystem::SubSystem()
        : Core::JSON::Container()
        , _subSystems()
    {
    }
    MetaData::SubSystem::SubSystem(const SubSystem& copy)
        : Core::JSON::Container()
        , _subSystems(copy._subSystems)
    {
        Iterator index(_subSystems.begin());

        while (index != _subSystems.end()) {
            Core::JSON::Container::Add(index->first.c_str(), &(index->second));
            index++;
        }
    }
    MetaData::SubSystem::~SubSystem()
    {
    }
    void MetaData::SubSystem::Add(const PluginHost::ISubSystem::subsystem type, const bool available)
    {
        const string name(Core::EnumerateType<PluginHost::ISubSystem::subsystem>(type).Data());

        // Create an element for this service with its callsign
        std::pair<Iterator, bool> index(_subSystems.insert(std::pair<string, Core::JSON::Boolean>(name, Core::JSON::Boolean())));

        // Store the override config in the JSON String created in the map
        Core::JSON::Container::Add(index.first->first.c_str(), &(index.first->second));

        index.first->second = available;
    }
}
}
