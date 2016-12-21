/*-----------------------------------------------------------------------------------*/
/*                                                                                   */
/* Copyright(c) 2011 Nuvoton Technology Corp. All rights reserved.                   */
/*                                                                                   */
/*-----------------------------------------------------------------------------------*/
/*-----------------------------------------------------------------------------------*/
/* Includes of system headers                                                        */
/*-----------------------------------------------------------------------------------*/
#include <stdio.h>
/*-----------------------------------------------------------------------------------*/
/* Includes of local headers                                                         */
/*-----------------------------------------------------------------------------------*/
#include "ISD93xx.h"
#include "SPIFlash_Cmds.h"
#include "DrvSDCARD.h"
/*-----------------------------------------------------------------------------------*/
/* Macro, type and constant definitions                                              */
/*-----------------------------------------------------------------------------------*/
// Erase group size = 16 MMC FLASH sectors
#define PHYSICAL_BLOCK_SIZE     512

// Command table value definitions
// Used in the DrvSDCARD_MMCcmdExec function to
// decode and execute MMC command requests
#define     EMPTY  0
#define     YES   1
#define     NO    0
#define     CMD   0
#define     RD    1
#define     WR    2
#define     RDB   3
#define     WDB   4
#define     R1    0
#define     R1b   1
#define     R2    2
#define     R3    3
#define     R7    4

// Start and stop data tokens for single and multiple
// block MMC data operations
#define     START_SBR      0xFE
#define     START_MBR      0xFE
#define     START_SBW      0xFE
#define     START_MBW      0xFC
#define     STOP_MBW       0xFD

// Mask for data response Token after an MMC write
#define     DATA_RESP_MASK 0x11

// Mask for busy Token in R1b response
#define     BUSY_BIT       0x80



#define BACK_FROM_ERROR { DrvSDCARD_SingleWrite(0xFF);DrvSPI_ClrCS(eDRVSPI_PORT0, eDRVSPI_SS0); return FALSE;}


typedef union {                        // byte-addressable unsigned long
    uint32_t l;
    uint8_t b[4];
    } UINT32;
typedef union {                        // byte-addressable unsigned int
    uint16_t i;
    uint8_t b[2];
    } UINT16;
