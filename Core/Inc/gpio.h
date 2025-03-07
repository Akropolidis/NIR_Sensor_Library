#ifndef GPIO_H_
#define GPIO_H_

#include <stdbool.h>
#include "stm32f4xx.h"

void GPIO_Init(void);
bool UserBtn_Control(void);


#endif /* GPIO_H_ */
