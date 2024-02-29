
// When defined, displays button actions
#define DEBUG_BUTTON_PRESSES

/********************************************************************/
// Buttons
volatile bool insideButtonsClick;
volatile bool buttonA_singleClickEvent;
volatile bool buttonA_doubleClickEvent;
volatile bool buttonA_longClickEvent;
volatile bool buttonB_singleClickEvent;
volatile bool buttonB_doubleClickEvent;
volatile bool buttonB_longClickEvent;
volatile bool buttonC_singleClickEvent;
volatile bool buttonC_doubleClickEvent;
volatile bool buttonC_longClickEvent;

/********************************************************************/
void buttonA_singleClick() {
  menuInputTime = millis();
  if(!menuDisplaySleepMode) {
    menu.registerKeyPress(TEENYMENU_KEY_UP);
#ifdef DEBUG_BUTTON_PRESSES
    static uint16_t count=0;
    char _tempStr[22];
    sprintf(_tempStr, "BtnA.Short.%d", ++count);
    msg_update(_tempStr);
#endif
  }
  displayRefresh = true;
}
void buttonA_doubleClick() {
  menuInputTime = millis();
  if(!menuDisplaySleepMode) {
#ifdef DEBUG_BUTTON_PRESSES
    static uint16_t count=0;
    char _tempStr[22];
    sprintf(_tempStr, "BtnA.Double.%d", ++count);
    msg_update(_tempStr);
#endif
  }
  displayRefresh = true;
}
void buttonA_longClick() {
  menuInputTime = millis();
  if(!menuDisplaySleepMode) {
#ifdef DEBUG_BUTTON_PRESSES
    static uint16_t count=0;
    char _tempStr[22];
    sprintf(_tempStr, "BtnA.Long.%d", ++count);
    msg_update(_tempStr);
#endif
  }
  displayRefresh = true;
}
void buttonB_singleClick() {
  menuInputTime = millis();
  if(!menuDisplaySleepMode) {
    menu.registerKeyPress(TEENYMENU_KEY_RIGHT);
#ifdef DEBUG_BUTTON_PRESSES
    static uint16_t count=0;
    char _tempStr[22];
    sprintf(_tempStr, "BtnB.Short.%d", ++count);
    msg_update(_tempStr);
#endif
  }
  displayRefresh = true;
}
void buttonB_doubleClick() {
  menuInputTime = millis();
  if(!menuDisplaySleepMode) {
    menu.registerKeyPress(TEENYMENU_KEY_LEFT);
#ifdef DEBUG_BUTTON_PRESSES
    static uint16_t count=0;
    char _tempStr[22];
    sprintf(_tempStr, "BtnB.Double.%d", ++count);
    msg_update(_tempStr);
#endif
  }
  displayRefresh = true;
}
void buttonB_longClick() {
  menuInputTime = millis();
  if(!menuDisplaySleepMode) {
    menu.registerKeyPress(TEENYMENU_KEY_LEFT);
#ifdef DEBUG_BUTTON_PRESSES
    static uint16_t count=0;
    char _tempStr[22];
    sprintf(_tempStr, "BtnB.Long.%d", ++count);
    msg_update(_tempStr);
#endif
  }
  displayRefresh = true;
}
void buttonC_singleClick() {
  menuInputTime = millis();
  if(!menuDisplaySleepMode) {
    menu.registerKeyPress(TEENYMENU_KEY_DOWN);
#ifdef DEBUG_BUTTON_PRESSES
    static uint16_t count=0;
    char _tempStr[22];
    sprintf(_tempStr, "BtnC.Short.%d", ++count);
    msg_update(_tempStr);
#endif
  }
  displayRefresh = true;
}
void buttonC_doubleClick() {
  menuInputTime = millis();
  if(!menuDisplaySleepMode) {
#ifdef DEBUG_BUTTON_PRESSES
    static uint16_t count=0;
    char _tempStr[22];
    sprintf(_tempStr, "BtnC.Double.%d", ++count);
    msg_update(_tempStr);
#endif
  }
  displayRefresh = true;
}
void buttonC_longClick() {
  menuInputTime = millis();
  if(!menuDisplaySleepMode) {
#ifdef DEBUG_BUTTON_PRESSES
    static uint16_t count=0;
    char _tempStr[22];
    sprintf(_tempStr, "BtnC.Long.%d", ++count);
    msg_update(_tempStr);
#endif
  }
  displayRefresh = true;
}

