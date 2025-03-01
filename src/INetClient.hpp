#ifndef CPPHV_INETCLIENT_HPP_
#define CPPHV_INETCLIENT_HPP_

#include <WebSocketClient.h>
#include <CJsonObject.hpp>
#include <map>
#include <Msg.hpp>
#include <hbase.h>
#include <htime.h>
#include <list>
#include <hlog.h>
#include <Msg.hpp>

using namespace hv;
namespace cpphv {
    class INetClient {
    private:
        using NetEventCall = std::function<void(neb::CJsonObject&)>;
        class NetEventCaller {
        public:
            NetEventCaller(NetEventCall call, std::string rand, std::string cmd) 
                : call(call), m_sendTime(gettimeofday_ms()), m_rand(rand), m_cmd(cmd) 
            {

            }
        public:
            NetEventCall call;
            time_t m_sendTime;
            std::string m_rand;
            std::string m_cmd; 
        };
    public:
        INetClient()
        {

        }

        void open(std::string linkName, std::string url)
        {
            m_linkName = linkName;
            m_openUrl = url;
            m_wsClient.onopen =  std::bind(&INetClient::onOpen, this);
            m_wsClient.onmessage = std::bind(&INetClient::onMessage, this, std::placeholders::_1);
            m_wsClient.onclose = std::bind(&INetClient::onClose, this);
            m_wsClient.open(m_openUrl.c_str());
        }

        void onOpen()
        {
            neb::CJsonObject json;
            json.Add("linkName", m_linkName);
            json.Add("url", m_openUrl);
            json.Add("rand", "");
            onMsgDo("open", json);
        }

        void onMessage(const std::string& msg)
        {
            neb::CJsonObject json;
            if (json.Parse(msg))
            {
                hloge("json.Parse error:%s", json.GetErrMsg().c_str());
                return;
            }

            std::string msgType;
            if (!json.Get("msgType", msgType))
            {
                hloge("json.Get msgType error.");
                return;
            }


            if (cpphv::msgType::request == msgType)
            {
                std::string cmd;
                if (!json.Get("cmd", cmd))
                {
                    hloge("json.Get cmd error.");
                    return;
                }

                onMsgDo(cmd, json);
                return;
            }

            if (cpphv::msgType::response == msgType)
            {
                std::string cmd;
                if (!json.Get("cmd", cmd))
                {
                    hloge("json.Get cmd error.");
                    return;
                }

                onMsgDo(cmd, json);
                return;
            }
        }

        void onClose()
        {

        }



        std::string getRandString()
        {
            char rands[10];
            hv_random_string(rands, sizeof(rands));
            return std::string(rands);
        }

        void setLinkName(std::string linkName)
        {
            m_linkName = linkName;
        }

        std::string getLinkName()
        {
            return m_linkName;
        }

        void setOpenUrl(std::string url)
        {
            m_openUrl = url;
        }

        std::string getOpenUrl()
        {
            return m_openUrl;
        }

        void regMsgCall(std::string cmd, NetEventCall call)
        {
            NetEventCaller caller(call, "",  cmd);
            m_netEventCallers.push_back(caller);
        }

        void onMsgDo(const std::string &cmd, neb::CJsonObject& msg)
        {
            // get rand;
            std::string rand;
            msg.Get("rand", rand);
            for (auto iter = m_netEventCallers.begin(); iter != m_netEventCallers.end(); iter++)
            {
                if (iter->m_cmd == cmd && iter->m_rand == rand)
                {
                    iter->call(msg);
                    if(!iter->m_rand.empty())
                        m_netEventCallers.erase(iter);
                    break;
                }
            }
        }

        int transfer(neb::CJsonObject& msg)
        {
            std::string msgStr = msg.ToString();
            return m_wsClient.send(msgStr);
        }

        template<typename T>
        int request(const std::string& cmd, const T& data)
        {
            neb::CJsonObject msg;
            msg.Add("cmd", cmd);
            msg.Add("msgType",  cpphv::msgType::request);
            
            // rand
            //char* rand[10];
            //hv_rand(rand, sizeof(rand));
            msg.Add("rand", "");
            msg.Add("timestamp",  (uint64)gettimeofday_ms());
            msg.Add("data", data);

            return transfer(msg);
        }

        template<typename T>
        int request(const std::string&cmd, const T& data, NetEventCall call)
        {
            neb::CJsonObject msg;
            msg.Add("cmd", cmd);
            msg.Add("msgType",  cpphv::msgType::request);

            std::string randString = getRandString();
            msg.Add("rand", randString);
            msg.Add("timestamp",  (uint64)gettimeofday_ms());
            msg.Add("data", data);
            int nRet =  transfer(msg);
            if (nRet > 0)
            {
                NetEventCaller caller(call, randString, cmd);
                m_netEventCallers.push_back(caller);
            }
        }

        template<typename T>
        int response(const std::string&cmd, const std::string& rand, const T&data, std::string state)
        {
            neb::CJsonObject msg;
            msg.Add("cmd", cmd);
            msg.Add("msgType",  cpphv::msgType::response);
            msg.Add("rand", rand);
            msg.Add("timestamp",  (uint64)gettimeofday_ms());
            msg.Add("data", data);
            msg.Add("state", state);
            return transfer(msg);
        }

        template<typename T>
        int push(const std::string& client, const std::string& cmd, const T& data)
        {
            neb::CJsonObject msg;
            msg.Add("Route", "");
            msg.Add("cmd", cmd);
            msg.Add("msgType",  cpphv::msgType::push);
            msg.Add("timestamp",  (uint64)gettimeofday_ms());
            msg.Add("data", data);
            return transfer(msg);
        }

        template<typename T>
        int broadcast(const std::string&cmd, const T& data)
        {
            neb::CJsonObject msg;
            msg.Add("cmd", cmd);
            msg.Add("msgType",  cpphv::msgType::broadcast);
            msg.Add("rand", "");
            msg.Add("timestamp",  (uint64)gettimeofday_ms());
            msg.Add("data", data);
            return transfer(msg);
        }


    private:
        std::list<NetEventCaller> m_netEventCallers;
    private:
        std::string m_linkName;
        std::string m_openUrl;
        WebSocketClient m_wsClient;
    };
}

#endif