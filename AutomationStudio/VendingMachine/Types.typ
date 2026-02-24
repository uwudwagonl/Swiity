(*
  Types.typ
  Vending Machine Project – User-Defined Data Types (UDT)
  IEC 61131-3 Structured Text  |  B&R Automation Studio
*)

TYPE
  (* ---------------------------------------------------------------
     State enumeration for the main vending-machine state machine
  --------------------------------------------------------------- *)
  VendingState_e : (
    STATE_IDLE         := 0,   (* Waiting for coin / card input        *)
    STATE_COIN_CHECK   := 1,   (* Validating inserted coin             *)
    STATE_SELECT       := 2,   (* Product selection active             *)
    STATE_DISPENSE     := 3,   (* Dispensing selected product          *)
    STATE_COLLECT      := 4,   (* Waiting for customer to collect      *)
    STATE_CHANGE       := 5,   (* Returning change                     *)
    STATE_ERROR        := 6,   (* Error / fault state                  *)
    STATE_MAINTENANCE  := 7    (* Maintenance / refill mode            *)
  );

  (* ---------------------------------------------------------------
     Product slot descriptor
  --------------------------------------------------------------- *)
  ProductSlot_t : STRUCT
    Name        : STRING[32];  (* Product name                         *)
    Price       : REAL;        (* Price in the local currency unit     *)
    Quantity    : UINT;        (* Remaining stock count                *)
    SlotActive  : BOOL;        (* TRUE = slot is enabled               *)
  END_STRUCT;

  (* ---------------------------------------------------------------
     Coin / payment input data
  --------------------------------------------------------------- *)
  PaymentData_t : STRUCT
    CoinInserted    : BOOL;    (* Coin-drop sensor signal              *)
    CoinValue       : REAL;    (* Value detected by coin validator     *)
    TotalCredit     : REAL;    (* Accumulated credit                   *)
    CreditAccepted  : BOOL;    (* Sufficient credit for selection      *)
  END_STRUCT;

  (* ---------------------------------------------------------------
     Sensor input bundle (mapped from Arduino via serial / I/O)
  --------------------------------------------------------------- *)
  SensorInputs_t : STRUCT
    CoinSensor       : BOOL;   (* Coin-drop detection                  *)
    ProductSensor    : BOOL;   (* Product dispensed / collected        *)
    TempSensor       : REAL;   (* Cabinet temperature (°C)             *)
    DoorSensor       : BOOL;   (* Service door open                    *)
    StockSensor      : ARRAY[0..7] OF BOOL; (* Per-slot stock presence *)
  END_STRUCT;

  (* ---------------------------------------------------------------
     Actuator output bundle (mapped to Arduino via serial / I/O)
  --------------------------------------------------------------- *)
  ActuatorOutputs_t : STRUCT
    MotorEnable      : BOOL;              (* Dispense motor enable      *)
    MotorSlot        : UINT;              (* Target slot index          *)
    ChangeMechanism  : BOOL;              (* Activate change dispenser  *)
    DisplayMsg       : STRING[64];        (* LCD / display message      *)
    StatusLED        : BOOL;              (* Status indicator LED       *)
    CoolingFan       : BOOL;              (* Cooling fan control        *)
  END_STRUCT;

END_TYPE
