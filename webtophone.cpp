#include <iostream>
#include <pjsua2.hpp>
#include "myAccount.h"
#include "myCall.h"
#include <json/json.h>
#include <boost/asio.hpp>
#include "webSocketServer.h"

using namespace pj;

int main()
{
    Endpoint ep;

    ep.libCreate();

    // Initialize endpoint
    EpConfig ep_cfg;
    ep.libInit(ep_cfg);

    // Create SIP transport. Error handling sample is shown
    TransportConfig tcfg;
    tcfg.port = 5060;
    try {
        ep.transportCreate(PJSIP_TRANSPORT_UDP, tcfg);
    }
    catch (Error& err) {
        std::cout << err.info() << std::endl;
        return 1;
    }
    // Start the library (worker threads etc)
    ep.libStart();
    std::cout << "*** PJSUA2 STARTED ***" << std::endl;

    MyAccount myAccount;          // Ensure MyAccount is properly initialized

    // Configure an AccountConfig
    AccountConfig acfg;
    acfg.idUri = "sip:omer2002simhi@sip.linphone.org";
    acfg.regConfig.registrarUri = "sip:sip.linphone.org";
    AuthCredInfo cred("digest", "*", "omer2002simhi", 0, "omer2002");
    acfg.sipConfig.authCreds.push_back(cred);

    // Create the account
    MyAccount* acc = new MyAccount;
    acc->create(acfg);

    // WebSocket server setup
    boost::asio::io_context ioc;
    WebSocketServer server(ioc, 8080, *acc);

    // Run the io_context to keep the server alive
    ioc.run();  // Instead of ioc.poll()

    // While loop to ensure continuous execution for the SIP stack
    while (true)
    {
        pj_thread_sleep(1000);
    }

    // Cleanup when the server is terminated
    return 0;
}
