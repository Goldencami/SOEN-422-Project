# SOEN-422-Project
Concordia University’s current physical access control allows largely unrestricted building entry. Even where card-access scanners are installed, their functionality remains extremely limited. These devices provide minimal feedback, do not coordinate entry/exit flows, and offer no information during errors or emergencies.

The Smart Entrance Control System aims to modernize campus access with a transparent, user-friendly, state-driven interface that enhances security and safety while reducing confusion for students and staff.

## Problem Summary
Most Concordia buildings either:
* allow unrestricted entry, or
* use basic card scanners with very limited user feedback.

Current access systems commonly:

* provide only a generic beep
* fail to communicate scanner states (locked, ready, error, offline)
* do not manage entry vs. exit traffic
* show no feedback during faults, busy signals, or network issues
* cannot interact with safety systems such as fire alarms or forced-entry detection

As a result, users often do not know:
* whether their card was detected
* whether an error is caused by the card or the system
* whether someone exiting has priority
* how to react during an emergency

This harms the user experience, slows down building flow, and creates potential safety risks.

## Proposed Solution
A next-generation card scanner that prioritizes clarity, coordination, and safety.

The system introduces features commonly found in modern transit and public infrastructure devices, including visual state indicators, coordinated traffic logic, and direct integration with emergency signaling.

### State-Driven User Interface
Two LCD screens—one on the entry side, one on the exit side—display clear, real-time instructions:
* “Please Scan Your ID”
* “Verifying…”
* “Access Granted, Please Enter”
* “Access Denied”
* “Exit in Progress, Please Wait”
* “Emergency Mode, Evacuate Now”
* “System Offline, Stand By”
This ensures users always know what is happening and what action is required.

### Smart Entry/Exit Coordination
The system detects individuals approaching from either side and uses logic to:
* temporarily pause new entry while someone is exiting
* display directional flow messages
* prevent collisions or confusion at high-traffic doors
* optimize overall throughput and safety

### Enhanced System Awareness
The device can:
* notify users of network or system faults
* switch to emergency mode during fire notifications
* provide real-time operational status
* reduce uncertainty caused by invisible system failures

## State Machine Diagram
<img width="900" height="1000" alt="State Machine Diagram" src="https://github.com/user-attachments/assets/a9b68d44-421a-4f2c-9537-101e9257418a" />

[Link to state machine](https://drive.google.com/file/d/1GU_f6bgyF_i9jYTB-THIqsMhVd-O8-gB/view?usp=sharing)

## Circuit
<img width="1261" height="930" alt="Image" src="https://github.com/user-attachments/assets/cdea50c8-ef0b-4b51-88ab-690e16958181" />

### RFID Pin Layout
```
 * Typical pin layout used:
 * -----------------------------------------------------------------------------------------
 *             MFRC522      Arduino       Arduino   Arduino    Arduino          Arduino
 *             Reader/PCD   Uno/101       Mega      Nano v3    Leonardo/Micro   Pro Micro
 * Signal      Pin          Pin           Pin       Pin        Pin              Pin
 * -----------------------------------------------------------------------------------------
 * SPI SS      SDA(SS)      10            53        D10        10               10
 * SPI MOSI    MOSI         11 / ICSP-4   51        D11        ICSP-4           16
 * SPI MISO    MISO         12 / ICSP-1   50        D12        ICSP-1           14
 * SPI SCK     SCK          13 / ICSP-3   52        D13        ICSP-3           15
```

## TTGO LoRA 32
<img width="679" height="508" alt="Image" src="https://github.com/user-attachments/assets/6566511f-7749-4ce7-ab57-6527369994e7" />

## Library
Please install the following library:
* RFID_MFRC522v2

## Team Members
* Maria Camila Guzman (40239038)
* Keshan Kathiripilay (40249407)
