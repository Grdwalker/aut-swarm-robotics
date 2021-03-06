/*
* twimux_interface.h
*
* Author : Esmond Mather and Matthew Witt
* Created: 11/07/2017 10:03:32 AM
*
* Project Repository:https://github.com/AdamParlane/aut-swarm-robotics
*
* Defines macros for TWI0 status, control and multiplexer addresses. Function definitions for TWI0
* and mux
*
* More Info:
* Atmel SAM 4N Processor Datasheet:http://www.atmel.com/Images/Atmel-11158-32-bit%20Cortex-M4-Microcontroller-SAM4N16-SAM4N8_Datasheet.pdf
* Farnell I2C Mux Datasheet:http://www.farnell.com/datasheets/1815500.pdf
*
* Functions:
* void twi0Init(void)
* void twi2Init(void)
* uint8_t twi0MuxSwitch(uint8_t channel)
* uint8_t twi0ReadMuxChannel(void)
* char twi0Write(unsigned char slave_addr, unsigned char reg_addr,
*						unsigned char length, unsigned char const *data)
* char twi2Write(unsigned char slave_addr, unsigned char reg_addr,
*						unsigned char length, unsigned char const *data)
* char twi0Read(unsigned char slave_addr, unsigned char reg_addr,
*						unsigned char length,	unsigned char *data)
* char twi2Read(unsigned char slave_addr, unsigned char reg_addr,
*						unsigned char length,	unsigned char *data)
*
*/

#ifndef TWIMUX_INTERFACE_H_
#define TWIMUX_INTERFACE_H_

//////////////[Includes]////////////////////////////////////////////////////////////////////////////
#include "../robot_setup.h"

//////////////[Defines]/////////////////////////////////////////////////////////////////////////////
////TWI status register timeout values (ms)
#define TWI_RXRDY_TIMEOUT	100
#define TWI_TXRDY_TIMEOUT	100
#define TWI_TXCOMP_TIMEOUT	100
#define TWI_RXRDY_RETRY		3

////General Commands
//if returns 1, then the receive holding register has a new byte to be read
#define twi0RxReady			(REG_TWI0_SR & TWI_SR_RXRDY)
#define twi2RxReady			(REG_TWI2_SR & TWI_SR_RXRDY)

//if returns 1, then the transmit holding register empty or not acknowledged error occurred
#define twi0TxReady			(REG_TWI0_SR & TWI_SR_TXRDY)
#define twi2TxReady			(REG_TWI2_SR & TWI_SR_TXRDY)

//returns 1 when transmission is complete
#define twi0TxComplete		(REG_TWI0_SR & TWI_SR_TXCOMP)
#define twi2TxComplete		(REG_TWI2_SR & TWI_SR_TXCOMP)

//returns 1 when not acknowledged error occurred
#define twi0NotAcknowledged	(REG_TWI0_SR & TWI_SR_NACK)
#define twi2NotAcknowledged	(REG_TWI2_SR & TWI_SR_NACK)

//Enable master mode and disable slave mode
#define twi0MasterMode		(REG_TWI0_CR |= TWI_CR_MSEN|TWI_CR_SVDIS)
#define twi2MasterMode		(REG_TWI2_CR |= TWI_CR_MSEN|TWI_CR_SVDIS)

//Enable slave mode (disable master mode)
#define twi0SlaveMode		(REG_TWI0_CR |= TWI_CR_MSDIS|TWI_CR_SVEN)
#define twi2SlaveMode		(REG_TWI2_CR |= TWI_CR_MSDIS|TWI_CR_SVEN)

//Set address of desired slave device to talk to
#define twi0SetSlave(value)	(REG_TWI0_MMR = TWI_MMR_DADR(value))
#define twi2SetSlave(value)	(REG_TWI2_MMR = TWI_MMR_DADR(value))

//Slave access flags (1 when master is trying to access this device as slave)
#define twi0SlaveAccess		(REG_TWI0_SR & TWI_SR_SVACC)
#define twi2SlaveAccess		(REG_TWI2_SR & TWI_SR_SVACC)

