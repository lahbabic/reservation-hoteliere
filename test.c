#include <stdio.h>
#include <stdlib.h>
#include <string.h>

char 		*extract_str(char *message, int i)
{
	char tmp[100]; char *req = NULL;
	int len = 0; int len_r = 0;
	while(i>=1){
		// récupérer la première sous-chaine qui commence à l'adresse message+len_r et la mettre dans tmp
		sscanf(message+len_r, "%s ",tmp);	 
		for (len = 0; tmp[len] != '\0' || len > 99; ++len); // compter le nombre de caractères dans tmp

		if(len == 0)
			return NULL;
		else if(req != NULL){
			free(req);
			req = (char*)malloc((len+2) * sizeof(char) );
		}
		else if(req == NULL)
			req = (char*)malloc((len+2) * sizeof(char) );

		strncpy(req,tmp,len+2);
		req[len+1] = '\0';
		len_r = strlen(req) + len_r+1;
		i--;
	}
	return req;
}


int main(int argc, char const *argv[])
{
	char *test = "Ma bite sur ton nez";
	char *res;
	res = extract_str(test,5);
	printf("%s\n", res);
	free(res);
	return 0;
}