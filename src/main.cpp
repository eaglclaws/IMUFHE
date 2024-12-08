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

std::vector<std::vector<double>> riemannSum(std::vector<std::vector<double>> data, int x, double deltaT)
{
    std::vector<std::vector<double>> ret;
    int count = 0;
    for (auto i = data.begin(); i != data.end(); i++) {
        double squareX = deltaT * i->at(0);
        double squareY = deltaT * i->at(1);
        if (count > 0) {
            squareX += data.at(count - 1).at(0);
            squareY += data.at(count - 1).at(1);
        }
        std::vector<double> vec = {squareX, squareY};
        std::cout << squareX << std::endl;
        if (count == x) {
            break;
        }
        count++;
        ret.push_back(vec);
    }
    return ret;
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
    /*
    for (double value : decoded) {
        std::cout << value << std::endl;
    }
    */
    std::vector<std::vector<double>> testData;
    for (int dt = 0; dt <= 100; dt++) {
        std::vector<double> point = {1, 1};
        testData.push_back(point);
        std::cout << std::tostring(testData) << std::endl;
    }
    std::vector<std::vector<double>> integral = riemannSum(testData, 10, 0.1);
    for (auto i = integral.begin(); i != integral.end(); i++) {
        std::cout << "(" << i->at(0) << " " << i->at(1) << ")" << std::endl;
    }
	  return 0;
}
