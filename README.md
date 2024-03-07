# SOS-Alert-Keychain

**Components required:**
1. A9g board
2. ATMega-328p microcontroller
3. 350mAh lipo battery
4. RGB led
5. Push Buttons
6. Speaker
7. Sim Card

**Working**

I have used here Atmega 328p microcontroller ic to send AT commands to board for gps and gsm communication. As shown in demo video, if blue push button is pressed more than 5 seconds than current location coordinates is fetched through gps satellite and is converted in form of google map link with "I'm here ! HELP " message string; is send to registerd mobile number. Just after sending message a call is established between 'HERO' and registerd mobile number and another person can converse with user. I have given mic and speaker quality test in demo video wherein HERO is using very small speaker that could be heard only if take this device near your ears. 350mA rechargeable battery is used where it gives standby mode power of around 20 days and normal usage gives upto 17 days battery backup but if it is used continously for more than 30 minutes than it gives battery backup of hardly one day. By pressing the green push button user can get acknowledgement of remaining battery capacity by blinking rgb led with different colours for different battery capacity in percentage.
