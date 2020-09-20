#include <linux/debugfs.h>
#include <linux/kernel.h>
#include <linux/module.h>
#include <linux/uaccess.h>
#include <asm/pgtable.h>

MODULE_LICENSE("GPL");

#define PADDR_MASK 0x0000ffffffffffff

/*
 *
 * OT7_2 paddr
 * Inhwi Hwang 2020.08.21
 * Kernel version: 4.15.0, 5.3.0
 * OS: ubuntu 18.04
 * machine: VMWare, VirtualBox
 *
 *
 *************************************
 * app.c code has a bug.
 * mmap occurs an error: "operation not permitted"
 * when offset is bigger than 0xff000
 *
 */


static struct dentry *dir, *output;
static struct task_struct *task;

struct packet {
	pid_t pid;
	unsigned long vaddr;
	unsigned long paddr;
};

static ssize_t read_output(struct file *fp,
                        char __user *user_buffer,
                        size_t length,
                        loff_t *position)
{
	struct packet pckt;
	struct task_struct *curr;
	struct mm_struct *mm;
	pgd_t *pgd;
	p4d_t *p4d;
	pud_t *pud;
	pmd_t *pmd;
	pte_t *pte;
	long paddr;

	copy_from_user(&pckt, user_buffer, length);
	printk("pid : %d\n", pckt.pid);	
	// Implement read file operation

	curr = pid_task(find_get_pid(pckt.pid), PIDTYPE_PID);
	mm = curr->mm;	
	printk("virtual address : %lx\n", pckt.vaddr);	
	
	pgd = pgd_offset(mm, pckt.vaddr);	
	if(!pgd_present(*pgd))
		return -1;
	printk("pgd : %lx\n", pgd_val(*pgd));	
	
	p4d = p4d_offset(pgd, pckt.vaddr);
	if(!p4d_present(*p4d))
		return -1;
	printk("p4d : %lx\n", p4d_val(*p4d));
	
	pud = pud_offset(p4d, pckt.vaddr);
	if(!pud_present(*pud))
		return -1;
	printk("pud : %lx\n", pud_val(*pud));

	pmd = pmd_offset(pud, pckt.vaddr);
	if(!pmd_present(*pmd)){
		printk("pmd not present\n");	
		return -1;
	}
	printk("pmd : %lx\n", pmd_val(*pmd));	

	pte = pte_offset_kernel(pmd, pckt.vaddr);
	if(!pte_present(*pte))
		return -1;
	printk("pte : %lx\n", pte_val(*pte));

	paddr = pte_val(*pte) & PAGE_MASK;
	paddr = paddr & PADDR_MASK;

	printk("paddr : %lx\n", paddr);

	pckt.paddr = paddr;
	copy_to_user(user_buffer, &pckt, length);
	return 0;
}

static const struct file_operations dbfs_fops = {
        // Mapping file operations with your functions
	.read = read_output
	
};

static int __init dbfs_module_init(void)
{
        // Implement init module

#if 1
        dir = debugfs_create_dir("paddr", NULL);

        if (!dir) {
                printk("Cannot create paddr dir\n");
                return -1;
        }

        // Fill in the arguments below
        output = debugfs_create_file("output", 0644, dir, NULL, &dbfs_fops);
#endif

	printk("dbfs_paddr module initialize done\n");

        return 0;
}

static void __exit dbfs_module_exit(void)
{
        // Implement exit module
	debugfs_remove_recursive(dir);
	printk("dbfs_paddr module exit\n");
}

module_init(dbfs_module_init);
module_exit(dbfs_module_exit);
