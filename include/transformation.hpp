#ifndef TRANSFORMATION_H
#define TRANSFORMATION_H

Ciphertext<?> calcTheta(CryptoContext<?>& cc, double timeDelta, Ciphertext<?> ctxW);
void calcTrigonWithTaylor(CryptoContext<?>& cc, Ciphertext<?>& ctxTheta, Ciphertext<?>& ctxSin, Ciphertext<?>& ctxCos);
void calcMatTransform(CryptoContext<?>& cc, Ciphertext<?>& ctxSin, Ciphertext<?>& ctxCos, Ciphertext<?>& ctxX, Ciphertext<?>& ctxY);

#endif
