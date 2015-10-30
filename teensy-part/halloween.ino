
#include <Audio.h>
#include <Wire.h>
#include <SPI.h>
#include <SD.h>
#include <SerialFlash.h>
#include <NewPing.h>

#define SDCARD_CS_PIN    9
#define SDCARD_MOSI_PIN  7
#define SDCARD_MISO_PIN  8
#define SDCARD_SCK_PIN   14

#define PING_TRIGGER    11
#define PING_ECHO       12
#define MAX_DISTANCE    200


File root;

AudioPlaySdWav      wavPlayer;
AudioOutputAnalog   audioOutput;
AudioConnection     patchCord1(wavPlayer, 0, audioOutput, 0);
bool wasPlaying = false;
NewPing sonar(PING_TRIGGER, PING_ECHO, MAX_DISTANCE);


void setup()
{
  // Open serial communications and wait for port to open:
  Serial.begin(9600);
  Serial1.begin(9600);
//  while (!Serial) {
//    delay(5); // wait for serial port to connect. Needed for Leonardo only
//  }
  // Audio connections require memory to work.  For more
  // detailed information, see the MemoryAndCpuUsage example
  AudioMemory(8);

  Serial.print("Initializing SD card...");
  // On the Ethernet Shield, CS is pin 4. It's set as an output by default.
  // Note that even if it's not used as the CS pin, the hardware SS pin 
  // (10 on most Arduino boards, 53 on the Mega) must be left as an output 
  // or the SD library functions will not work. 
  pinMode(SDCARD_CS_PIN, OUTPUT);
  
  SPI.setMOSI(SDCARD_MOSI_PIN);
  SPI.setMISO(SDCARD_MISO_PIN);
  SPI.setSCK(SDCARD_SCK_PIN);

  int count = 0;
  while(1) {
    if (!SD.begin(SDCARD_CS_PIN)) {
      Serial.print(count);
      Serial.println(" - initialization failed!");
      count++;
    } else {
      Serial.println("initialization done.");
      break;
    }
  }

  root = SD.open("/");
  
  printDirectory(root, 0);

  Serial.println("done!");
}


void handsOn() {
  Serial.println("1");
  Serial1.print('1');
}

void handsOff() {
  Serial.println("0");
  Serial1.print('0');
}

void loop()
{
  unsigned long test;
  // nothing happens after setup finishes.
  if (wasPlaying) {
    if (!wavPlayer.isPlaying()) {
      Serial.println("finished playing");
      wasPlaying = false;
      handsOff();
    }
  } else {
    int uS = sonar.ping();
    int cm = uS / US_ROUNDTRIP_CM;
    Serial.print("Distance "); Serial.println(cm);
    
    if (cm == 0) {
      // problem
      delay(5);
      return;
    }
    
    if (cm < 30) {
      // Someone is close - scare him!
      handsOn();
      playASound();
    } else {
      // Wait 70ms before checking again.
      delay(70);
    }
  }
}

int filesCount = 0;

void playASound() {
  if (filesCount == 0) {
    //Serial.println("No sound files");
    delay(1000);
    return;
  }
  
  int num = random(filesCount);
  char filename[32];
  int index = 0;
  root = SD.open("/");
  
  if (!findFileByIndex(root, filename, index, num)) {
    Serial.println("Failed to find requested audio file");
    return;
  }

  Serial.print("Playing ");
  Serial.println(filename);

  wavPlayer.play(filename);
  wasPlaying = true;

    // A brief delay for the library read WAV info
  delay(5);
}

bool findFileByIndex(File dir, char filename[], int& index, int num) {
   while(true) {  
     File entry =  dir.openNextFile();
     if (! entry) {
       // no more files
       //Serial.println("**nomorefiles**");
       break;
     }
     Serial.print(entry.name());
     if (entry.isDirectory()) {
       //Serial.println("/");
       //printDirectory(entry, numTabs+1);
     } else {
       // files have sizes, directories do not
       Serial.print("\t\t");
       Serial.print(entry.size(), DEC);
       Serial.print("\t");
       Serial.print("#"); Serial.println(filesCount);
      if (index == num) {
        strcpy(filename, entry.name());
        return true;
      }
      index++;
     }
     entry.close();
   }
  return false;
}

void printDirectory(File dir, int numTabs) {
   while(true) {
     
     File entry =  dir.openNextFile();
     if (! entry) {
       // no more files
       //Serial.println("**nomorefiles**");
       break;
     }
     for (uint8_t i=0; i<numTabs; i++) {
       Serial.print('\t');
     }
     Serial.print(entry.name());
     if (entry.isDirectory()) {
       //Serial.println("/");
       //printDirectory(entry, numTabs+1);
     } else {
       // files have sizes, directories do not
       Serial.print("\t\t");
       Serial.print(entry.size(), DEC);
       Serial.print("\t");
       Serial.print("#"); Serial.println(filesCount);
       filesCount++;
     }
     entry.close();
   }
}


