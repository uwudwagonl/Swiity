(*
  Main.st
  Vending Machine Project – Main Cyclic Task
  IEC 61131-3 Structured Text  |  B&R Automation Studio

  This program is called once per PLC scan cycle.
  It wires the state-machine function block to global I/O variables.
*)

PROGRAM MAIN

VAR
  fbVending : VendingStateMachine; (* State-machine function block instance *)
END_VAR

(* ---------------------------------------------------------------
   Call the vending state machine every cycle
--------------------------------------------------------------- *)
fbVending(
  Sensors   := gSensors,
  Actuators => gActuators,
  State     => gState
);

(* ---------------------------------------------------------------
   Mirror outputs to HMI display string
--------------------------------------------------------------- *)
gHmiMessage := gActuators.DisplayMsg;

END_PROGRAM
