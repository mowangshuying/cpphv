#ifndef CPPHV_MSG_HPP_
#define CPPHV_MSG_HPP_

#include <string>

namespace cpphv {
    namespace msgType {
        const std::string request =  "request";
        const std::string response = "response";
        const std::string push =     "push";
        const std::string broadcast = "broadcast";
        const std::string pushS =     "pushS";
    }

    namespace msgState {
        const std::string ok = "ok";
        const std::string error = "error";
        const std::string timeout = "timeout";
        const std::string notFound = "notFound";
        const std::string serverBusy = "serverBusy";
        const std::string serverOffline = "serverOffline";
    }

}


#endif