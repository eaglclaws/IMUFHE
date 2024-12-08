#ifndef MAIN_H
#define MAIN_H
#include "openfhe.h"
#include "parseCSV.hpp"
#include "transformation.hpp"
#include <iostream>
//헤더 통합용으로 사용할 예정
lbcrypto::CryptoContext<lbcrypto::DCRTPoly> CreateCryptoContext()
{
    lbcrypto::CCParams<lbcrypto::CryptoContextCKKSRNS> params;
    params.SetMultiplicativeDepth(6);
    params.SetBatchSize(4);
    params.SetScalingModSize(20);
    params.SetScalingTechnique(lbcrypto::FLEXIBLEAUTO);
    auto cc = lbcrypto::GenCryptoContext(params);
    cc->Enable(lbcrypto::PKE | lbcrypto::KEYSWITCH | lbcrypto::LEVELEDSHE);
    return cc;
}

lbcrypto::KeyPair<lbcrypto::DCRTPoly> GenerateKeys(lbcrypto::CryptoContext<lbcrypto::DCRTPoly> &cc)
{
    auto keyPair = cc->KeyGen();
    cc->EvalMultKeyGen(keyPair.secretKey);
    return keyPair;
}

lbcrypto::Ciphertext<lbcrypto::DCRTPoly> EncryptVector(lbcrypto::CryptoContext<lbcrypto::DCRTPoly> &cc, lbcrypto::KeyPair<lbcrypto::DCRTPoly> &keyPair, const std::vector<double> &input)
{
    auto plaintext = cc->MakeCKKSPackedPlaintext(input);
    return cc->Encrypt(keyPair.publicKey, plaintext);
}

lbcrypto::Ciphertext<lbcrypto::DCRTPoly> EncryptDouble(lbcrypto::CryptoContext<lbcrypto::DCRTPoly> &cc, lbcrypto::KeyPair<lbcrypto::DCRTPoly> &keyPair, const double &input)
{
    auto plaintext = cc->MakeCKKSPackedPlaintext(std::vector<double>{input});
    return cc->Encrypt(keyPair.publicKey, plaintext);
}
#endif
