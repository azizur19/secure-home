// set baud rate to 9600
void setupGSM(void)
{
  Serial.begin(9600);
}
void make_call_to(String phone_num)
{
  sendATCommand("AT");
  sendATCommand("AT+CMGF=1");
  sendATCommand("ATD"+phone_num+";"); // Enter the phone number you want to call here (prefix country code)//+8801785009293
}

void send_sms(String phone_num, String sms)
{
  sendATCommand("AT");
  sendATCommand("AT+CMGF=1");
  sendATCommand("AT+CMGS=\""+phone_num+"\""); // Enter your phone number here (prefix country code)
  sendATCommand(sms); // Enter your message here
  sendATCommand(String((char)(26)));
}

//void sendATCommand(String command)
//{
//  mySerial.println(command);
//  delay(500);
//
//  while (mySerial.available())
//  {
//    Serial.write(mySerial.read());
//  }
//  Serial.println();
//}


void sendATCommand(String command)
{
  Serial.println(command);
  delay(500);

  while (Serial.available())
  {
    Serial.write(Serial.read());
  }
  Serial.println();
}
