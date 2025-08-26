#include "ringbuff.h"

/**
 * @brief 向传入的环形缓冲区句柄对应的缓冲区内写入一个数据
 * 
 * @param rb 预先定义的任意的环形缓冲区实例
 * @param data 欲写入环形缓冲区内的数据
 * @return uint8_t 0：成功写入   1：缓冲区满
 */
uint8_t rb_write(ringbuff_handle *rb,uint8_t data)
{
	if( ((rb->head + 1)%rb->ringbuff_NUM) == rb->tail)
	{
		/*环形缓冲区已经满,不覆盖*/
		return 1;
	}
	rb->p_ringbuff[rb->head]=data;
	rb->head=(rb->head+1)%rb->ringbuff_NUM;
	return 0;
}

/**
 * @brief 从传入的环形缓冲区句柄对应的缓冲区内读出一个数据
 * 
 * @param rb 预先定义的任意的环形缓冲区实例
 * @param data 用于传出读取到的缓冲区变量
 * @return uint8_t 0：成功读取   1：缓冲区空
 */
uint8_t rb_read(ringbuff_handle *rb,uint8_t *data)
{
	if(rb->head == rb->tail)
	{
		/*环形缓冲区已空*/
		return 1;
	}
	*data = rb->p_ringbuff[rb->tail];
	rb->tail=(rb->tail+1)%rb->ringbuff_NUM;
	return 0;
}
