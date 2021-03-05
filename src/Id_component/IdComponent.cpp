/******************************************************************************
 *                                                                            *
 * Copyright (C) 2020 Fondazione Istituto Italiano di Tecnologia (IIT)        *
 * All Rights Reserved.                                                       *
 *                                                                            *
 ******************************************************************************/

#include <yarp/os/Network.h>
#include <yarp/os/RpcServer.h>
#include <yarp/os/LogStream.h>

#include <IdService.h>

#include <mutex>
#include <set>

class IdComponent : public IdService
{
public:
    IdComponent() = default;

    bool open()
    {
        this->yarp().attachAsServer(server_port);
        if (!server_port.open("/IdComponent")) {
            yError("Could not open ");
            return false;
        }

        return true;
    }

    void close()
    {
        server_port.close();
    }

    std::string request_id(const std::string& preferred_id) override
    {
        std::lock_guard<std::mutex> lock(mutex);
        for (size_t i = 0; ; ++i) {
            std::string id = preferred_id + std::to_string(i);
            if (ids.find(id) == ids.end()) {
                // FIXME Query yarp server to ensure that /<id>/BT_rpc/server
                //       is not taken
                ids.insert(id);
                return id;
            }
        }
    }

    void release_id(const std::string& id) override
    {
        std::lock_guard<std::mutex> lock(mutex);
        ids.erase(id);
    }

private:
    yarp::os::RpcServer server_port;
    std::mutex mutex;
    std::set<std::string> ids;
};

int main()
{
    yarp::os::Network yarp;

    IdComponent idComponent;
    if (!idComponent.open()) {
        return 1;
    }

    while (true) {
        yInfo("Server running happily");
        yarp::os::Time::delay(10);
    }

    idComponent.close();

    return 0;
}
