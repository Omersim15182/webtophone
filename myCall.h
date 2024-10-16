#ifndef MY_CALL_H
#define MY_CALL_H
#include <iostream>
#include <pjsua2.hpp>
using namespace pj;



class MyCall : public Call {
public:
    MyCall(Account& acc, int call_id = PJSUA_INVALID_ID)
        : Call(acc, call_id) { }

    // Info of the call status
    virtual void onCallMediaState(OnCallMediaStateParam& prm) override {
        CallInfo info = getInfo();
        std::cout << "======= CallState is: " << info.stateText << " =======" << std::endl;

        for (size_t i = 0; i < info.media.size(); ++i) {
            CallMediaInfo media = info.media[i];
            std::cout << "Media stream " << i << " status: ";

            // Check if the media is active
            if (media.status == PJSUA_CALL_MEDIA_ACTIVE) {
                std::cout << "Media is active." << std::endl;
                // Get the media port
                AudioMedia* aud_med = (AudioMedia*)getMedia(i);
                AudDevManager& mgr = Endpoint::instance().audDevManager();
                // Connect the call audio media to the sound device
                aud_med->startTransmit(mgr.getPlaybackDevMedia());
                mgr.getCaptureDevMedia().startTransmit(*aud_med);
                
            }
            else {
                std::cout << "Media is inactive." << std::endl;
            }
        }
        
    
    }
};

#endif // MY_CALL_H
