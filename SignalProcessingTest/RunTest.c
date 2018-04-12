#include "SignalProcessing.h"
#define DATA_LENGTH 50000

int main()
{
	FILE *fileIn1 = NULL;
	FILE *fileIn2 = NULL;
	FILE *fileIn3 = NULL;
	FILE *fileOut = NULL;
	INPUT_TYPE rawEcgData[DATA_LENGTH] = { 0 };//����
	OUTPUT_TYPE filteredEcgData[DATA_LENGTH] = { 0 };//������

	LPF_INTFC fir1Coeff[(FIR_1ST_LENGTH >> 1) + 1] = { 0 };
	LPF_INTFC fir2Coeff[FIR_2ND_LENGTH >> 1] = { 0 };
	QUEUE_ARRAY* rrsBuf1 = NULL;
	QUEUE_ARRAY* rrsBuf2 = NULL;
	QUEUE_ARRAY* ifirBuf1 = NULL;
	QUEUE_ARRAY* ifirBuf2 = NULL;
	NODE_INDEX* nodeIdx = NULL;

	errno_t err;
	int i = 0;

	//�������ݺ��˲�ϵ��
	err = fopen_s(&fileIn1, "rawECG.dat", "rb");
	if (err == 0)
	{
		fread(rawEcgData, sizeof(INPUT_TYPE), DATA_LENGTH, fileIn1);
		fclose(fileIn1);
	}
	else
	{
		printf("Can't open raw file!\n");
		return -1;
	}
	err = fopen_s(&fileIn2, "firCoeff1.dat", "rb");
	if (err == 0)
	{
		fread(fir1Coeff, sizeof(LPF_INTFC), (FIR_1ST_LENGTH >> 1) + 1, fileIn2);
		fclose(fileIn2);
	}
	else
	{
		printf("Can't open coefficient file!\n");
		return -1;
	}
	err = fopen_s(&fileIn3, "firCoeff2.dat", "rb");
	if (err == 0)
	{
		fread(fir2Coeff, sizeof(LPF_INTFC), (FIR_2ND_LENGTH >> 1), fileIn3);
		fclose(fileIn3);
	}
	else
	{
		printf("Can't open coefficient file!\n");
		return -1;
	}

	//Ϊ���ݻ���������ռ�
	rrsBuf1 = CreateQueue(RRS_FORWARD_DELAY << 1); 
	rrsBuf2 = CreateQueue(RRS_BACKWARD_DELAY << 1);
	ifirBuf1 = CreateQueue(FIR_1ST_LENGTH);
	ifirBuf2 = CreateQueue(FIR_2ND_LENGTH* INTERP_FACTOR);
	nodeIdx = CreateNodeIdx();

	//*********************************************************�źŴ���***********************************************
	while (i < DATA_LENGTH)
	{
		filteredEcgData[i] = SignalProcess((const)rawEcgData[i], rrsBuf1, rrsBuf2, nodeIdx, ifirBuf1, ifirBuf2, fir1Coeff, fir2Coeff);
		i++;
	}
	//*****************************************************************************************************************

	//�ͷŻ�����
	FreeNode(nodeIdx);
	FreeQueue(ifirBuf2);
	FreeQueue(ifirBuf1);
	FreeQueue(rrsBuf2);
	FreeQueue(rrsBuf1);
	//���д���ļ�
	err = fopen_s(&fileOut, "filteredECG.dat", "wb");
	if (err == 0)
	{
		fwrite(filteredEcgData, sizeof(OUTPUT_TYPE), DATA_LENGTH, fileOut);
		fclose(fileOut);
	}
	else
	{
		printf("Can't open file\n");
		return -1;
	}

	return 0;
}