#include "lcd.h"

static void lcd_write_4_bits(uint8_t value);
static void lcd_mdelay(uint32_t count);
static void lcd_udelay(uint32_t count);
static void lcd_enable(void);

gpio_handle_t lcd;

void lcd_init(void)
{
  //1. configure GPIO pins to use for LCD connection

  lcd.gpiox                    = LCD_GPIO_PORT;
  lcd.gpio_config.pin_mode     = GPIO_MODE_OUT;
  lcd.gpio_config.pin_op_type  = GPIO_OP_TYPE_PP;
  lcd.gpio_config.pin_pu_pd    = GPIO_PUPD_NO;
  lcd.gpio_config.pin_speed    = GPIO_SPEED_FAST;

  lcd.gpio_config.pin_number   = LCD_GPIO_RS; // Register Select pin [pull it to 0 for command and 1 for data]
  GPIO_Init(&lcd);

  lcd.gpio_config.pin_number   = LCD_GPIO_RW; // Read/Write pin [we only write hence RW=0 through program execution]
  GPIO_Init(&lcd);

  lcd.gpio_config.pin_number   = LCD_GPIO_EN; // Enable pin [High to low transition on this pin make LCD latches (read data)]
  GPIO_Init(&lcd);

  lcd.gpio_config.pin_number   = LCD_GPIO_D4; // 8 parallel data pins [DB0 to DB7, only using 4-bit DB4 to DB7]
  GPIO_Init(&lcd);

  lcd.gpio_config.pin_number   = LCD_GPIO_D5;
  GPIO_Init(&lcd);

  lcd.gpio_config.pin_number   = LCD_GPIO_D6;
  GPIO_Init(&lcd);

  lcd.gpio_config.pin_number   = LCD_GPIO_D7;
  GPIO_Init(&lcd);

  GPIO_WriteToOutputPin(lcd.gpiox, LCD_GPIO_RS, GPIO_PIN_RESET);
  GPIO_WriteToOutputPin(lcd.gpiox, LCD_GPIO_RW, GPIO_PIN_RESET);
  GPIO_WriteToOutputPin(lcd.gpiox, LCD_GPIO_EN, GPIO_PIN_RESET);
  GPIO_WriteToOutputPin(lcd.gpiox, LCD_GPIO_D4, GPIO_PIN_RESET);
  GPIO_WriteToOutputPin(lcd.gpiox, LCD_GPIO_D5, GPIO_PIN_RESET);
  GPIO_WriteToOutputPin(lcd.gpiox, LCD_GPIO_D6, GPIO_PIN_RESET);
  GPIO_WriteToOutputPin(lcd.gpiox, LCD_GPIO_D7, GPIO_PIN_RESET);

  //2. LCD Initialisation by instruction [HD44780U - Reference Manual; page: 46, figure: 24; 4-Bit Interface]
  //
  // 40ms after Vcc rises to 2.7V or 15ms when Vcc rises to 4.5V
  lcd_mdelay(40);

  // RS=0 R/W=0 DB7=0 DB6=0 DB5=1 DB4=1
  GPIO_WriteToOutputPin(lcd.gpiox, LCD_GPIO_RS, GPIO_PIN_RESET); // RS=0 `Register Select` pull to low for lcd commands
  GPIO_WriteToOutputPin(lcd.gpiox, LCD_GPIO_RW, GPIO_PIN_RESET); // RW=0 `Read/Write` pull to low throughout program execution
  lcd_write_4_bits(0x3);                                         // DB7=0 DB6=0 DB5=1 DB4=1

  lcd_mdelay(5);                                                 // 4.1ms delay

  // RS=0 R/W=0 DB7=0 DB6=0 DB5=1 DB4=1
  GPIO_WriteToOutputPin(lcd.gpiox, LCD_GPIO_RS, GPIO_PIN_RESET); // RS=0 `Register Select` pull to low for lcd commands
  GPIO_WriteToOutputPin(lcd.gpiox, LCD_GPIO_RW, GPIO_PIN_RESET); // RW=0 `Read/Write` pull to low throughout program execution
  lcd_write_4_bits(0x3);                                         // DB7=0 DB6=0 DB5=1 DB4=1

  lcd_udelay(150);                                               // 100us delay

  // RS=0 R/W=0 DB7=0 DB6=0 DB5=1 DB4=1
  GPIO_WriteToOutputPin(lcd.gpiox, LCD_GPIO_RS, GPIO_PIN_RESET); // RS=0 `Register Select` pull to low for lcd commands
  GPIO_WriteToOutputPin(lcd.gpiox, LCD_GPIO_RW, GPIO_PIN_RESET); // RW=0 `Read/Write` pull to low throughout program execution
  lcd_write_4_bits(0x3);                                         // DB7=0 DB6=0 DB5=1 DB4=1

  // RS=0 R/W=0 DB7=0 DB6=0 DB5=1 DB4=0
  GPIO_WriteToOutputPin(lcd.gpiox, LCD_GPIO_RS, GPIO_PIN_RESET); // RS=0 `Register Select` pull to low for lcd commands
  GPIO_WriteToOutputPin(lcd.gpiox, LCD_GPIO_RW, GPIO_PIN_RESET); // RW=0 `Read/Write` pull to low throughout program execution
  lcd_write_4_bits(0x2);                                         // DB7=0 DB6=0 DB5=1 DB4=0

  // Function set comes next in LCD initialisation by instruction [RM page: 46, figure: 24]
  // Function set will set the interface to be 4 bits long however interface length is 8 bits
  // RS  RW   [  DB7   DB6    DB5    DB3   ]
  //  0   0   [   0     0      1      DL(0)] -> 2   DL=1 => 8bits, DL=0 4bits
  //  0   0   [  N(1)  F(0)    -      -    ] -> 8   N=1 => 2-lines, N=0 => 1-line; F=1 => 5x10 dots, F=0 => 5x8 dots
  lcd_send_command(LCD_CMD_DL4BIT_N2LINES_F5X8DOTS); // 0x28 as shown above
  lcd_send_command(LCD_CMD_DISPLAYON_CURSORON);      // 0x0E display ON, cursor ON
  lcd_display_clear();                               // 0x01 display clear
  lcd_send_command(LCD_CMD_INC_DDRAM);               // 0x06 `entry mode set` increment the RAM, sets cursor move direction Table 6

}

