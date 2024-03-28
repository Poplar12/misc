#ifndef LIST_H
#define LIST_H
 
struct list_head {
    struct list_head *next, *prev;
};
 
//双链表的头初始化,next, prev指向自己
#define LIST_HEAD_INIT(name) { &(name), &(name) }
 
//通过函数初始化头
static inline void INIT_LIST_HEAD(struct list_head *list)
{
    list->next = list;
    list->prev = list;
}
 
//添加一个新的结点
static inline void __list_add(struct list_head *new, struct list_head *prev, struct list_head *next)
{
    next->prev = new;
    new->next = next;
    new->prev = prev;
    prev->next = new;
}
 
//头插法
static inline void list_add(struct list_head *new, struct list_head *head)
{
    __list_add(new, head, head->next);
}
 
//尾插法
static inline void list_add_tail(struct list_head *new, struct list_head *head)
{
    __list_add(new, head->prev, head);
}
 
//删除某个结点
static inline void __list_del(struct list_head *prev, struct list_head *next)//将要删除的结点从链表中释放出来
{
    next->prev = prev;
    prev->next = next;
}
static inline void list_del(struct list_head *entry) //这个函数才是最后的删除函数
{
    __list_del(entry->prev, entry->next);
    entry->next = (void *)0;
    entry->prev = (void *)0;
}
 
//判断结点是否为空
static inline int list_empty(const struct list_head *head)
{
    return head->next == head;
}
 
//已知结构体中的某个成员的地址ptr,得到结构体的地址
#define list_entry(ptr, type, member) \
    ((type *)((char *)(ptr)-(unsigned long)(&((type *)0)->member)))
 
//遍历链表, pos为链表结点, head为链表头, member为链表中的成员, type为结点类型
#define list_for_each_entry(pos, head, member, type)        \
    for (pos = list_entry((head)->next, type, member);      \
            &pos->member != (head);                         \
            pos = list_entry(pos->member.next, type, member))
#endif

