  #define PIN_A_RED    2                                                                     
  #define PIN_A_YELLOW 3                                                                     
  #define PIN_A_GREEN  4
  #define PIN_B_RED    5
  #define PIN_B_YELLOW 6
  #define PIN_B_GREEN  7                                                                     
  #define BTN_A        8
  #define BTN_B        9                                                                     
                  
  #define RED    0
  #define YELLOW 1
  #define GREEN  2
                                                                                             
  #define YELLOW_TIME  2000   // ms
                                                                                             
  struct Light {  
      int pin_r, pin_y, pin_g;
      int state;                                                                             
      int vehicles;
  };                                                                                         
                  
  struct Light A = { PIN_A_RED, PIN_A_YELLOW, PIN_A_GREEN, RED, 0 };
  struct Light B = { PIN_B_RED, PIN_B_YELLOW, PIN_B_GREEN, RED, 0 };
                                                                                             
  // Turn on the correct LED, turn off the others                                            
  void apply(struct Light *x) {                                                              
      digitalWrite(x->pin_r, x->state == RED);                                               
      digitalWrite(x->pin_y, x->state == YELLOW);
      digitalWrite(x->pin_g, x->state == GREEN);                                             
  }
                                                                                             
  void set_state(struct Light *x, int s) {
      x->state = s;
      apply(x);                                                                              
  }
                                                                                             
  // Adaptive green time based on vehicle count
  unsigned long green_time(struct Light *x) {
      if (x->vehicles > 10) return 10000;
      if (x->vehicles >  5) return  7000;                                                    
      return 5000;
  }                                                                                          
                  
  int           phase = 1;
  unsigned long phase_start;
                                                                                             
  void setup() {
      Serial.begin(9600);                                                                    
                  
      int out_pins[] = { PIN_A_RED, PIN_A_YELLOW, PIN_A_GREEN,                               
                         PIN_B_RED, PIN_B_YELLOW, PIN_B_GREEN };
      for (int i = 0; i < 6; i++) pinMode(out_pins[i], OUTPUT);                              
                                                                                             
      pinMode(BTN_A, INPUT_PULLUP);
      pinMode(BTN_B, INPUT_PULLUP);                                                          
                  
      // Start immediately: A green, B red                                                   
      set_state(&A, GREEN);
      set_state(&B, RED);                                                                    
      phase_start = millis();

      Serial.println("Traffic light running.");
  }

  void loop() {                                                                              
      // Buttons add vehicles (held = keeps incrementing, press once = +1)
      if (digitalRead(BTN_A) == LOW) A.vehicles++;                                           
      if (digitalRead(BTN_B) == LOW) B.vehicles++;
                                                                                             
      unsigned long elapsed = millis() - phase_start;
                                                                                             
      switch (phase) {
          case 1:  // A is GREEN
              if (elapsed >= green_time(&A)) {
                  set_state(&A, YELLOW);
                  phase = 2;  phase_start = millis();                                        
              }
              break;                                                                         
                  
          case 2:  // A is YELLOW -> switch to B                                             
              if (elapsed >= YELLOW_TIME) {
                  set_state(&A, RED);    A.vehicles = 0;                                     
                  set_state(&B, GREEN);
                  phase = 3;  phase_start = millis();
                  Serial.println("B is now GREEN");                                          
              }
              break;                                                                         
                  
          case 3:  // B is GREEN
              if (elapsed >= green_time(&B)) {
                  set_state(&B, YELLOW);                                                     
                  phase = 4;  phase_start = millis();
              }                                                                              
              break;

          case 4:  // B is YELLOW -> back to A
              if (elapsed >= YELLOW_TIME) {
                  set_state(&B, RED);    B.vehicles = 0;
                  set_state(&A, GREEN);                                                      
                  phase = 1;  phase_start = millis();
                  Serial.println("A is now GREEN");                                          
              }   
              break;
      }
  }

