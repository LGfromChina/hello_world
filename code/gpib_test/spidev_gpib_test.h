#ifndef __SPIDEV_GPIB_TEST_H__
#define __SPIDEV_GPIB_TEST_H__

/*********************************包含文件声明*********************************/
/*#include <sys\exception.h>
#include <cdefBF532.h>
#include <stdlib.h>
#include "general.h"*/
/*********************************外部变量声明*********************************/
#define DM3000 1
/********************************常数和类型声明********************************/
/*
gpibdevc.h
*/
#define error_reading                   1<<0
#define error_writing                   1<<1
#define error_unknown_command           1<<2
#define NEWLINE                   0x0a /* line feed */


/* define i/o addresses of the registers on the 7210 */



#define Data_In				            gpib_r(0)
#define Data_Out(n)			            gpib_w(0,n)

#define INT_Status01 		            gpib_r(1)
#define INT_Mask01(n) 		            gpib_w(1,n)

#define INT_Status02 		            gpib_r(2) 
#define INT_Mask02(n)		            gpib_w(2,n) 

#define Serial_Poll_Status	            gpib_r(3)
#define Serial_Poll_Mode(n)	            gpib_w(3,n)

#define Addr_Status			            gpib_r(4)
#define Addr_Mode(n)		            gpib_w(4,n)

#define Cmd_Pass		 	            gpib_r(5)
#define Aux(n)			 	            gpib_w(5,n)

#define Addr_0			 	            gpib_r(6)
#define Addr_0_1(n)		 	            gpib_w(6,n)

#define Addr_1			 	            gpib_r(7)
#define EndOfStr(n)		 	            gpib_w(7,n)




/* isr1 bits*/
#define b_bi                            0x01
#define b_bo                            0x02
#define b_err                           0x04
#define b_dcas                          0x08
#define b_end                           0x10
#define b_get                           0x20

/* imr1 bits*/
#define b_dma0                          0x80
#define b_dma1                          0x40

#define b_bi_ie                         0x01
#define b_bo_ie                         0x02
#define b_err_ie                        0x04
#define b_dcas_ie                       0x08
#define b_end_ie                        0x10
#define b_get_ie                        0x20

/* isr2 bits*/
/*
#define b_unc 0x20
#define b_apt 0x10
#define b_ma 0x04
#define b_srq 0x02
#define b_ifc 0x01
*/
/* imr2 bits*/
#define b_gint_ie                       0x80
/*
#define b_err_ie 0x40
#define b_unc_ie 0x20
#define b_apt_ie 0x10
#define b_dcas_ie 0x08
#define b_ma_ie 0x04
#define b_srq_ie 0x02
#define b_ifc_ie 0x01
*/
/* adsr bits */
#define b_rem                           0x80
#define b_llo                           0x40
#define b_atn                           0x20
#define b_lpas                          0x10
#define b_tpas                          0x08
#define b_la                            0x04
#define b_ta                            0x02
#define b_ulpa                          0x01
/* imr2 bits */
#define b_glint                         0x80
#define b_stbo_ie                       0x40
#define b_nlen                          0x20
#define b_lloc_ie                       0x08
#define b_atni_ie                       0x04
#define b_cic_ie                        0x01
/* bcr bits */
#define b_bcr_atn                       0x80
#define b_bcr_dav                       0x40
#define b_bcr_ndac                      0x20
#define b_bcr_nrfd                      0x10
#define b_bcr_eoi                       0x08
#define b_bcr_srq                       0x04
#define b_bcr_ifc                       0x02.78
#define b_bcr_ren                       0x01

/* auxr fields (shadow registers)*/
#define f_icr                           0x20
#define f_accra                         0x80
#define f_accrb                         0xa0
#define f_accre                         0xc0
#define f_accrf                         0xd0
#define f_accri                         0xe0


