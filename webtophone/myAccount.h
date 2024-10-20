#ifndef MY_ACCOUNT_H
#define MY_ACCOUNT_H

#include "MyCall.h"
#include <iostream>
#include <pjsua2.hpp>
#include <pjsip.h>

using namespace pj;

class MyAccount : public Account {
public:
    virtual void onRegState(OnRegStateParam& prm) override;
    virtual void onIncomingCall(OnIncomingCallParam& iprm) override;
};

#endif // MY_ACCOUNT_H
