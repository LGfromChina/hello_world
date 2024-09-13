#if 0
/*
 * SPI testing utility (using spidev driver)
 *
 * Copyright (c) 2007  MontaVista Software, Inc.
 * Copyright (c) 2007  Anton Vorontsov <avorontsov@ru.mvista.com>
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License.
 *
 * Cross-compile with cross-gcc -I/path/to/cross-kernel/include
 */

#include <stdint.h>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <getopt.h>
#include <fcntl.h>
#include <sys/ioctl.h>
#include <linux/types.h>
#include <linux/spi/spidev.h>



//#include <asm/io.h>
//#include <linux/miscdevice.h>
//#include <asm/uaccess.h>  /* copy_to_usr copy_from_usr*/

//#include <linux/sched.h>
//#include <linux/init.h>
//#include <linux/ioctl.h>
//#include <linux/delay.h>
//#include <linux/bcd.h>
//#include <linux/capability.h>
//#include <linux/cdev.h>
//#include <linux/gpio.h>
//#include <linux/interrupt.h>
//#include <linux/of_gpio.h>
//#include <linux/interrupt.h>
//#include <linux/irq.h>
//#include <linux/irqdomain.h>
//#include <linux/sched.h>  //wake_up_process()
//#include <linux/kthread.h>//kthread_create()/kthread_run()
//#include <linux/err.h>    //IS_ERR()/PTR_ERR()

#define ARRAY_SIZE(a) (sizeof(a) / sizeof((a)[0]))

static void pabort(const char *s)
{
	perror(s);
	abort();
}

static const char *device = "/dev/spidev2.0";
//static const char *device = "/dev/spidev.conf-fpga";
static uint32_t mode=0;
static uint8_t bits =8;
static uint32_t speed = 10000000;
static uint16_t delay;
static int verbose;
/*
uint8_t default_tx[] = {
	0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF,
	0x40, 0x00, 0x00, 0x00, 0x00, 0x95,
	0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF,
	0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF,
	0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF,
	0xF0, 0x0D,
};

*/
uint8_t default_tx[] = {
	//0x01, 0x02, 0x03, 0x04, 0x05, 0x06,
	0x01,0x01,0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF,
};
uint8_t default_rx[ARRAY_SIZE(default_tx)] = {0, };
char *input_tx;

static void hex_dump(const void *src, size_t length, size_t line_size, char *prefix)
{
	int i = 0;
	const unsigned char *address = src;
	const unsigned char *line = address;
	unsigned char c;

	printf("%s | ", prefix);
	//while ((length-- )>0) =
	while (length-- > 0) {
		printf("%02X ", *address++);
		if (!(++i % line_size) || (length == 0 && i % line_size)) {
			if (length == 0) {
				while (i++ % line_size)
					printf("__ ");
			}
			printf(" | ");  /* right close */
			while (line < address) {
				c = *line++;
				printf("%c", (c < 33 || c == 255) ? 0x2E : c);
			}
			printf("\n");
			if (length > 0)
				printf("%s | ", prefix);
		}
	}
}

/*
 *  Unescape - process hexadecimal escape character
 *      converts shell input "\x23" -> 0x23
 */
static int unescape(char *_dst, char *_src, size_t len)
{
	int ret = 0;
	char *src = _src;
	char *dst = _dst;
	unsigned int ch;

	while (*src) {
		if (*src == '\\' && *(src+1) == 'x') {
			sscanf(src + 2, "%2x", &ch);
			src += 4;
			*dst++ = (unsigned char)ch;
		} else {
			*dst++ = *src++;
		}
		ret++;
	}
	return ret;
}

static void transfer(int fd, uint8_t const *tx, uint8_t const *rx, size_t len)
{
	int ret;

	struct spi_ioc_transfer tr = {
		.tx_buf = (unsigned long)tx,
		.rx_buf = (unsigned long)rx,
		.len = len,
		.delay_usecs = delay,
		.speed_hz = speed,
		.bits_per_word = bits,
	};

/*
	if (mode & SPI_TX_QUAD)
		tr.tx_nbits = 4;
	else if (mode & SPI_TX_DUAL)
		tr.tx_nbits = 2;
	if (mode & SPI_RX_QUAD)
		tr.rx_nbits = 4;
	else if (mode & SPI_RX_DUAL)
		tr.rx_nbits = 2;
	if (!(mode & SPI_LOOP)) {
		if (mode & (SPI_TX_QUAD | SPI_TX_DUAL))
			tr.rx_buf = 0;
		else if (mode & (SPI_RX_QUAD | SPI_RX_DUAL))
			tr.tx_buf = 0;
	}
*/

	ret = ioctl(fd, SPI_IOC_MESSAGE(1), &tr);
	if (ret < 1)
		pabort("can't send spi message");

	hex_dump(tx, len, 32, "TX");
	hex_dump(rx, len, 32, "RX");
}

