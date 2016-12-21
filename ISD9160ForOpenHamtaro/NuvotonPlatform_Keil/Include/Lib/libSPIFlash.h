
// qkdang

#ifndef SFLASHHDR
#define SFLASHHDR

/////////Flash cmd status
#define CMDSUCCESS     0
#define CMDFAIL        1
#define CMDFLASHBUSY   2
#define CMDINVALID     3

typedef struct tagSFLASH_CTX
{
	unsigned long spi_base;
	unsigned long spi_chnl; // 1, 2
	void (*spi_setcs)(const struct tagSFLASH_CTX *ctx, int csOn);
}SFLASH_CTX;

extern void sflash_set(const SFLASH_CTX *ctx);
extern void sflash_write_ena(const SFLASH_CTX *ctx);
extern int sflash_erase(const SFLASH_CTX *ctx, unsigned long addr, unsigned long len);
extern int sflash_eraseall(const SFLASH_CTX *ctx);
extern int sflash_canwrite(const SFLASH_CTX *ctx);
extern int sflash_write(const SFLASH_CTX *ctx, unsigned long addr, unsigned long* p, unsigned long bytes);
extern int sflash_read(const SFLASH_CTX *ctx, unsigned long addr, unsigned long* p, unsigned long bytes);
extern int sflash_fread(const SFLASH_CTX *ctx, unsigned long addr, unsigned long* p, unsigned long bytes);
extern int sflash_getid(const SFLASH_CTX *ctx);
extern int sflash_power_down(const SFLASH_CTX *ctx);
extern int sflash_release_power_down(const SFLASH_CTX *ctx);

extern int sflash_check_busy(const SFLASH_CTX *ctx);
extern int sflash_nonblock_eraseall(const SFLASH_CTX *ctx);
extern int sflash_nonblock_power_down(const SFLASH_CTX *ctx);
extern int sflash_nonblock_sectorerase(const SFLASH_CTX *ctx, unsigned long addr);
extern int sflash_nonblock_write(const SFLASH_CTX *ctx, unsigned long addr, unsigned long* p, unsigned long bytes);
extern int sflash_nonblock_read(const SFLASH_CTX *ctx, unsigned long addr, unsigned long* p, unsigned long bytes);
extern int sflash_nonblock_MIDIread(const SFLASH_CTX *ctx, unsigned long addr, unsigned char* p, long bytes);
extern int sflash_nonblock_fread(const SFLASH_CTX *ctx, unsigned long addr, unsigned long* p, unsigned long bytes);


#define	LibSPIFlash_Open	  		sflash_set						//Set ISD9160 related register
#define	LibSPIFlash_WriteEnable		sflash_write_ena				//command 06
#define	LibSPIFlash_Erase			sflash_erase				  	//command 20
#define	LibSPIFlash_EraseAll		sflash_eraseall				  	//command 60
//#define	LibSPIFlash_ReadWEL			sflash_canwrite
#define	LibSPIFlash_Write			sflash_write					//command 02, page program
#define	LibSPIFlash_FastRead		sflash_fread 					//command 0B
#define	LibSPIFlash_Read		   	sflash_read						//command 03
#define	LibSPIFlash_GetID			sflash_getid					//command 9F
#define	LibSPIFlash_PowerDown		sflash_power_down				//command B9
#define LibSPIFlash_ReleasePD	   	sflash_release_power_down		//command AB

#define	LibSPIFlash_Nonblock_Erase	   	sflash_nonblock_sectorerase						//command 20
#define	LibSPIFlash_Nonblock_EraseAll	sflash_nonblock_eraseall				//command 60
#define	LibSPIFlash_Nonblock_PowerDown	sflash_nonblock_power_down			//command B9
#define LibSPIFlash_Nonblock_Write	   	sflash_nonblock_write		        //command 02


#endif // SFLASHHDR
