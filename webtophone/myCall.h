#ifndef MY_CALL_H
#define MY_CALL_H

#include <iostream>
#include <pjsua2.hpp>
using namespace pj;

class MyCall : public Call {
public:
    MyCall(Account& acc, int call_id = PJSUA_INVALID_ID);
    virtual void onCallMediaState(OnCallMediaStateParam& prm) override;
};

#endif // MY_CALL_H