// High to low transition for LCD to latch [Reference Manual page: 25, Table: 6]
static void lcd_enable(void)
{
  // RS=1 R/W=0 DB7 to DB0 n/a | Check BF (Busy Flag) or wait as done below also instructed in Table 6, page: 25
  GPIO_WriteToOutputPin(lcd.gpiox, LCD_GPIO_EN, GPIO_PIN_SET);
  lcd_udelay(10);
  GPIO_WriteToOutputPin(lcd.gpiox, LCD_GPIO_EN, GPIO_PIN_RESET);
  lcd_udelay(100); // executiong time should be > 37us
}

static void lcd_write_4_bits(uint8_t value)
{
  GPIO_WriteToOutputPin(lcd.gpiox, LCD_GPIO_D4, (value >> 0) & 0x1);
  GPIO_WriteToOutputPin(lcd.gpiox, LCD_GPIO_D5, (value >> 1) & 0x1);
  GPIO_WriteToOutputPin(lcd.gpiox, LCD_GPIO_D6, (value >> 2) & 0x1);
  GPIO_WriteToOutputPin(lcd.gpiox, LCD_GPIO_D7, (value >> 3) & 0x1);

  lcd_enable();
}

void lcd_send_command(uint8_t command)
{


}

void lcd_display_clear(void)
{
  lcd_send_command(LCD_CMD_DISPLAY_CLEAR);
  lcd_mdelay(2);                             // page: 24, display clear execution time 2ms
}

static void lcd_mdelay(uint32_t count)
{
  for (uint32_t i=0; i< (count * 1000); i++);
}

static void lcd_udelay(uint32_t count)
{
  for (uint32_t i=0; i< (count * 1); i++);
}