static void print_usage(const char *prog)
{
	printf("Usage: %s [-DsbdlHOLC3]\n", prog);
	puts("  -D --device   device to use (default /dev/spidev1.1)\n"
	     "  -s --speed    max speed (Hz)\n"
	     "  -d --delay    delay (usec)\n"
	     "  -b --bpw      bits per word \n"
	     "  -l --loop     loopback\n"
	     "  -H --cpha     clock phase\n"
	     "  -O --cpol     clock polarity\n"
	     "  -L --lsb      least significant bit first\n"
	     "  -C --cs-high  chip select active high\n"
	     "  -3 --3wire    SI/SO signals shared\n"
	     "  -v --verbose  Verbose (show tx buffer)\n"
	     "  -p            Send data (e.g. \"1234\\xde\\xad\")\n"
	     "  -N --no-cs    no chip select\n"
	     "  -R --ready    slave pulls low to pause\n"
	     "  -2 --dual     dual transfer\n"
	     "  -4 --quad     quad transfer\n");
	exit(1);
}

static void parse_opts(int argc, char *argv[])
{
	while (1) {
		static const struct option lopts[] = {
			{ "device",  1, 0, 'D' },
			{ "speed",   1, 0, 's' },
			{ "delay",   1, 0, 'd' },
			{ "bpw",     1, 0, 'b' },
			{ "loop",    0, 0, 'l' },
			{ "cpha",    0, 0, 'H' },
			{ "cpol",    0, 0, 'O' },
			{ "lsb",     0, 0, 'L' },
			{ "cs-high", 0, 0, 'C' },
			{ "3wire",   0, 0, '3' },
			{ "no-cs",   0, 0, 'N' },
			{ "ready",   0, 0, 'R' },
			{ "dual",    0, 0, '2' },
			{ "verbose", 0, 0, 'v' },
			{ "quad",    0, 0, '4' },
			{ NULL, 0, 0, 0 },
		};
		int c;

		c = getopt_long(argc, argv, "D:s:d:b:lHOLC3NR24p:v", lopts, NULL);

		if (c == -1)
			break;

		switch (c) {
		case 'D':
			device = optarg;
			break;
		case 's':
			speed = atoi(optarg);
			break;
		case 'd':
			delay = atoi(optarg);
			break;
		case 'b':
			bits = atoi(optarg);
			break;
		case 'l':
			mode |= SPI_LOOP;
			break;
		case 'H':
			mode |= SPI_CPHA;
			break;
		case 'O':
			mode |= SPI_CPOL;
			break;
		case 'L':
			mode |= SPI_LSB_FIRST;
			break;
		case 'C':
			mode |= SPI_CS_HIGH;
			break;
		case '3':
			mode |= SPI_3WIRE;
			break;
		case 'N':
			mode |= SPI_NO_CS;
			break;
		case 'v':
			verbose = 1;
			break;
		case 'R':
			mode |= SPI_READY;
			break;
		case 'p':
			input_tx = optarg;
			break;
		case '2':
			mode |= SPI_TX_DUAL;
			break;
		case '4':
			mode |= SPI_TX_QUAD;
			break;
		default:
			print_usage(argv[0]);
			break;
		}
	}
	if (mode & SPI_LOOP) {
		if (mode & SPI_TX_DUAL)
			mode |= SPI_RX_DUAL;
		if (mode & SPI_TX_QUAD)
			mode |= SPI_RX_QUAD;
	}
}
//int main()
//{
	////gpio_request(229, "PH5");
	////gpio_direction_output(229,1);
    ////////printk("PA14 = %d\n",gpio_get_value(229));
	//////printf("end\n");
	//return 0;	
//}
//#if 0
int main(int argc, char *argv[])
{
	int ret = 0;
	int fd;
	uint8_t *tx;
	uint8_t *rx;
	int size;
	int  sendflag;
	//parse_opts(argc, argv);
	//mode=
	fd = open(device, O_RDWR);
	if (fd < 0)
		pabort("can't open device");
	/*
	 * spi mode
	 */
	ret = ioctl(fd, SPI_IOC_WR_MODE, &mode);
	if (ret == -1)
		pabort("can't set spi mode");

	ret = ioctl(fd, SPI_IOC_RD_MODE, &mode);
	if (ret == -1)
		pabort("can't get spi mode");
	//bits=16;
	/*
	 * bits per word
	 */
	ret = ioctl(fd, SPI_IOC_WR_BITS_PER_WORD, &bits);
	if (ret == -1)
		pabort("can't set bits per word");

	ret = ioctl(fd, SPI_IOC_RD_BITS_PER_WORD, &bits);
	if (ret == -1)
		pabort("can't get bits per word");

	/*
	 * max speed hz
	 */
	ret = ioctl(fd, SPI_IOC_WR_MAX_SPEED_HZ, &speed);
	if (ret == -1)
		pabort("can't set max speed hz");

	ret = ioctl(fd, SPI_IOC_RD_MAX_SPEED_HZ, &speed);
	if (ret == -1)
		pabort("can't get max speed hz");

	printf("spi mode: 0x%x\n", mode);
	printf("bits per word: %d\n", bits);
	printf("max speed: %d Hz (%d MHz)\n", speed, speed/1000000);
	//while(scanf("%d,&n)&&n!=0)
	//while(scanf("%d",&sendflag),sendflag)
	////while(scanf("please input sendflag(1 means send,0 means don't send ):%d\n",&sendflag))
	//{
		////scanf("please input sendflag(1 means send,0 means don't send ):%d\n",&sendflag);
		//if(sendflag==1)
		//{
			//if (input_tx) 
			//{
				//size = strlen(input_tx+1);
				//tx = malloc(size);
				//rx = malloc(size);
				//size = unescape((char *)tx, input_tx, size);
				//transfer(fd, tx, rx, size);
				//free(rx);
				//free(tx);
			//} 
			//else 
			//{
		//transfer(fd, default_tx, default_rx, sizeof(default_tx));
			//}
		//usleep(500000);
		//}
	//else
		//{
		//printf("not send!\n");
		//}
	
	//}
	while(1)
	{
		transfer(fd, default_tx, default_rx, sizeof(default_tx));
		usleep(500000);
		}
	close(fd);
	return ret;
}
//#endif
#endif

















