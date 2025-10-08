/*
 * wwdg.h
 *
 *  Created on: 7 paź 2024
 *      Author: asmnop
 */

#ifndef WWDG_H_
#define WWDG_H_


#include "universal_lib.h"


void WWDG_init(void);
void WWDG_reset_delayed(const uint8_t delay);
void WWDG_reset_immediate(void);


#endif /* WWDG_H_ */
