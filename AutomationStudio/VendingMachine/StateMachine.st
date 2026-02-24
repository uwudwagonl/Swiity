(*
  StateMachine.st
  Vending Machine Project – State-Machine Logic
  IEC 61131-3 Structured Text  |  B&R Automation Studio

  Call this function block once per PLC cycle from Main.st.
*)

FUNCTION_BLOCK VendingStateMachine

VAR_INPUT
  Sensors   : SensorInputs_t;   (* Live sensor data                    *)
END_VAR

VAR_OUTPUT
  Actuators : ActuatorOutputs_t;(* Computed actuator commands          *)
  State     : VendingState_e;   (* Current state (for HMI / logging)   *)
END_VAR

VAR
  _dispenseTimer   : TON;
  _collectTimer    : TON;
  _errorTimer      : TON;
  _coinSensorPrev  : BOOL := FALSE; (* Previous coin-sensor state for edge detection *)
END_VAR

(* ---------------------------------------------------------------
   Main state-machine CASE statement
--------------------------------------------------------------- *)
CASE gState OF

  (* ---- IDLE -------------------------------------------------- *)
  STATE_IDLE:
    Actuators.DisplayMsg  := 'Insert coin';
    Actuators.StatusLED   := TRUE;
    Actuators.MotorEnable := FALSE;

    IF Sensors.CoinSensor THEN
      gState := STATE_COIN_CHECK;
    END_IF

  (* ---- COIN CHECK -------------------------------------------- *)
  STATE_COIN_CHECK:
    Actuators.DisplayMsg := 'Checking coin...';

    (* Accumulate credit on rising edge of coin sensor only *)
    IF Sensors.CoinSensor AND NOT _coinSensorPrev THEN
      gPayment.TotalCredit := gPayment.TotalCredit + gPayment.CoinValue;
    END_IF
    _coinSensorPrev := Sensors.CoinSensor;

    (* Always allow product selection once any credit exists *)
    IF gPayment.TotalCredit > 0.0 THEN
      gState := STATE_SELECT;
    END_IF

  (* ---- SELECT ------------------------------------------------ *)
  STATE_SELECT:
    Actuators.DisplayMsg := 'Select product (0-7)';

    (* Validate selection: slot must be active and affordable *)
    IF gSelectedSlot < cNumSlots
       AND gProductSlots[gSelectedSlot].SlotActive
       AND gProductSlots[gSelectedSlot].Quantity > 0
       AND gPayment.TotalCredit >= gProductSlots[gSelectedSlot].Price THEN

      gPayment.CreditAccepted := TRUE;
      gState := STATE_DISPENSE;
    END_IF

  (* ---- DISPENSE ---------------------------------------------- *)
  STATE_DISPENSE:
    Actuators.DisplayMsg      := 'Dispensing...';
    Actuators.MotorEnable     := TRUE;
    Actuators.MotorSlot       := gSelectedSlot;

    _dispenseTimer(IN := TRUE, PT := cDispenseTimeMs);

    IF _dispenseTimer.Q THEN
      _dispenseTimer(IN := FALSE);
      Actuators.MotorEnable := FALSE;

      (* Deduct price, update stock *)
      gPayment.TotalCredit                       := gPayment.TotalCredit
                                                    - gProductSlots[gSelectedSlot].Price;
      gProductSlots[gSelectedSlot].Quantity      := gProductSlots[gSelectedSlot].Quantity - 1;
      gTotalSales                                := gTotalSales + 1;

      gState := STATE_COLLECT;
    END_IF

  (* ---- COLLECT ----------------------------------------------- *)
  STATE_COLLECT:
    Actuators.DisplayMsg := 'Please collect your item';
    _collectTimer(IN := TRUE, PT := cCollectTimeoutMs);

    IF Sensors.ProductSensor OR _collectTimer.Q THEN
      _collectTimer(IN := FALSE);
      gState := STATE_CHANGE;
    END_IF

  (* ---- CHANGE ----------------------------------------------- *)
  STATE_CHANGE:
    IF gPayment.TotalCredit > 0.0 THEN
      Actuators.DisplayMsg     := 'Returning change';
      Actuators.ChangeMechanism := TRUE;
    ELSE
      Actuators.ChangeMechanism := FALSE;
      gPayment.TotalCredit      := 0.0;
      gPayment.CreditAccepted   := FALSE;
      gState := STATE_IDLE;
    END_IF

    (* Once credit is cleared, return to idle *)
    IF gPayment.TotalCredit <= 0.0 THEN
      Actuators.ChangeMechanism := FALSE;
      gState := STATE_IDLE;
    END_IF

  (* ---- ERROR ------------------------------------------------- *)
  STATE_ERROR:
    Actuators.DisplayMsg  := 'Error – call service';
    Actuators.StatusLED   := NOT Actuators.StatusLED; (* Blink *)
    Actuators.MotorEnable := FALSE;

    _errorTimer(IN := TRUE, PT := T#5000ms);
    IF _errorTimer.Q THEN
      _errorTimer(IN := FALSE);
      gErrorCode := 0;
      gState     := STATE_IDLE;
    END_IF

  (* ---- MAINTENANCE ------------------------------------------ *)
  STATE_MAINTENANCE:
    Actuators.DisplayMsg := 'Maintenance mode';
    Actuators.StatusLED  := FALSE;
    (* Maintenance logic handled by HMI / operator *)

  ELSE
    gState := STATE_ERROR;

END_CASE

(* ---------------------------------------------------------------
   Temperature monitoring (runs in every state)
--------------------------------------------------------------- *)
IF Sensors.TempSensor > cMaxTemperature THEN
  Actuators.CoolingFan := TRUE;
ELSIF Sensors.TempSensor < cMinTemperature THEN
  Actuators.CoolingFan := FALSE;
END_IF

(* ---------------------------------------------------------------
   Service door interlock
--------------------------------------------------------------- *)
IF Sensors.DoorSensor AND gState <> STATE_MAINTENANCE THEN
  gState              := STATE_MAINTENANCE;
  Actuators.MotorEnable := FALSE;
END_IF

State     := gState;

END_FUNCTION_BLOCK
