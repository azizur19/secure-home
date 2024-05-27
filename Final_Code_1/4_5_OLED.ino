//display.clearDisplay() – all pixels are off
//display.drawPixel(x,y, color) – plot a pixel in the x,y coordinates
//display.setTextSize(n) – set the font size, supports sizes from 1 to 8
//display.setCursor(x,y) – set the coordinates to start writing text
//display.print(“message”) – print the characters at location x,y
//display.display() – call this method for the changes to make effect\


void setup_oled(void) {
  oled.begin(0x3C, true);
  delay(100);
  oled.clearDisplay();
  oled.setTextSize(TEXT_SIZE);
  oled.setTextColor(SH110X_WHITE);
  oled.setCursor(0, 10);
  // Display static text
  oled.println("OLED SET UP DONE");
  oled.display(); 
  delay(3000);


//  lcdPrint("  IOT Based Smart");          // print message at the first row
//  lcdAppend(1, "  Home Automation"); // print message at the second row lcd.setCursor(0, 2);
//  lcdAppend(2, "Developed By:Titumir"); // print message at the second row lcd.setCursor(0, 2);
//  lcdAppend(3, "EEE, BUET"); // print message at the second row lcd.setCursor(0, 2);
//  delay(3000);
}

void oledPrint(String str)
{
  oled.clearDisplay();
  oled.setCursor(0,0);
  oled.println(str);
  oled.display();
}

void oledPrint(int row, String str)
{
  oled.clearDisplay();
  oled.setCursor(0,0);
  oled.println(str);
  oled.display();
}
void oledPrint(int column, int row, String str)
{
  oled.setCursor(column, row);
  oled.println(str);
  oled.display();
}

void oledAppend(int row, String str)
{
  oled.setCursor(0, row*LINE_GAP);
  oled.println(str);
  oled.display();
}

void oledAppend(int column, int row, String str)
{
  oled.setCursor(column, row*LINE_GAP);
  oled.println(str);
  oled.display();
}
