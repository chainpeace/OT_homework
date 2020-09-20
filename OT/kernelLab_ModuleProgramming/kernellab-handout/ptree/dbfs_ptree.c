#include <linux/debugfs.h>
#include <linux/kernel.h>
#include <linux/module.h>
#include <linux/uaccess.h>
#include <linux/slab.h>
#include <uapi/asm-generic/errno.h>



/*
 *
 * OT7_1 ptree
 * Inhwi Hwang 2020.08.12
 * Kernel version: 4.15.0, 5.3.0
 * OS: ubuntu 18.04
 * machine: VMWare
 *
 */


MODULE_LICENSE("GPL");

static struct dentry *dir, *inputdir, *ptreedir;
static struct task_struct *curr;

struct debugfs_blob_wrapper *blob;
static char * buf;

struct ptree_struct
{
	
	struct task_struct *task;
	struct list_head head;

};

static struct ptree_struct *new_pt(struct task_struct *task)
{
	struct ptree_struct *pt;

	if(!task)
		return NULL;

	pt = kmalloc(sizeof(struct ptree_struct), GFP_KERNEL);

	if(!pt){
		printk(KERN_ERR "memory allocation error.\n");
		return NULL;
	}

	pt->task = task;
	printk(KERN_INFO "new ptree struct %u, %s\n", pt->task->pid, pt->task->comm);
	
	return pt;
}

static ssize_t write_pid_to_input(struct file *fp, 
                                const char __user *user_buffer, 
                                size_t length, 
                                loff_t *position)
{
	int bufsize;
        char * str;
	pid_t input_pid;
	struct ptree_struct *pt, *next;
	
	buf = kmalloc(sizeof(char)*100, GFP_KERNEL);
	str = kmalloc(length*sizeof(char), GFP_KERNEL);
       
	copy_from_user(str, user_buffer, length);
	sscanf(str, "%u", &input_pid);
	kfree(str);
	
	curr = pid_task(find_get_pid(input_pid), PIDTYPE_PID);

	if(curr == NULL)
		return -EINVAL;
	
	pt = new_pt(curr);
	if(!pt) return -1;
	
	LIST_HEAD(pt_list);
	list_add(&pt->head, &pt_list);

	while(curr->pid!=1){
		curr = curr->parent;
		pt = new_pt(curr);
		list_add(&pt->head, &pt_list);
	}

	list_for_each_entry_safe(pt,next, &pt_list, head){
		printk(KERN_INFO "%s (%d)\n", pt->task->comm, pt->task->pid);
		bufsize = snprintf(buf, 100, "%s%s (%d)\n", buf, pt->task->comm, pt->task->pid);
		list_del(&pt->head);
		kfree(pt);
	}
	if(bufsize == 100)
		printk(KERN_INFO "buffer size is not enough\n");
	blob->data = buf;
	blob->size = strlen(buf)+1;

        return length;
}

static const struct file_operations dbfs_fops = {
        .write = write_pid_to_input,
};

static int __init dbfs_module_init(void)
{

#if 1
    dir = debugfs_create_dir("ptree", NULL);
        
        if (!dir) {
                printk("Cannot create ptree dir\n");
                return -1;
        }

	blob = kmalloc(sizeof(struct debugfs_blob_wrapper), GFP_KERNEL);
	blob->data ="";
	blob->size = strlen(blob->data) + 1;

    inputdir = debugfs_create_file("input", S_IRUSR|S_IWUSR, dir, NULL, &dbfs_fops);
	ptreedir = debugfs_create_blob("ptree", 0644, dir, blob);
#endif
	printk("dbfs_ptree module initialize done\n");

        return 0;
}

static void __exit dbfs_module_exit(void)
{
        // Implement exit module code
	kfree(buf);
	kfree(blob);
	debugfs_remove(ptreedir);	
	debugfs_remove(inputdir);
	debugfs_remove(dir);
	printk("dbfs_ptree module exit\n");
}

module_init(dbfs_module_init);
module_exit(dbfs_module_exit);
