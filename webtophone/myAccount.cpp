#include "MyAccount.h"
#include <iostream>
#include <string>
#include <map>

using namespace std;


void MyAccount::onRegState(OnRegStateParam& prm) {

    AccountInfo ai = getInfo();
    std::cout << (ai.regIsActive ? "*** Register:" : "*** Unregister:")
        << " code=" << prm.code << std::endl;
}

void MyAccount::onIncomingCall(OnIncomingCallParam& iprm) {

    MyCall* call = new MyCall(*this, iprm.callId);
    std::cout << "Incoming call from: " << iprm.callId << std::endl;
}

void MyAccount::addAccount(const std::string& phoneNumber, const AccountConfig& acfg) {

    MyAccount* account = new MyAccount();
    account->create(acfg); // Create the account 
    accounts_[phoneNumber] = account; // Store the account
    std::cout << "Account added for phone number: " << phoneNumber << std::endl;
}

MyAccount* MyAccount::getAccountByPhoneNumber(const std::string& phoneNumber) {

    auto it = accounts_.find(phoneNumber);
    if (it != accounts_.end()) {
        return it->second; 
    }
    return nullptr;
}
