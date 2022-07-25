#include <defs.h>
#include <riscv.h>
#include <stdio.h>
#include <string.h>
#include <swap.h>
#include <swap_lru.h>
#include <list.h>
#include <pmm.h>

list_entry_t pra_list_head, *curr_ptr2 = NULL;

static int
_lru_init_mm(struct mm_struct *mm)
{
    // TODO
    list_init(&pra_list_head);
    mm->sm_priv = &pra_list_head;
    // cprintf(" mm->sm_priv %x in fifo_init_mm\n",mm->sm_priv);
    return 0;
    // mm->sm_priv = NULL; // set the head node to NULL
    // return 0;
}

static int
_lru_map_swappable(struct mm_struct *mm, uintptr_t addr, struct Page *page, int swap_in)
{
    // // TODO
    list_entry_t *head = (list_entry_t *)mm->sm_priv;
    list_entry_t *entry = &(page->pra_page_link);

    assert(entry != NULL && head != NULL);
    // record the page access situlation

    //(1)link the most recent arrival page at the back of the pra_list_head qeueue.
    // (*(unsigned char *)page->pra_vaddr) ++;
    list_add(head, entry);
    return 0;
    // list_entry_t *head = (list_entry_t *)mm->sm_priv; // get the head node.
    // list_entry_t *entry = &(page->pra_page_link);     // get the current entry.
    // (*(unsigned char *)page->pra_vaddr) ++;
    // assert(entry != NULL);
    // // record the page access situation

    // if (head == NULL)
    // { // currently, there are no head, so that we should let the entry be the head
    //     list_init(entry);
    //     mm->sm_priv = entry;
    // }
    // else
    // {
    //     //(1)link the most recent arrival page at the back of the pra_list_head qeueue.
    //     list_add_before(head, entry); // since the link is a reverse link, so we just add the entry before the head.
    // }

    // return 0;
}

static int
_lru_swap_out_victim(struct mm_struct *mm, struct Page **ptr_page, int in_tick)
{
    // TODO
    list_entry_t *head = (list_entry_t *)mm->sm_priv;
    assert(head != NULL);
    assert(in_tick == 0);
    list_entry_t *curr_ptr;
    curr_ptr = list_next(head);
    list_entry_t *e = list_next(head);
    unsigned int min = -1;
    cprintf("1111111111111111111111111111\n");
    while (curr_ptr != head)
    {
        cprintf("222222\n");
        struct Page *p = le2page(curr_ptr, pra_page_link);
        cprintf("%-llx\n", p);
        unsigned int c = *(unsigned int *)p->pra_vaddr;
        if (c < min)
        {
            min = c;
            *ptr_page = p;
            e = curr_ptr;
        }
        curr_ptr = list_next(curr_ptr);
    }
    cprintf("%d\n", e);
    list_del(e);
    return 0;
    // TODO
    // list_entry_t *head = (list_entry_t *)(mm->sm_priv);
    // assert(head != NULL);
    // list_entry_t *current_ptr = 0;
    // list_entry_t *victim = 0;
    // *ptr_page = 0;
    // unsigned int min = 100;
    // while (head != current_ptr)
    // { // iterate the link list.

    //     if (current_ptr == 0)
    //     {
    //         current_ptr = head;
    //     }

    //     struct Page *page = le2page(current_ptr, pra_page_link);

    //     int number = *(unsigned char *)page -> pra_vaddr;
    //     cprintf("The number is %d\n", number);
    //     if (number < min)
    //     {
    //         min = number;
    //         *ptr_page = page;
    //         victim = current_ptr;
    //         mm->sm_priv = list_next(current_ptr); // first set the position of head pointer and delete the victim page.
    //         break;
    //     }
    //     current_ptr = list_next(current_ptr);
    // }
    // list_del(victim);

    
    // return 0;
}

static int
_lru_check_swap(void)
{

    int i = 0x10;

    cprintf("---------LRU check begin----------\n");
    cprintf("write Virt Page 3 in lru_check_swap\n");
    *(unsigned char *)0x3000 = i;
    i += 1;
    assert(pgfault_num == 4);
    cprintf("write Virt Page 1 in lru_check_swap\n");
    *(unsigned char *)0x1000 = i;
    i += 1;
    assert(pgfault_num == 4);
    cprintf("write Virt Page 4 in lru_check_swap\n");
    *(unsigned char *)0x4000 = i;
    i += 1;
    assert(pgfault_num == 4);
    cprintf("write Virt Page 2 in lru_check_swap\n");
    *(unsigned char *)0x2000 = i;
    i += 1;
    assert(pgfault_num == 4);
    cprintf("write Virt Page 5 in lru_check_swap\n");
    *(unsigned char *)0x5000 = i;
    i += 1;
    assert(pgfault_num == 5);
    cprintf("write Virt Page 3 in lru_check_swap\n");
    *(unsigned char *)0x3000 = i;
    i += 1;
    assert(pgfault_num == 6);
    cprintf("write Virt Page 1 in lru_check_swap\n");
    *(unsigned char *)0x1000 = i;
    i += 1;
    assert(pgfault_num == 7);
    cprintf("write Virt Page 4 in lru_check_swap\n");
    *(unsigned char *)0x4000 = i;
    i += 1;
    assert(pgfault_num == 8);
    cprintf("write Virt Page 4 in lru_check_swap\n");
    *(unsigned char *)0x4000 = i;
    i += 1;
    assert(pgfault_num == 8);
    cprintf("write Virt Page 5 in lru_check_swap\n");
    *(unsigned char *)0x5000 = i;
    i += 1;
    assert(pgfault_num == 8);
    cprintf("write Virt Page 2 in lru_check_swap\n");
    *(unsigned char *)0x2000 = i;
    i += 1;
    assert(pgfault_num == 9);
    cprintf("write Virt Page 3 in lru_check_swap\n");
    *(unsigned char *)0x3000 = i;
    i += 1;
    assert(pgfault_num == 10);
    cprintf("LRU check succeed!\n");

    return 0;
}

static int
_lru_init(void)
{
    return 0;
}

static int
_lru_set_unswappable(struct mm_struct *mm, uintptr_t addr)
{
    return 0;
}

static int
_lru_tick_event(struct mm_struct *mm)
{
    return 0;
}

struct swap_manager swap_manager_lru =
    {
        .name = "lru swap manager",
        .init = &_lru_init,
        .init_mm = &_lru_init_mm,
        .tick_event = &_lru_tick_event,
        .map_swappable = &_lru_map_swappable,
        .set_unswappable = &_lru_set_unswappable,
        .swap_out_victim = &_lru_swap_out_victim,
        .check_swap = &_lru_check_swap,
};