// This structure defines entries into the command table;
typedef struct {
    uint8_t command_byte;      // OpCode;
    uint8_t arg_required;      // Indicates argument requirement;
    uint8_t ui8CRC;               // Holds CRC for command if necessary;
    uint8_t trans_type;        // Indicates command transfer type;
    uint8_t response;          // Indicates expected response;
    uint8_t var_length;        // Indicates varialble length transfer;
} COMMAND;
// Command table for MMC.  This table contains all commands available in SPI
// mode;  Format of command entries is described above in command structure
// definition;
COMMAND __I command_list[] = {
    { 0,NO ,0x95,CMD,R1 ,NO },     // CMD0;  GO_IDLE_STATE: reset card;
    { 1,NO ,0xFF,CMD,R1 ,NO },     // CMD1;  SEND_OP_COND: initialize card;
	{ 8,YES,0xFF,CMD,R7 ,NO },     // CMD8;	SEND_IF_COND
    { 9,NO ,0xFF,RD ,R1 ,NO },     // CMD9;  SEND_CSD: get card specific data;
    {10,NO ,0xFF,RD ,R1 ,NO },     // CMD10; SEND_CID: get card identifier;
    {12,NO ,0xFF,CMD,R1b,NO },     // CMD12; STOP_TRANSMISSION: end read;
    {13,NO ,0xFF,CMD,R2 ,NO },     // CMD13; SEND_STATUS: read card status;
    {16,YES,0xFF,CMD,R1 ,NO },     // CMD16; SET_BLOCKLEN: set block size;
    {17,YES,0xFF,RDB ,R1 ,NO },    // CMD17; READ_SINGLE_BLOCK: read 1 block;
    {18,YES,0xFF,RD ,R1 ,YES},     // CMD18; READ_MULTIPLE_BLOCK: read > 1;
	{23,NO ,0xFF,CMD,R1 ,NO },     // CMD23; SET_BLOCK_COUNT
    {24,YES,0xFF,WR ,R1 ,NO },     // CMD24; WRITE_BLOCK: write 1 block;
    {25,YES,0xFF,WR ,R1 ,YES},     // CMD25; WRITE_MULTIPLE_BLOCK: write > 1;
    {27,NO ,0xFF,CMD,R1 ,NO },     // CMD27; PROGRAM_CSD: program CSD;
    {28,YES,0xFF,CMD,R1b,NO },     // CMD28; SET_WRITE_PROT: set wp for group;
    {29,YES,0xFF,CMD,R1b,NO },     // CMD29; CLR_WRITE_PROT: clear group wp;
    {30,YES,0xFF,CMD,R1 ,NO },     // CMD30; SEND_WRITE_PROT: check wp status;
    {32,YES,0xFF,CMD,R1 ,NO },     // CMD32; TAG_SECTOR_START: tag 1st erase;
    {33,YES,0xFF,CMD,R1 ,NO },     // CMD33; TAG_SECTOR_END: tag end(single);
    {34,YES,0xFF,CMD,R1 ,NO },     // CMD34; UNTAG_SECTOR: deselect for erase;
    {35,YES,0xFF,CMD,R1 ,NO },     // CMD35; TAG_ERASE_GROUP_START;
    {36,YES,0xFF,CMD,R1 ,NO },     // CMD36; TAG_ERASE_GROUP_END;
    {37,YES,0xFF,CMD,R1 ,NO },     // CMD37; UNTAG_ERASE_GROUP;
    {38,YES,0xFF,CMD,R1b,NO },     // CMD38; ERASE: erase all tagged sectors;
    {42,YES,0xFF,CMD,R1 ,NO },     // CMD42; LOCK_UNLOCK;
	{55,NO ,0xFF,CMD,R1 ,NO },     // CMD55; APP_CMD
    {58,NO ,0xFF,CMD,R3 ,NO },     // CMD58; READ_OCR: read OCR register;
    {59,YES,0xFF,CMD,R1 ,NO },     // CMD59; CRC_ON_OFF: toggles CRC checking;
	{0x80+13,NO ,0xFF,CMD,R2 ,NO },// ACMD13; SD_SEND_STATUS: read card status;
	{0x80+23,YES,0xFF,CMD,R1 ,NO },// ACMD23;SD_SET_WR_BLK_ERASE_COUNT
	{0x80+41,YES,0xFF,CMD,R1 ,NO } // ACMD41; SD_SEND_OP_COND: initialize card;
    };
/*-----------------------------------------------------------------------------------*/
/* Parameter checking definitions                                                    */
/*-----------------------------------------------------------------------------------*/
int8_t Is_Initialized=0,SDtype=0;
uint32_t LogicSector=0;

/*-----------------------------------------------------------------------------------*/
/* SD CARD Protocol                                                                  */
/*-----------------------------------------------------------------------------------*/
/*
	Transfer Length:48bit
	BIT POSITION       WIDTH(BITS)          VAULE
			[47]                1               0:Start bit
			[46]				1				1:Transmit   0:Receive
		 [45:40]				6				CMD8:001000
		  [39:8]			   32               Reserved
		   [7:1]				7				Reserved
		     [0]				1				1:End bit

*/
/*-----------------------------------------------------------------------------------*/
/* Function: 	 DrvSDCARD_GenCRC                                                    */
/*                                                                                   */
/* Parameters:                                                                       */
/*               uint32_t u32Data         Input Data                                 */
/*               uint32_t u32GenPoly      CRC7:0x1200   CRC16:0x1021                 */
/*               uint32_t u32Accum        CRC value                                  */
/*                                                                                   */
/*                                                                                   */
/* Returns:                                                                          */
/*               uint32_t u32Accum      CRC value                                    */
/*               			                                                         */
/* Description:                                                                      */
/*               This function is used to generate CRC value                         */
/*-----------------------------------------------------------------------------------*/
static uint32_t DrvSDCARD_GenCRC(uint32_t u32Data, uint32_t u32GenPoly, uint32_t u32Accum)
{
	volatile uint8_t i;

	u32Data <<= 8;
	for (i=8; i>0; i--)
	{
		if ((u32Data ^ u32Accum) & 0x8000)
			u32Accum = (u32Accum << 1) ^ u32GenPoly;
		else
			u32Accum <<= 1;
		u32Data <<= 1;
	}
	return u32Accum;
}
/*-----------------------------------------------------------------------------------*/
/* Function: 	 DrvSDCARD_SingleWrite                                               */
/*                                                                                   */
/* Parameters:                                                                       */
/*               UINT32 u32Data        Data to send                                  */
/*                                                                                   */
/* Returns:                                                                          */
/*               None			                                                     */
/*                                                                                   */
/* Description:                                                                      */
/*               This function is used to send data though SPI                       */
/*                 to general clock for SDCARD operation                             */
/*-----------------------------------------------------------------------------------*/
static uint32_t DrvSDCARD_SingleWrite(uint32_t u32Data)
{
	uint32_t SPIdata=u32Data;
	DrvSPI_BurstTransfer(eDRVSPI_PORT0,1,2);
	DrvSPI_SingleWrite(eDRVSPI_PORT0,&SPIdata);
    while (DrvSPI_Busy(eDRVSPI_PORT0));
	DrvSPI_DumpRxRegister(eDRVSPI_PORT0,&SPIdata,1);
	return SPIdata;
}

