#include <defs.h>
#include <riscv.h>
#include <stdio.h>
#include <string.h>
#include <swap.h>
#include <swap_clock.h>
#include <list.h>
#include <pmm.h>

list_entry_t pra_list_head, *curr_ptr = NULL;

/**
 * init pra_list_head and let  mm->sm_priv point to the addr of pra_list_head.
 *              Now, From the memory control struct mm_struct, we can access clock PRA
 */
static int
_clock_init_mm(struct mm_struct *mm)
{
    // TODO
    mm->sm_priv = NULL; // set the head node to NULL
    return 0;
}

static int
_clock_map_swappable(struct mm_struct *mm, uintptr_t addr, struct Page *page, int swap_in)
{
    list_entry_t *head = (list_entry_t *)mm->sm_priv; // get the head node.
    list_entry_t *entry = &(page->pra_page_link);     // get the current entry.
    assert(entry != NULL);
    // record the page access situation
    pte_t *ptep = get_pte(mm->pgdir, page->pra_vaddr, 0);
    *ptep = *ptep & PTE_A; // set the acccess bit to accessed
    if (head == NULL)
    { // currently, there are no head, so that we should let the entry be the head
        list_init(entry);
        mm->sm_priv = entry;
    }
    else
    {
        //(1)link the most recent arrival page at the back of the pra_list_head qeueue.
        list_add_before(head, entry); // since the link is a reverse link, so we just add the entry before the head.
    }

    return 0;
}

static int
_clock_swap_out_victim(struct mm_struct *mm, struct Page **ptr_page, int in_tick)
{

    // TODO
    list_entry_t *head = (list_entry_t *)(mm->sm_priv);
    assert(head != NULL);
    list_entry_t *victim = 0;
    *ptr_page = 0;
    while (head != victim)
    { // iterate the link list.

        if (victim == 0)
        {
            victim = head;
        }

        struct Page *page = le2page(victim, pra_page_link);
        pte_t *ptep = get_pte(mm->pgdir, page->pra_vaddr, 0);
        bool accessed = *ptep & PTE_A; // get the access bit
        if (!accessed)
        {
            *ptr_page = page;
            mm->sm_priv = list_next(victim); // first set the position of head pointer and delete the victim page.
            list_del(victim);
            break;
        }

        *ptep = *ptep & ~PTE_A; // if the access bit is accessed, then it should be cleared
        victim = list_next(victim);
    }

    if (*ptr_page == 0)
    { // if cannot find the victim, then we should replace the head page
        *ptr_page = le2page(head, pra_page_link);
        mm->sm_priv = list_next(head); // set the new position of head page.
        list_del(head);
    }
    return 0;
}

static int
_clock_check_swap(void)
{

    cprintf("---------Clock check begin----------\n");
    cprintf("write Virt Page c in clock_check_swap\n");
    *(unsigned char *)0x3000 = 0x0c;
    assert(pgfault_num == 4);
    cprintf("write Virt Page a in clock_check_swap\n");
    *(unsigned char *)0x1000 = 0x0a;
    assert(pgfault_num == 4);
    cprintf("write Virt Page d in clock_check_swap\n");
    *(unsigned char *)0x4000 = 0x0d;
    assert(pgfault_num == 4);
    cprintf("write Virt Page b in clock_check_swap\n");
    *(unsigned char *)0x2000 = 0x0b;
    assert(pgfault_num == 4);
    cprintf("write Virt Page e in clock_check_swap\n");
    *(unsigned char *)0x5000 = 0x0e;
    assert(pgfault_num == 5);
    cprintf("write Virt Page b in clock_check_swap\n");
    *(unsigned char *)0x2000 = 0x0b;
    assert(pgfault_num == 5);
    cprintf("write Virt Page a in clock_check_swap\n");
    *(unsigned char *)0x1000 = 0x0a;
    cprintf("pgfault = %d\n", pgfault_num);
    assert(pgfault_num == 6);
    cprintf("write Virt Page b in clock_check_swap\n");
    *(unsigned char *)0x2000 = 0x0b;
    assert(pgfault_num == 6);
    cprintf("write Virt Page c in clock_check_swap\n");
    *(unsigned char *)0x3000 = 0x0c;
    assert(pgfault_num == 7);
    cprintf("write Virt Page d in clock_check_swap\n");
    *(unsigned char *)0x4000 = 0x0d;
    assert(pgfault_num == 8);
    cprintf("write Virt Page e in clock_check_swap\n");
    *(unsigned char *)0x5000 = 0x0e;
    assert(pgfault_num == 9);
    cprintf("write Virt Page a in clock_check_swap\n");
    assert(*(unsigned char *)0x1000 == 0x0a);
    *(unsigned char *)0x1000 = 0x0a;
    assert(pgfault_num == 9);
    cprintf("Clock check succeed!\n");

    return 0;
}

static int
_clock_init(void)
{
    return 0;
}

static int
_clock_set_unswappable(struct mm_struct *mm, uintptr_t addr)
{
    return 0;
}

static int
_clock_tick_event(struct mm_struct *mm)
{
    return 0;
}

struct swap_manager swap_manager_clock =
    {
        .name = "clock swap manager",
        .init = &_clock_init,
        .init_mm = &_clock_init_mm,
        .tick_event = &_clock_tick_event,
        .map_swappable = &_clock_map_swappable,
        .set_unswappable = &_clock_set_unswappable,
        .swap_out_victim = &_clock_swap_out_victim,
        .check_swap = &_clock_check_swap,
};
