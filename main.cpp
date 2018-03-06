/*
 **************************************************************************************
 *       Filename:  main.cpp
 *    Description:   source file
 *
 *        Version:  1.0
 *        Created:  2018-03-06 16:01:52
 *
 *       Revision:  initial draft;
 **************************************************************************************
 */

#include "webrtc/base/helpers.h"
#include "webrtc/base/opensslidentity.h"
#include "webrtc/base/sslfingerprint.h"
#include <iostream>
using namespace std;
using namespace rtc;

void demo_identity() {
    string comname = "tal-ares";
    OpenSSLIdentity* identity = (OpenSSLIdentity*)OpenSSLIdentity::GenerateWithExpiration(comname, KeyParams::RSA(2048), 10000);
    if(!identity) {
        cout << "ERROR" << endl;
        return;
    }
    const OpenSSLCertificate& cert = identity->certificate();
    cout << cert.ToPEMString() << endl;

    cout << identity->PrivateKeyToPEMString() << endl;
    SSLFingerprint* fp = SSLFingerprint::Create(string("sha-256"), identity);
    if(!fp) {
        cout << "fail to create fp" << endl;
        return;
    }
    cout << fp->ToString() << endl;
    delete fp;
    delete identity;
    cout << "END" << endl;
}

int main()
{
    demo_identity();
    return 0;
}



/********************************** END **********************************************/