#include <ctype.h>
#include <stdint.h>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <getopt.h>
#include <fcntl.h>
#include <sys/ioctl.h>
#include <linux/types.h>
#include <linux/spi/spidev.h>
#include "spidev_gpib_test.h"


 
#include <signal.h>  
#include <sys/stat.h>
#include <errno.h>
#include <poll.h>

#define GPIB_SPI_MAGIC        'x'
#define IOCTL_IRQ_CMD          _IO(GPIB_SPI_MAGIC, 0x00)
//gpib
//#define PFB_MASK_IRQ		20
uint8_t gpib_rx_buffer[1000]={0};
uint8_t gpib_tx_buffer[1000]={0};
/*------------------------------ 外部变量声明 ------------------------------*/
unsigned char 	OutputDataEnabled; 
short 	OutputDataPtr; 
short 	OutputDataSize;
/*------------------------------- 局部变量 ---------------------------------*/
/* 
state variable. one of the following values
idle_state 1
reading_state 2
writing_state 3
*/
uint8_t GPIB_state;
/* 
buffer variables
index is an offset from the base address of the i/o buffer.
io_buffer is both the GPIB input and output buffer.
*/
static uint8_t index_gpibbuffer = 0;
/* 
isr0_byte and isr1_byte hold data from the 7210 status registers which has not yet
been processed.
*/
static uint8_t isr0_byte = 0;
static uint8_t isr1_byte = 0;
//typedef unsigned char u8;
#if 1//调试信息，屏蔽 zzy modified 09-11-26
volatile uint8_t tmp_isr00 = 0;
volatile uint8_t tmp_isr01 = 0;
volatile uint8_t tmp_bsr0 = 0;
volatile uint8_t tmp_bsr1 = 0;
volatile uint8_t tmp_addr_status = 0;
volatile uint8_t tmp_state1 = 0;
volatile uint8_t tmp_state2 = 0;
volatile uint8_t tmp_state3 = 0;
volatile uint8_t tmp_state4 = 0;
#endif

#define SPI_CMD_IDN_FLAG        5//0b101
#define SPI_CMD_IDN_DONT_CARE   4//0b100

// 1: Write, 0: Read
#define SPI_RW_IDN_WR          1 //0b1
#define SPI_RW_IDN_RD          0// 0b0
static const char *spi_device = "/dev/spidev2.0";
static const char *gpib_irq_device = "/dev/gpib_irq";
static uint32_t mode = 0;
static uint32_t speed = 10000000;
static int g_fd= 0;
static int g_gpib_irq_fd= 0;
typedef union gpib_spi_pack
{
    struct
    {
        uint8_t addr : 4;
        uint8_t wr   : 1;
        uint8_t idn  : 3;
    };
    uint8_t payload;
} gpib_spi_pack;

static void pabort(const char *s)
{
    perror(s);
    abort();
}

static int SpiWrite(int fd, uint8_t *tx_buf)
{
    int ret;
    int N = 1;
    uint8_t *ts_buf_0 = tx_buf;

    struct spi_ioc_transfer tr[] = {
        {
            .tx_buf = (unsigned long)ts_buf_0,
            .len = 2,
            .delay_usecs = 10,
        }
    };
    ret = ioctl(fd, SPI_IOC_MESSAGE(N), &tr);

    return ret;
}
static int SpiRead(int fd, uint8_t *tx_buf, uint8_t *rx_buf)
{
    int ret;
    int N = 1;
    uint8_t *ts_buf_0 = rx_buf;

    ret = SpiWrite(fd, tx_buf);

    struct spi_ioc_transfer tr[] = {
        {
            .rx_buf = (unsigned long)ts_buf_0,
            .len = 2,
            .delay_usecs = 0,
        }
    };

    ret += ioctl(fd, SPI_IOC_MESSAGE(N), &tr);

    return ret;
}
static int Spiopen(void)
{
    g_fd = open(spi_device, O_RDWR | O_NOCTTY | O_NDELAY);
    if (g_fd <= 0)
        {return -1;}
    return 0;        
}
static int Spiclose(void)
{   
    if(g_fd>0)   
        {      
            close(g_fd);
            return 0;
        }
    return 1;
}
static void spi_dev_config(void)
{
    int ret =0;
    mode |= SPI_CPHA;
    ret = ioctl(g_fd, SPI_IOC_WR_MODE, &mode);
    if (ret == -1)
    {
        pabort("Can't set spi Mode");
    }
    ret = ioctl(g_fd, SPI_IOC_WR_MAX_SPEED_HZ, &speed);
    if (ret == -1)
    {
        pabort("Can't set max speed hz");
    }
}
unsigned char gpib_r(unsigned char addr)
{
    gpib_spi_pack pack = { 0 };
    uint8_t tx_buf[2] = { 0 };
    uint8_t rx_buf[2] = { 0 };
    pack.idn = SPI_CMD_IDN_FLAG;
    pack.wr  = SPI_RW_IDN_RD;
    pack.addr = addr;
    tx_buf[0] = pack.payload;
    tx_buf[1] = 0;
    SpiRead(g_fd, tx_buf, rx_buf);
    // printf("rx_buf[0] = 0x%02x,rx_buf[1]=0x%02x\n", rx_buf[0], rx_buf[1]);
	return rx_buf[1];
}
void gpib_w(unsigned char addr,unsigned char data)
{
    gpib_spi_pack pack = { 0 };
    uint8_t tx_buf[2] = { 0 };
    uint8_t rx_buf[2] = { 0 };
    pack.idn = SPI_CMD_IDN_FLAG;
    pack.wr  = SPI_RW_IDN_WR;
    pack.addr = addr;
    tx_buf[0] = pack.payload;
    tx_buf[1] = data;
    SpiWrite(g_fd, tx_buf);
    // printf("write data: 0x%02x\n", tx_buf[1]);
}

