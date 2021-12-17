/*
 * Air Piano V12. John Dingley.   July 2021
 * Has gaming self-centering joystick potentiometer included to allow pitch bending as you play by moving the joystick.
 * I have found the best way to remove random noise and so random notes is to take multiple samples of distance of your hand above each sensor
 * then take the median value of these.
 * Further noise reduction and removal of outlier values is used in addition to this.
 * This gives you low rate of annoying random notes but does mean you have to play it in a measured quite precise way too to get best results.
 * The heights of the threshold between each layer of notes, 3 of them, are user definable and can be seen in the User Defined Variables section below.
 * 
 * 
 * Reading data from the Sharp sensor is based on the SharpIR library by Giuseppe Masino as outlined below,
 * Using SharpIR library to calculate the distance beetween the sensor and an obstacle
 * Created by Giuseppe Masino, 15 June 2016
 *
 * -----------------------------------------------------------------------------------

 * See the Sharp sensor datasheet for the pin reference, the pin configuration is the same for all models.
 * There is the datasheet for the model GP2Y0A41SK0F:
 *
 * http://www.robotstore.it/open2b/var/product-files/78.pdf
 *
 */


//import the library in the sketch
#include <SharpIR.h>

#include <RunningMedian.h>


// Virtual button numbers. Imagine you are looking at airborne keyboard from players side
//--------------high-------------       boundary value sensor value called high   i.e. the height of this imaginary band between top row of notes and middle row of notes is a variable called high
// 16  17  18  19  20  21  22  23       median sensor value range of mid to high 
//--------------mid--------------       boundary value sensor value called mid    i.e. the height of this imaginary band between top row of notes and middle row of notes is a variable called mid 
// 8   9   10  11  12  13  14  15       median sesnor value range of low to mid
//--------------low--------------       boundary value sensor value called low    i.e. the height of this imaginary band between top row of notes and middle row of notes is a variable called low
// 0   1   2   3   4   5   6   7        median sensor value range of 0 to low
//================================  <------ Base of the keyboard with sensors pointing upwards




//XXXXXXXXXXXXXXXXX   USER DEFINED VARIABLES you can experiment with the heights of the 3 rows of notes in vertical space XXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXX
// Imaginary height of interface between low notes and the middle range is int low
// Imaginary height of interface between middle level of notes and the upper set, above the deck, is int mid
// Imaginary height of interface between upper set notes and the free space above to seiling of the room is int high
int low = 18;
int mid = 26;
int high = 34;
//XXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXX







// The 8 notes of the scale as MIDI values. Lower row of imaginary buttons is one octave below this.
// Upper row of imaginary buttons is one octave above this.
// These notes represent the middle row of imaginary airborne buttons. Here they are an E scale.

int a = 64;
int b = 66;
int c = 68;
int d = 69;
int e = 71;
int f = 73;
int g = 75;
int h = 76;


int sortedArray[10]; // this array must be sorted!!

int rawarray[100];
int rawarraycount;

int modeCt = 0;
int modeV = -1;

int ct = 0;
int v = -1;
int counting = 1;
int firstcycle;


// PitchBend variables

int pitchbendsample;
int PitchValue;
int lowertrigger;
int highertrigger;
int midpoint;




// Analog Input for pitchbend
int pitchbendPin = A9;   // Also is pin 23  check this




RunningMedian samples = RunningMedian(40);
RunningMedian samples1 = RunningMedian(40);
RunningMedian samples2 = RunningMedian(40);
RunningMedian samples3 = RunningMedian(40);
RunningMedian samples4 = RunningMedian(40);
RunningMedian samples5 = RunningMedian(40);
RunningMedian samples6 = RunningMedian(40);
RunningMedian samples7 = RunningMedian(40);

