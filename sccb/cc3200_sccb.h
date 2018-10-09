#ifndef _CC3200_SCCB_H_
#define _CC3200_SCCB_H_

#include typedef.h



extern void SCCB_Init(void);
extern unsigned char SCCB_ReadReg(unsigned char ucReg)
extern void SCCB_WriteReg(unsigned char ucReg,unsigned char ucData)


#endif
