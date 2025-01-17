#ifndef AMKINVERTER_CAN_H_
#define AMKINVERTER_CAN_H_

#include <Ifx_Types.h>
#include "Configuration.h"
#include "ConfigurationIsr.h"
#include <stdint.h>
#include "Multican.h"
#include "CanCommunication.h"

#define AMK_TORQUE_LIM  2143
#define AMK_MAXIMUM_SPEED   18000

typedef struct
{
	uint8 AMK_bReserve : 8;
	boolean AMK_bSystemReady :1;
	boolean AMK_bWarn : 1;
	boolean AMK_bSError : 1;
	boolean AMK_bQuitDcOn : 1;
	boolean AMK_bDcOn : 1;
	boolean AMK_bQuitInverterOn : 1;
	boolean AMK_bInverterOn : 1;
	boolean AMK_bDerating : 1;
}AMK_Status;

typedef struct
{
	uint8 AMK_bReserve1 : 8;
	boolean AMK_bInverterOn : 1;
	boolean AMK_bDcOn : 1;
	boolean AMK_bEnable : 1;
	boolean AMK_bErrorReset : 1;
	uint8 AMK_bReserve2 : 4;
}AMK_Control;

typedef union
{
	uint32 RecievedData[2];
	struct{
		uint8 AMK_bReserve : 8;
		boolean AMK_bSystemReady : 1;
		boolean AMK_bSError : 1;
		boolean AMK_bWarn : 1;
		boolean AMK_bQuitDcOn : 1;
		boolean AMK_bDcOn : 1;
		boolean AMK_bQuitInverterOn : 1;
		boolean AMK_bInverterOn : 1;
		boolean AMK_bDerating : 1;
		sint16 AMK_ActualVelocity : 16;
		sint16 AMK_TorqueCurrent : 16;
		sint16 AMK_MagnetizingCurrent : 16;
	}S;
}amkActualValues1;

typedef union
{
	uint32 RecievedData[2];
	    struct{
			sint16 AMK_TempMotor : 16;
	        sint16 AMK_TempInverter : 16;
	        uint16 AMK_ErrorInfo : 16;
	        sint16 AMK_TempIGBT : 16;
	    }S;
}amkActualValues2;

typedef union
{
	uint32 TransmitData[2];
	    struct{
			uint8 AMK_bReserve1 : 8;
			boolean AMK_bInverterOn : 1;
			boolean AMK_bDcOn : 1;
			boolean AMK_bEnable : 1;
			boolean AMK_bErrorReset : 1;
			uint8 AMK_bReserve2 : 4;
	    	sint16 AMK_TargetVelocity: 16;	//Should be changed to index6 (1/rpm) 
	        sint16 AMK_TorqueLimitPositv : 16;
	        sint16 AMK_TorqueLimitNegativ : 16;
	    }S;
}amkSetpoint1;

typedef union{
	uint32 TransmitData[2];
	struct{
		uint16 EFon;
		uint16 BE1on;
		uint16 BE2on;
		uint16 Remain;
	}B;
}Inv_switch_msg_t;


typedef struct
{
	uint16 ID_AMK_Ac1;
	uint16 ID_AMK_Ac2;
	uint16 ID_AMK_Set;
}ID_set;


typedef enum AmkState_e
{
    AmkState_S0 = 0,    //Power On
    AmkState_S1 = 1,    //System Ready
    AmkState_S2 = 2,    //DC On
    AmkState_S3 = 3,    //EF on
    AmkState_S4 = 4,    //BE1 on
    AmkState_S5 = 5,    //Enable/InverterEnable on
    AmkState_RTD    //Ready To Drive
} AmkState_t;

typedef struct 
{
	AmkState_t r2d;

	float32 fl;
	float32 fr;
	float32 rl;
	float32 rr;

	float32 spdFl;
	float32 spdFr;
	float32 spdRl;
	float32 spdRr;

	boolean brakeOn;

	IfxCpu_mutexLock mutex;
}AmkInverterPublic_t;

