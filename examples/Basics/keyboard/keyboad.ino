#include <M5ez.h>
#include <Free_Fonts.h>

#define KEYBOARD_X (2)
#define KEYBOARD_Y (26)

#define KEY_W (45)
#define KEY_H (50)

#define COLS (7)
#define ROWS (4)

#define MAX_SHIFT_MODE (4)

char keymap[MAX_SHIFT_MODE][ROWS][COLS] =
{
  {
    {'a', 'b', 'c', 'd', 'e', 'f', 'g'},
    {'h', 'i', 'j', 'k', 'l', 'm', 'n'},
    {'o', 'p', 'q', 'r', 's', 't', 'u'},
    {'v', 'w', 'x', 'y', 'z', ' ', '\002'}, // 002 = shift
  },
  {
    {'A', 'B', 'C', 'D', 'E', 'F', 'G'},
    {'H', 'I', 'J', 'K', 'L', 'M', 'N'},
    {'O', 'P', 'Q', 'R', 'S', 'T', 'U'},
    {'V', 'W', 'X', 'Y', 'Z', ' ', '\002'}, // 002 = shift
  },
  {
    {'`', '1', '2', '3', '4', '5', '6'},
    {'7', '8', '9', '0', '-', '=', '['},
    {']', '\\', ';', '\'', ',', '.', '/'},
    {' ', ' ', ' ', ' ', ' ', ' ', '\002'}, // 002 = shift
  },
  {
    {'~', '!', '@', '#', '$', '%', '^'},
    {'&', '*', '(', ')', '_', '+', '{'},
    {'}', '|', ':', '"', '<', '>', '?'},
    {' ', ' ', ' ', ' ', ' ', ' ', '\002'}, // 002 = shift
  },
};

Button *button_list[ROWS][COLS];

typedef enum {
  KEY_MODE_LETTER = 0,
  KEY_MODE_NUMBER = 1,
} key_mode_t;

String input_text = "";
key_mode_t key_mode = KEY_MODE_LETTER;
bool shift_mode = false;
bool keyboard_done = false;

ButtonColors bc_on = {BLUE, GREEN, DARKGREY};
ButtonColors bc_off = {BLACK, GREEN, DARKGREY};

void runKeyboard(void);
void initKeyboard(void);
void deinitKeyboard(void);
void drawKeyboard(void);
void buttonEvent(Event& e);

void runKeyboard()
{
  initKeyboard();
  drawKeyboard();
  keyboard_done = false;
  while(keyboard_done == false)
  {
    M5.Touch.update();
  }
  while(M5.BtnB.isPressed())
  {
    M5.Touch.update();
  }
  deinitKeyboard();
}

void initKeyboard()
{
  M5.Lcd.fillScreen(TFT_BLACK);
  M5.Lcd.setTextSize(1);
  M5.Lcd.setTextColor(TFT_GREEN, TFT_BLACK);
  M5.Lcd.setFreeFont(FF1);

  // Button A
  M5.Lcd.drawString("delete", 50, 233, 2);
  // Button B
  M5.Lcd.drawString("done", 160, 233, 2);
  // Button C
  M5.Lcd.drawString("mode", 265, 233, 2);

  for(int r = 0; r < ROWS; r++)
  {
    for(int c = 0; c < COLS; c++)
    {
      button_list[r][c] = new Button(0, 0, 0, 0, false, "", bc_off, bc_on);
      button_list[r][c]->setTextSize(1);
    }
  }

  M5.Events.addHandler(buttonEvent, E_TOUCH + E_BTNONLY);

  input_text = "";
  key_mode = KEY_MODE_LETTER;
  shift_mode = false;
}

void deinitKeyboard()
{
  M5.Events.delHandlers(buttonEvent, nullptr, nullptr);

  for(int r = 0; r < ROWS; r++)
  {
    for(int c = 0; c < COLS; c++)
    {
      delete(button_list[r][c]);
      button_list[r][c] = NULL;
    }
  }
}

void drawKeyboard()
{
  int x, y;

  for(int r = 0; r < ROWS; r++)
  {
    for(int c = 0; c < COLS; c++)
    {
      x = (KEYBOARD_X + (c * KEY_W));
      y = (KEYBOARD_Y + (r * KEY_H));
      button_list[r][c]->set(x, y, KEY_W, KEY_H);

      int key_page = 0;

      if(key_mode == KEY_MODE_NUMBER) key_page += 2;
      if(shift_mode == true) key_page += 1;

      String key;
      char ch = keymap[key_page][r][c];

      if(ch == '\002')  // Shift
      {
        button_list[r][c]->setFreeFont(FF1);
        key = "shft";
      }
      else
      {
        button_list[r][c]->setFreeFont(FF3);
        key = String(ch);
      }
      button_list[r][c]->setLabel(key.c_str());
      button_list[r][c]->draw();
    }
  }
}

void buttonEvent(Event& e)
{
  Button& b = *e.button;

  // Backspace
  if(e.button == &M5.BtnA)
  {
    input_text = input_text.substring(0, input_text.length() - 1);
  }
  // Done
  else if(e.button == &M5.BtnB)
  {
    keyboard_done = true;
    return;
  }
  // Key mode
  else if(e.button == &M5.BtnC)
  {
    switch(key_mode)
    {
      case KEY_MODE_LETTER:
        key_mode = KEY_MODE_NUMBER;
        break;
      default:
      case KEY_MODE_NUMBER:
        key_mode = KEY_MODE_LETTER;
        break;
    }
    shift_mode = false;
    drawKeyboard();
    return;
  }
  else
  {
    if(String(b.label) == "shft")
    {
      shift_mode = !shift_mode;
      drawKeyboard();
      return;
    }
    input_text += b.label;
  }
  // Clear input text area
  M5.Lcd.fillRect(0, 0, M5.Lcd.width(), KEYBOARD_Y - 1, TFT_BLACK);
  M5.Lcd.setFreeFont(FF2);
  M5.Lcd.drawString(input_text, 0, 10);
}

void setup()
{
  ez.begin();
}

void loop()
{
  ez.msgBox("M5ez keyboard test", input_text, "Keyboard");
  runKeyboard();
}