/*-----------------------------------------------------------------------------------*/
/* Function: 	 DrvSDCARD_MMCcmdExec		                                         */
/*                                                                                   */
/* Parameters:                                                                       */
/*               uint8_t 	nCmd          Set command register                       */
/*               uint32_t 	nArg          Set command argument                       */
/*               uint8_t 	*pchar        Get register and data                      */
/*               uint32_t 	*response     Get response                               */
/*                                                                                   */
/* Returns:                                                                          */
/*               TRUE	get response		                                         */
/*               FALSE	1.SD Card busy, 2.Card moved, 3.Timeout	                     */
/*					             											         */
/* Description:                                                                      */
/*               This function is used to Send SDCARD CMD and Receive Response       */
/*-----------------------------------------------------------------------------------*/
uint32_t DrvSDCARD_MMCcmdExec (uint8_t nCmd, uint32_t nArg,uint8_t *pchar, uint32_t *response)
{
  	uint8_t	loopguard;
  	COMMAND current_command;      			// Local space for the command table
  	UINT32	long_arg;               		// Local space for argument
  	static uint32_t current_blklen = 512;
  	uint32_t old_blklen = 512;
  	int32_t counter = 0;     				// Byte counter for multi-byte fields;
  	UINT16 card_response;           			// Variable for storing card response;
  	uint8_t data_resp;      				// Variable for storing data response;
  	UINT16 dummy_CRC;      					// Dummy variable for storing CRC field;

  	card_response.i = 0;

  	current_command = command_list[nCmd];// Retrieve desired command table entry
											// from code space;
	if(current_command.command_byte & 0x80)	// Detect ACMD
	{
		if(DrvSDCARD_MMCcmdExec(APP_CMD,EMPTY,EMPTY,response)==FALSE)//Send APP_CMD
			return FALSE;
	}


  	DrvSPI_SetCS(eDRVSPI_PORT0, eDRVSPI_SS0);	// CS = 0
  	DrvSDCARD_SingleWrite(0xFF);
  	DrvSDCARD_SingleWrite((current_command.command_byte | 0x40)&0x7f);
	printf("CMD:%d,",current_command.command_byte&0x7f);

  	long_arg.l = nArg;              	// Make argument byte addressable;
                                      		// If current command changes block
                                      		// length, update block length variable
                                      		// to keep track;
		                                    // Command byte = 16 means that a set
		                                    // block length command is taking place
		                                    // and block length variable must be
		                                    // set;
  	if(current_command.command_byte == 16)
	{current_blklen = nArg;}
                                       // Command byte = 9 or 10 means that a
                                       // 16-byte register value is being read
                                       // from the card, block length must be
                                       // set to 16 bytes, and restored at the
                                       // end of the transfer;
  	if((current_command.command_byte == 9)||(current_command.command_byte == 10))
  	{
    	old_blklen = current_blklen;     // Command is a GET_CSD or GET_CID,
    	current_blklen = 16;             // set block length to 16-bytes;
  	}
                                       // If an argument is required, transmit
                                       // one, otherwise transmit 4 bytes of
                                       // 0x00;
  	if(current_command.arg_required == YES)
  	{
		dummy_CRC.i = DrvSDCARD_GenCRC((current_command.command_byte | 0x40), 0x1200, 0);
    	for(counter=3;counter>=0;counter--)
		{
			DrvSDCARD_SingleWrite(long_arg.b[counter]);
			dummy_CRC.i = DrvSDCARD_GenCRC(long_arg.b[counter], 0x1200, dummy_CRC.i);

		}
		dummy_CRC.i = (dummy_CRC.i >> 8)| 0x01;
		DrvSDCARD_SingleWrite(dummy_CRC.b[0]);
  	} else
	{
    	counter = 0;
    	while(counter <= 3)
		{
    		DrvSDCARD_SingleWrite(0x00);
			counter++;
		}
		DrvSDCARD_SingleWrite(current_command.ui8CRC);
  	}

											// The command table entry will indicate
                                       		// what type of response to expect for
                                       		// a given command;  The following
                                       		// conditional handles the MMC response;
	if(current_command.response == R1)  	// Read the R1 response from the card;
	{
  		loopguard=0;
    	do{
			card_response.b[0] = DrvSDCARD_SingleWrite(0xFF);
      		if(!++loopguard) break;
    	}while((card_response.b[0] & BUSY_BIT));
		printf("R1:0x%x, counter:%d\n",card_response.b[0],loopguard);
		if(!loopguard){BACK_FROM_ERROR;}
		*response=card_response.b[0];
    }
    else if(current_command.response == R1b)// Read the R1b response;
	{
    	loopguard = 0;
      	do {
			card_response.b[0] =  DrvSDCARD_SingleWrite(0xFF);
			if(!++loopguard) break;
      	}while((card_response.b[0] & BUSY_BIT));
		while((DrvSDCARD_SingleWrite(0xFF)&0xFF) == 0x00);
	}
	else if(current_command.response == R2)
	{
    	loopguard=0;
      	do{
		    card_response.b[0] = DrvSDCARD_SingleWrite(0xFF);
        	if(!++loopguard) break;
      	}while((card_response.b[0] & BUSY_BIT));
	    card_response.b[1] = DrvSDCARD_SingleWrite(0xFF);
		printf("R2:0x%x, counter:%d\n",card_response.i,loopguard);
		if(!loopguard) { BACK_FROM_ERROR; }
		*response=card_response.i;
    }else if(current_command.response == R3)
	{                               // Read R3 response;
    	loopguard=0;
      	do {
        	card_response.b[0] = DrvSDCARD_SingleWrite(0xFF);
		    if(!++loopguard) break;
      	} while((card_response.b[0] & BUSY_BIT));
  		printf("R3:0x%x, counter:%d\n",card_response.b[0],loopguard);
  	  	if(!loopguard) { BACK_FROM_ERROR; }
      	counter = 0;
      	while(counter <= 3)              // Read next three bytes and store them
      	{                                // in local memory;  These bytes make up
        	counter++;                    // the Operating Conditions Register
	      	*pchar++ = DrvSDCARD_SingleWrite(0xFF);
	    }
		*response=card_response.b[0];
    }else
	{                               // Read R7 response;
    	loopguard=0;
      	do {
        	card_response.b[0] = DrvSDCARD_SingleWrite(0xFF);
		    if(!++loopguard) break;
      	} while((card_response.b[0] & BUSY_BIT));
  		printf("R7:0x%x, counter:%d\n",card_response.b[0],loopguard);
  	  	if(!loopguard) { BACK_FROM_ERROR; }
      	counter = 0;
      	while(counter <= 3)              // Read next three bytes and store them
      	{                                // in local memory;  These bytes make up
        	counter++;                    // the Operating Conditions Register
	      	*pchar++ = DrvSDCARD_SingleWrite(0xFF);
	    }
		*response=card_response.b[0];
    }


	switch(current_command.trans_type)  // This conditional handles all data
    {                                   // operations;  The command entry
                                       // determines what type, if any, data
                                       // operations need to occur;
		case RDB:                         // Read data from the MMC;
  			loopguard = 0;

	     	while((DrvSDCARD_SingleWrite(0xFF)&0xFF)!=START_SBR)
			{
    	  		if(!++loopguard) {BACK_FROM_ERROR;}
				DrvSYS_Delay(1);
	      	}
          	counter = 0;                  	// Reset byte counter;
                                       		// Read <current_blklen> bytes;

			SPI0->TX[0] = 0xFFFFFFFF;
			if(pchar)
			{
				/*Set pchar+counter is a multiple of 4*/
				while(((uint32_t)pchar+counter)&0x03)
				{
					SPI0->CNTRL.GO_BUSY = 1;
					while(SPI0->CNTRL.GO_BUSY);
					*(pchar+counter++)=SPI0->RX[0];
				}
				/*Read data by word*/
				SPI0->CNTRL.TX_BIT_LEN=0;
				SPI0->CNTRL.BYTE_ENDIAN=1;
				SPI0->CNTRL.TX_NUM=1;
				for (; counter<current_blklen-7; )
				{

					SPI0->CNTRL.GO_BUSY = 1;
					while(SPI0->CNTRL.GO_BUSY);
					*((uint32_t*)(pchar+counter))=SPI0->RX[0];
					counter+=4;
					*((uint32_t*)(pchar+counter))=SPI0->RX[1];
					counter+=4;
				}
				SPI0->CNTRL.TX_NUM=0;
				SPI0->CNTRL.BYTE_ENDIAN=0;
				SPI0->CNTRL.TX_BIT_LEN=8;
				/*Read data by byte*/
				for (; counter<current_blklen; counter++)
				{
					SPI0->CNTRL.GO_BUSY = 1;
					while(SPI0->CNTRL.GO_BUSY);
					*(pchar+counter)=SPI0->RX[0];
				}

			}else
			{
				for (; counter<current_blklen; counter++)
				{
					SPI0->CNTRL.GO_BUSY = 1;
					while(SPI0->CNTRL.GO_BUSY);
				}
			}
           	dummy_CRC.b[1] = DrvSDCARD_SingleWrite(0xFF);	// After all data is read, read the two
           	dummy_CRC.b[0] = DrvSDCARD_SingleWrite(0xFF);	// CRC bytes;  These bytes are not used
                               					// in this mode, but the placeholders
                   								// must be read anyway;
          	break;
		case RD:                         // Read data from the MMC;
  			loopguard = 0;

	     	while((DrvSDCARD_SingleWrite(0xFF)&0xFF)!=START_SBR)
			{
    	  		if(!++loopguard) {BACK_FROM_ERROR;}
	      	}
          	counter = 0;                  	// Reset byte counter;
                                       		// Read <current_blklen> bytes;
			if(pchar)
			{
				for (counter=0; counter<current_blklen; counter++)
				{
					SPI0->TX[0] = 0xFF;
					SPI0->CNTRL.GO_BUSY = 1;
					while(SPI0->CNTRL.GO_BUSY);
					*(pchar+counter)=SPI0->RX[0];
				}
			}else
			{
				for (counter=0; counter<current_blklen; counter++)
				{
					SPI0->TX[0] = 0xFF;
					SPI0->CNTRL.GO_BUSY = 1;
					while(SPI0->CNTRL.GO_BUSY);
				}
			}
           	dummy_CRC.b[1] = DrvSDCARD_SingleWrite(0xFF);	// After all data is read, read the two
           	dummy_CRC.b[0] = DrvSDCARD_SingleWrite(0xFF);	// CRC bytes;  These bytes are not used
                               					// in this mode, but the placeholders
                   								// must be read anyway;
          	break;

		case WR:
			DrvSDCARD_SingleWrite(0xFF);
			DrvSDCARD_SingleWrite(START_SBW);

			for (counter=0; counter<current_blklen; counter++)
			{
				SPI0->TX[0] = *(pchar+counter);
				SPI0->CNTRL.GO_BUSY = 1;
				dummy_CRC.i = DrvSDCARD_GenCRC(*(pchar+counter), 0x1021, dummy_CRC.i);
				while(SPI0->CNTRL.GO_BUSY);
			}
			DrvSDCARD_SingleWrite(dummy_CRC.b[1]);
			DrvSDCARD_SingleWrite(dummy_CRC.b[0]);

        	loopguard = 0;
	        do                            // Read Data Response from card;
	        {
	          	data_resp = DrvSDCARD_SingleWrite(0xFF);
	          	if(!++loopguard) break;
	        }while((data_resp & DATA_RESP_MASK) != 0x01);	// When bit 0 of the MMC response
	                                       					// is clear, a valid data response
	                                       					// has been received;

	        if(!loopguard) { BACK_FROM_ERROR; }


	        while((DrvSDCARD_SingleWrite(0xFF)&0xFF)!=0xFF);//Wait for Busy
			DrvSDCARD_SingleWrite(0xFF);
	        break;
		default: break;
	}
    DrvSPI_ClrCS(eDRVSPI_PORT0, eDRVSPI_SS0);	// CS = 1
    if((current_command.command_byte == 9)||(current_command.command_byte == 10)) {
    	current_blklen = old_blklen;
	}
	printf("True\n");
    return TRUE;
}
/*-----------------------------------------------------------------------------------*/
/* Function: 	 DrvSDCARD_MMCflashInit                                              */
/*                                                                                   */
/* Parameters:                                                                       */
/*               None                                                                */
/*                                                                                   */
/* Returns:                                                                          */
/*               None                                                                */
/*                                                                                   */
/* Description:                                                                      */
/*               This function is used to initialize the flash card                  */
/*-----------------------------------------------------------------------------------*/
void DrvSDCARD_MMCflashInit(void)
{
	uint32_t response;
	uint8_t loopguard;
  	uint32_t i;
  	uint8_t counter = 0;
  	uint8_t pchar[16];         		// Data pointer for storing MMC
  	uint32_t c_size,bl_len;
  	uint8_t c_mult;


	Is_Initialized = 0;

	DrvSPI_ClrCS(eDRVSPI_PORT0, eDRVSPI_SS0);	// CS = 1
	DrvSYS_Delay(1000);
	//--------------------------------------------------------
	//	Send 74 SD clcok in SD mode for Toshiba SD Card
	//--------------------------------------------------------
  	for(counter = 0; counter < 10; counter++) {
    	i=DrvSDCARD_SingleWrite(0xFF);
  	}
	DrvSYS_Delay(1000);

  	DrvSPI_SetCS(eDRVSPI_PORT0, eDRVSPI_SS0);  // CS = 0
	while(DrvSDCARD_MMCcmdExec(GO_IDLE_STATE,EMPTY,EMPTY,&response)==FALSE);
  	if(response!=0x01)
		return;

	if(DrvSDCARD_MMCcmdExec(SEND_IF_COND,0x15A,pchar,&response) && response==1)
	{/* SDC ver 2.00 */
		if (pchar[2] == 0x01 && pchar[3] == 0x5A)
		{	/* The card can work at vdd range of 2.7-3.6V */
			loopguard=0;
			do
			{
				DrvSDCARD_MMCcmdExec(SD_SEND_OP_COND,0x40000000,EMPTY,&response);//Enable HCS(OCR[30])
				if(!++loopguard) break;
				DrvSYS_Delay(50);
			}while(response!=0);
			if(!loopguard) return;

			DrvSDCARD_MMCcmdExec(READ_OCR,EMPTY,pchar,&response);
			SDtype=(pchar[0]&0x40)?SDv2|SDBlock:SDv2;
			printf("SDv2\n");
		}
	}else
	{/* SDv1 or MMCv3 */
		 DrvSDCARD_MMCcmdExec(SD_SEND_OP_COND,0x00,EMPTY,&response);
		if (response <= 1)
		{
			loopguard=0;
			do
			{
				DrvSDCARD_MMCcmdExec(SD_SEND_OP_COND,0x00,EMPTY,&response);
				if(!++loopguard) break;
				DrvSYS_Delay(50);
			}while(response!=0);
			if(!loopguard) return;
			SDtype=SDv1;	/* SDv1 */
			printf("SDv1\n");
		} else
		{
			loopguard=0;
			do
			{
				DrvSDCARD_MMCcmdExec(SEND_OP_COND,0x00,EMPTY,&response);
				if(!++loopguard) break;
				DrvSYS_Delay(50);
			}while(response!=0);
			if(!loopguard) return;
			SDtype=MMCv3;	/* MMCv3 */
			printf("MMCv3\n");
		}
		DrvSDCARD_MMCcmdExec(SET_BLOCKLEN,(uint32_t)PHYSICAL_BLOCK_SIZE,EMPTY,&response);
	}
	if(DrvSDCARD_MMCcmdExec(SEND_CSD,EMPTY,pchar,&response)==FALSE)
		return;

  	if(response==0)
	{
    	printf("Change speed:");
    	for(i=0;i<16;i++) {	printf("0x%X ",pchar[i]);}

  	} else {
    	printf("CARD STATUS 0x%X:\n",response);
  		for(i=0;i<16;i++) {	printf("0x%X ",pchar[i]);}
    	LogicSector=0;
    	return;
  	}

 	if(SDtype&SDBlock)// Determine the number of MMC sectors;
	{
		bl_len = 1 << (pchar[5] & 0x0f) ;
		c_size = ((pchar[7] & 0x3F) << 16) |(pchar[8] << 8) | (pchar[9]);
		LogicSector=c_size*((512*1024)/bl_len);
	}else
	{
	  	bl_len = 1 << (pchar[5] & 0x0f) ;
  		c_size = ((pchar[6] & 0x03) << 10) |(pchar[7] << 2) | ((pchar[8] &0xc0) >> 6);
  		c_mult = (((pchar[9] & 0x03) << 1) | ((pchar[10] & 0x80) >> 7));
		LogicSector=(c_size+1)*(1 << (c_mult+2))*(bl_len/512);
  	}
		printf("\nLogicSector:%d, PHYSICAL_SIZE:%dMB\n",LogicSector,(LogicSector/2/1024));

  	loopguard = 0;
  	while((DrvSDCARD_MMCcmdExec(READ_SINGLE_BLOCK,0,0,&response)==FALSE)) {
    	if(!++loopguard) break;
  	}
  	Is_Initialized = 1;
}
/*-----------------------------------------------------------------------------------*/
/* Function: 	 DrvSDCARD_Open                                                      */
/*                                                                                   */
/* Parameters:                                                                       */
/*               None                                                                */
/*                                                                                   */
/* Returns:                                                                          */
/*               E_DRVSDCARD_INITIAL_FAIL    Intial SDCARD Failed                    */
/*               E_SUCCESS			         Success                                 */
/*                                                                                   */
/* Description:                                                                      */
/*               This function is used to Open GPIO function and intial SDCARD       */
/*-----------------------------------------------------------------------------------*/
uint32_t DrvSDCARD_Open(void)
{
	uint32_t Temp[2];
	Temp[0]=0xFFFFFFFF;
	Temp[1]=Temp[0];

    DrvSYS_SetIPClock(E_SYS_SPI0_CLK,1);
    DrvGPIO_InitFunction(FUNC_SPI0); // enable SPI function and pin

	DrvSPI_Open(eDRVSPI_PORT0,eDRVSPI_MASTER,eDRVSPI_TYPE1,8);
	DrvSPI_DisableAutoCS(eDRVSPI_PORT0);
	DrvSPI_SetSlaveSelectActiveLevel(eDRVSPI_PORT0, eDRVSPI_ACTIVE_LOW_FALLING);
	DrvSPI_SetEndian(eDRVSPI_PORT0, eDRVSPI_MSB_FIRST);
	DrvSPI_SetTxRegister(eDRVSPI_PORT0,Temp,2);
	DrvSPI_SetClock(eDRVSPI_PORT0,300000,300000);
    DrvSDCARD_MMCflashInit();
    DrvSYS_Delay(1000);
	DrvSPI_SetClock(eDRVSPI_PORT0,12000000,12000000);

	if (Is_Initialized)
	{
		printf("SDCARD INIT OK\n\n");
		return E_SUCCESS;
	}
	else
		return E_DRVSDCARD_INITIAL_FAIL;
}

