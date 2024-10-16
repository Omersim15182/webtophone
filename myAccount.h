#ifndef MY_ACCOUNT_H
#define MY_ACCOUNT_H

#include "MyCall.h"
#include <iostream>
#include <pjsua2.hpp>
#include <pjsip.h>

using namespace pj;

class MyAccount : public Account {
public:
    virtual void onRegState(OnRegStateParam& prm) override {
        AccountInfo ai = getInfo();
        std::cout << (ai.regIsActive ? "*** Register:" : "*** Unregister:")
            << " code=" << prm.code << std::endl;
    }

    virtual void onIncomingCall(OnIncomingCallParam& iprm) override {
        MyCall* call = new MyCall(*this, iprm.callId);
        std::cout << "Incoming call from: " << iprm.callId << std::endl;

        // Answer the incoming call
        /*CallOpParam prm;
        prm.statusCode = PJSIP_SC_OK;
        call->answer(prm);
        std::cout << "Incoming call answered." << std::endl;*/
    }
};

#endif // MY_ACCOUNT_H
