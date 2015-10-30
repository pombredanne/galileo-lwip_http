/* groveLCDUtils.c - Utility functions for the Grove-LCD */

/*
 * Copyright (c) 2015, Wind River Systems, Inc.
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

/*
 * Utility functions for the Grove-LCD
 */

#if defined(CONFIG_STDOUT_CONSOLE)
#include <stdio.h>
#define PRINT           printf
#else
#include <misc/printk.h>
#define PRINT           printk
#endif

#include <string.h>

#include <zephyr.h>
#include <i2c.h>
#include "hd44780Lcd.h"
#include "groveLCDUtils.h"

/*******************************************************************************
* groveLcdCommand - Send a 1 byte command to the LCD controller
*
* Create a I2C packet with byte 0 set to 0 (Command byte) and byte 1 set to
* the value passed by the user
*
* Arguments: i2c   - I2C device structure pointer
*            value - 1 byte command (See HD44780 docs)
*
* Returns:  void
*           Exits on error via lcd_err_exit()
*/
void groveLcdCommand (struct device *i2c, unsigned char value)
    {
    uint16_t i2c_addr = LCD_ADDR;
    uint8_t buf[] = {0, value};

    if (i2c == NULL)
        return;

    i2c_polling_write(i2c, buf, sizeof(buf), i2c_addr);

    /*
     * Wait long enough for command to work.  See GroveLCD documents
     */

    switch(value)
        {
        case LCD_CLEAR:        // Wait 1.53 ms (rounded to 1.6 ms)
        case LCD_CURSORHOME:
            task_sleep(1600 * sys_clock_ticks_per_sec / 1000);
            break;

        default:              // Wait 39 usec (rounded up to 40)
            task_sleep(40 * sys_clock_ticks_per_sec / 1000);
            break;
        }
    }


/*******************************************************************************
* groveLcdColorSet - Sets the backlight color of the Grove LCD display
*
* Create a I2C packet with 2 bytes that are used to access the RGB register
* and assign the red, green and blue values that controls the backlight color
* of the Grove LCD display.
*
* Arguments: i2c   - I2C device structure pointer
*            red   - byte (0-255) controlling color red intensity
*            green - byte (0-255) controlling color green intensity
*            blue  - byte (0-255) controlling color blue intensity
*
* Returns:  void
*/
void groveLcdColorSet (struct device *i2c, uint8_t red, uint8_t green,
                       uint8_t blue)
    {
    uint16_t i2c_addr = RGB_ADDR;
    uint8_t buf[] = {0, 0};

    if (i2c == NULL)
        return;

    buf[0] = 0; buf[1] = 0;
    i2c_polling_write(i2c, buf, sizeof(buf), i2c_addr);
    buf[0] = 1; buf[1] = 0;
    i2c_polling_write(i2c, buf, sizeof(buf), i2c_addr);
    buf[0] = 8; buf[1] = 0xaa;
    i2c_polling_write(i2c, buf, sizeof(buf), i2c_addr);

    buf[0] = 4; buf[1] = red;
    i2c_polling_write(i2c, buf, sizeof(buf), i2c_addr);
    buf[0] = 3; buf[1] = green;
    i2c_polling_write(i2c, buf, sizeof(buf), i2c_addr);
    buf[0] = 2; buf[1] = blue;
    i2c_polling_write(i2c, buf, sizeof(buf), i2c_addr);
    }


/*******************************************************************************
* groveLcdCursorSet - Positions the Grove LCD display cursor
*
* Position the Grove LCD display cursor in the row and column requested.
* Note that it is possible to position the cursor outside of visible range.
*
* Arguments: i2c    - I2C device structure pointer
*            row    - byte (visible range 0-1) set cursor row position
*            column - byte (visible range 0-15) set cursor column position
*
* Returns:  void
*/
void groveLcdCursorSet (struct device *i2c, uint8_t row, uint8_t column)
    {
    uint16_t i2c_addr = LCD_ADDR;
    uint8_t buf[] = {0, 0};

    if (i2c == NULL)
        return;

    column = (row == 0 ? (column | LCD_SETDDRAMADDR) :
                         (column | LCD_SETDDRAMADDR | LCD_SETCGRAMADDR));
    buf[1] = column;
    i2c_polling_write(i2c, buf, sizeof(buf), i2c_addr);
    }


