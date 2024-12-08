#ifndef TRANSFORMATION_H
#define TRANSFORMATION_H

#include "openfhe.h"

using namespace lbcrypto;

/*
  각도 Theta 계산
  Ciphertext 각속도 값 ctxW 에 timeDelta 값을 곱해서 각도를 얻는 func
  CryptoContext cc , timeDelta , 암호화된 각속도 벡터 필요
*/
Ciphertext<DCRTPoly> calcTheta(CryptoContext<DCRTPoly>& cc, double timeDelta, Ciphertext<DCRTPoly> ctxW);

/*
  삼각 함수 계산
  Taylor Series 를 통해 ctxTheta 에서 암호화된 Sin 값과 암호화된 Cos 값을 얻는 func
  CryptoContext cc , 암호화된 각도 값 , 암호를 생성할 수 있는 KeyPair , ctxSin 과 ctxCos 을 저장할 변수 필요
*/
void calcTrigonWithTaylor(CryptoContext<DCRTPoly>& cc, Ciphertext<DCRTPoly>& ctxTheta, Ciphertext<DCRTPoly>& ctxSin, Ciphertext<DCRTPoly>& ctxCos, KeyPair<DCRTPoly>& keyPair);

/*
  가속도 벡터 역행렬 변환
  r1 = (cos * ax) + (sin * ay) = 변환된 ax
  r2 = (-sin * ax) + (cos * ay) = 변환된 ay
  4개의 section 계산 -> ctxX 와 ctxY 에 변환된 가속도 벡터를 업데이트
  CryptoContext cc , 암호화된 sin 과 cos 값 , 암호화된 ax 와 aY 값 변수 필요
*/
void calcMatTransform(CryptoContext<DCRTPoly>& cc, Ciphertext<DCRTPoly>& ctxSin, Ciphertext<DCRTPoly>& ctxCos, Ciphertext<DCRTPoly>& ctxX, Ciphertext<DCRTPoly>& ctxY);

#endif
