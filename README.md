Overview


Create an account in edge impulse and open commannd prompt on your device.
Upload a basic code of MPU6050 into microcontroller(This guide helps in esp32/esp32-cam).
Based on the time delay set in the code, the frequency will change during the data collection process.
The base code for MPU6050 will be attached to this.
Open Powershell/Cmd Prompt(Powershell works better)
Install edge impulse using the prompt from the website
Then type "edge-impulse-data-forwarder --clean", This command is used to create a new library where all the data is stored.
Connect the device to your computer and select the COM port that's connected to 
Give it a name and start the recording process
Head to the edge impulse website and you will see your device there
Give it a name for the training dataset, and start recording the data.
Once the data is created it can be sent through various mathematical equations to get the required.


Step 1:
Upload the basic code and follow steps from 9 to 14 in the overview.

Step 2: 
After the recording process, 
1. Click on Create Impulse in the side bar and select the following settings
2.Window increase (stride)- 250ms
3.Window size-1500ms
4.Choose all the axes
5.Choose classification
6. Choose output features
7.Save the impulse
8.Now click on Spectral features from the menu
9.Scale axes-1
Input decimation ratio-1
Type-none
Analysis Type 
FFT
FFT length-32
Take log of spectrum? YES
 
Overlap FFT frames? YES
 
Improve low frequency resolution? NO

10. Click save parameters

11.Click classifer from the menu and select the following options


Number of training cycles-100
Use learned optimizer? NO
 
Learning rate-0.005
Training processor? CPU

**NOTE: PLAY AROUND WITH THE CYCLES AND LEARNING RATE FOR OPTIMUM RESULTS**

12. Click on Save&Train. Once done it will show the accuracy of each classification.

Step3: Deployment

Once satisfied with the results, click on deployment from the menu
1. Choose Arduino Library.
2. This installs the zip file

Step 4: Arduino IDE Setup

1. Sketch->Include Lib->Add Zip Lib
2. Select the zip that just installed.
3. Once done, u need to find the header file's name so that u can include it in your regular code.
4.File->Examples->Find the zip file's name->static_buffer->static-buffer.
5.Once this open's you will see the header file for your CUSTOM TRAINED MODEL.
6.This Repositery is a guide for remotely running the ML Model and once the threshold is met, it sends the data to a whatsapp user using Whatbot_API


Step 5:

1. The setup of whatabot can be looked through from their website
2. This code will send the message when the threshold is met.