int init_GPIB_device(void)
{
    int ret =0;
    // ret = Spiopen();
    // if(ret<0)
    //     return  ret;
    //spi_dev_config();
    index_gpibbuffer = 0;
	GPIB_state = idle_state;
    //IRQ_Disable(PFB_MASK_IRQ);
	/* ...Reset the 7210. Logically remove device from the GPIB. The device will
	* ignore GPIB activity until it is initialized.
	*/
	Aux(CMD_CHIP_RST);
	/* ...clear the status registers.*/
	isr0_byte = 0;
	isr1_byte = 0;
	/* ...set clock speed
	*/
	Aux(f_icr | f_8mhz);
	/* ...set T1 delay
	*/
	/* ...enable interrupts
	*/
	INT_Mask01(~b_bo_ie);
 	INT_Mask01(b_bi_ie | b_dcas_ie | b_get_ie);		
	//INT_Mask01(b_bo_ie | b_bi_ie | b_dcas_ie | b_get_ie);
	INT_Mask02(b_gint_ie);
	/* ...invert interrupts level
	*/
	Aux(f_accrb | b_inv);
//	Aux=f_accrb | 0x1f;
//	Aux=0x14;
	/* ...configure the NEWLINE character to end incoming and outgoing GPIB messages
	*/
	EndOfStr(NEWLINE);
	Aux(f_accra | b_xeos | b_reos);
	/* ...set the GPIB address of the device
	*/
	Addr_Mode(0x31);
	Addr_0_1(Get_GPIB_Addr());
	Addr_0_1(0xe0);/*only one address*/
	/* ...enable the device to receive data and commands from the GPIB network
	*/
	Aux(c_nswrst);

    return 0;
	//IRQ_Enable(PFB_MASK_IRQ);
}
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
void Set_GPIB_Addr()
{
	/* ...set the GPIB address of the device
	*/
	Addr_Mode(0x31);
	Addr_0_1(Get_GPIB_Addr());
	Addr_0_1(0xe0);/*only one address*/
}

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
unsigned short Get_GPIB_Addr(void)
{
	unsigned short gpib_address;
	/* read the GPIB address from nvram.*/
	gpib_address =3; //get from where ?g_stIO.strGPIB.u8Addr;//7;//nvram.GPIB_Addr;
	if(gpib_address > 30)
	{
		gpib_address = 30;
		//g_stIO.strGPIB.u8Addr = 30;
	}
	
	/*if(gpib_address < 1 )
	{
		gpib_address = 1;
		g_stIO.strGPIB.u8Addr = 1;
	}*/
	//Bug 编号 0004878 zzy modified 08-11-12
	if(gpib_address < 0 )
	{
		gpib_address = 0;
		//g_stIO.strGPIB.u8Addr = 0;
	}
	
	return (gpib_address);
}

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
void handle_DCAS_int(void) 
{
	/* ...reinitialize variables and buffers
	*/
	index_gpibbuffer = 0;
	GPIB_state = idle_state;

	Aux(c_rhdf);

	isr0_byte = 0x00;
	isr1_byte = 0x00;
	/* ...update serial poll response byte
	*/
	clear_status_byte_bits( b_mav |
			b_rsv |
			error_reading |
			error_writing |
			error_unknown_command);
	/* ...acknowledge command received and processed by releasing DAC holdoff
	*/
	Aux(c_nonvalid);
}


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
void handle_BI_int(void) 
{
	printf("Enter BI\n");
/*	
	if (SCP_InputBufferIsFull(SCP_GPIB) == 1)
	{
	   // 禁止PF中断
	   //UsbIntEnableCtrl(false);
	   
	   //不能直接屏蔽所有的PFB口上的中断，只需要屏蔽GPIB对应的中断信号即可 zzy modified 2011-05-27
	   *pFIO_MASKB_D &= ~PF_GPIB_MASK;
	   return;
	}*/
	
	int j = 0;
	/* ...update the gpib state
	*/
	if (GPIB_state == idle_state) {
		index_gpibbuffer = 0;
		GPIB_state = reading_state;
	}
	/* ...if the controller is sending data to the device while the device is trying to
	* send data to the controller, set an error flag in the status byte to warn the
	* controller.
	*/
	else if (GPIB_state == writing_state){
		clear_status_byte_bits(b_mav);
		set_status_byte_bits(b_rsv | error_reading);
		GPIB_state = reading_state;
		index_gpibbuffer = 0;
	}
	/* read data from the 7210 into the input buffer. To improve performance loop.70
	* instead of exiting the interrupt handler and then calling the interrupt handler
	* again.
	*/
	do{
		/* read the data into the buffer. If the buffer is full, then
		* the routine doesn't accept the new byte.
		*/
		
		
 		if(index_gpibbuffer < GPIB_BUFFER_SIZE) {
		   
		*(gpib_rx_buffer+index_gpibbuffer) = toupper(Data_In & 0xff);
		
	    index_gpibbuffer++;
	    
		}
		else
		{
			Data_In;	
		}
		/* check for another incoming byte unless the end of the string
		* is detected.*/
	
		//清除DI位，以通过下面程序判断下一状态是否仍为输入状态
		for (j=0;j<10;j++)
		{
			isr0_byte = isr0_byte & ~b_bi;
		}
		
//		if(!(isr0_byte & b_end))
		{
			isr0_byte = isr0_byte | (uint8_t) INT_Status01;
            
            // read the INT status 02 register, and clear the INT
	        isr1_byte = isr1_byte | (INT_Status02 & 0x0ff);
            
			//delay_us(500);
			/*if(isr0_byte&b_bi)
			{
				isr0_byte = isr0_byte & ~b_end;
			}*/
		}
		
	}while (isr0_byte&b_bi);
	  
  	/* ..if a complete message has been received, interpret before exiting*/
//	INT_Mask01(~b_bo_ie);//在读了一个新命令后暂时屏蔽BO中断 zzy added 11-17
//	INT_Mask01(b_bi_ie | b_dcas_ie | b_get_ie);//zzy added 11-17
	
	if (isr0_byte & b_end ) 
	{
		#if 1//由于命令解析中忘了调用打开的函数，所以暂时先屏蔽，用于调试，等调试完了之后再打开 zzy modified 09-03-27
		/*asm("ssync;");
		//*pFIO_MASKB_D	&= ~PF_GPIB_MASK;
		*pSIC_IMASK &= ~(0x00100000);//USB_INT_MASK; //屏蔽中断
		asm("ssync;");*/
		#endif
		
		if (index_gpibbuffer < GPIB_BUFFER_SIZE)
		{
			*(gpib_rx_buffer+index_gpibbuffer) = 0x00;
		}
		else
		{
			*(gpib_rx_buffer+index_gpibbuffer-1)= 0x00;
		}

		parse_input_buffer();//03950 mask
	}

	printf("rx_buffer: %s\n", gpib_rx_buffer);
	printf("index_gpibbuffer: %d\n", index_gpibbuffer);
	fflush(stdout);

}

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
void handle_BO_int(void) 
{
	short temp;
	short temp1;
	/* ...update the GPIB state
	*/
/*	INT_Mask01(~b_bi_ie);
 	INT_Mask01(b_bo_ie | b_dcas_ie | b_get_ie);	*/
	if (GPIB_state == idle_state) {
		/* ...only write if data has been written to the output buffer
		*/
		if (index_gpibbuffer == 0) {
			
		isr0_byte= isr0_byte & ~b_bo;
		isr0_byte = isr0_byte & ~b_end;//清除结束标志
	
		}
		else {
		index_gpibbuffer = 0;
		GPIB_state = writing_state;
		}
	}
	/* ...If the controller and the device are both trying to read data, set an error.71
	* flag in the serial poll response byte
	*/
	else if (GPIB_state == reading_state){
		set_status_byte_bits(error_writing);
		isr0_byte = isr0_byte & ~b_bo;
	}
	/* ...write the data bytes to the 7210 until the listener stops listening or the
	* device runs out of data. To improve performance loop instead of exiting the
	* interrupt handler and then calling the interrupt handler again.
	*/
	
	unsigned char *buf=(unsigned char *)gpib_tx_buffer;
	
	while (isr0_byte&b_bo)
	{
		/* ...output a byte from the output buffer
		*/		
		if(OutputDataEnabled==0)
		{
			
			Data_Out(*(gpib_tx_buffer+index_gpibbuffer));
			index_gpibbuffer++;
			/* ...check if 7210 is ready to send another byte
			*/
			isr0_byte = isr0_byte & ~b_bo;
			isr0_byte = isr0_byte | (uint8_t) INT_Status01;
			/* ...If the device is out of data, update the serial poll response register, stop
			* sending data and reset the buffer
			*/
			if (*(gpib_tx_buffer+index_gpibbuffer-1) == NEWLINE) 
			{
				clear_status_byte_bits(b_mav | b_rsv);
				GPIB_state = idle_state;
				index_gpibbuffer=0;
				isr0_byte = isr0_byte & ~b_bo;
			}
		}
		else
		{
			//unsigned char *buf=(unsigned char *)gpib_tx_buffer;
			
			if(OutputDataPtr<OutputDataSize)
			{
				Data_Out(buf[OutputDataPtr]);
				OutputDataPtr++;
			}
			else
			{
			 	Data_Out(NEWLINE);
			 	OutputDataPtr++;
			}
			
			//OutputDataPtr++;
			
			/*if(OutputDataPtr == 1028)
			{
				OutputDataPtr = OutputDataPtr;
			}*/
			
			/* ...check if 7210 is ready to send another byte
			*/
			
            tmp_isr01 = (uint8_t) INT_Status01;//调试信息，屏蔽 zzy modified 09-11-26
            //Aux(0x51);
            //tmp_bsr0 = (uint8_t) Addr_1;//调试信息，屏蔽 zzy modified 09-11-26
            //Aux(0x50);
			isr0_byte = isr0_byte & ~b_bo;
			isr0_byte = isr0_byte | tmp_isr01;//调试信息，屏蔽 zzy modified 09-11-26
            // read the INT status 02 register, and clear the INT
	        //isr1_byte = isr1_byte | (INT_Status02 & 0x0ff);
            
			/* ...If the device is out of data, update the serial poll response register, stop
			* sending data and reset the buffer
			*/
			if (OutputDataPtr>OutputDataSize) 
			{
				OutputDataEnabled=0;
				OutputDataPtr=0;
				OutputDataSize=0;
				
				clear_status_byte_bits(b_mav | b_rsv);
				GPIB_state = idle_state;
				isr0_byte = isr0_byte & ~b_bo;
				

				INT_Mask01(~b_bo_ie);
				INT_Mask01(b_bi_ie | b_dcas_ie | b_get_ie);
				
			}
		}
	}
	
	#if 0
	/*读数据的时候，需要做的判断 zzy added 08-11-12*/
	//如果有数据可读，正常读
	if(OutFlag == true)
	{
		OutFlag = false;//将输出标志清除
	}
	//如果没有数据可读
	else
	{
		//如果是查询命令，然后来读，没有任何响应
		if(g_bQueryCmd == true)
		{
		}
		//如果是设置命令，然后来读，产生查询错误
		else
		{
			ErrFlag = true;
			SCP_SetRegBit(g_u16ESR,ESR_QYE);
			strcpy(ErrText,"Query Unterminated");
			SCPI_ERRQUEUE_Excu(); 
		}
	}
	#endif
}

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
void handle_GET_trigger(void) 
{
/*
* insert code to implement the device specific trigger action
*/
/* release DAC holdoff to acknowledge to other routines and to the GPIB controller
* that the device specific trigger action has been completed.
*/
	isr1_byte = isr1_byte & ~b_get;
	Aux(c_nonvalid);
}

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
void set_status_byte_bits(uint8_t srq_byte) 
{
	uint8_t srq_response_byte;
	srq_response_byte = (uint8_t) Serial_Poll_Status;
	srq_response_byte = srq_response_byte | srq_byte;
	srq_response_byte = srq_response_byte & ~b_rsv;
	Serial_Poll_Mode ( srq_response_byte );
	/* ...use the rsv2 command to request a serial poll instead of the rsv bit. The rsv2
	* command clears itself after the serial poll and mixing rsv2 and rsv can cause
	* undefined behavior.
	*/
/*	if(srq_byte & b_rsv){
		Aux = c_rsv2;
	}
*/
}


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
void clear_status_byte_bits(uint8_t srq_byte)
{
	uint8_t srq_response_byte;
	srq_response_byte = (uint8_t) Serial_Poll_Status;
	srq_response_byte = srq_response_byte & ~srq_byte;
	srq_response_byte = srq_response_byte & ~b_rsv;
	Serial_Poll_Mode ( srq_response_byte);
	/* ...use the rsv2 command to stop requesting a serial poll instead of the rsv bit.
	* The rsv2 command clears itself after the serial poll and mixing rsv2 and rsv can
	* cause undefined behavior.
	*/
/*	if(srq_byte & b_rsv){
		Aux=c_nrsv2;
	}
*/
}
/*******************************************************************************
* 函数名: parse_input_buffer
* 描  述: 
* 输入参数: 
*
* 输出参数：
*
* 返回值: 
* 说 明：当接收到数据后解析
* parse the message and call the correct routine. Also, reset the buffer and set the
* device in idle state. If the command just received is invalid, send an error message
* to the GPIB controller.
*******************************************************************************/
void parse_input_buffer() 
{
	unsigned short ret=0;
	int i;
	/* ...check ranges on variables
	*/
	/* ...reset the state variables, since the device now has stopped reading GPIB data
	*/
	index_gpibbuffer = 0;
	GPIB_state = idle_state;
	
//	printf("command is %s\n",gpib_rx_buffer);
	
	/* ...parse the message in the input buffer
	*/
	
	for (i=0;i<10;i++)
	{
		isr0_byte = isr0_byte & ~b_end;
	}
	
 	//g_bRemoteOpMode = true;
 	i = strlen((const char*)gpib_rx_buffer);
 	if(*((char*)gpib_rx_buffer + i-1) == '\n')
 	{
 		*((char*)gpib_rx_buffer + i-1) = 0;
 	}
 					
 //	OutputDataEnabled = false;

 
 	/*if(u32CmdLen == 0)
 	{
 		memset(CmdBuffer,'\0',0x50000);
 		u32CmdLen = strlen((char*)gpib_rx_buffer);
 		strcpy(CmdBuffer,(char*)gpib_rx_buffer);
 		ret=Parse_cmd((char*)(CmdBuffer),IOTYPE_GPIB);
 	}
 	else
 	{
 		strcat(CmdBuffer,(char*)gpib_rx_buffer);
 		ret=Parse_cmd((char*)(CmdBuffer + u32CmdLen),IOTYPE_GPIB);
 		u32CmdLen += strlen((char*)gpib_rx_buffer);
 		if(u32CmdLen > 0x50000)
 		{
 			u32CmdLen = 0;
 		}
 	}
 	
    u32CmdNumber++;
    */
 	
	ret = Parse_cmd((char*)gpib_rx_buffer);
}
/*******************************************************************************
* 函数名: GPIB_Write_Buf
* 描  述: 
* 输入参数: 
*
* 输出参数：
*
* 返回值: 
* 说 明：
* 将数据保存到发送BUF中，然后启动GPIB发送数据
*******************************************************************************/
void GPIB_Write_Buf(unsigned char flag,char* data)
{
	OutputDataEnabled = 0;
	OutputDataPtr = 0;
	strcpy((char *)gpib_tx_buffer,(const char*)data);	/* 复制到发送BUF中 */
	OutputDataSize = strlen(data);
	GPIB_state = writing_state;
	OutputDataEnabled=1;
	index_gpibbuffer=0;
	
	INT_Mask01(b_bo_ie | b_bi_ie | b_dcas_ie | b_get_ie);
#if 0
	if(flag==NULL)
	{
		/* ...terminate the data string and request service now that the data is
		* in the buffer
		*/
		//output_data_to_io_buffer(NEWLINE);
		set_status_byte_bits(b_mav | b_rsv);
	}
	else
	{
		/* ...if the device received an unrecognized command, send an error
		* message
		*/
		set_status_byte_bits(b_rsv | error_unknown_command);
	}
#endif	
}

