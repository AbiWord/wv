#!/bin/awk -f
{
if (match($1,"[[:digit:]]")) 
	{
	if (match($2,"0x")) 
		{

		if (dup == $2)
			printf("/*error, dup offset of above line*/ ");
		dup = $2;
		
		x = split($4,a,"[\[]");
		if (x > 1) 
			printf(a[1]" "$3"["a[2]); 
		else
			printf($4" "$3); 
		}
	}
if (match($5,":")) printf($5); 
if ((match($1,"[[:digit:]]")) && (match($2,"0x"))) 
	printf(" ;				/* ") 

if (match($5,":")) 
	printf("Bitfield "$6 " "); 

if ((match($1,"[[:digit:]]")) && (match($2,"0x"))) 
	printf($2" */\n") 


if (match($3,":")) 
	printf($2" "$1); 
if (match($3,":")) printf($3); 
if (match($3,":")) 
	printf(" ;			/* Bitfield "$4" */\n") 
}
