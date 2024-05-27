#define VOLTAGE_OFFSET  2.5   //512 out of 1023 in Arduino UNO
#define VOLTAGE_SCALL   (0.362)
#define CURRENT_OFFSET  474   //474 out of 1023 in Arduino UNO
#define CURRENT_SCALL   (69e-7)  


void power_setup(void) {
  pinMode(CURRENT_SEN_PIN, INPUT);
  pinMode(VOLTAGE_SEN_PIN, INPUT);
  randomSeed(analogRead(VOLTAGE_SEN_PIN) + millis());
}


float rms_voltage(void)
{
  //find mean of 10 cycle
  long t = millis();
  long sum_min = 0;
  long sum_max = 0;
  for(int i=0; i<10; i++){ //avg of 10 cycle
    int tem_min = 4096;
    int tem_max = 0;
    t = millis();
    for(; millis()-t < 25; ){ // wait 25ms to confirm 1 complete cycle
      int tem = analogRead(VOLTAGE_SEN_PIN);
      // log min
      if(tem < tem_min)
        tem_min = tem;
      if(tem > tem_max)
        tem_max = tem;
      delayMicroseconds(200);
    }
    sum_min += tem_min;
    sum_max += tem_max;
  }
  return (sum_max-sum_min)/10.0 * (230/1800.0);

//  voltage_rms =  232.603 + (random(1000)-500)*2/500.0;
//  return voltage_rms;
}



float rms_current(void)
{
  //find mean of 10 cycle
  long t = millis();
  long sum_min = 0;
  long sum_max = 0;
  for(int i=0; i<10; i++){ //avg of 10 cycle
    int tem_min = 4096;
    int tem_max = 0;
    t = millis();
    for(; millis()-t < 25; ){ // wait 25ms to confirm 1 complete cycle
      int tem = analogRead(CURRENT_SEN_PIN);
      // log min
      if(tem < tem_min)
        tem_min = tem;
      if(tem > tem_max)
        tem_max = tem;
      delayMicroseconds(200);
    }
    sum_min += tem_min;
    sum_max += tem_max;
  }
  return (sum_max-sum_min)/10.0 * (0.9/1950.0);

//  voltage_rms =  232.603 + (random(1000)-500)*2/500.0;
//  return voltage_rms;
}
