#include "MyCall.h"
#include <iostream>

using namespace std;

MyCall::MyCall(Account& acc, int call_id)
    : Call(acc, call_id) {}

void MyCall::onCallMediaState(OnCallMediaStateParam& prm) {
    CallInfo info = getInfo();
    std::cout << "======= CallState is: " << info.stateText << " =======" << std::endl;

    for (size_t i = 0; i < info.media.size(); ++i) {
        CallMediaInfo media = info.media[i];
        std::cout << "Media stream " << i << " status: ";

        if (media.status == PJSUA_CALL_MEDIA_ACTIVE) {
            std::cout << "Media is active." << std::endl;
            AudioMedia* aud_med = (AudioMedia*)getMedia(i);
            AudDevManager& mgr = Endpoint::instance().audDevManager();
            aud_med->startTransmit(mgr.getPlaybackDevMedia());
            mgr.getCaptureDevMedia().startTransmit(*aud_med);
        }
        else {
            std::cout << "Media is inactive." << std::endl;
        }
    }
}
