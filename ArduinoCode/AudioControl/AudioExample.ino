//-------------- Arduino --------------
// Base code taken from https://www.theamplituhedron.com/articles/How-to-use-the-Serial-MP3-Player-UART-with-Speaker-by-OPEN-SMART-with-Arduino/
// Modified by Kyle Sedgwick for the purpose of Escape Y2K project.

// Connections
// Arduino:           
//                                Serial MP3 Player Module (OPEN-SMART)
// D8 --------------------------- TX
// D7 --------------------------- RX

// Include required libraries:
#include <SoftwareSerial.h>

// Define the RX and TX pins to establish UART communication with the MP3 Player Module.
#define MP3_RX 8 // to TX
#define MP3_TX 7 // to RX

// Define the required MP3 Player Commands:

// Select storage device to TF card
static int8_t select_SD_card[] = {0x7e, 0x03, 0X35, 0x01, 0xef}; // 7E 03 35 01 EF
// Play with index: /01/001xxx.mp3
static int8_t play_first_song[] = {0x7e, 0x04, 0x41, 0x00, 0x01, 0xef}; // 7E 04 41 00 01 EF
// Play with index: /01/002xxx.mp3
static int8_t play_second_song[] = {0x7e, 0x04, 0x41, 0x00, 0x02, 0xef}; // 7E 04 41 00 02 EF
// Play the song.
static int8_t play[] = {0x7e, 0x02, 0x01, 0xef}; // 7E 02 01 EF
// Pause the song.
static int8_t pause[] = {0x7e, 0x02, 0x02, 0xef}; // 7E 02 02 EF
// + Volume
static int8_t volUp[] = {0x7e, 0x02, 0x05, 0xef}; // 7E 02 02 EF
// - Volume
static int8_t volDown[] = {0x7e, 0x02, 0x06, 0xef}; // 7E 02 02 EF


// Define the Serial MP3 Player Module.
SoftwareSerial MP3(MP3_RX, MP3_TX);

void setup() {
  // Initiate the serial monitor.
  Serial.begin(9600);
  // Initiate the Serial MP3 Player Module.
  MP3.begin(9600);
  // Select the SD Card.
  send_command_to_MP3_player(select_SD_card, 5);
  send_command_to_MP3_player(volDown, 4);
  send_command_to_MP3_player(volDown, 4);
  send_command_to_MP3_player(volDown, 4);
  send_command_to_MP3_player(volDown, 4);
  send_command_to_MP3_player(volDown, 4);
}

void loop() {
	// Play the second song.
  send_command_to_MP3_player(play_first_song, 6);
  delay(426000);

	send_command_to_MP3_player(play_second_song, 6);
  delay(615000);
}


void send_command_to_MP3_player(int8_t command[], int len){
  Serial.print("\nMP3 Command => ");
  for(int i=0;i<len;i++){ MP3.write(command[i]); Serial.print(command[i], HEX); }
  delay(1000);
}