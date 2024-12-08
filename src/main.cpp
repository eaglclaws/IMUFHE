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
        //std::cout << squareX << std::endl;
        if (count == x) {
            break;
        }
        count++;
        ret.push_back(vec);
    }
    return ret;
}

std::vector<double> getErrorRate(std::vector<std::vector<double>> test, std::vector<std::vector<double>> truth)
{
    double xerr = 0, yerr = 0;
    for (auto i = test.begin(), j = truth.begin(); i != test.end() && j != truth.end(); i++, j++) {
        double temp_xerr = (i->at(0) - j->at(0)) / j->at(0);
        double temp_yerr = (i->at(1) - j->at(1)) / j->at(1);
	xerr += temp_xerr;
	yerr += temp_yerr;
    }
    return std::vector<double>{xerr / test.size(), yerr / test.size()};
}

int main(int argc, char *agrv[])
{
    std::cout << GetOPENFHEVersion() << std::endl;
    auto cc = CreateCryptoContext();
    auto keyPair = GenerateKeys(cc);
    std::vector<std::vector<double>> accel, displacement, truthAccel;
    std::vector<double> omega;
    std::vector<lbcrypto::Ciphertext<lbcrypto::DCRTPoly>> ctxOmega, ctxDisplacement, ctxTheta;
    std::vector<std::vector<lbcrypto::Ciphertext<lbcrypto::DCRTPoly>>> ctxAccel, fixedAccel;
    std::cout << "Loading vectors...";
    prepareData(accel, omega, displacement, truthAccel);
    std::cout << std::endl << "Loaded vectors! accel size : " << accel.size() << " omega size : " << omega.size() << std::endl;
    auto it_accel = accel.begin();
    auto it_omega = omega.begin();
    //int count = 0;
    std::cout << "Encrypting values..." << std::endl;
    for (; it_accel != accel.end() && it_omega != omega.end(); it_accel++, it_omega++) {
	//std::cout << ++count << "/1000" << std::endl;
        std::vector<lbcrypto::Ciphertext<lbcrypto::DCRTPoly>> cAccel = {
	    EncryptDouble(cc, keyPair, it_accel->at(0)), EncryptDouble(cc, keyPair, it_accel->at(1))
	};
	auto cOmega = EncryptDouble(cc, keyPair, *it_omega);
	ctxAccel.push_back(cAccel);
	ctxOmega.push_back(cOmega);
    }
    //count = 0;
    std::cout <<  "Obtaining thetas..." << std::endl;
    lbcrypto::Ciphertext<lbcrypto::DCRTPoly> ctxThetaSum;
    int scount = 0;
    for (auto i = ctxOmega.begin(); i != ctxOmega.end(); i++) {
	//std::cout << ++count << "/1000" << std::endl;
        if (scount == 0) {
            ctxThetaSum = calcTheta(cc, 0.01, *i);
	} else {
            ctxThetaSum = cc->EvalAdd(ctxThetaSum, calcTheta(cc, 0.01, *i));
	}
	ctxTheta.push_back(ctxThetaSum);
	scount++;
    }
    auto it_ctxaccel = ctxAccel.begin();
    auto it_theta = ctxTheta.begin();
    //count = 0;
    std::cout << "Adjusting acceleration..." << std::endl;
    for (;it_ctxaccel != ctxAccel.end() && it_theta != ctxTheta.end(); it_ctxaccel++, it_theta++) {
	//std::cout << ++count << "/1000" << std::endl;
    	lbcrypto::Ciphertext<lbcrypto::DCRTPoly> ctxCos, ctxSin, ctxX = it_ctxaccel->at(0), ctxY = it_ctxaccel->at(1);
	//std::cout << "Run trig" << std::endl;
	calcTrigonWithTaylor(cc, *it_theta, ctxSin, ctxCos, keyPair);
	//std::cout << "Run transform" << std::endl;
	calcMatTransform(cc, ctxSin, ctxCos, ctxX, ctxY);
	fixedAccel.push_back(std::vector<lbcrypto::Ciphertext<lbcrypto::DCRTPoly>>{ctxX, ctxY});
    }
    //count = 0;
    std::cout << "Decrypting data..." << std::endl;
    std::vector<std::vector<double>> decryptedAccel;
    for (auto i = fixedAccel.begin(); i != fixedAccel.end(); i++) {
	//std::cout << ++count << "/1000" << std::endl;
	std::vector<lbcrypto::Plaintext> plaintext = {Decrypt(cc, keyPair, i->at(0)), Decrypt(cc, keyPair, i->at(1))};
	auto decoded = {Extract(plaintext.at(0)).at(0), Extract(plaintext.at(1)).at(0)};
	decryptedAccel.push_back(decoded);
    }
    std::cout << std::endl;
    auto errorRate = getErrorRate(truthAccel, decryptedAccel);
    std::cout << "x error : " << errorRate.at(0) << " | y error : " << errorRate.at(1) << std::endl;
    /*
    std::vector<double> data = {1.0, 1.0};
    auto ciphertext = EncryptVector(cc, keyPair, data);
    auto plaintext = Decrypt(cc, keyPair, ciphertext);
    std::vector<double> decoded = Extract(plaintext);
    */
    /*
    for (double value : decoded) {
        std::cout << value << std::endl;
    }
    std::vector<std::vector<double>> testData;
    for (int dt = 0; dt <= 100; dt++) {
        std::vector<double> point = {1, 1};
        testData.push_back(point);
        std::cout << std::to_string(testData) << std::endl;
    }
    std::vector<std::vector<double>> integral = riemannSum(testData, 10, 0.1);
    for (auto i = integral.begin(); i != integral.end(); i++) {
        std::cout << "(" << i->at(0) << " " << i->at(1) << ")" << std::endl;
    }
    */
    return 0;
}
