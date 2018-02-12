/* 粗延时函数，微秒 */
void owl_udelay(unsigned short time)
{    
	unsigned short i=0;  
	while(time--)
	{
		i=10;  //自己定义
		while(i--) ;    
	}
}

/* 毫秒级的延时 */
void owl_mdelay(unsigned short time)
{    
	unsigned short i=0;  
	while(time--)
	{
		i=12000;  //自己定义
		while(i--);    
	}
}


