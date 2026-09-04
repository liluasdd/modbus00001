#ifndef    USER_APP
#define USER_APP
/* ----------------------- Modbus includes ----------------------------------*/
#include "mb.h"
//#include "mb_m.h"
#include "mbconfig.h"
#include "mbframe.h"
#include "mbutils.h"

/* -----------------------Slave Defines -------------------------------------*/
#define S_DISCRETE_INPUT_START                    0		
#define S_DISCRETE_INPUT_NDISCRETES               16
#define S_COIL_START                              0
#define S_COIL_NCOILS                             64
#define S_REG_INPUT_START                         0
#define S_REG_INPUT_NREGS                         125
#define S_REG_HOLDING_START                       1
#define S_REG_HOLDING_NREGS                       125
/* salve mode: holding register's all address */
#define          S_HD_RESERVE                     0
/* salve mode: input register's all address */
#define          S_IN_RESERVE                     0
/* salve mode: coil's all address */
#define          S_CO_RESERVE                     0
/* salve mode: discrete's all address */
#define          S_DI_RESERVE                     0

/* -----------------------Master Defines -------------------------------------*/
#define M_DISCRETE_INPUT_START                    M_DISCRETE_INPUT_START
#define M_DISCRETE_INPUT_NDISCRETES               M_DISCRETE_INPUT_NDISCRETES
#define M_COIL_START                              M_COIL_START
#define M_COIL_NCOILS                             M_COIL_NCOILS
#define M_REG_INPUT_START                         M_REG_INPUT_START
#define M_REG_INPUT_NREGS                         M_REG_INPUT_NREGS
#define M_REG_HOLDING_START                       M_REG_HOLDING_START
#define M_REG_HOLDING_NREGS                       M_REG_HOLDING_NREGS
/* master mode: holding register's all address */
#define          M_HD_RESERVE                     M_HD_RESERVE
/* master mode: input register's all address */
#define          M_IN_RESERVE                     M_IN_RESERVE
/* master mode: coil's all address */
#define          M_CO_RESERVE                     M_CO_RESERVE
/* master mode: discrete's all address */
#define          M_DI_RESERVE                     M_DI_RESERVE

#endif