void gpib_irq_route_7210_interrupts(void) 
{
	/* route_7210_interrupts handles the hardware interrupt from the 7210.
	* It determines what caused the interrupt and calls the appropriate function.
	* If no interrupts are pending, then it does nothing.
	*/
	/* ...read isr0 and isr1, the values must be saved because the act of reading the bits
	* in the registers clears the bits.
	*/
	
#if 1//调试信息，屏蔽 zzy modified 09-11-26
    //set the register page to page 0
    Aux(0x50);
    //read the Address Status Register (4R page 0)
    tmp_addr_status = (uint8_t) Addr_Status;
    
    //set the register page to page 1
    Aux(0x51);
    //read the State Register (4R page 1)
    tmp_state1 = (uint8_t) Addr_Status;
    //set the register page to page 2
    Aux(0x52);
    //read the State Register (4R page 2)
    tmp_state2 = (uint8_t) Addr_Status;
    //set the register page to page 3
    Aux(0x53);
    //read the State Register (4R page 3)
    tmp_state3 = (uint8_t) Addr_Status;
    //set the register page to page 4
    Aux(0x54);
    //read the State Register (4R page 4)
    tmp_state4 = (uint8_t) Addr_Status;
    //set the register page to page 0 (default)
    Aux(0x50);
    
    //set the register page to page 1
    Aux(0x51);
    //read the Bus State Register (7R page 1)
    tmp_bsr0 = (uint8_t) Addr_1;
    //set the register page to page 0 (default)
    Aux(0x50);
#endif

    tmp_isr00 = (uint8_t) (INT_Status01 & 0xff);//调试信息，屏蔽 zzy modified 09-11-26
	isr0_byte = isr0_byte | tmp_isr00;//调试信息，屏蔽 zzy modified 09-11-26
	isr1_byte = isr1_byte | (INT_Status02 & 0x0ff);

	/* ...determine the cause of the interrupt and handle it
	*/

	printf("isr0_byte: 0x%x\n", isr0_byte);
	fflush(stdout);
		
	if (isr0_byte & b_bi) 
	{
		handle_BI_int();
	}
	
	if (isr0_byte & b_bo)
	{
		handle_BO_int();
		//u16IntCnt++;
	}
	
	if (isr0_byte & b_get)
	{
		handle_GET_trigger();
	}
	
	if (isr0_byte & b_dcas)
	{
		handle_DCAS_int();
	}
		
	
}


