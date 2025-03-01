#ifndef CPPHV_GATESERVER_HPP_
#define CPPHV_GATESERVER_HPP_

#include <INetServer.hpp>
#include <INetTransfer.hpp>
#include <Router.hpp>

namespace cpphv {
    class GateServer{
        public:
            void init()
            {
                m_iNetServer.init("0.0.0.0", 4567);
                m_iNetServer.m_transferGateServer2linkServer = std::bind(&GateServer::transferGateServer2LinekServer, this, std::placeholders::_1, std::placeholders::_2);
                m_iNetServer.m_transferLinkServer2GateServer = std::bind(&GateServer::transferGateServer2LinekServer, this, std::placeholders::_1, std::placeholders::_2);
            }

            void run()
            {
                m_iNetServer.run();
            }

            void stop()
            {
                m_iNetServer.stop();
            }

            void transferGateServer2BusinessServer(const WebSocketChannelPtr& channel, neb::CJsonObject& msg)
            {
                std::string cmd;
                if(!msg.Get("cmd", cmd))
                {
                    return;
                }

                neb::CJsonObject routeJson;
                if (!msg.Get("route", routeJson))
                {
                    return;
                }

                std::string clientPeer = Router::getClientPeer(routeJson);
                if (clientPeer.empty())
                {
                    return;
                }

                std::string linkPeer = channel->peeraddr();
                routeJson = Router::getJson(clientPeer, linkPeer);
                msg.Replace("route", routeJson);
                m_iNetTransfer.onNetMsgDo(cmd, msg);
            }

            void transferGateServer2LinekServer(const WebSocketChannelPtr& channel, neb::CJsonObject& msg)
            {
                neb::CJsonObject routeJson;
                if(!msg.Get("route", routeJson))
                {
                    return;
                }

                std::string linkPeer = Router::getLinkPeer(routeJson);
                routeJson.Delete("link");
                msg.Replace("route", routeJson);
                m_iNetServer.transfer(linkPeer, msg);
            }
        private:
            INetServer m_iNetServer;
            INetTransfer m_iNetTransfer;            
    };
}

#endif