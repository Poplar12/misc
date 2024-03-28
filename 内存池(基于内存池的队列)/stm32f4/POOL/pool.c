#include "pool.h"
#include "print.h"
/* 定一个大数组作为内存池 */
unsigned char mem_pool[948] = { 0 };

/* 内存池状态显示 */
#if SHOW_POOL_INFO
pool_info_t pool_info = { 0 };
#endif

/* 功能: 内存池初始化
 * 参数: arg_num: 不定参后面的参数个数; ...: 不定参的第一个为划分块的大小, 第二个为划分块的数量,依次类推
 * 返回值: 
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
        pool_size = va_arg(ap, int);    /* 不定参数的第一个值为划分的内存大小 */
        pool_num = va_arg(ap, int);     /* 不定参数的第二个值为划分的块数 */

        /* 对按设置大小和块数进行划分 */
        for (j = 0; j < pool_num; j++) {
            /* 划分的内存大小超过了数组的大小 */
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

/* 功能: 内存分配 
 * 参数: size: 需要申请的内存大小
 * 返回值: 申请的内存地址
 */
void* pool_malloc(unsigned int size)
{
    pool_t* p = (pool_t*)mem_pool;
    pool_t temp;
    temp.block_size = size;
    temp.next = &temp;

    /* 分配内存: 找到内存池中未使用的最小内存 */
    while (p) {
        if (p->is_used == 0 && p->block_size >= size) {
            /* 第一次找到一个未使用的位置 */
            if(temp.next == &temp)
            {
                temp.next = p;
            }
            /* 找到一个更小的且内满足的内存 */
            if(p->block_size <= temp.next->block_size)
            {
                temp.next = p;
            }
        }
        p = p->next;
    }

    /* 没找到返回0 */
    if (temp.next == &temp) {
        return 0;
    }

    /* 标识该块内存已被使用 */
    temp.next->is_used = 1;

#if SHOW_POOL_INFO
    pool_info.used_block_num++;
#endif

    return temp.next->buf;
}

/* 功能: 内存释放 
 * 参数: p : 为申请的地址的地址, 使用二级指针为了释放的时候不再指向内存池地址, 防止释放后不申请再次使用内存, 而导致的内存异常
 * 返回值: 1释放成功, 0释放失败
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
    LOG_INFO("总共有多少块：%d\r\n", pool_info.total_block_num);
    LOG_INFO("使用了多少块：%d\r\n", pool_info.used_block_num);
    LOG_INFO("内存池用掉了数组的大小：%d\r\n", pool_info.used_pool_mm);
    LOG_INFO("pool_t占的字节数: %d\r\n\r\n", sizeof(pool_t));
}
#endif