/* bsr bits are identical to bcr*/
/* isr2 bits */
#define b_nba                           0x80
#define b_stbo                          0x40
#define b_nl                            0x20
#define b_eos                           0x10
#define b_lloc                          0x08
#define b_atni                          0x04
#define b_cic                           0x01
/* adr bits */
#define b_edpa                          0x80
#define b_dal                           0x40
#define b_dat                           0x20
/* spmr/spsr bits */
#define b_rsv                           0x40
#define b_mav                           0x10
/* accra bits */
#define b_bin                           0x10
#define b_xeos                          0x08
#define b_reos                          0x04
/* accrb bits */
#define b_iss                           0x10
#define b_inv                           0x08
#define b_lwc                           0x04
#define b_speoi                         0x02
#define b_atct                          0x01
/* accre bits */
#define b_dhadt                         0x08
#define b_dhadc                         0x04
/* accrf bits */
#define b_dhata                         0x08
#define b_dhala                         0x04
#define b_dhuntl                        0x02
#define b_dhall                         0x01
/* accri bits */
#define b_ustd                          0x08.79
#define b_pp1                           0x04
#define b_dmae                          0x01
/* accr~icr bits */
#define f_1mhz                          0x01
#define f_2mhz                          0x02
#define f_3mhz                          0x03
#define f_4mhz                          0x04
#define f_5mhz                          0x05
#define f_6mhz                          0x06
#define f_7mhz                          0x07
#define f_8mhz                          0x08
/* aux commands */
#define CMD_CHIP_RST                    0x02
#define c_nswrst                        0x00
#define c_rhdf                          0x03
#define c_nonvalid                      0x07
/*
#define c_pieosr 0x9e
#define c_vstdl 0x97
#define c_swrst 0x80
#define c_valid 0x81
#define c_nhdfa 0x03
#define c_hdfa 0x83
#define c_nhdfe 0x04
#define c_hdfe 0x84
#define c_nbaf 0x05
#define c_nfget 0x06
#define c_fget 0x86
#define c_nrtl 0x07
#define c_rtl 0x87
#define c_feoi 0x08
#define c_nlon 0x09
#define c_lon 0x89
#define c_nton 0x0a
#define c_ton 0x8a
#define c_gts 0x0b
#define c_tca 0x0c
#define c_tcs 0x0d
#define c_nrpp 0x0e
#define c_rpp 0x8e
#define c_nsic 0x0f
#define c_sic 0x8f
#define c_nsre 0x10
#define c_sre 0x90
#define c_rqc 0x11
#define c_rlc 0x12
#define c_ndai 0x13
#define c_dai 0x93
#define c_pts 0x14
#define c_nstdl 0x15
#define c_stdl 0x95
#define c_nshdw 0x16
#define c_shdw 0x96
#define c_nvstdl 0x17
#define c_nrsv2 0x18
#define c_rsv2 0x98
#define c_sw7210 0x99
#define c_reqf 0x1a
#define c_reqt 0x9a
#define c_ch_rst 0x1c
#define c_nist 0x1d
#define c_ist 0x9d
#define c_piimr2 0x1e
#define c_pibcr 0x1f
#define c_clrpi 0x9c
#define c_piaccr 0x9f
*/
//typedef unsigned char u_8;//注销
/* buffer and GPIB state declarations */
//typedef unsigned char u8;
#define GPIB_BUFFER_SIZE                1000//64

#define idle_state                      1
#define reading_state                   2
#define writing_state                   3


/* state variable. one of the following values
idle_state 1
reading_state 2
writing_state 3
*/
extern uint8_t GPIB_state;
/* buffer variables
index is an offset from the base address of the i/o buffer.
io_buffer is both the GPIB input and output buffer.
*/
//extern uint8_t index;


/*
extern bool 	OutputDataEnabled; 
extern short 	OutputDataPtr; 
extern short 	OutputDataSize;
*/

//#define PF_GPIB_MASK                0x0020    
#define LONG_RET 1000
#define SHORT_RET 1003

#define GPIB_BASE_ADDR              0x20380000
/*******************************************************************************
* 函数名: init_GPIB_device
* 描  述: 初始化GPIB
* 输入参数: 
*
* 输出参数：
*
* 返回值: -1 means failed open device
* 说 明：
*******************************************************************************/
int init_GPIB_device(void);


/*******************************************************************************
* 函数名: initialize_microcontroller
* 描  述: 初始化使用的PF口
* 输入参数: 
*
* 输出参数：
*
* 返回值: 
* 说 明：
*******************************************************************************/
void initialize_microcontroller();
/*******************************************************************************
* 函数名: initialize_7210
* 描  述: 初始化7210
* 输入参数: 
*
* 输出参数：
*
* 返回值: 
* 说 明：
* this routine initializes the 7210 and enables the device to receive commands from
* the GPIB network..
*******************************************************************************/
void initialize_7210();

/*******************************************************************************
* 函数名: Set_GPIB_Addr
* 描  述: 设置GPIB地址
* 输入参数: 
*
* 输出参数：
*
* 返回值: 
* 说 明：
*******************************************************************************/
void Set_GPIB_Addr();

/*******************************************************************************
* 函数名: Get_GPIB_Addr
* 描  述: 获取GPIB地址
* 输入参数: 
*
* 输出参数：
*
* 返回值: 
* 说 明：
*******************************************************************************/
unsigned short Get_GPIB_Addr(void);
/*******************************************************************************
* 函数名: route_7210_interrupts
* 描  述: 中断响应函数
* 输入参数: 
*
* 输出参数：
*
* 返回值: 
* 说 明：
*******************************************************************************/
void route_7210_interrupts(void);


