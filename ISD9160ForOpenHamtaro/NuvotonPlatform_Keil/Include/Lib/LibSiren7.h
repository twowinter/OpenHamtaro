/*----------------------------------------------------------------------------------------*/
/*                                                                                        */
/* Copyright(c) 2011 Nuvoton Technology Corp. All rights reserved.                        */
/*                                                                                        */
/*----------------------------------------------------------------------------------------*/
#ifndef SIREN7ENDECHDR
#define SIREN7ENDECHDR

/*---------------------------------------------------------------------------------------------------------*/
/* Macro, type and constant definitions                                                                    */
/*---------------------------------------------------------------------------------------------------------*/
#ifdef MAX_SAMPLE_RATE
#undef MAX_SAMPLE_RATE
#endif
#define MAX_SAMPLE_RATE 32000
#define MAX_FRAMESIZE   (MAX_SAMPLE_RATE/50)

#ifndef MAX_DCT_LENGTH
#ifndef DCT_LENGTH
#define DCT_LENGTH          320
#endif // DCT_LENGTH
#ifdef SIREN14
#define MAX_DCT_LENGTH      640
#else
#define MAX_DCT_LENGTH  DCT_LENGTH
#endif
#endif // MAX_DCT_LENGTH

#ifndef NUMBER_OF_REGIONS
#define NUMBER_OF_REGIONS 14
#endif

#ifndef MAX_NUMBER_OF_REGIONS
#ifdef SIREN14
#define MAX_NUMBER_OF_REGIONS   28
#else
#define MAX_NUMBER_OF_REGIONS NUMBER_OF_REGIONS
#endif
#endif

#pragma pack(4)

typedef struct tsSiren7_CODEC_CTL
{
	signed  long  bit_rate;
	signed short  bandwidth;
	signed short  number_of_bits_per_frame;
	signed short  number_of_regions;
	signed short  frame_size;
}sSiren7_CODEC_CTL;

typedef struct tsSiren7_ENC_CTX
{
    signed short  history[MAX_FRAMESIZE];
    signed short  mlt_coefs[MAX_FRAMESIZE]; // this is not history data
}sSiren7_ENC_CTX;

#ifndef Rand_Obj_defined
#define Rand_Obj_defined
typedef struct
{
    signed short seed0;
    signed short seed1;
    signed short seed2;
    signed short seed3;
}Rand_Obj;
#endif // Rand_Obj_defined

typedef struct tsSiren7_DEC_CTX
{
    signed short  old_mag_shift;
#ifndef NO_FRAME_ERROR_CHECK
    signed short  old_decoder_mlt_coefs[MAX_DCT_LENGTH];
#endif
    signed short  old_samples[MAX_DCT_LENGTH>>1];
    Rand_Obj randobj;
    signed short  decoder_mlt_coefs[MAX_DCT_LENGTH]; // this is not history data
}sSiren7_DEC_CTX;

#pragma pack()

/*---------------------------------------------------------------------------------------------------------*/
/* Define Function Prototypes                                                                              */
/*---------------------------------------------------------------------------------------------------------*/
extern void LibS7EnBufReset(signed short ssFrameSize, sSiren7_ENC_CTX *sS7EncCtx);

extern void LibS7Encode(const sSiren7_CODEC_CTL *sCctl, sSiren7_ENC_CTX *sS7EncCtx, 
                        signed short *w16InData, signed short *w16OutData);
extern void LibS7DeBufReset(signed short ssFrameSize, sSiren7_DEC_CTX *sS7DecCtx);
extern void LibS7Decode(const sSiren7_CODEC_CTL *sCctl, sSiren7_DEC_CTX *sS7DecCtx, 
                        signed short *w16InData, signed short *w16OutData);
extern void LibS7Init(sSiren7_CODEC_CTL *sCodecCtl, signed long w32BitRate, signed short w16Bandwidth);

#endif // SIREN7ENDECHDR
