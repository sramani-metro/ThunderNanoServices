
#include <interfaces/json/JsonData_TimeSync.h>
#include "TimeSync.h"
#include "Module.h"

namespace WPEFramework {

namespace Plugin {

    using namespace JsonData::TimeSync;

    // Registration
    //

    void TimeSync::RegisterAll()
    {
        Register<void,TimeResultData>(_T("time"), &TimeSync::endpoint_time, this);
        Register<void,void>(_T("synchronize"), &TimeSync::endpoint_synchronize, this);
        Register<SetParamsData,void>(_T("set"), &TimeSync::endpoint_set, this);
    }

    void TimeSync::UnregisterAll()
    {
        Unregister(_T("set"));
        Unregister(_T("synchronize"));
        Unregister(_T("time"));
    }

    // API implementation
    //

    // Returns the synchronized time.
    // Return codes:
    //  - ERROR_NONE: Success
    uint32_t TimeSync::endpoint_time(TimeResultData& response)
    {
        response.Time =  _client->SyncTime();
        response.Source = _client->Source();

        return Core::ERROR_NONE;
    }

    // Synchronizes time.
    // Return codes:
    //  - ERROR_NONE: Success
    //  - ERROR_INPROGRESS: Operation in progress
    //  - Core: Returned when the source configuration is missing or invalid.
    uint32_t TimeSync::endpoint_synchronize()
    {
        uint32_t result = _client->Synchronize();

        if ((result != Core::ERROR_NONE) && (result != Core::ERROR_INPROGRESS) && (result != Core::ERROR_INCOMPLETE_CONFIG)) {
            result = Core::ERROR_INCOMPLETE_CONFIG;
        }

        return result;
    }

    // Sets the current time from an external source.
    // Return codes:
    //  - ERROR_NONE: Success
    //  - ERROR_BAD_REQUEST: Returned when the requested time was invalid
    uint32_t TimeSync::endpoint_set(const SetParamsData& params)
    {
        uint32_t result = Core::ERROR_NONE;

        if (params.Time.IsSet()) {
            const uint64_t& time = params.Time.Value();
            Core::Time newTime(time);
            if (newTime.IsValid()) {
                Core::SystemInfo::Instance().SetTime(newTime);
            }
            else {
                result = Core::ERROR_BAD_REQUEST;
            }
        }

        if (result == Core::ERROR_NONE) {
            EnsureSubsystemIsActive();
        }

        return result;
    }

} // namespace Plugin

}

