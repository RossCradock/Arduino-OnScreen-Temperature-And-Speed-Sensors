
/*
Written by Ross Cradock.
Program to measure temperature, rotational speed of shaft and speed of the kart then to be displayed on a screen.
Program is used in conjunction with a 1.77" Arduino SPI LCD screen. This screen requires Arduino 1.5.8 IDE to work correctly as of 23/April/2016.
The screen wiring is found here https://www.arduino.cc/en/Guide/TFTtoBoards
The sensors used are a temperature sensor and a Hall-effect sensor.
The Hall-effect sensor needs to be located near a magnet attached to a disc on the axel (driven gear or disc brake) which will interfere with the Hall-effect sensor.
*/

	#include <TFT.h>  // Arduino LCD library
	#include <SPI.h>

	#define cs   10 // pin definition for SPI screen
	#define dc   9
	#define rst  8

	TFT TFTscreen = TFT(cs, dc, rst); // create an instance of the library

	volatile byte revolutions = 0; //volatile is how the variable 'revolutions' is treated. in this case it is bytes
	unsigned long rpm = 0; // unsigned means that the integer is only positive going from 0-65,535(2^16) but then rolls over
	unsigned long timeold = 0; // long is a 32 bit variable and unsigned means it will only be positive so 'timeold' will only go from 0-4,294,967,296(2^32)
	unsigned long tempLoop = 0; // initialise iteration counter
	float newTempValue;

        char sensorPrintout[5]; //temperature sensor character array
	char RPMPrint[4]; //Hall Effect sensor character array
	char KMPrint[4]; // Speed character array


	void setup() {

		TFTscreen.begin();//to start TFT library 
		TFTscreen.background(0, 0, 0);  //colour the entire screen black

		// write the static text to the screen
		TFTscreen.stroke(255, 255, 255);  // set the font color to white
		TFTscreen.setTextSize(2);  // set the font size
		TFTscreen.text("Temp:\n ", 0, 0);  // write the text 'temp:' to the top left corner of the screen
		// degree symbol
		TFTscreen.stroke(255, 255, 0);
		TFTscreen.setTextSize(1);
		TFTscreen.text("o\n ", 80, 22);

		TFTscreen.stroke(255, 255, 0);
		TFTscreen.setTextSize(3);
		TFTscreen.text("C\n ", 90, 20);

		TFTscreen.stroke(255, 255, 0);
		TFTscreen.setTextSize(3);
		TFTscreen.text("RPM\n ", 80, 60);

		TFTscreen.stroke(255, 255, 255);
		TFTscreen.setTextSize(2);
		TFTscreen.text("Speed\n ", 0, 42);
	  
		TFTscreen.stroke(255, 255, 0);
		TFTscreen.setTextSize(3);
		TFTscreen.text("kph\n ", 80, 90);
	  
		attachInterrupt(0, rpmFunction, FALLING);//0 = number of the interupt pin 2, rpmFunction is the function, FALLING means the ISR triggers rpmFunction when pin goes from high to low

	}

	void loop() {

		if (revolutions >= 1) {   //occurs if interupt occurs
			TFTscreen.fill(0, 0, 0); //clears screen of speed readings
			TFTscreen.stroke(0, 0, 0);
			TFTscreen.rect(0, 60, 80, 60);
			
			rpm = 60000 / (millis() - (timeold)) * revolutions; //60000 to get from milliseconds to minutes, millis() returns the number of milliseconds since program started(new),
			timeold = millis();//update time signature
			revolutions = 0;   //revolutions return to zero so as not to pass if statement again
			
			String srpm = String(rpm); //make the rpm interger a string
			srpm.toCharArray(RPMPrint, 4); //store string in a character array
			
			//writes the rpm to screen
			TFTscreen.setTextSize(3);
			TFTscreen.stroke(255, 255, 255);
			TFTscreen.text(RPMPrint, 2, 60);
				
			/*
                        changes the rpm to a go kart speed in kilometres per hour
                        ((rpm * 2 * pi * radius of wheel) / 1000) * 60
                        */
			String krpm = String(rpm * 0.256262);
			krpm.toCharArray(KMPrint, 4);
				
			TFTscreen.setTextSize(3);
			TFTscreen.stroke(255, 255, 255);
			TFTscreen.text(KMPrint, 2, 90);
		}
	  
		float TempValue = analogRead(A0); // Read the value of the temp sensor on A0
		TempValue = TempValue * 0.4882812; //  convert to celsius
		TempValue =  TempValue - 270.65;
		
		if (tempLoop >= 3000){ // tempLoop is to slow down the number of times the screen updates the temperature. If there was no loop the screen would update as quick as possible and the flashing is difficult to read.
        		TFTscreen.stroke(0, 0, 0);
        		TFTscreen.text(sensorPrintout, 2, 20);
        
        		if (TempValue > 70) { // this is the warning if the temperature goes over 70 degrees and it makes the reading flash red on the screen
        			TFTscreen.stroke(255, 0, 0); // print the sensor value
        			TFTscreen.text(sensorPrintout, 2, 20);
        			TFTscreen.setTextSize(4);  
        			
        			delay(250);// wait for a moment
        			
        			TFTscreen.fill(0, 0, 0);
        			TFTscreen.stroke(0, 0, 0);
        			TFTscreen.rect(0, 20, 80, 60); 
        		}
		  
          		String sensorVal = String(TempValue);
          		sensorVal.toCharArray(sensorPrintout, 5); // convert the reading to a char array
          	  
          		TFTscreen.stroke(255, 255, 255);
          		TFTscreen.setTextSize(3);
          		TFTscreen.text(sensorPrintout, 2, 20);
          
		} else{
        		tempLoop++;
		}
	}

	void rpmFunction() {
	   
		revolutions++;
		//Each rotation, this interrupt function is run once
		
	}
