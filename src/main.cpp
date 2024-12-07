#include "main.hpp"
#include <vector>
#include <complex>

lbcrypto::Plaintext Decrypt(lbcrypto::CryptoContext<lbcrypto::DCRTPoly> &cc, lbcrypto::KeyPair<lbcrypto::DCRTPoly> &keyPair, const lbcrypto::Ciphertext<lbcrypto::DCRTPoly> &ciphertext)
{
    lbcrypto::Plaintext plaintext;
    cc->Decrypt(keyPair.secretKey, ciphertext, &plaintext);
    plaintext->SetLength(plaintext->GetLength());
    return plaintext;
}

std::vector<double> Extract(const lbcrypto::Plaintext &plaintext)
{
    const std::vector<std::complex<double>> complexValues = plaintext->GetCKKSPackedValue();

    std::vector<double> realValues;
    realValues.reserve(complexValues.size());
    for (size_t i = 0; i < complexValues.size(); i++) {
        realValues.push_back(complexValues.at(i).real());
    }

    return realValues;
}

int main(int argc, char *agrv[])
{
    std::cout << GetOPENFHEVersion() << std::endl;
    auto cc = CreateCryptoContext();
    auto keyPair = GenerateKeys(cc);
    std::vector<double> data = {1.0, 1.0};
    auto ciphertext = EncryptVector(cc, keyPair, data);
    auto plaintext = Decrypt(cc, keyPair, ciphertext);
    std::vector<double> decoded = Extract(plaintext);

    for (double value : decoded) {
        std::cout << value << std::endl;
    }
	  return 0;
}