//Create a new instance of the library
//Call the sensor "sensor"
//The model of the sensor is "GP2YA41SK0F"
//The sensor output pin is attached to the pin A0
SharpIR sensor( SharpIR::GP2Y0A21YK0F, A0 );
//The sensor output pin is attached to the pin A1
SharpIR sensor1( SharpIR::GP2Y0A21YK0F, A1 );
//The sensor output pin is attached to the pin A2
SharpIR sensor2( SharpIR::GP2Y0A21YK0F, A2 );
//The sensor output pin is attached to the pin A3
SharpIR sensor3( SharpIR::GP2Y0A21YK0F, A3 );
//The sensor output pin is attached to the pin A4
SharpIR sensor4( SharpIR::GP2Y0A21YK0F, A4 );
//The sensor output pin is attached to the pin A5
SharpIR sensor5( SharpIR::GP2Y0A21YK0F, A5 );
//The sensor output pin is attached to the pin A6
SharpIR sensor6( SharpIR::GP2Y0A21YK0F, A6 );
//The sensor output pin is attached to the pin A7
SharpIR sensor7( SharpIR::GP2Y0A21YK0F, A7 );



//Variables

int starttime;


int note;
int newnote;
int prevcurrentnote;
int notesoundingnow;

int counter;
int counteron;
















void setup()
{
  //Enable the serial comunication to screen serial viewer
  //Serial.begin(115200);
  delay(1000);


  midpoint = analogRead(pitchbendPin);
  highertrigger = midpoint + 50;   // sets up a dead zone in the pitchbend potentiometer of 50 either way of midpoint before note is bent
  lowertrigger = midpoint - 50; 

  
  //Serial.println("STARTING");
  delay(6000);
}




