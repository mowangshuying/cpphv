#ifndef CPPHV_LINKSERVER_HPP_
#define CPPHV_LINKSERVER_HPP_

#include <INetServer.hpp>
#include <INetClient.hpp>
#include <Router.hpp>
namespace cpphv {
    class LinkServer{
        public:
            LinkServer()
            {

            }

            void init()
            {
                // connet to gateserver;
                m_ssGate.open("linkServer", "ws://127.0.0.1:4567");

                // init netserver;
                m_netServer.init("0.0.0.0", 5000);

                // bind func;
                m_netServer.m_transferLinkServer2GateServer = std::bind(&LinkServer::transferLinkServer2GateServer, this, std::placeholders::_1, std::placeholders::_2);
                m_netServer.m_transferLinkServer2Client = std::bind(&LinkServer::transerLinkServer2Client, this, std::placeholders::_1, std::placeholders::_2);
            }

            void run()
            {
                m_netServer.run();
            }

            void stop()
            {
                m_netServer.stop();
            }
            void transferLinkServer2GateServer(const WebSocketChannelPtr& channel, neb::CJsonObject& msg)
            {
                // add linkPeer;
                // add route;
                neb::CJsonObject routeJson =  Router::getJson(channel->peeraddr());
                msg.Add("route", routeJson);

                // send to gateServer;
                m_ssGate.transfer(msg);
            }

            void transerLinkServer2Client(const WebSocketChannelPtr& channel, neb::CJsonObject& msg)
            {
                neb::CJsonObject routeJson;
                if (!msg.Get("route", routeJson))
                {
                    return;
                }

                std::string clientPeer = Router::getClientPeer(routeJson);
                msg.Delete("route");
                m_netServer.transfer(clientPeer, msg);
            }
        private:
            INetServer m_netServer;
            INetClient m_ssGate;
    };
}

#endif