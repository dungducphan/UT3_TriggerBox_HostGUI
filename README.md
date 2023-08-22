# UT3 TriggerBox GUI Control

Qt5/6 GUI Control v1.0 for the [UT3 Timing Unit v1.0](https://github.com/dungducphan/UT3_TriggerBox/releases/tag/v1.0)

![IMG-0691](https://github.com/dungducphan/UT3_TriggerBox_HostGUI/assets/27539190/9142ec2b-1234-4bd8-8b5d-15e3d92b10bb)

Right now, users need to make sure compatability between the hardware, firmware and software versions of this product. The hardware version is printed on the daughter board (the green board in the previous picture).

# Build
Dependendies: termios, Qt5/6.
Toolkit: QtCreator.
Open the `CMakeLists.txt` using the QtCreator, configure the project and hit Build All. 

# Usage
With [firmware version 1.0](https://github.com/dungducphan/UT3_TriggerBox/releases/tag/v1.0), users cannot control the delays of individual trigger outputs. Triggers B[0-7] and D[0-3] are tighten to Delay Lines 0 and 1, respectively. All other Delay Lines are not used. The values of Delay Line 0 and 1 can be changed. For UT3, the delay values must be between 0 an 100 ms.

![Screenshot from 2023-08-22 14-05-47](https://github.com/dungducphan/UT3_TriggerBox_HostGUI/assets/27539190/86ae6365-0f67-4f3b-b64f-f79c41e61a5f)

# TANGO Control Integration
To be determined...
