---
layout: default
title: Connecting OAT to Astroberry
parent: How to use the OAT
nav_order: 4

---

# **Connecting Arduino based OAT to Astroberry Server.**

This guide assumes you have Astroberry installed on a Raspberry Pi and you are able to access the desktop. (See [Astroberry Quick Start Guide](https://www.astroberry.io/docs/index.php?title=Astroberry_Server#Quick_Start "Astroberry Quick Start Guide") for Astroberry setup.)



---
OAT Arduino must have either **SUPPORTS_SERIAL** or **HEADLESS_CLIENT** enabled.

***

1. Connect OAT and cameras to Raspberry Pi  


2. Launch KStars from the main menu bar or Applications -> Education -> KStars menu  
   ![](../Astroberry_Connect/Launch_KStars.png)  
    
   
2. Launch Ekos from the menu bar Tools -> Ekos (or shortcut Ctrl-K)  
   ![Launch Ekos](../Astroberry_Connect/Launch_Ekos.png)  


3. Under "Select Profile" click the Add Profile button  
   ![Create Profile](../Astroberry_Connect/Create_Profile.png)  

   
4. Enter a name for your new profile.  Select "LX200 GPS" for your Mount, your imaging camera driver for "CCD" and "INDI Webcam" for your Guider.  
   
   NOTE: If "INDI Webcam" is not in the list of drivers, it can be installed by opening a terminal window and typing "sudo apt-get install INDI-webcam"  
   
   ![Select Gear](../Astroberry_Connect/Select_Gear.png)  


5. Under Start & Stop Ekos, click the Start button.  
   ![Start Ekos](../Astroberry_Connect/Start_Ekos.png)  

   
6. setup Mount: In the LX200 GPS -> Connection tab set baud rate to 57600 and set the port to the system port by clicking on the button next to "System Ports"  If this is blank try clicking on the "Scan Ports" button.  
   ![Configure OAT Port](../Astroberry_Connect/Configure_OAT_Port.png)  


7. In the LX200 GPS -> Main Control tab you may have to click on the "Connect" button the first time the mount is set up.  
   ![Connect OAT](../Astroberry_Connect/Connect_OAT.png)  
   
 
7. setup Primary Camera: In the Primary Camera -> Options tab, verify "Primary" is selected for Telescope.  Adjust any other settings as needed by your camera.  
   ![Configure Imaging Camera](../Astroberry_Connect/Configure_Primary_Camera.png)  


8. setup Guide Camera: In the INDI Webcam -> Options tab, click the "Guide" button next to Telescope to assign this as the guide camera.  
   ![Configure Guide Camera](../Astroberry_Connect/Configure_Guide_Camera.png)  


9. Once all attached devices show a green Connection indicator click the "Close" button at the bottom right corner.  
   ![Close Dialog Box](../Astroberry_Connect/Close.png)  


10. If all accessories have connected successfully, the Ekos window should now have tabs for CCD, Align, Mount and Guide.  
   ![Connected Tabs](../Astroberry_Connect/Complet_tabs.png)  


   

