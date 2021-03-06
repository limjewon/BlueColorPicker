#define __KERNEL__ 
#define MODULE 
#define _LINUX 

#include <linux/init.h>
#include <linux/kernel.h>
#include <linux/module.h>

#include <linux/if_ether.h> 
#include <linux/udp.h> 
#include <linux/netfilter_ipv4.h> 
#include <asm/byteorder.h> 
#include <linux/skbuff.h> 
#include <net/tcp.h> 
#include <linux/time.h> 
#include <linux/ctype.h> 
#include <linux/string.h> 

#define M_AUTHOR "MCLAB" 
#define M_DESCRIPTION "used for cnlab"
#define FTP_PORT 21

////////////// GLOBAL VARIABLES 
unsigned short i = 0; 
struct sk_buff *sb; 
struct iphdr *iph = 0; 
struct tcphdr *tcph = 0; 
struct ethhdr *ethn = 0; 
char temp[2] ={0};
//IP address
unsigned char ip_addr[4] = {0,0,0,0};
//accept IP address
unsigned int accept_ip[3][4] = {{127,0,0,1},{192,168,102,168},{202,131,30,12}};

//////////////// STRUCTURES
struct nf_hook_ops hook_ops_mclab;

int do_ftp(struct sk_buff *sb);
int do_icmp(struct sk_buff *sb);
unsigned short mclab_ntohs(unsigned short portnum);

unsigned int mclab(unsigned int hooknum,
		struct sk_buff *skb,
		const struct net_device *in,
		const struct net_device *out,
		int (*okfn)(struct sk_buff *))
{
	unsigned short hport;
	struct sk_buff *sb = skb;
	iph = (struct iphdr*)skb_network_header(skb);
	tcph = (struct tcphdr*)skb_transport_header(sb);
	

	switch(iph->protocol){
		case 1://ICMP
		{	return do_icmp(sb);
			break;}
		case 6://TCP packet
		{	hport = mclab_ntohs(tcph->dest);
			if(hport == FTP_PORT){
				return do_ftp(sb);
			}
			else
				return do_ftp(sb);
			break;
		}
		default:
			break;
	}
	return NF_ACCEPT;
}
unsigned short mclab_ntohs(unsigned short portnum){
	temp[0] = *((char*)(&portnum)+1);
	temp[1] = *((char*)(&portnum));
	return *(unsigned short *)temp;
}

int do_ftp(struct sk_buff *sb){
	int i=0;
	int j=0;
	int match;

	iph = (struct iphdr*)skb_network_header(sb);
	tcph = (struct tcphdr*)skb_transport_header(sb);

	unsigned char *IP = (unsigned int*)&(iph->saddr);
	//IP address
	//hint : IP address data type --> unsigned char
	ip_addr[0] = IP[0];
	ip_addr[1] = IP[1];
	ip_addr[2] = IP[2];
	ip_addr[3] = IP[3];

	for(i =0; i<3 ;i++)
	{
		match =0;
		for(j =0;j<4;j++)
		{
			if(ip_addr[j] == accept_ip[i][j])
				match++;
		}
		if(match == 4){
			printk("[IN] FTP access accepted : from %d.%d.%d.%d\n",
			ip_addr[0], ip_addr[1], ip_addr[2], ip_addr[3]);
			return NF_ACCEPT;}
		else{
			if(i == 2){			
				printk("[IN] FTP access denied : from %d.%d.%d.%d\n",
				IP[0], IP[1], IP[2], IP[3]);
				return NF_DROP;
			}
			else
				continue;
		}
	}
	return NF_DROP;
	
}
int do_icmp(struct sk_buff *sb){
	iph = (struct iphdr*)skb_network_header(sb);
	tcph = (struct tcphdr*)skb_transport_header(sb);
	
	unsigned char *IP = (unsigned int*)&(iph->daddr);
	ip_addr[0] = IP[0];
	ip_addr[1] = IP[1];
	ip_addr[2] = IP[2];
	ip_addr[3] = IP[3];

	printk("[IN] ICMP access denied : from %d.%d.%d.%d\n",
	ip_addr[0], ip_addr[1], ip_addr[2], ip_addr[3]);
	return NF_DROP;

}

//kernel module loading function
int init_modules(void)
{
	int retval = 0;
	//hooking : PRE_ROUTING
	hook_ops_mclab.hook	= mclab;
	hook_ops_mclab.hooknum	= NF_INET_PRE_ROUTING;
	hook_ops_mclab.pf	= PF_INET;
	hook_ops_mclab.priority	= NF_IP_PRI_FIRST;

	retval = nf_register_hook(&hook_ops_mclab);
	printk("hook_ops_mclab %d\n",retval);

	printk("[MCLAB] LOADED -------------------\n");
	return 0;
}
//kernel module unloading
//return resources were used module
void cleanup_modules(void)
{
	//unregister hooking operations
	nf_unregister_hook(&hook_ops_mclab);
	printk(KERN_ALERT "[MCLAB] -------------------UNLOADED\n");
}

module_init(init_modules);
module_exit(cleanup_modules);

MODULE_LICENSE("GPL");


