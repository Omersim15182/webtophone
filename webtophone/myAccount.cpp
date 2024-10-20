#include "MyAccount.h"
#include <iostream>

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