void loop()
{

rawarraycount = 0;
// COMMENT Read the sensors and decide which zone your hand is residing in, 3 zones for each vertical area above each sensor. Sensors numbered 0 to 7 where 0 is the leftmost sensor.
// Repeat this set of measurements 12 times and put the result of each reading in an array of values, we will end up with 12 values in the array. Ideally these will all be the same note value between 0 and 23
// but of course there are some errors. Notes are numbered from 0 to 23 see chart above.
for(int z = 0; z < 12; z++) {

  // COLUMN 0     Reads 9 at lowest value
  int x = sensor.getDistance(); //Calculate the distance in centimeters and store the value in a variable
  samples.add(x);

  int columnZero = samples.getMedian();
  if(columnZero <= low ){ 
                          //button0valueNew = LOW; 
                          newnote = 0;
                           //Serial.println(" button 0 being pressed");
                            rawarray[rawarraycount] = newnote;
                            rawarraycount = rawarraycount + 1;
                         }
  if(columnZero > low && columnZero <= mid ){ 
                          //button8valueNew = LOW;
                          newnote = 8;
                           //Serial.println("Button 8 being pressed");
                            rawarray[rawarraycount] = newnote;
                            rawarraycount = rawarraycount + 1;
                                       }
  if(columnZero > mid && columnZero <= high ){ 
                          //button16valueNew = LOW;
                          newnote = 16;
                           //Serial.println("Button 16 being pressed");
                            rawarray[rawarraycount] = newnote;
                            rawarraycount = rawarraycount + 1;
                                         }
 
                                        
  //Serial.print("Distance 0:");
  //Serial.println( columnZero ); //Print the value to the serial monitor


  // COLUMN 1     Reads 18 at lowest value
  x = sensor1.getDistance(); //Calculate the distance in centimeters and store the value in a variable
  samples1.add(x);

  int columnOne = samples1.getMedian();
  if(columnOne <= low){ newnote = 1;  rawarray[rawarraycount] = newnote;  rawarraycount = rawarraycount + 1;}
  if(columnOne > low && columnOne <= mid ){  newnote = 9;  rawarray[rawarraycount] = newnote;  rawarraycount = rawarraycount + 1;}
  if(columnOne > mid && columnOne <= high ){  newnote = 17;  rawarray[rawarraycount] = newnote;  rawarraycount = rawarraycount + 1;}
  //Serial.print("Distance 1:");
  //Serial.println( columnOne ); //Print the value to the serial monitor


  
  // COLUMN 2     Reads 18 at lowest value
  x = sensor2.getDistance(); //Calculate the distance in centimeters and store the value in a variable
  samples2.add(x);

  int columnTwo = samples2.getMedian();
  if(columnTwo <= low ){  newnote = 2;  rawarray[rawarraycount] = newnote;  rawarraycount = rawarraycount + 1;}
  if(columnTwo > low && columnTwo <= mid ){  newnote = 10;  rawarray[rawarraycount] = newnote;  rawarraycount = rawarraycount + 1;}
  if(columnTwo > mid && columnTwo <= high ){  newnote = 18;  rawarray[rawarraycount] = newnote;  rawarraycount = rawarraycount + 1;}
  //Serial.print("Distance 2:");
  //Serial.println( columnTwo ); //Print the value to the serial monitor
    

  // COLUMN 3     Reads 9 at lowest value
  x = sensor3.getDistance(); //Calculate the distance in centimeters and store the value in a variable
  samples3.add(x);

  int columnThree = samples3.getMedian();
  if(columnThree <= low ){  newnote = 3;  rawarray[rawarraycount] = newnote;  rawarraycount = rawarraycount + 1;}
  if(columnThree > low && columnThree <= mid ){  newnote = 11;  rawarray[rawarraycount] = newnote;  rawarraycount = rawarraycount + 1;}
  if(columnThree > mid && columnThree <= high ){  newnote = 19;  rawarray[rawarraycount] = newnote;  rawarraycount = rawarraycount + 1;}
  //Serial.print("Distance 3:");
  //Serial.println( columnThree ); //Print the value to the serial monitor
 

  // COLUMN 4     Reads 9 at lowest value
  x = sensor4.getDistance(); //Calculate the distance in centimeters and store the value in a variable
  samples4.add(x);

  int columnFour = samples4.getMedian();
  if(columnFour <= low ){  newnote = 4;  rawarray[rawarraycount] = newnote;  rawarraycount = rawarraycount + 1;}
  if(columnFour > low && columnFour <= mid ){  newnote = 12;  rawarray[rawarraycount] = newnote;  rawarraycount = rawarraycount + 1;}
  if(columnFour > mid && columnFour <= high ){  newnote = 20;  rawarray[rawarraycount] = newnote;  rawarraycount = rawarraycount + 1;}
  //Serial.print("Distance 4:");
  //Serial.println( columnFour ); //Print the value to the serial monitor
  
  

  // COLUMN 5     Reads 18 at lowest value
  x = sensor5.getDistance(); //Calculate the distance in centimeters and store the value in a variable
  samples5.add(x);

  int columnFive = samples5.getMedian();
  if(columnFive <= low ){  newnote = 5;  rawarray[rawarraycount] = newnote;  rawarraycount = rawarraycount + 1;}
  if(columnFive > low && columnFive <= mid ){  newnote = 13;  rawarray[rawarraycount] = newnote;  rawarraycount = rawarraycount + 1;}
  if(columnFive > mid && columnFive <= high ){  newnote = 21;  rawarray[rawarraycount] = newnote;  rawarraycount = rawarraycount + 1;}
  //Serial.print("Distance 5:");
  //Serial.println( columnFive ); //Print the value to the serial monitor
  

  // COLUMN 6     Reads 9 at lowest value
  x = sensor6.getDistance(); //Calculate the distance in centimeters and store the value in a variable
  samples6.add(x);

  int columnSix = samples6.getMedian();
  if(columnSix <= low ){  newnote = 6;  rawarray[rawarraycount] = newnote;  rawarraycount = rawarraycount + 1;}
  if(columnSix > low && columnSix <= mid ){  newnote = 14;  rawarray[rawarraycount] = newnote;  rawarraycount = rawarraycount + 1;}
  if(columnSix > mid && columnSix <= high ){  newnote = 22;  rawarray[rawarraycount] = newnote;  rawarraycount = rawarraycount + 1;}
  //Serial.print("Distance 6:");
  //Serial.println( columnSix ); //Print the value to the serial monitor
  

  // COLUMN 7     Reads 18 at lowest value
  x = sensor7.getDistance(); //Calculate the distance in centimeters and store the value in a variable
  samples7.add(x);

  int columnSeven = samples7.getMedian();
  if(columnSeven <= low ){ 
             //button7valueNew = LOW;
             newnote = 7;
  rawarray[rawarraycount] = newnote;
  rawarraycount = rawarraycount + 1;             
             //Serial.println("Button 7 being pressed");
                        }
  if(columnSeven > low && columnSeven <= mid ){ 
            // button15valueNew = LOW;
             newnote = 15;
  rawarray[rawarraycount] = newnote;
  rawarraycount = rawarraycount + 1;             
             //Serial.println("Button 14 being pressed");
                          }
  if(columnSeven > mid && columnSeven <= high ){ 
             //button23valueNew = LOW;
             newnote = 23;
   rawarray[rawarraycount] = newnote;
  rawarraycount = rawarraycount + 1;            
             //Serial.println("Button 23 being pressed");
                          }
  //Serial.print("Distance 7:");
  //Serial.println( columnSeven ); //Print the value to the serial monitor




//Serial.print("Newnote =  ");
//Serial.println(newnote);

                   }  // end of z for loop
//XXXXXXXXXXXXXXXXXXXXXXXXX


// COMMENT: This next bit of code picks out the MODE or commonest, note value from our array of 12 samples of the note being played.
// This is first part of our filtering out of bum notes process.

                                 modeCt = 0;
                                 modeV = -1;

                                 ct = 0;
                                 v = -1;
                                 //Serial.print("RawArray = ");
                                 for(int i = 0; i< rawarraycount; i++) {
                                 //Serial.print(rawarray[i]);
                                 //Serial.print(", ");
                                 if(rawarray[i] != v) {
                                 v = rawarray[i];
                                 ct = 0;
                                                }

                                 ct ++;

                                 if(ct > modeCt) {
                                 modeCt = ct;
                                 modeV = v;
                                  }
                                  }
                                 //Serial.println(" ");
                                 //Serial.print("First Pass Mode= ");
                                 //Serial.println(modeV);

                                 










 // COMMENT  As we keep going round the main void program loop, we take 12 distance measuremeents, work out what note these represent,then find the commonest value.
 // Each time we go round the main loop we then take the commonest note from each sample of 12 measurements and call it modeV.
 // We then add this to another array of 10 values and move all the previous 9 mode values along one in the array
 // with one dropping off the end each time. This array called sortedArray is a set of the last 10 mode values of at any point in time.




  sortedArray[0] = sortedArray[1];
  sortedArray[1] = sortedArray[2];
  sortedArray[2] = sortedArray[3];
  sortedArray[3] = sortedArray[4];
  sortedArray[4] = sortedArray[5];
  sortedArray[5] = sortedArray[6];
  sortedArray[6] = sortedArray[7];
  sortedArray[7] = sortedArray[8];
  sortedArray[8] = sortedArray[9];
  sortedArray[9] = modeV;
  

                            // }
// Print values of each of the 10 values in the new array. EACH of these 10 note values is the mode of a previous set of 12 distance sensor measurements.
  //Serial.print("Array: ");
  //Serial.print(sortedArray[9]);
  //Serial.print("/");
  //Serial.print(sortedArray[8]);
  //Serial.print("/");
  //  Serial.print(sortedArray[7]);
  //Serial.print("/");
  //   Serial.print(sortedArray[6]);
  //Serial.print("/");       
  //Serial.print(sortedArray[5]);
  //Serial.print("/");
  //Serial.print(sortedArray[4]);
  //Serial.print("/");
  //Serial.print(sortedArray[3]);
  //Serial.print("/");
  //Serial.print(sortedArray[2]);
  //Serial.print("/");
  //  Serial.print(sortedArray[1]);
  //Serial.print("/");
  //Serial.print(sortedArray[0]);
  //Serial.println("/");

// COMMENT: We now find the commonest or mode, value of the last 10 values in the rolling record of the most recent values, where each value is already the mode of 12 sets of distance measurements.
  
                  // if (millis - starttime > 500 && newnote != notesoundingnow){
modeCt = 0;
modeV = -1;

ct = 0;
v = -1;
                                 for(int i = 0; i< 10; i++) {
                                 if(sortedArray[i] != v) {
                                 v = sortedArray[i];
                                 ct = 0;
                                                }

                                 ct ++;

                                 if(ct > modeCt) {
                                 modeCt = ct;
                                 modeV = v;
                                  }
                                  }
//Serial.print("modeV of Second Array = ");
//Serial.println(modeV);


// COMMENT This new mode value is now our definite best guess at what note you are actually trying to play with noise as much as possible filtered out.
// Our next decision is, is this new note DIFFERENT to the one that is already being played?
// If so we need to change the note being played. If not we actually don't need to do anything at all except keep sampling the sensors really fast.


            if(modeV != notesoundingnow && counteron == 0){
                                counter = 0;
                                counteron = 1;  // turn counter on
                                      }

            if(counteron == 1){
                        counter = counter + 1;                
                              }
                              


           if(counter == 10){
                        counter = 0;
                        counteron = 0;    //turn counter off and reset it

                   // Recaclculate the mode
                   modeCt = 0;
                   modeV = -1;

                   ct = 0;
                   v = -1;
                                 for(int i = 0; i< 10; i++) {
                                 if(sortedArray[i] != v) {
                                 v = sortedArray[i];
                                 ct = 0;
                                                }

                                 ct ++;

                                 if(ct > modeCt) {
                                 modeCt = ct;
                                 modeV = v;
                                  }
                                  }


           
 // Serial.print("Array: ");
 // Serial.print(sortedArray[9]);
  //Serial.print("/");
 // Serial.print(sortedArray[8]);
 // Serial.print("/");
 //   Serial.print(sortedArray[7]);
 // Serial.print("/");
 //    Serial.print(sortedArray[6]);
 // Serial.print("/");       
 // Serial.print(sortedArray[5]);
 // Serial.print("/");
 // Serial.print(sortedArray[4]);
 // Serial.print("/");
 // Serial.print(sortedArray[3]);
 // Serial.print("/");
 // Serial.print(sortedArray[2]);
 // Serial.print("/");
 //   Serial.print(sortedArray[1]);
 // Serial.print("/");
 // Serial.print(sortedArray[0]);
 // Serial.println("/");
             
                            




           if(modeV != notesoundingnow){   // If v really has changed then we now formally change to the new note to be played i.e. modeV is NOT_EQUAL to the note being sounded right now.
           //Serial.print("CHANGING TO NEW NOTE: ");

// COMMENT Turn off the note currently being played.

           
int x;
           x = a-12;
           usbMIDI.sendNoteOff(x, 0, 1);
           x = b-12;
           usbMIDI.sendNoteOff(x, 0, 1);
           x = c-12;
           usbMIDI.sendNoteOff(x, 0, 1);
           x = d-12;
           usbMIDI.sendNoteOff(x, 0, 1);
           x = e-12;
           usbMIDI.sendNoteOff(x, 0, 1);
           x = f-12;
           usbMIDI.sendNoteOff(x, 0, 1);
           x = g-12;
           usbMIDI.sendNoteOff(x, 0, 1);
           x = h-12;
           usbMIDI.sendNoteOff(x, 0, 1);

 
           x = a;
           usbMIDI.sendNoteOff(x, 0, 1);
           x = b;
           usbMIDI.sendNoteOff(x, 0, 1);
           x = c;
           usbMIDI.sendNoteOff(x, 0, 1);
           x = d;
           usbMIDI.sendNoteOff(x, 0, 1);
           x = e;
           usbMIDI.sendNoteOff(x, 0, 1);
           x = f;
           usbMIDI.sendNoteOff(x, 0, 1);
           x = g;
           usbMIDI.sendNoteOff(x, 0, 1);
           x = h;
           usbMIDI.sendNoteOff(x, 0, 1);   


           x = a+12;
           usbMIDI.sendNoteOff(x, 0, 1);
           x = b+12;
           usbMIDI.sendNoteOff(x, 0, 1);
           x = c+12;
           usbMIDI.sendNoteOff(x, 0, 1);
           x = d+12;
           usbMIDI.sendNoteOff(x, 0, 1);
           x = e+12;
           usbMIDI.sendNoteOff(x, 0, 1);
           x = f+12;
           usbMIDI.sendNoteOff(x, 0, 1);
           x = g+12;
           usbMIDI.sendNoteOff(x, 0, 1);
           x = h+12;
           usbMIDI.sendNoteOff(x, 0, 1);     

  // COMMENT Then turn ON the new note you want to be played.                      
                             

if(modeV == 0){ note = a-12;  usbMIDI.sendNoteOn(note, 127, 1); }
if(modeV == 1){ note = b-12;  usbMIDI.sendNoteOn(note, 127, 1); }
if(modeV == 2){ note = c-12;  usbMIDI.sendNoteOn(note, 127, 1); }
if(modeV == 3){ note = d-12;  usbMIDI.sendNoteOn(note, 127, 1); }
if(modeV == 4){ note = e-12;  usbMIDI.sendNoteOn(note, 127, 1); }
if(modeV == 5){ note = f-12;  usbMIDI.sendNoteOn(note, 127, 1); }
if(modeV == 6){ note = g-12;  usbMIDI.sendNoteOn(note, 127, 1); }
if(modeV == 7){ note = h-12;  usbMIDI.sendNoteOn(note, 127, 1); }
if(modeV == 8){ note = a;  usbMIDI.sendNoteOn(note, 127, 1); }
if(modeV == 9){ note = b;  usbMIDI.sendNoteOn(note, 127, 1); }
if(modeV == 10){ note = c;  usbMIDI.sendNoteOn(note, 127, 1); }
if(modeV == 11){ note = d;  usbMIDI.sendNoteOn(note, 127, 1); }
if(modeV == 12){ note = e;  usbMIDI.sendNoteOn(note, 127, 1); }
if(modeV == 13){ note = f;  usbMIDI.sendNoteOn(note, 127, 1); }
if(modeV == 14){ note = g;  usbMIDI.sendNoteOn(note, 127, 1); }
if(modeV == 15){ note = h;  usbMIDI.sendNoteOn(note, 127, 1); }
if(modeV == 16){ note = a + 12;  usbMIDI.sendNoteOn(note, 127, 1); }
if(modeV == 17){ note = b + 12;  usbMIDI.sendNoteOn(note, 127, 1); }
if(modeV == 18){ note = c + 12;  usbMIDI.sendNoteOn(note, 127, 1); }
if(modeV == 19){ note = d + 12;  usbMIDI.sendNoteOn(note, 127, 1); }
if(modeV == 20){ note = e + 12;  usbMIDI.sendNoteOn(note, 127, 1); }
if(modeV == 21){ note = f + 12;  usbMIDI.sendNoteOn(note, 127, 1); } 
if(modeV == 22){ note = g + 12;  usbMIDI.sendNoteOn(note, 127, 1); }
if(modeV == 23){ note = h + 12;  usbMIDI.sendNoteOn(note, 127, 1); }

// COMMENT Print the note we are now playing, hopefully the corect one.

//Serial.print(modeV);
//Serial.print("     MIDI value: ");
//Serial.println(note);

notesoundingnow = modeV;
                }          // having redone the mode we now see if v really has changed
                             



}  // end of if counter == 10










prevcurrentnote = notesoundingnow;










    

     //---------------------------------------------- CHECK STATUS OF PITCHBEND INPUT -----------------------------------------------
      pitchbendsample = analogRead(pitchbendPin);  // 0 to 1023 in value

     // pitchbend value to send via MIDI should be in the range of -8192 to +8192 where zero is no pitchbend
     // so if we take the potentiometer value and subtract 512 from it we will then get a value of -512 to +511
     // NOTE: We may need to put in a dead zone between say 505 and 519 to account for the potentiometer not quite returning to exact centre point of its travel otherwise DG would always play slightly sharp or slightly flat
     // if we then multiply that value by 15 we will get a range of -7680 to +7665 which is within range and should give us a big swing of pitch for initial tests
     
     //PitchValue = (pitchbendsample - 512) * 15;

     PitchValue = 0;
     
     if(pitchbendsample < lowertrigger){ PitchValue = (lowertrigger - pitchbendsample) * (-15); }
     if(pitchbendsample > highertrigger){ PitchValue = (pitchbendsample - highertrigger) * 15; }
     

     
     usbMIDI.sendPitchBend(PitchValue, 1);    // Channel 1
     usbMIDI.sendPitchBend(PitchValue, 2);    // Channel 2
     //MIDI.sendPitchBend(PitchValue, 1);    // Channel 1
     //MIDI.sendPitchBend(PitchValue, 2);    // Channel 2

     //Serial.print("PitchValue: ");
     //Serial.println(PitchValue);
     







  
  //Serial.println("----------------------------------------------------------------------------------------------------------");

  //currTime = millis();
  //int cycletime = currTime - prevTime;
  //Serial.print("Cycle Time: ");
  //Serial.println(cycletime);
 // Serial.println("------------------------------------------------------------------------------------------------------------");
  
  
  //prevTime = currTime;

  delay(5);  // This is really IMPORTANT TO KEEP IN THE CODE. Without it it seems to pick up extra random notes. 
    // puts gap between samples going into array to allow sensors to stabilise when hand is moved

  // MIDI Controllers should discard incoming MIDI messages.
  while (usbMIDI.read()) {
  }
  
  
}