unsigned short Parse_cmd(unsigned char* cmd_str)
{
	if (strcmp(gpib_rx_buffer, "*IDN?") == 0)
	{
		printf("Receive IDN\n");
		strcpy(gpib_tx_buffer, "RIGOL001\n");
		SendParseDataOut_GPIB();
	}
	return 0;
}

void SendParseDataOut_GPIB()
{
	OutputDataPtr = 0;
	OutputDataSize = strlen(gpib_tx_buffer);
	GPIB_state = writing_state;
	OutputDataEnabled = 1;
	index_gpibbuffer = 0;
	INT_Mask01(b_bo_ie | b_bi_ie | b_dcas_ie | b_get_ie);
}


static int Gpib_irq_open(void)
{
    g_gpib_irq_fd = open(gpib_irq_device, O_RDWR | O_NOCTTY | O_NDELAY);
    if (g_gpib_irq_fd <= 0)
        {return -1;}
    return 0;        
}
static int Gpib_irq_close(void)
{   
    if(g_gpib_irq_fd >0)   
        {      
            close(g_gpib_irq_fd );
            return 0;
        }
    return 1;
}
//g_gpib_irq_fd
int main(int argc, char *argv[])
{
    int fd = 0;
    int ret = 0;
    uint8_t addr = 0;
    uint8_t data = 0;
    gpib_spi_pack pack = { 0 };
    if(Gpib_irq_open() < 0)
        {
            printf("open gpib irq failed \n");
            return -1;
        }
    if(Spiopen() < 0)
        {
            printf("open spi dev failed \n");
            return -1;
        }
    spi_dev_config();

    //ret = init_GPIB_device();
/*    if(ret <0)
        {
            printf("init gpib failed\n ");
            return -2
        }
*/
    uint8_t tx_buf[2] = { 0 };
    uint8_t rx_buf[2] = { 0 };

    if (argc == 2)
    {
        // Read
        addr = strtoul(argv[1], 0, 0);

        pack.idn = SPI_CMD_IDN_FLAG;
        pack.wr  = SPI_RW_IDN_RD;
        pack.addr = addr;

        tx_buf[0] = pack.payload;
        tx_buf[1] = 0;

        SpiRead(g_fd, tx_buf, rx_buf);
        printf("rx_buf = 0x%02x%02x\n", rx_buf[0], rx_buf[1]);
        Spiclose();
        Gpib_irq_close();
        return 0;
    }
    else if (argc == 3)
    {
        // Write
        addr = strtoul(argv[1], 0, 0);
        data = strtoul(argv[2], 0, 0);

        pack.idn = SPI_CMD_IDN_FLAG;
        pack.wr  = SPI_RW_IDN_WR;
        pack.addr = addr;

        tx_buf[0] = pack.payload;
        tx_buf[1] = data;

        SpiWrite(g_fd, tx_buf);
        Spiclose();
        Gpib_irq_close();
        return 0;
    }
    ret = init_GPIB_device();
    ioctl(g_gpib_irq_fd,IOCTL_IRQ_CMD,1);
  /* else
    {
        printf("Usage:\n");
        printf("Use ( ./spi_test.bin addr ) to Read\n");
        printf("Use ( ./spi_test.bin addr data) to Write\n");
    }*/
    int sum = 0;
    struct pollfd fds;
	//fd = open("/dev/gpib_irq",O_RDWR);
	fds.fd = g_gpib_irq_fd;
	fds.events = POLLIN;
    while(1)
	{
		ret = poll(&fds, 1, -1);//阻塞等待中断发生
		if (ret <= 0)
        	{
     			printf("Poll error, ret = %d\n", ret);
    		}
		else
		{   
            printf("trig\n");
			read(g_gpib_irq_fd, &ret, 1);//中断发生后进入此分支，通过read方法清空可读内容并重置中断标志
           // printf("111trig\n");
            ioctl(g_gpib_irq_fd,IOCTL_IRQ_CMD,0);
            printf("trig1\n");
			gpib_irq_route_7210_interrupts();
            ioctl(g_gpib_irq_fd,IOCTL_IRQ_CMD,1);
            printf("trig2\n");
			sum++;
			printf("%d\n", sum);
			
			//if(sum % 10000 ==0) printf("%d\n", sum);//频率测试用
			//ioctl(fd, IOCTL_VM_DONE);//高电平持续20ms，故无法以高于50Hz的频率执行
			//ioctl(fd, IOCTL_TRIG_MODE, IRQ_TYPE_EDGE_FALLING);//其余可选参数：IRQ_TYPE_EDGE_RISING，IRQ_TYPE_EDGE_BOTH，IRQ_TYPE_LEVEL_HIGH，IRQ_TYPE_LEVEL_LOW
		}
	}
    Spiclose();
    Gpib_irq_close();
    return 0;
}


