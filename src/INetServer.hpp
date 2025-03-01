#ifndef CPPHV_INETSERVER_HPP_
#define CPPHV_INETSERVER_HPP_

#include <string>
#include <CJsonObject.hpp>
#include <functional>
#include <WebSocketServer.h>
#include <WebSocketChannel.h>
#include <hlog.h>
#include <Msg.hpp>
#include <Router.hpp>

using namespace hv;

namespace cpphv {
    class INetServer {
        private:
            using NetEventCall = std::function<void(const WebSocketChannelPtr&, neb::CJsonObject&)>;
            using TransferCall = std::function<void(const WebSocketChannelPtr&, neb::CJsonObject&)>;
        public:
            INetServer() 
            {
                m_transferLinkServer2GateServer = nullptr;
                //m_transferBusinessServer2GateServer = nullptr;
                m_transferGateServer2BusinessServer = nullptr;
                m_transferGateServer2linkServer = nullptr;
                m_transferLinkServer2Client = nullptr;
            }
            void init(std::string strIp, int nPort)
            {
                m_wsServer.setHost(strIp.c_str());
                m_wsServer.setPort(nPort);

                // bind func;
                //HttpService hs;
                hs.GET("/ping", [](const HttpContextPtr& ctx) {
                    return ctx->send("pong");
                });

                //WebSocketService ws;
                ws.onopen = std::bind(&INetServer::onOpen, this, std::placeholders::_1, std::placeholders::_2);
                ws.onmessage = std::bind(&INetServer::onMessage, this, std::placeholders::_1, std::placeholders::_2);
                ws.onclose = std::bind(&INetServer::onClose, this, std::placeholders::_1);

                m_wsServer.registerHttpService(&hs);
                m_wsServer.registerWebSocketService(&ws);
            }

            void run()
            {
                m_wsServer.start();
            }

            void stop()
            {
                m_wsServer.stop();
            }
            void onOpen(const WebSocketChannelPtr& channel, const HttpRequestPtr& req)
            {
                std::string peeraddr = channel->peeraddr();
                m_wsChannelMap[peeraddr] = channel;
            }

            void onMessage(const WebSocketChannelPtr& channel, const std::string msg)
            {
                neb::CJsonObject json;
                if (!json.Parse(msg))
                {
                    hloge("json.Parse error:%s.", json.GetErrMsg().c_str());
                    return;
                }

                std::string msgType;
                if (!json.Get("msgType", msgType)) {
                    hloge("json.Get msgType error.");
                    return;
                }

                neb::CJsonObject routeJson;
                if (!json.Get("route", routeJson)) {
                    hloge("json.Get route error=%s.", json.GetErrMsg().c_str());
                    return;
                }

                // businessserver -> gateserver -> linkserver -> client
                if (msgType == cpphv::msgType::response)
                {

                    std::string linkPeer = Router::getLinkPeer(routeJson);
                    if (!linkPeer.empty())
                    {
                        // find linkserver
                        // del link peer
                        if (m_transferGateServer2linkServer)
                            m_transferGateServer2linkServer(channel, json);
                        return;
                    }

                    std::string clientPeer = Router::getClientPeer(routeJson);
                    if(!clientPeer.empty())
                    {
                        // find client;
                        // del client peer;
                        if (m_transferLinkServer2Client)
                            m_transferLinkServer2Client(channel, json);
                        return;
                    }
                }

                // client - > linkserver -> gateserver -> businessserver
                if (msgType == cpphv::msgType::request)
                {
                    // get cmd;
                    std::string cmd;
                    if (!json.Get("cmd", cmd))
                    {
                        hloge("json.Get cmd error.");
                        return;
                    }

                    // do it at local;
                    if (onNetMsgDo(channel, cmd, json))
                    {
                        return;
                    }

                    // transfer 
                    // msg from client
                    std::string clientPeer = Router::getClientPeer(routeJson);
                    std::string linkPeer = Router::getLinkPeer(routeJson);
                    if (clientPeer.empty() && linkPeer.empty())
                    {
                        // add clientPeer;
                        // send to gateServer;
                        if (m_transferLinkServer2GateServer)
                            m_transferLinkServer2GateServer(channel, json);
                        return;
                    }

                    if (!clientPeer.empty() && linkPeer.empty())
                    {
                        // add linkPeer;
                        // send to businessServer;
                        if(m_transferGateServer2BusinessServer)
                            m_transferGateServer2BusinessServer(channel, json);
                        return;
                    }    
                }


            }   

            void onClose(const WebSocketChannelPtr& channel)
            {
                auto itf = m_wsChannelMap.find(channel->peeraddr());
                if (itf != m_wsChannelMap.end())
                {
                    m_wsChannelMap.erase(itf);
                }
            }

            WebSocketChannelPtr getChannel(std::string peeraddr)
            {
                auto itf = m_wsChannelMap.find(peeraddr);
                if (itf != m_wsChannelMap.end())
                {
                    return itf->second;
                }
                return nullptr;
            }

            int transfer(std::string peeraddr, neb::CJsonObject& msg)
            {
                auto channel = getChannel(peeraddr);
                if (channel == nullptr)
                    return -1;
                return channel->send(msg.ToString());
            }
            void regMsgCall(std::string cmd, NetEventCall call)
            {
                if(call != nullptr)
                    return;
                m_msgCallMap[cmd] = call;
            }

            bool onNetMsgDo(const WebSocketChannelPtr& channel,const std::string& cmd, neb::CJsonObject& msg)
            {
                auto itf = m_msgCallMap.find(cmd);
                if (itf != m_msgCallMap.end())
                {
                   itf->second(channel, msg);
                   return true;
                }
                return false;
            }      
        public:
            TransferCall m_transferLinkServer2GateServer;
            //TransferCall m_transferBusinessServer2GateServer;
            TransferCall m_transferGateServer2BusinessServer;
            TransferCall m_transferGateServer2linkServer;
            TransferCall m_transferLinkServer2Client;
        private:
            std::map<std::string, WebSocketChannelPtr> m_wsChannelMap; 
            std::map<std::string, NetEventCall> m_msgCallMap;
            HttpService hs;
            WebSocketService ws;
            WebSocketServer m_wsServer;
    };
}

#endif