//Is master trying to read from us?
#define twi0SlaveReadMode	(REG_TWI0_SR & TWI_SR_SVREAD)
#define twi2SlaveReadMode	(REG_TWI2_SR & TWI_SR_SVREAD)

//End of slave access flag
#define twi0EndSlaveAccess	(REG_TWI0_SR & TWI_SR_EOSACC)
#define twi2EndSlaveAccess	(REG_TWI2_SR & TWI_SR_EOSACC)

//Transmit holding register
#define twi0Send(value)		(REG_TWI0_THR = value)
#define twi2Send(value)		(REG_TWI2_THR = value)

//Receive holding register
#define twi0Receive			REG_TWI0_RHR
#define twi2Receive			REG_TWI2_RHR

//Initiates single byte data read
#define twi0StartSingle		(REG_TWI0_CR = TWI_CR_START|TWI_CR_STOP)
#define twi2StartSingle		(REG_TWI2_CR = TWI_CR_START|TWI_CR_STOP)

//Initiates multi byte data read
#define twi0Start			(REG_TWI0_CR = TWI_CR_START)
#define twi2Start			(REG_TWI2_CR = TWI_CR_START)

//Stops data transmission after next byte
#define twi0Stop			(REG_TWI0_CR |= TWI_CR_STOP)
#define twi2Stop			(REG_TWI2_CR |= TWI_CR_STOP)

//Call this macro to read from slave registers
#define twi0SetReadMode		(REG_TWI0_MMR |= TWI_MMR_MREAD)
#define twi2SetReadMode		(REG_TWI2_MMR |= TWI_MMR_MREAD)

//Address of slave internal register to read/write
#define twi0RegAddr(value)	(REG_TWI0_IADR = value)
#define twi2RegAddr(value)	(REG_TWI2_IADR = value)

//Set slave register address size (0-3 bytes) 0 means no internal registers
#define twi0RegAddrSize(value) (REG_TWI0_MMR |= TWI_MMR_IADRSZ(value))
#define twi2RegAddrSize(value) (REG_TWI2_MMR |= TWI_MMR_IADRSZ(value))

//Perform software reset
#define twi0Reset			(REG_TWI0_CR = TWI_CR_SWRST)
#define twi2Reset			(REG_TWI2_CR = TWI_CR_SWRST)

////Device slave addresses
#define TWI0_MUX_ADDR				0x70		//Mux Address 000
#define TWI0_LIGHTSENS_ADDR			0x10		//Light sensors
#define TWI0_PROXSENS_ADDR			0x39		//Proximity sensors
#define TWI0_FCHARGE_ADDR			0x6B		//Battery Charger (Fast Charge Controller)
#define TWI0_IMU_ADDR				0x68		//IMU
#define TWI2_IMU_ADDR				0x68		//IMU
#define TWI2_SLAVE_ADDR				0x44		//This devices' slave address

////TWI Mux channels
//Only one active at a time
#define MUX_LIGHTSENS_R				0xF8		//Mux Channel 0, Side Panel A
#define MUX_LIGHTSENS_L				0xF9		//Mux Channel 1, Side Panel A
#define MUX_PROXSENS_A				0xFA		//Mux Channel 2, Side Panel A
#define MUX_PROXSENS_B				0xFF		//Mux Channel 7, Side Panel B
#define MUX_PROXSENS_C				0xFE		//Mux Channel 6, Side Panel C
#define MUX_PROXSENS_D				0xFD		//Mux Channel 5, Side Panel D
#define MUX_PROXSENS_E				0xFC		//Mux Channel 4, Side Panel E
#define MUX_PROXSENS_F				0xFB		//Mux Channel 3, Side Panel F


//TWI logging
#define TWI_LOG_NUM_ENTRIES			20			//Number of entries to log in the TWI event log.

//////////////[Type Definitions]////////////////////////////////////////////////////////////////////

//Operation results for TWI operations
typedef enum TwiErrorFlags
{
	TWIERR_NONE,		//Successful
	TWIERR_TXRDY,		//Timed out waiting for TXRDY
	TWIERR_RXRDY,		//Timed out waiting for RXRDY
	TWIERR_TXCOMP		//Timed out waiting for TXCOMP
} TwiErrorFlags;