/********************************************************************/
void buttons_tick() {
  if(insideButtonsClick) return;
  M5.update();
  if(M5.BtnA.event==E_TAP)         buttonA_singleClickEvent=true;
  if(M5.BtnA.event==E_DBLTAP)      buttonA_doubleClickEvent=true;
  if(M5.BtnA.event==E_LONGPRESSED) buttonA_longClickEvent=true;
  if(M5.BtnB.event==E_TAP)         buttonB_singleClickEvent=true;
  if(M5.BtnB.event==E_DBLTAP)      buttonB_doubleClickEvent=true;
  if(M5.BtnB.event==E_LONGPRESSED) buttonB_longClickEvent=true;
  if(M5.BtnC.event==E_TAP)         buttonC_singleClickEvent=true;
  if(M5.BtnC.event==E_DBLTAP)      buttonC_doubleClickEvent=true;
  if(M5.BtnC.event==E_LONGPRESSED) buttonC_longClickEvent=true;
}

/********************************************************************/
void buttons_click() {
  insideButtonsClick = true;
  if(buttonA_singleClickEvent) {
    buttonA_singleClick();
    buttonA_singleClickEvent = false;
  }
  if(buttonA_doubleClickEvent) {
    buttonA_doubleClick();
    buttonA_doubleClickEvent = false;
  }
  if(buttonA_longClickEvent) {
    buttonA_longClick();
    buttonA_longClickEvent = false;
  }
  if(buttonB_singleClickEvent) {
    buttonB_singleClick();
    buttonB_singleClickEvent = false;
  }
  if(buttonB_doubleClickEvent) {
    buttonB_doubleClick();
    buttonB_doubleClickEvent = false;
  }
  if(buttonB_longClickEvent) {
    buttonB_longClick();
    buttonB_longClickEvent = false;
  }
  if(buttonC_singleClickEvent) {
    buttonC_singleClick();
    buttonC_singleClickEvent = false;
  }
  if(buttonC_doubleClickEvent) {
    buttonC_doubleClick();
    buttonC_doubleClickEvent = false;
  }
  if(buttonC_longClickEvent) {
    buttonC_longClick();
    buttonC_longClickEvent = false;
  }
  insideButtonsClick = false; 
}

/********************************************************************/
void buttons_setup() {
  insideButtonsClick = false; 
  // Button locations
  //M5.BtnA.set( 10, 240, 110, 40); M5Core2.h default for rotation=1
  //M5.BtnB.set(130, 240,  70, 40); M5Core2.h default for rotation=1
  //M5.BtnC.set(230, 240,  80, 40); M5Core2.h default for rotation=1
  //M5.BtnA.set(240,  10, 40, 110); For rotation=2
  //M5.BtnB.set(240, 130, 40,  70); For rotation=2
  //M5.BtnC.set(240, 230, 40,  80); For rotation=2
  M5.BtnA.set(230,  10, 50, 110);
  M5.BtnB.set(230, 115, 50,  70);
  M5.BtnC.set(230, 220, 50,  80);
  // Timing
  M5.BtnA.tapTime       = 80; // default=150
  M5.BtnB.tapTime       = 80;
  M5.BtnC.tapTime       = 80;
  M5.BtnA.dbltapTime    = 300; // default=300
  M5.BtnB.dbltapTime    = 300;
  M5.BtnC.dbltapTime    = 300;
  M5.BtnA.longPressTime = 350;
  M5.BtnB.longPressTime = 350;
  M5.BtnC.longPressTime = 350;
}

