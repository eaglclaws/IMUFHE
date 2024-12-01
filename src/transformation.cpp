#include "imu_transformation.h"
#include "openfhe.h"
#include <omp.h>

using namespace lbcrypto;

// Crypto parameter 설정
// CCParams<CryptoContextCKKSRNS> parameters;
// parameter 추가

// CryptoContext 생성
// CryptoContext<DCRTPoly> cc = GenCryptoContext(parameters);
// Context 허용 설정 추가

// 키 생성
// auto keyPair = cc->KeyGen();
// cc->EvalMultKeyGen(keyPair.secretKey);  // MultLevel 대체 키 생성

// 평문 생성
// Plaintext ptx = cc->MakeCKKSPackedPlaintext(vector<double>(...));
// 암호문 생성
// Ciphertext<DCRTPoly> ctx = cc->Encrypt(keyPair.publicKey, ptx);

// 각도 Theta 계산
Ciphertext<?> calcTheta(CryptoContext<?>& cc, double timeDelta, Ciphertext<?> ctxW) {
  // Ciphertext - Plaintext multiplication
  auto ctxTheta = cc->EvalMult(ctxW, timeDelta);
  return ctxTheta;
}

// 삼각 함수 계산
// 각도 : radian 단위 -> radian = 각도 * (pi/180)
void calcTrigonWithTaylor(CryptoContext<?>& cc, Ciphertext<?>& ctxTheta, Ciphertext<?>& ctxSin, Ciphertext<?>& ctxCos) {
  // factorial number
  int ft = 1;

  // sin cos 초기화
  ctxSin = cc->Encrypt(keyPair.publicKey, cc->MakeCKKSPackedPlaintext(std::vector<double>{1}));
  ctxCos = ctxTheta;
  auto ctxOrigin = ctxTheta->Clone();

  // sin + cos 8 항
  for (int i = 2; i < 8; i++) {
    ft *= i;

    // 상수 상태 역수 계산
    double temp = 1 / (double) ft;

    // Theta * Theta
    ctxTheta = cc->EvalMultAndRelinearize(ctxTheta, ctxOrigin);
    auto ctxTemp = cc->EvalMult(ctxTheta, temp);

    if (i == 2) {
      ctxCos = cc->EvalSub(ctxCos, ctxTemp);
    } else if (i == 3) {
      ctxSin = cc->EvalSub(ctxSin, ctxTemp);
    } else if (i == 4) {
      ctxCos = cc->EvalAdd(ctxCos, ctxTemp);
    } else if (i == 5) {
      ctxSin = cc->EvalAdd(ctxSin, ctxTemp);
    } else if (i == 6) {
      ctxCos = cc->EvalSub(ctxCos, ctxTemp);
    } else if (i == 7) {
      ctxSin = cc->EvalSub(ctxSin, ctxTemp);
    }
  }
}

// 가속도 벡터 역행렬 변환
// r1 = (cos * ax) + (sin * ay)
// r2 = (-sin * ax) + (cos * ay)
// 4 section 독립적으로 계산
void calcMatTransform(CryptoContext<?>& cc, Ciphertext<?>& ctxSin, Ciphertext<?>& ctxCos, Ciphertext<?>& ctxX, Ciphertext<?>& ctxY) {
  // Temp Variable
  Ciphertext<?> temp1, temp2, temp3, temp4;

  #pragma omp parallel sections
{
    #pragma omp section
    temp1 = cc->EvalMultAndRelinearize(ctxCos, ctxX);

    #pragma omp section
    temp2 = cc->EvalMultAndRelinearize(ctxSin, ctxY);

    #pragma omp section
    temp3 = cc->EvalMultAndRelinearize(ctxSin, ctxX);

    #pragma omp section
    temp4 = cc->EvalMultAndRelinearize(ctxCos, ctxY);
  }

  // ax Transformation
  ctxX = cc->EvalAnd(temp1, temp2);

  // ay Transformation
  auto negTemp3 = cc->EvalMult(temp3, -1);
  ctxY = cc->EvalAdd(negTemp3, temp4);
}

    