/*
int main(int argc, char *argv[])
{
    int fd = 0;
    int ret = 0;
    uint8_t addr = 0;
    uint8_t data = 0;
    gpib_spi_pack pack = { 0 };

    fd = open(device, O_RDWR | O_NOCTTY | O_NDELAY);
    if (fd < 0)
    {
        pabort("Can't Open Device");
    }

    mode |= SPI_CPHA;
    ret = ioctl(fd, SPI_IOC_WR_MODE, &mode);
    if (ret == -1)
    {
        pabort("Can't set spi Mode");
    }
    ret = ioctl(fd, SPI_IOC_WR_MAX_SPEED_HZ, &speed);
    if (ret == -1)
    {
        pabort("Can't set max speed hz");
    }

    uint8_t tx_buf[2] = { 0 };
    uint8_t rx_buf[2] = { 0 };

    if (argc == 2)
    {
        // Read
        addr = strtoul(argv[1], 0, 0);

        pack.idn = SPI_CMD_IDN_FLAG;
        pack.wr  = SPI_RW_IDN_RD;
        pack.addr = addr;

        tx_buf[0] = pack.payload;
        tx_buf[1] = 0;

        SpiRead(fd, tx_buf, rx_buf);
        printf("rx_buf = 0x%02x%02x\n", rx_buf[0], rx_buf[1]);
    }
    else if (argc == 3)
    {
        // Write
        addr = strtoul(argv[1], 0, 0);
        data = strtoul(argv[2], 0, 0);

        pack.idn = SPI_CMD_IDN_FLAG;
        pack.wr  = SPI_RW_IDN_WR;
        pack.addr = addr;

        tx_buf[0] = pack.payload;
        tx_buf[1] = data;

        SpiWrite(fd, tx_buf);
    }
    else
    {
        printf("Usage:\n");
        printf("Use ( ./spi_test.bin addr ) to Read\n");
        printf("Use ( ./spi_test.bin addr data) to Write\n");
    }

    close(fd);

    return 0;
}
*/

















