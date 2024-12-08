#include <string.h>
#include <errno.h>
#include <asm/segment.h>

#define STR_MAX_LEN 24
#define DEBUG_MSG 0
char msg[STR_MAX_LEN]; //最多23个字符，外带一个回车符号

int 
sys_iam(const char* name)
{
	int len = 0;
	int tmp[30];
	int i = 0;
	for ( len = 0; len < 30; len++) 
	{
		tmp[len] = get_fs_byte(name+len);		
		if (tmp[len] == '\0' || tmp[len] == '\n') 
		{
			break;		
		}
	}
	if (len >= 24) 
	{
		return -EINVAL;		
	}
	for(i=0;i <= len;i++)
	{
		msg[i] = tmp[i];
	}
	return	len;
}

int sys_iam2(const char* name) {
    #if DEBUG_MSG
    printk("Now we are in Kernel's %s\r\n", __FUNCTION__);
    #endif
    int len = 0;
	char tmp[30];
	int i = 0;
	for ( len = 0; len < 30; len++) 
	{
		tmp[len] = get_fs_byte(name+len);		
		if (tmp[len] == '\0' || tmp[len] == '\n') 
		{
			break;		
		}
	}
    tmp[len] = '\0';
    #if DEBUG_MSG
    printk("Your name %s's len=%d\r\n", tmp, len);
    #endif
	if (len >= STR_MAX_LEN) 
	{   
        #if DEBUG_MSG
        printk("Your name %s is too long\r\n", tmp);
        #endif
        return -EINVAL;
	}
	for(i=0;i <= len;i++)
	{
		msg[i] = tmp[i];
	}
    #if DEBUG_MSG
    printk("kernek msg=\"%s\"'s len=%d\r\n", msg, len);
    printk("Copy done!\r\n");
    #endif
	return	len;
}

int
sys_whoami(char* name,unsigned int size)
{
	int len = 0;
	int i = 0;
	for(; msg[len] != '\0'; len++ );
	if (len > size) {
		return -EINVAL;	
	}
	for (i = 0; i < size; ++i) {
		put_fs_byte(msg[i],name+i);	
		if (msg[i] == '\0') {
			break;	
		}
	}
	return i;
}

int sys_whoami2(char* name,unsigned int size)
{
    #if DEBUG_MSG
    printk("Now we are in Kernel's %s\r\n", __FUNCTION__);
    #endif
	int len = strlen(msg);
	int i = 0;
	if (len > size) {
        #if DEBUG_MSG
		printk("Your name %s is too long\r\n", msg);
        #endif
        return -EINVAL;
	}
    #if DEBUG_MSG
    printk("Copy begin! %s's len=%d\r\n", msg, len);
    printk("Copying ...");
    #endif
	for (i = 0; i < size; ++i) {
		put_fs_byte(msg[i],name+i);	
        #if DEBUG_MSG
        printk("%c",msg[i]);
        #endif
		if (msg[i] == '\0') {
			break;	
		}
	}
    #if DEBUG_MSG
    printk("\r\n");
    printk("cur naeme=%s len=%d\r\n", name, len);
    printk("Copy done!\r\n");
    #endif
	return i;
}
