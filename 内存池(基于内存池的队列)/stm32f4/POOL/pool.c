#include "pool.h"
#include "print.h"
/* ��һ����������Ϊ�ڴ�� */
unsigned char mem_pool[948] = { 0 };

/* �ڴ��״̬��ʾ */
#if SHOW_POOL_INFO
pool_info_t pool_info = { 0 };
#endif

/* ����: �ڴ�س�ʼ��
 * ����: arg_num: �����κ���Ĳ�������; ...: �����εĵ�һ��Ϊ���ֿ�Ĵ�С, �ڶ���Ϊ���ֿ������,��������
 * ����ֵ: 
 */
int init_pool(int arg_num, ...)
{
    unsigned char i, j;
    unsigned int pool_size, pool_num;
    va_list ap;
    pool_t* temp;
    pool_t* p = (pool_t*)mem_pool;
    temp = p;

    va_start(ap, arg_num);
    for (i = 0; i < arg_num / 2; i++) {
        pool_size = va_arg(ap, int);    /* ���������ĵ�һ��ֵΪ���ֵ��ڴ��С */
        pool_num = va_arg(ap, int);     /* ���������ĵڶ���ֵΪ���ֵĿ��� */

        /* �԰����ô�С�Ϳ������л��� */
        for (j = 0; j < pool_num; j++) {
            /* ���ֵ��ڴ��С����������Ĵ�С */
            if (p->buf + pool_size - 1 >  mem_pool + sizeof(mem_pool)) {
                temp->next = NULL;
                return 1;
            }

            p->block_size = pool_size;
            p->next = (struct block*)(p->buf + pool_size);
            p->is_used = 0;
            temp = p;
            p = p->next;

    #if SHOW_POOL_INFO
            pool_info.total_block_num++;
            pool_info.used_pool_mm += pool_size + sizeof(pool_t);
    #endif
        }
    }
    va_end(ap);

    return 1;
}

/* ����: �ڴ���� 
 * ����: size: ��Ҫ������ڴ��С
 * ����ֵ: ������ڴ��ַ
 */
void* pool_malloc(unsigned int size)
{
    pool_t* p = (pool_t*)mem_pool;
    pool_t temp;
    temp.block_size = size;
    temp.next = &temp;

    /* �����ڴ�: �ҵ��ڴ����δʹ�õ���С�ڴ� */
    while (p) {
        if (p->is_used == 0 && p->block_size >= size) {
            /* ��һ���ҵ�һ��δʹ�õ�λ�� */
            if(temp.next == &temp)
            {
                temp.next = p;
            }
            /* �ҵ�һ����С������������ڴ� */
            if(p->block_size <= temp.next->block_size)
            {
                temp.next = p;
            }
        }
        p = p->next;
    }

    /* û�ҵ�����0 */
    if (temp.next == &temp) {
        return 0;
    }

    /* ��ʶ�ÿ��ڴ��ѱ�ʹ�� */
    temp.next->is_used = 1;

#if SHOW_POOL_INFO
    pool_info.used_block_num++;
#endif

    return temp.next->buf;
}

/* ����: �ڴ��ͷ� 
 * ����: p : Ϊ����ĵ�ַ�ĵ�ַ, ʹ�ö���ָ��Ϊ���ͷŵ�ʱ����ָ���ڴ�ص�ַ, ��ֹ�ͷź������ٴ�ʹ���ڴ�, �����µ��ڴ��쳣
 * ����ֵ: 1�ͷųɹ�, 0�ͷ�ʧ��
 */
unsigned char pool_free(void** p)
{
    unsigned char free_succes = 0;
    pool_t* pt = (pool_t*)mem_pool;

    while (pt) {
        if (pt->buf == *p) {
            free_succes = 1;
            pt->is_used = 0;

#if SHOW_POOL_INFO
        pool_info.used_block_num--;
#endif

            *p = NULL;
            break;
        }
        pt = pt->next;
    }

    return free_succes;
}

#if SHOW_POOL_INFO
void get_pool_info(void)
{
    LOG_INFO("�ܹ��ж��ٿ飺%d\r\n", pool_info.total_block_num);
    LOG_INFO("ʹ���˶��ٿ飺%d\r\n", pool_info.used_block_num);
    LOG_INFO("�ڴ���õ�������Ĵ�С��%d\r\n", pool_info.used_pool_mm);
    LOG_INFO("pool_tռ���ֽ���: %d\r\n\r\n", sizeof(pool_t));
}
#endif
