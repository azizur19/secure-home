//void setupLCD(void) {
//  lcd.init(); // initialize the lcd
//  lcd.backlight();
//  //lcd.setCursor(0, 2);       // (X,Y) (Column, Row)
//  lcdPrint("  IOT Based Smart");          // print message at the first row
//  lcdAppend(1, "  Home Automation"); // print message at the second row lcd.setCursor(0, 2);
//  lcdAppend(2, "Developed By:Titumir"); // print message at the second row lcd.setCursor(0, 2);
//  lcdAppend(3, "EEE, BUET"); // print message at the second row lcd.setCursor(0, 2);
//  delay(3000);
//}
//
//void lcdPrint(String str)
//{
//  lcd.clear();
//  lcd.setCursor(0,0);
//  lcd.print(str);
//}
//
//void lcdPrint(int row, String str)
//{
//  lcd.clear();
//  lcd.setCursor(0,row);
//  lcd.print(str);
//}
//void lcdPrint(int column, int row, String str)
//{
//  lcd.clear();
//  lcd.setCursor(column, row);
//  lcd.print(str);
//}
//
//void lcdAppend(int row, String str)
//{
//  lcd.setCursor(0,row);
//  lcd.print(str);
//}
//
//void lcdAppend(int column, int row, String str)
//{
//  lcd.setCursor(column, row);
//  lcd.print(str);
//}
