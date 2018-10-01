/*
 * simpleserial.h
 *
 * Created: 11/30/2018 5:16:39 PM
 *  Author: User
 */ 


#ifndef SIMPLESERIAL_H_
#define SIMPLESERIAL_H_

void simpleserial_init(void);
int simpleserial_addcmd(char c, unsigned int len, uint8_t (*fp)(uint8_t*));
void simpleserial_get(void);
void simpleserial_put(char c, int size, uint8_t* output);

#endif /* SIMPLESERIAL_H_ */