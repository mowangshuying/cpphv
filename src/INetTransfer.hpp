#ifndef CPPHV_INETTRANSFER_HPP_
#define CPPHV_INETTRANSFER_HPP_

#include <list>
#include <map>
#include <vector>

#include <WebSocketChannel.h>
#include <CJsonObject.hpp>

namespace cpphv {
    class Listener {
        public:
            Listener()
            {
                m_curIndex = 0;
            }
        public:
            void add(WebSocketChannelPtr channel)
            {
                bool bfind = false;
                for (auto iter = m_listeners.begin(); iter != m_listeners.end(); iter++)
                {
                    if ((*iter)->peeraddr() == channel->peeraddr())
                    {
                        bfind = true;
                    }
                }

                if(!bfind)
                {
                    m_listeners.push_back(channel);
                }
            }

            void del(WebSocketChannelPtr channel)
            {
                for (auto iter = m_listeners.begin(); iter != m_listeners.end(); iter++)
                {
                    if ((*iter)->peeraddr() == channel->peeraddr())
                    {
                        m_listeners.erase(iter);
                        break;
                    }
                }
            }

            WebSocketChannelPtr get()
            {
                auto size = m_listeners.size();
                if (size == 0)
                    return nullptr;
                
                if (size == 1)
                    m_listeners[0];

                if (m_curIndex >= size)
                    m_curIndex = 0;

                return m_listeners[m_curIndex++];
            }


        private:
            std::vector<WebSocketChannelPtr> m_listeners;
            int m_curIndex;
    };

    class INetTransfer {
        public:
            void add(std::string cmd, WebSocketChannelPtr channel)
            {
                auto itf = m_listenerMap.find(cmd);
                if (itf != m_listenerMap.end())
                {
                    itf->second.add(channel);
                }
                else
                {
                    Listener listener;
                    listener.add(channel);
                    m_listenerMap[cmd] = listener;
                }
            }

            void del(WebSocketChannelPtr channel)
            {
                for(auto iter = m_listenerMap.begin(); iter != m_listenerMap.end(); iter++)
                {
                    iter->second.del(channel);
                }
            }

            void del(std::string cmd, WebSocketChannelPtr channel)
            {
                auto itf = m_listenerMap.find(cmd);
                if(itf != m_listenerMap.end())
                {
                    itf->second.del(channel);
                }
            }

            void onNetMsgDo(std::string cmd, neb::CJsonObject & msg)
            {
                auto itf = m_listenerMap.find(cmd);
                if(itf == m_listenerMap.end())
                {
                    return;
                }

                auto channel = itf->second.get();
                if (channel)
                {
                    channel->send(msg.ToString());
                }

            }
        private:
            std::map<std::string, Listener> m_listenerMap;
    };
}

#endif