/*-----------------------------------------------------------------------------------*/
/* Function: 	 DrvSDCARD_Close                                                     */
/*                                                                                   */
/* Parameters:                                                                       */
/*               None                                                                */
/*                                                                                   */
/* Returns:                                                                          */
/*               None		                                                         */
/*               			                                                         */
/*                                                                                   */
/* Description:                                                                      */
/*               This function is used to close SDCARD                               */
/*-----------------------------------------------------------------------------------*/
void DrvSDCARD_Close(void)
{
	DrvSPI_Close(eDRVSPI_PORT0);
}
/*-----------------------------------------------------------------------------------*/
/* Function: 	 DrvSDCARD_GetCardSize                                               */
/*                                                                                   */
/* Parameters:                                                                       */
/*               PUINT32 pu32TotSecCnt                                               */
/*               			                                                         */
/* Returns:                                                                          */
/*               TRUE	The size is already saved in arg1                            */
/*               FALSE	The size is zero                                             */
/*                                                                                   */
/* Description:                                                                      */
/*               This function is used to get card size                              */
/*               after SDCARD is opened                                              */
/*-----------------------------------------------------------------------------------*/
uint32_t DrvSDCARD_GetCardSize(uint32_t* pu32TotSecCnt)
{
	if (LogicSector == 0)
		return FALSE;
	else
		*pu32TotSecCnt = LogicSector;

	return TRUE;
}
/*-----------------------------------------------------------------------------------*/
/* Function: 	 DrvSDCARD_GetLogicSector                                            */
/*                                                                                   */
/* Parameters:                                                                       */
/*               None                                                                */
/*               			                                                         */
/* Returns:                                                                          */
/*               The Logic Sector size                                               */
/*                                                                                   */
/* Description:                                                                      */
/*               This function is used to get card total sector                      */
/*               after SDCARD is opened                                              */
/*-----------------------------------------------------------------------------------*/
uint32_t DrvSDCARD_GetLogicSector(void)
{
	return LogicSector;
}

