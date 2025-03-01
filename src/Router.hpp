#ifndef CPPHV_ROUTE_HPP_
#define CPPHV_ROUTE_HPP_

#include <string>
#include <CJsonObject.hpp>

class Router {
    public:
        static neb::CJsonObject getJson(std::string clientPeer)
        {
            neb::CJsonObject json;
            json.Add("client", clientPeer);
            return json;
        }

        static neb::CJsonObject getJson(std::string clientPeer, std::string linkPeer)
        {
            neb::CJsonObject json;
            json.Add("client", clientPeer);
            json.Add("link", linkPeer);
            return json;
        }

        static std::string getClientPeer(neb::CJsonObject json)
        {
            std::string clientPeer;
            if (json.Get("client", clientPeer))
            {
                return clientPeer;
            }
            return "";
        }

        static std::string getLinkPeer(neb::CJsonObject json)
        {
            std::string linkPeer;
            if (json.Get("link", linkPeer))
            {
                return linkPeer;
            }
            return "";
        }
};

#endif