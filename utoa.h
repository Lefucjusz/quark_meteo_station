/*
 * utoa.h
 *
 *  Created on: 21.05.2022
 *      Author: Lefucjusz
 */

#ifndef UTOA_H_
#define UTOA_H_

/* This is NON-STANDARD utoa(), it works only with base 10 numbers!
 * (actually every utoa() is non-standard, but you get the point...)
 */
char* utoa(unsigned number, char* result);

#endif /* UTOA_H_ */