/*******************************************************************************
* groveLcdInit - Initialize the Grove LCD display mode
*
* Configures the Grove LCD display for 2 line display and a visible blinking
* cursor.
*
* Arguments: i2c - I2C device structure pointer
*
* Returns:  void
*/
void groveLcdInit (struct device *i2c)
    {
    uint16_t i2c_addr = LCD_ADDR;
    uint8_t buf[] = {0, 0};

    PRINT("Initialize LCD ... ");

    if (i2c == NULL)
        {
        PRINT("failed! \n");
        return;
        }

    /* 2 line display */

    buf[1] = (LCD_FUNCTIONSET | LCD_4BITMODE | LCD_2LINE | LCD_5x8DOTS);
    i2c_polling_write(i2c, buf, sizeof(buf), i2c_addr);

    /* Cursor off, blinking off */

    buf[1] = (LCD_DISPLAYCONTROL | LCD_DISPLAYON | LCD_CURSOROFF | LCD_BLINKOFF);
    i2c_polling_write(i2c, buf, sizeof(buf), i2c_addr);

    PRINT("done\n");
    }

/*******************************************************************************
* groveLcdClear - Clears the Grove LCD display
*
* Create a I2C packet with byte 0 set to 0 (Command byte) and byte 1 set to
* LCD_CLEAR
*
* Arguments: i2c - I2C device structure pointer
*
* Returns:  void
*/
void groveLcdClear (struct device *i2c)
    {
    uint16_t i2c_addr = LCD_ADDR;
    uint8_t buf[] = {0, LCD_CLEAR};

    if (i2c == NULL)
        return;

    i2c_polling_write(i2c, buf, sizeof(buf), i2c_addr);
    }


/*******************************************************************************
* groveLcdWrite - Send a multibyte string to the Display RAM on LCD controller
*
* Create a I2C packet with byte 0 set to 0x40 (DPRAM) and the rest of the
* callers buffer after that.  Send it all off to the LCD I2C controller.
*
* Arguments: i2c   - I2C device structure pointer
*            value - array of DPRAM values (See HD44780 docs)
*            len   - Number of bytes in value[]
*
* Returns:  void
*           Exits on error via lcd_err_exit()
*/
void groveLcdWrite (struct device *i2c, unsigned char *string, int len)
    {
    int i;
    uint8_t buf[] = {LCD_SETCGRAMADDR, 0};

    if (i2c == NULL)
        return;

    if (len > 127)
        return;

    for (i = 0; i < len; i++)
        {
        buf[1] = string[i];
        i2c_polling_write(i2c, buf, sizeof(buf), LCD_ADDR);
        }

    task_sleep(45 * sys_clock_ticks_per_sec / 1000);

    }

/*******************************************************************************
* groveLcdPrint - Display a string on the Grove LCD display
*
* Displays a given string on the Grove LCD display starting at row and column
*
* Arguments: i2c    - I2C device structure pointer
*            row    - byte, 0 or 1 for row 0 or row 1
*            column - byte, 0 - 15 for visible columns
*            string - string pointer
*            len    - length of the string to display
*
* Returns:  void
*/
void groveLcdPrint (struct device *i2c, uint8_t row, uint8_t column,
                    char * string, uint8_t len)
    {
    int i;
    uint8_t buf[] = {LCD_SETCGRAMADDR, 0};

    if (i2c == NULL)
        return;

    groveLcdCursorSet(i2c, row, column);

    for (i = 0; i < len; i++)
        {
        buf[1] = string[i];
        i2c_polling_write(i2c, buf, sizeof(buf), LCD_ADDR);
        }
    }
