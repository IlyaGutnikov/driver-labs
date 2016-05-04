    /* hello.c – Простой загружаемый модуль ядра */
    #include <linux/module.h>
    #include <linux/version.h>
    #include <linux/kernel.h>
    #include <linux/sched.h>
    #include <linux/proc_fs.h>

    void print_process_info(void) {
    
    struct task_struct *p;

    for_each_process(p) {
        
        printk("Task %s (pid = %d) \n", p->comm, task_pid_nr(p));
    }

    }

    static int __init hello_init(void) /* Инициализация */
    {
    printk(KERN_INFO "Hello: registered\n");
    print_process_info();
    return 0;
    }

    static void __exit hello_exit(void) /* Деинициализаия */
    {
    printk(KERN_INFO "Hello: unregistered\n");
    }

    module_init(hello_init);
    module_exit(hello_exit);

    MODULE_LICENSE("GPL");
    MODULE_AUTHOR("Ilya Gutnikov");
    MODULE_DESCRIPTION("Simple loadable kernel module");
