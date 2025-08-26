#ifndef __RINGBUFF_H
#define __RINGBUFF_H
#include <stdint.h>

//ringbuff
typedef struct{
	const uint32_t ringbuff_NUM;
	uint8_t *const p_ringbuff;
	uint32_t tail;
	uint32_t head;
}ringbuff_handle;

#define RING_BUF_DEF(x,y)   			\
		uint8_t x##_ringbuff[y];		\
		ringbuff_handle x={				\
			.p_ringbuff=x##_ringbuff,	\
			.head=0,					\
			.tail=0,					\
			.ringbuff_NUM = y			\
		}

		
uint8_t rb_write(ringbuff_handle *rb,uint8_t data);
uint8_t rb_read(ringbuff_handle *rb,uint8_t *data);
		
#endif // !__RINGBUFF_H
