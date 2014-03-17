#include <stdio.h>

struct mgs {
	char *str;
	int len;
};

void init(struct mgs *ptr,char *str)
{
	int len;
	len = strlen(str);
	ptr->str = (char *) malloc(len);
	strcpy(ptr->str,str);
	ptr->len = len;
}

int main()
{
	struct mgs obj;

	printf("%s %d\n",obj.str,obj.len);
	
	init(&obj,"Veda Solutions");
	
	printf("%s %d\n",obj.str,obj.len);
}