typedef struct TwiEvent
{
	uint32_t timeStamp;					//Time of operation
	uint8_t twiBusNumber;				//TWI peripheral number (0, 1, 2)
	uint8_t slaveAddress;				//Slave device address
	uint8_t regAddress;					//Internal address
	uint8_t readOp;						//Direction, 1 = read 0 = write
	uint8_t transferLen;				//Number of bytes being transferred
	uint8_t bytesTransferred;			//Number of bytes successfully transferred.
	TwiErrorFlags operationResult;		//Outcome of operation.
} TwiEvent;



//////////////[Functions]///////////////////////////////////////////////////////////////////////////
/*
* Function:
* void twi0Init(void)
*
* Initialises TWI0. Master clock should be setup first.
*
* Inputs:
* none
*
* Returns:
* none
*
*/
void twi0Init(void);

/*
* Function:
* void twi2Init(void);
*
* Initialises TWI2. Master clock should be setup first.
*
* Inputs:
* none
*
* Returns:
* none
*
*/
void twi2Init(void);

/*
* Function:
* uint8_t twi0MuxSwitch(uint8_t channel)
*
* Sets the I2C multiplexer to desired channel.
*
* Inputs:
* uint8_t channel: Mux channel number 0x8 (Ch0) to 0xF (Ch7)
*
* Returns:
* none
*
*/
uint8_t twi0MuxSwitch(uint8_t channel);

/*
* Function:
* uint8_t twi0ReadMuxChannel(void)
*
* Returns the channel the Mux is currently set to.
*
* Inputs:
* none
*
* Returns:
* a byte that holds a number from 0x8 (Ch0) to 0xF (Ch7) representing the current channel
*
*/
uint8_t twi0ReadMuxChannel(void);

/*
* Function: char twiNWrite(unsigned char slave_addr, unsigned char reg_addr,
*								unsigned char length, unsigned char const *data)
*
* Writes bytes out to TWI devices. Allows multiple bytes to be written at once if desired
*
* Inputs:
* slave_addr is the address of the device to be written to on TWIn. reg_addr is the
* 8bit address of the register being written to. length is the number of bytes to be written. *data
* points to the data bytes to be written.
*
* Returns:
* returns 0 on success. otherwise will return 1 on timeout
*
* Implementation:
* Master mode on TWIn is enabled, TWIn is prepared for transmission ie slave and register addresses
* are set and register address size is set to 1 byte. Next, transmission takes place but there are
* slightly different procedures for single and multi byte transmission. On single byte
* transmission, the STOP state is set in the TWI control register immediately after the byte to be
* sent is loaded into the transmission holding register. On multi-byte transmission, the STOP
* flag isn't set until all bytes have been sent and the transmission holding register is clear.
*
*/
char twi0Write(unsigned char slave_addr, unsigned char reg_addr,
					unsigned char length, unsigned char const *data);
char twi2Write(unsigned char slave_addr, unsigned char reg_addr,
					unsigned char length, unsigned char const *data);

/*
* Function: char twiNRead(unsigned char slave_addr, unsigned char reg_addr,
*								unsigned char length, unsigned char const *data)
*
* TWI interface read functions. Allows reading multiple bytes sequentially
*
* Inputs:
* slave_addr is the address of the device to be read from on TWIn. The address varies even for the
* IMU driver because the IMU and compass have different TWI slave addresses. reg_addr is the address
* of the register being read from. length is the number of bytes to be read. The IMU automatically
* increments the register address when reading more than one byte. *data points to the location in
* memory where the retrieved data will be stored.
*
* Returns:
* returns 0 on success.
*
*/
char twi0Read(unsigned char slave_addr, unsigned char reg_addr,
					unsigned char length, unsigned char *data);
char twi2Read(unsigned char slave_addr, unsigned char reg_addr,
					unsigned char length, unsigned char *data);

//Logs TWI0 transfer events
uint8_t twi0LogEvent(TwiEvent event);

#endif /* TWIMUX_INTERFACE_H_ */