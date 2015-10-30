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
 * DESCRIPTION
 * Header file for Hitachi HD44780 LCD controller
 */

#ifndef _GROVELCDUTILS_H_
#define _GROVELCDUTILS_H_



#define LCD_ADDR 0x003e
#define RGB_ADDR 0x0062


extern void groveLcdPrint(struct device *i2c,
                          uint8_t row, uint8_t column,
                          char * string, uint8_t len);
extern void groveLcdCommand(struct device *i2c, unsigned char value);
extern void groveLcdColorSet (struct device *i2c,
                              uint8_t red, uint8_t green, uint8_t blue);
extern void groveLcdCursorSet (struct device *i2c,
                               uint8_t row, uint8_t column);
extern void groveLcdInit (struct device *i2c);
extern void groveLcdClear (struct device *i2c);
extern void groveLcdWrite(struct device *i2c,
                          unsigned char *string, int len);
extern void groveLcdPrint(struct device *i2c,
                          uint8_t row, uint8_t column,
                          char * string, uint8_t len);

#endif /* _GROVELCDUTILS_H_ */