struct Monitor
{
    int InverterTemp;
    struct {
        uint16 error_FL;
        uint16 error_RL;
        uint16 error_RR;
        uint16 error_FR;
    }InverterErrorState;
    struct {
        sint16 temp_FL;
        sint16 temp_RL;
        sint16 temp_RR;
        sint16 temp_FR;
    }MotorTemp;
    struct{
        sint16 velocity_FL;
        sint16 velocity_RL;
        sint16 velocity_RR;
        sint16 velocity_FR;
    } MotorVelocity;
    // struct MotorCurrent{
    //     uint16 velocity_RL;
    //     uint16 velocity_FL;
    //     uint16 velocity_RR;
    //     uint16 velocity_FR;
    // }
};

typedef struct 
{
	struct Monitor monitor;
	IfxCpu_mutexLock mutex;
} AmkInverterMonitorPublic_t;
 
 /*************************** For logger ****************************/
typedef union AmkSetpoint1_FL
{
    // infineon tc (VCU)
    // -> uint32 tx_data[2];
    uint32 tx_data[2];
    struct
    {
        uint8 AMK_bReserve1           :8;         //[0-7]
        boolean AMK_bInverterOn            :1;         //[8]
        boolean AMK_bDcOn                  :1;         //[9]
        boolean AMK_bEnable                :1;         //[10]
        boolean AMK_bErrorReset            :1;         //[11]
        uint8 AMK_bReserve2           :4;         //[12-15]
		sint16 AMK_Torque_setpoint    :16;	    //[16-31]
		sint16 AMK_TorqueLimitPositv  :16;        //[32-47]
		sint16 AMK_TorqueLimitNegativ :16;        //[48-63]
    } s;
} amk_setpoint1_fl_t;



/******* Logging *******/
// id 0x40101
// amk_setpoint1_fr
// VCU to LOGGER

typedef union AmkSetpoint1_FR
{
    uint32 tx_data[2];
    struct
    {
        uint8 AMK_bReserve1           :8;         //[0-7]
        boolean AMK_bInverterOn            :1;         //[8]
        boolean AMK_bDcOn                  :1;         //[9]
        boolean AMK_bEnable                :1;         //[10]
        boolean AMK_bErrorReset            :1;         //[11]
        uint8 AMK_bReserve2           :4;         //[12-15]
		sint16 AMK_Torque_setpoint    :16;	    //[16-31]
		sint16 AMK_TorqueLimitPositv  :16;        //[32-47]
		sint16 AMK_TorqueLimitNegativ :16;        //[48-63]
    } s;
} amk_setpoint1_fr_t;

// id 0x40102
// amk_actual_values1_fl
// VCU to LOGGER

typedef union AmkActualValues1_FL
{
    uint32 tx_data[2];
    struct
    {
        uint8 AMK_bReserve                    :8;         //[0-7]
        boolean AMK_bSystemReady                   :1;         //[8]
        boolean AMK_bSError                        :1;         //[9]
        boolean AMK_bWarn                          :1;         //[10]
        boolean AMK_bQuitDcOn                      :1;         //[11]
        boolean AMK_bDcOn                          :1;         //[12]
        boolean AMK_bQuitInverterOn                :1;         //[13]
        boolean AMK_bInverterOn                    :1;         //[14]
        boolean AMK_bDerating                      :1;         //[15]
        sint16 AMK_ActualVelocity             :16;        //[16-31]
        sint16 AMK_TorqueCurrent              :16;        //[32-47]
        sint16 AMK_MagnetizingCurrent         :16;        //[48-63]
    } s;
} amk_actual_values1_fl_t;

// id 0x40103
// amk_actual_values1_fr
// VCU to LOGGER

typedef union AmkActualValues1_FR
{
    uint32 tx_data[2];
    struct
    {
        uint8 AMK_bReserve                    :8;     //[0-7]
        boolean AMK_bSystemReady                   :1;     //[8]
        boolean AMK_bSError                        :1;     //[9]
        boolean AMK_bWarn                          :1;     //[10]
        boolean AMK_bQuitDcOn                      :1;     //[11]
        boolean AMK_bDcOn                          :1;     //[12]
        boolean AMK_bQuitInverterOn                :1;     //[13]
        boolean AMK_bInverterOn                    :1;     //[14]
        boolean AMK_bDerating                      :1;     //[15]
        sint16 AMK_ActualVelocity             :16;    //[16-31]
        sint16 AMK_TorqueCurrent              :16;    //[32-47]
        sint16 AMK_MagnetizingCurrent         :16;    //[48-63]
    } s;
} amk_actual_values1_fr_t;

