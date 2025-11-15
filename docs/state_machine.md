# Mill Controller State Machine

The controller logic in `src/Controller.cpp` is organized as a state machine that reacts to key inputs, NFC card events, configuration status, and timeouts. The diagram below summarizes the primary transitions between the states defined in `include/Controller_defines.h`.

```mermaid
stateDiagram-v2
    direction LR
    [*] --> WaitForUser
    WaitForUser --> PayOne : Left key
    WaitForUser --> AskForSplitPayment : Right key & split enabled
    WaitForUser --> PayTwo : Right key & split disabled
    WaitForUser --> ShowLastUser : Both keys
    WaitForUser --> ReadCreditUser : NFC card present
    WaitForUser --> EnterKey : Left key held (config mode)
    WaitForUser --> SceenSaferState : Inactivity timeout
    WaitForUser --> NVMError : Config load failed

    EnterKey --> SelectTiToAdapt : Authentication ok
    EnterKey --> WaitForUser : Both keys (exit)
    EnterKey --> EnterKey : Left/Right key advance or toggle digit

    SelectTiToAdapt --> AdaptTiSingle : Left key
    SelectTiToAdapt --> AdaptTiDouble : Right key
    SelectTiToAdapt --> WaitForUser : Both keys (cancel)

    AdaptTiSingle --> AdaptTiSingle : Left/Right adjust single time
    AdaptTiSingle --> AdaptTiDouble : Both keys & config not saved
    AdaptTiSingle --> WaitForUser : Both keys & config saved

    AdaptTiDouble --> AdaptTiDouble : Left/Right adjust double time
    AdaptTiDouble --> WaitForUser : Both keys (save & exit)

    ShowCredit --> PayOne : Left key
    ShowCredit --> AskForSplitPayment : Right key
    ShowCredit --> ShowLastUser : Both keys
    ShowCredit --> ReadCreditUser : NFC card presents another user

    ShowLastUser --> PayOne : Left key
    ShowLastUser --> AskForSplitPayment : Right key
    ShowLastUser --> WaitForUser : Both keys
    ShowLastUser --> ReadCreditUser : NFC card present

    NVMError --> AdaptTiSingle : Left/Right key (reconfigure)
    NVMError --> WaitForUser : Both keys (dismiss)

    SceenSaferState --> PayOne : Left key
    SceenSaferState --> AskForSplitPayment : Right key
    SceenSaferState --> ShowLastUser : Both keys
    SceenSaferState --> ReadCreditUser : NFC card present

    ReadCreditUser --> ShowCredit : Card processed

    AskForSplitPayment --> PayTwo : Left key (full double)
    AskForSplitPayment --> PayTwo_1 : Right key (split double)
    AskForSplitPayment --> WaitForUser : Both keys (cancel)

    PayOne --> Single : Write success & credit ok
    PayOne --> LowCredit : Insufficient credit
    PayOne --> WaitForUser : Cancel or repeated write failure

    PayTwo --> Double : Write success & credit ok
    PayTwo --> LowCredit : Insufficient credit
    PayTwo --> WaitForUser : Cancel, invalid card, or repeated write failure

    PayTwo_1 --> PayTwo_2 : First user charged
    PayTwo_1 --> LowCredit : Insufficient credit
    PayTwo_1 --> WaitForUser : Cancel

    PayTwo_2 --> Double : Second user charged
    PayTwo_2 --> RepayState : Cancel during second tap

    RepayState --> DoneState : Refund applied
    RepayState --> RepayState : Waiting for refund card

    DoneState --> WaitForUser : Card removed

    Single --> StopState : Left/Right key pressed
    Double --> StopState : Left/Right key pressed
    StopState --> WaitForUser : Left key (abort)
    StopState --> FinishState : Right key (resume)
    FinishState --> StopState : Left/Right key pressed again

    FreePullState --> Single : Left key
    FreePullState --> Double : Right key
    FreePullState --> WaitForUser : Both keys

    LowCredit --> WaitForUser : Timeout
```

## Notes

- `StateBegin(...)` resets timers and shared variables whenever a new state is entered, so transitions labeled “cancel” generally restore the controller to `WaitForUser` with a clean slate.
- Many display-oriented states (for example `LowCredit`, `ShowCredit`, `DoneState`, and `SceenSaferState`) rely on `TimeOut(...)` checks in `Controller::States` to fall back to `WaitForUser` after a delay, which is omitted from the diagram for readability except where it represents the primary exit path.
- Error handling in the payment states retries NFC writes several times before abandoning the operation and returning to `WaitForUser`.
