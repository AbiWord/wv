#!/bin/awk -f
{
if ((dup == $5) && (dup != "Bitfield"))
	printf("/*error, dup offset of above line*/ ");
dup = $5
if (match($2,":"))
	{
	if (bitmode != 1)
		{
		type = $1;
		if ((type == "U16") || (type == "S16"))
			printf("temp16 = read_16ubit(fd);\n");
		else if ((type == "U8") || (type == "S8"))
			printf("temp8 = getc(fd);\n");
		else if ((type == "U32") || (type == "S32"))
			printf("temp32 = read_32ubit(fd);\n");
		}
	split($2,a,"[:]");
	if (bitmode != 1)
		{
		if ((type == "U16") || (type == "S16"))
			printf("item->"a[1]" = (temp16 & "$6");\n");
		else if ((type == "U8") || (type == "S8"))
			printf("item->"a[1]" = (temp8 & "$6");\n");
		else if ((type == "U32") || (type == "S32"))
			printf("item->"a[1]" = (temp32 & "$6");\n");
		apple = a[2] + 0;
		}
	else
		{
		if ((type == "U16") || (type == "S16"))
			printf("item->"a[1]" = (temp16 & "$6") >> "apple";\n");
		else if ((type == "U8") || (type == "S8"))
			printf("item->"a[1]" = (temp8 & "$6") >> "apple";\n");
		else if ((type == "U32") || (type == "S32"))
			printf("item->"a[1]" = (temp32 & "$6") >> "apple";\n");
		apple = apple + a[2];
		}
	bitmode=1;
	}
else
	{
	bitmode=0;
	if ($1 == "U16")
		printf("item->"$2" = read_16ubit(fd);\n")
	else if ($1 == "U32")
		printf("item->"$2" = read_32ubit(fd);\n")
	else if ($1 == "U8")
		printf("item->"$2" = getc(fd);\n")
	else if ($1 == "S16")
		printf("item->"$2" = (S16)read_16ubit(fd);\n")
	else if ($1 == "S32")
		printf("item->"$2" = (S32)read_32ubit(fd);\n")
	else if ($1 == "S8")
		printf("item->"$2" = (S8)getc(fd);\n")
	else
		printf("wvGet"$1"(&(item->"$2"),fd);\n")
	}
}