/*-----------------------------------------------------------------------------------*/
/* Function: 	 DrvSDCARD_GetVersion                                                */
/*                                                                                   */
/* Parameters:                                                                       */
/*               None                                                                */
/*                                                                                   */
/* Returns:                                                                          */
/*               SDCARD Driver Version                                               */
/*               			                                                         */
/* Description:                                                                      */
/*               This function is used to Get SD driver version                      */
/*-----------------------------------------------------------------------------------*/
uint32_t DrvSDCARD_GetVersion(void)
{
	return (DRVSDCARD_MAJOR_NUM << 16) | (DRVSDCARD_MINOR_NUM << 8) | DRVSDCARD_BUILD_NUM;
}

/*-----------------------------------------------------------------------------------*/
/* Function: 	 DrvSDCARD_SpiRead                                                   */
/*                                                                                   */
/* Parameters:                                                                       */
/*               uint32_t addr		Set start address for LBA                        */
/*               uint32_t size      Set data size (byte)                             */
/*               uint32_t buffer    Data buffer pointer read from SD card            */
/*																		             */
/* Returns:                                                                          */
/*               None                                                                */
/*               			                                                         */
/* Description:                                                                      */
/*               This function is used to Get data from SD card                      */
/*-----------------------------------------------------------------------------------*/
void DrvSDCARD_SpiRead(uint32_t addr, uint32_t size, uint8_t* buffer)
{
    /* This is low level read function of USB Mass Storage */
	uint32_t response;
	if(SDtype&SDBlock)
	{

		while(size >= PHYSICAL_BLOCK_SIZE)
		{
			while(DrvSDCARD_MMCcmdExec(READ_SINGLE_BLOCK,addr,buffer,&response)==FALSE);
			addr   ++;
			buffer += PHYSICAL_BLOCK_SIZE;
			size  -= PHYSICAL_BLOCK_SIZE;
		}

	}else
	{
		addr*=PHYSICAL_BLOCK_SIZE;
		while(size >= PHYSICAL_BLOCK_SIZE)
		{
			while(DrvSDCARD_MMCcmdExec(READ_SINGLE_BLOCK,addr,buffer,&response)==FALSE);
			addr   += PHYSICAL_BLOCK_SIZE;
			buffer += PHYSICAL_BLOCK_SIZE;
			size  -= PHYSICAL_BLOCK_SIZE;
		}
	}
}

