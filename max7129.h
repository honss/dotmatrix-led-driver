#ifndef __MAX7219_H__
#define __MAX7219_H__
extern char matrix[8];
void drawMatrix(char * matrix);
extern float value;
void updateMatrix(float number, char * matrix);
#endif