/*******************************************************************************
* 函数名: handle_DCAS_int
* 描  述: 中断响应函数
* 输入参数: 
*
* 输出参数：
*
* 返回值: 
* 说 明：
* This routine resets only the GPIB interface of the device and not
* the device itself. Its primary use is recovering after an error on the GPIB.
* If a GPIB error occurs and the device locks up or appears to hang, the GPIB
* controller can issue the SDC or DCL command and place the device into its idle GPIB
* state, clear its buffers and start over.
*******************************************************************************/
void handle_DCAS_int(void);
/*******************************************************************************
* 函数名: handle_BI_int
* 描  述: 中断响应函数
* 输入参数: 
*
* 输出参数：
*
* 返回值: 
* 说 明：
* The Byte In handler reads a byte from the 7210 and stores it in the input buffer.
* If the device has not finished writing data from a previous command message, the
* device overwrites the old data and issues an error message to the GPIB controller.
*******************************************************************************/
void handle_BI_int(void);
/*******************************************************************************
* 函数名: handle_BO_int
* 描  述: 中断响应函数
* 输入参数: 
*
* 输出参数：
*
* 返回值: 
* 说 明：
* This routine places the next byte from the output buffer into the 7210. If
* the device was reading data and received a spurious command to write, the function
* issues an error message to the GPIB controller and exits.
*******************************************************************************/
void handle_BO_int(void);
/*******************************************************************************
* 函数名: handle_GET_trigger
* 描  述: 
* 输入参数: 
*
* 输出参数：
*
* 返回值: 
* 说 明：
* Device Dependent Routines
* This routine performs a device specific trigger action.
*******************************************************************************/
void handle_GET_trigger(void);
/*******************************************************************************
* 函数名: output_data_to_io_buffer
* 描  述: GPIB Buffer Routines
* 输入参数: 
*
* 输出参数：
*
* 返回值: 
* 说 明：
* output_data_to_io_buffer writes data to the output buffer. Because its input is
* provided by other functions, it has no error checking.
*******************************************************************************/
void output_data_to_io_buffer( uint8_t data_out);

/*******************************************************************************
* 函数名: parse_input_buffer
* 描  述: 
* 输入参数: 
*
* 输出参数：
*
* 返回值: 
* 说 明：
* parse the message and call the correct routine. Also, reset the buffer and set the
* device in idle state. If the command just received is invalid, send an error message
* to the GPIB controller.
*******************************************************************************/
void parse_input_buffer();

/*******************************************************************************
* 函数名: GPIB_Write_Buf
* 描  述: 
* 输入参数: 
*
* 输出参数：
*
* 返回值: 
* 说 明：
* 启动发送数据
*******************************************************************************/
void GPIB_Write_Buf(unsigned char flag,char* data);
/*******************************************************************************
* 函数名: set_status_byte_bits
* 描  述: Status Byte management routines
* 输入参数: 
*
* 输出参数：
*
* 返回值: 
* 说 明：
* this routine encapsulates writes to the 7210's spmr register. It allows the
* calling routine to set any combination of bits without affecting the others. It
* presents a consistent interface for requesting serial polls. Its counterpart is
* clear_status_byte_bits()
*******************************************************************************/
void set_status_byte_bits(uint8_t srq_byte);

/*******************************************************************************
* 函数名: clear_status_byte_bits
* 描  述: Status Byte management routines
* 输入参数: 
*
* 输出参数：
*
* 返回值: 
* 说 明：
* this routine encapsulates writes to the 7210's spmr register. It allows the
* calling routine to clear any combination of bits without affecting the others by
* clearing the bits corresponding to the asserted bits of its input. It presents a
* consistent interface for ceasing to request serial polls. Its counterpart is
* set_status_byte_bits().
*******************************************************************************/
void clear_status_byte_bits(uint8_t srq_byte);

//---------------------------------------------------------------------------//
// Function:	write_gpib									                 //
//																			 //
// Parameters:	addr---is the address of writting							 //
//              data---is the data of writting								 //
// Return:		None														 //
//																			 //
// Description:	this function is used to write data to 7210 register         //  
//				 whose address is addr                                       //
//---------------------------------------------------------------------------//
void gpib_w(unsigned char addr,unsigned char data);
//---------------------------------------------------------------------------//
// Function:	read_gpib									                 //
//																			 //
// Parameters:	addr---is the address of reading							 //
//              							                                 //
// Return:		unsigned char---is the reading result				         //
//																			 //
// Description:	this function is used to read 7210 register whose            //
//				address is addr                                              //
//---------------------------------------------------------------------------//
unsigned char gpib_r(unsigned char addr);


unsigned short Parse_cmd(unsigned char* cmd_str);
void SendParseDataOut_GPIB();

#endif