/*-----------------------------------------------------------------------------------*/
/* Function: 	 DrvSDCARD_SpiWrite                                                  */
/*                                                                                   */
/* Parameters:                                                                       */
/*               uint32_t addr		Set start address LBA                            */
/*               uint32_t size      Set data size (byte)                             */
/*               uint32_t buffer    Data buffer pointer to be written to SD card     */
/*																		             */
/* Returns:                                                                          */
/*               None                                                                */
/*               			                                                         */
/* Description:                                                                      */
/*               This function is used to store data into SD card                    */
/*-----------------------------------------------------------------------------------*/
void DrvSDCARD_SpiWrite(uint32_t addr, uint32_t size, uint8_t* buffer)
{
	uint32_t response;
	if(SDtype&SDBlock)
	{
		while(size >= PHYSICAL_BLOCK_SIZE)
		{
			while(DrvSDCARD_MMCcmdExec(WRITE_BLOCK,addr,buffer,&response)==FALSE);
			addr   ++;
			buffer += PHYSICAL_BLOCK_SIZE;
			size  -= PHYSICAL_BLOCK_SIZE;
		}
	}else
	{
		addr*=PHYSICAL_BLOCK_SIZE;
		while(size >= PHYSICAL_BLOCK_SIZE)
		{
			while(DrvSDCARD_MMCcmdExec(WRITE_BLOCK,addr,buffer,&response)==FALSE);
			addr   += (PHYSICAL_BLOCK_SIZE);
			buffer += PHYSICAL_BLOCK_SIZE;
			size  -= PHYSICAL_BLOCK_SIZE;
		}
	}
}




