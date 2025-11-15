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
<img width="1211" height="1471" alt="Image" src="https://github.com/user-attachments/assets/a9b68d44-421a-4f2c-9537-101e9257418a" />

## Team Members
* Maria Camila Guzman (40239038)
* Keshan Kathiripilay (40249407)
