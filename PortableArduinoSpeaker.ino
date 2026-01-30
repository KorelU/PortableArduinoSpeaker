#include <SoftwareSerial.h>
#include <DFRobotDFPlayerMini.h>

SoftwareSerial mySoftwareSerial(10, 11);  // RX, TX
DFRobotDFPlayerMini myDFPlayer;

bool isPaused = false;
bool isRepeating = false;

void setup() {
  Serial.begin(115200);
  mySoftwareSerial.begin(9600);

  Serial.println("\nDFRobot DFPlayer Mini");
  Serial.println("Initializing DFPlayer module ...");

  if (!myDFPlayer.begin(mySoftwareSerial)) {
    Serial.println("Failed to initialize DFPlayer.");
    Serial.println("Check connections and SD card.");
    while (true); // Halt
  }

  Serial.println("DFPlayer Mini initialized.");
  myDFPlayer.setTimeOut(500);
  myDFPlayer.volume(30); // Default volume
  myDFPlayer.EQ(DFPLAYER_EQ_NORMAL);

  printMenu();
}
bool isPlaying = false;

void loop() {
  if (Serial.available()) {
    isPlaying = false; // REset the is playing so we are not always looping
    String input = Serial.readStringUntil('\n');
    input.trim();

    if (input.length() == 0) return;

    char command = input.charAt(0);

    switch (command) {
      case '1' ... '9':
        playByIndex(command - '0');
        break;

      case 'f':
        handleFolderPlay(input);
        break;

      case 's':
        myDFPlayer.stop();
        Serial.println("Music stopped.");
        break;

      case 'p':
        togglePause();
        break;

      case 'r':
        toggleRepeat();
        break;

      case 'v':
        setVolume(input.substring(1));
        break;

      case '+':
        myDFPlayer.volumeUp();
        Serial.print("Volume increased to: ");
        Serial.println(myDFPlayer.readVolume());
        break;

      case '-':
        myDFPlayer.volumeDown();
        Serial.print("Volume decreased to: ");
        Serial.println(myDFPlayer.readVolume());
        break;

      case '<':
        myDFPlayer.previous();
        delay(200); // Wait for player to update state
        Serial.print("Previous track: ");
        Serial.println(myDFPlayer.readCurrentFileNumber());
        break;

      case '>':
        myDFPlayer.next();
        delay(200);
        Serial.print("Next track: ");
        Serial.println(myDFPlayer.readCurrentFileNumber());
        break;
      case 'l':
        setVolume("20");
        playByIndex(1);
        toggleRepeat();
        break;

      default:
        Serial.println("Invalid command. Try again.");
        break;
    }

    printMenu();
  } else {
    if (!isPlaying) {
      myDFPlayer.loop(1); // Play track 1
      isPlaying = true;   // Mark as playing
    }      
  }
}

void playByIndex(uint8_t index) {
  myDFPlayer.play(index);
  Serial.print("Playing track ");
  Serial.println(index);
}

void togglePause() {
  isPaused = !isPaused;
  if (isPaused) {
    myDFPlayer.pause();
    Serial.println("Playback paused.");
  } else {
    myDFPlayer.start();
    Serial.println("Playback resumed.");
  }
}

void toggleRepeat() {
  isRepeating = !isRepeating;
  if (isRepeating) {
    myDFPlayer.enableLoop();
    Serial.println("Repeat mode enabled.");
  } else {
    myDFPlayer.disableLoop();
    Serial.println("Repeat mode disabled.");
  }
}

void setVolume(String volStr) {
  int volume = volStr.toInt();
  if (volume >= 0 && volume <= 30) {
    myDFPlayer.volume(volume);
    Serial.print("Volume set to: ");
    Serial.println(volume);
  } else {
    Serial.println("Invalid volume. Must be 0–30.");
  }
}

void handleFolderPlay(String input) {
  int indexF = input.indexOf('f');
  int indexS = input.indexOf('s');
  if (indexF == -1 || indexS == -1 || indexF >= indexS) {
    Serial.println("Invalid 'f' command. Use format: f<folder>s<track>");
    return;
  }

  int folder = input.substring(indexF + 1, indexS).toInt();
  int song = input.substring(indexS + 1).toInt();

  if (folder < 1 || folder > 99 || song < 1 || song > 255) {
    Serial.println("Folder or song number out of range.");
    return;
  }

  myDFPlayer.playFolder(folder, song);
  Serial.print("Playing folder ");
  Serial.print(folder);
  Serial.print(", song ");
  Serial.println(song);
}

void printMenu() {
  Serial.println(F("\n=================================================================="));
  Serial.println(F("Commands:"));
  Serial.println(F(" [1-9]        Play track number (from root)"));
  Serial.println(F(" fXsY         Play song Y from folder X (e.g., f2s5)"));
  Serial.println(F(" s            Stop playback"));
  Serial.println(F(" p            Pause/resume playback"));
  Serial.println(F(" r            Toggle repeat mode"));
  Serial.println(F(" vX           Set volume to X (0-30)"));
  Serial.println(F(" + / -        Increase/decrease volume"));
  Serial.println(F(" < / >        Previous/next track"));
  Serial.println(F("==================================================================\n"));
}
