#define MAX_MESSAGE_LENGTH 64

void setup() {
  Serial.begin(9600);
  Serial.setTimeout(1);

  Serial.println("Ready");
}

void loop() {
 //Check to see if anything is available in the serial receive buffer

 while (Serial.available() > 0)
 {
   //Create a place to hold the incoming message
   static char message[MAX_MESSAGE_LENGTH];
   static unsigned int message_pos = 0;

   //Read the next available byte in the serial receive buffer
   char inByte = Serial.read();

  // message end flag 'E'
   if (inByte != 'E')
   {
     //Add the incoming byte to our message
     message[message_pos] = inByte;
     message_pos++;
   }
   //Full message received...
   else
   {
      //Add null character to string
      message[message_pos] = '\0';

      //Reset for the next message
      message_pos = 0;

      Serial.print("received ");
      Serial.println(message);

      //Do whatever you want with message
      
      Serial.println("done");
   }
 }
 
}
