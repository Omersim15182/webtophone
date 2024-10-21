#ifndef MY_ACCOUNT_H
#define MY_ACCOUNT_H

#include "MyCall.h"
#include <iostream>
#include <pjsua2.hpp>
#include <pjsip.h>
#include <map>
#include <string>

using namespace pj;

class MyAccount : public Account {
public:
    virtual void onRegState(OnRegStateParam& prm) override;
    virtual void onIncomingCall(OnIncomingCallParam& iprm) override;


    void addAccount(const std::string& phoneNumber, const AccountConfig& acfg);
    MyAccount* getAccountByPhoneNumber(const std::string& phoneNumber);


private:
    std::map<std::string, MyAccount*> accounts_;
};

#endif // MY_ACCOUNT_H