// id 0x40104
// amk_actual_values2_fl
// VCU to LOGGER

typedef union AmkActualValues2_FL
{
    uint32 tx_data[2];
    struct
    {
	    sint16 AMK_TempMotor      :16;    //[0-15]
		sint16 AMK_TempInverter   :16;    //[16-31]
		uint16 AMK_ErrorInfo      :16;    //[32-47]
		sint16 AMK_TempIGBT       :16;    //[48-63]
    } s;
} amk_actual_values2_fl_t;

// id 0x40105
// amk_actual_values2_fr
// VCU to LOGGER

typedef union AmkActualValues2_FR
{
    uint32 tx_data[2];
    struct
    {
	    sint16 AMK_TempMotor      :16;    //[0-15]
		sint16 AMK_TempInverter   :16;    //[16-31]
		uint16 AMK_ErrorInfo      :16;    //[32-47]
		sint16 AMK_TempIGBT       :16;    //[48-63]
    } s;
} amk_actual_values2_fr_t;
/******* Logging ********/



/*********************************************************************/

IFX_EXTERN AmkInverterPublic_t AmkInverterPublic;

IFX_EXTERN AmkInverterMonitorPublic_t AmkInverterMonitorPublic;

IFX_EXTERN Inv_switch_msg_t Inv_switch_msg;
IFX_EXTERN amkActualValues1 INV_FL_AMK_Actual_Values1;
IFX_EXTERN amkActualValues1 INV_RL_AMK_Actual_Values1;
IFX_EXTERN amkActualValues1 INV_RR_AMK_Actual_Values1;
IFX_EXTERN amkActualValues1 INV_FR_AMK_Actual_Values1;

IFX_EXTERN amkActualValues2 INV_FL_AMK_Actual_Values2;
IFX_EXTERN amkActualValues2 INV_RL_AMK_Actual_Values2;
IFX_EXTERN amkActualValues2 INV_RR_AMK_Actual_Values2;
IFX_EXTERN amkActualValues2 INV_FR_AMK_Actual_Values2;


IFX_EXTERN  amkSetpoint1 INV1_AMK_Setpoint1;
IFX_EXTERN amkSetpoint1 INV2_AMK_Setpoint1;
IFX_EXTERN amkSetpoint1 INV3_AMK_Setpoint1;
IFX_EXTERN amkSetpoint1 INV4_AMK_Setpoint1;

IFX_EXTERN ID_set Inverter1;
IFX_EXTERN ID_set Inverter2;
IFX_EXTERN ID_set Inverter3;
IFX_EXTERN ID_set Inverter4;

IFX_EXTERN AmkState_t AmkState;

IFX_EXTERN amk_setpoint1_fl_t amk_setpoint1_fl;
IFX_EXTERN amk_setpoint1_fr_t amk_setpoint1_fr;
IFX_EXTERN amk_actual_values1_fl_t amk_actual_values1_fl;
IFX_EXTERN amk_actual_values1_fr_t amk_actual_values1_fr;
IFX_EXTERN amk_actual_values2_fl_t amk_actual_values2_fl;
IFX_EXTERN amk_actual_values2_fr_t amk_actual_values2_fr;

IFX_EXTERN void AmkInverter_can_init(void);
IFX_EXTERN void AmkInverter_can_Run(void);
IFX_EXTERN void AmkInverter_can_write(amkSetpoint1 *INV, CanCommunication_Message TC, uint16 tV);
IFX_EXTERN void AmkInverter_writeMessage(uint16 Value1, uint16 Value2);
IFX_EXTERN void AmkInverter_writeMessage2(uint16 Value1, uint16 Value2);
IFX_EXTERN void InverterControlSet();
IFX_EXTERN void AmkInverter_Start(boolean rtdFlag);

#endif 
