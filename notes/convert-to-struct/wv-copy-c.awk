#!/bin/awk -f
{
if ((dup == $5) && (dup != "Bitfield"))
	printf("/*error, dup offset of above line*/ ");
dup = $5
if (match($2,":"))
	{
	split($2,a,"[:]");
	if (bitmode != 1)
		{
		if ((type == "U16") || (type == "S16"))
			printf("dest->"a[1]" = src->"a[1]";\n");
		else if ((type == "U8") || (type == "S8"))
			printf("dest->"a[1]" = src->"a[1]";\n");
		else if ((type == "U32") || (type == "S32"))
			printf("dest->"a[1]" = src->"a[1]";\n");
		apple = a[2] + 0;
		}
	else
		{
		if ((type == "U16") || (type == "S16"))
			printf("dest->"a[1]" = src->"a[1]";\n");
		else if ((type == "U8") || (type == "S8"))
			printf("dest->"a[1]" = src->"a[1]";\n");
		else if ((type == "U32") || (type == "S32"))
			printf("dest->"a[1]" = src->"a[1]";\n");
		apple = apple + a[2];
		}
	bitmode=1;
	}
else
	{
	bitmode=0;
	if ($1 == "U16")
		printf("dest->"$2" = src->"$2";\n")
	else if ($1 == "U32")
		printf("dest->"$2" = src->"$2";\n")
	else if ($1 == "U8")
		printf("dest->"$2" = src->"$2";\n")
	else if ($1 == "S16")
		printf("dest->"$2" = src->"$2";\n")
	else if ($1 == "S32")
		printf("dest->"$2" = src->"$2";\n")
	else if ($1 == "S8")
		printf("dest->"$2" = src->"$2";\n")
	else
		printf("wvCopy"$1"(&dest->"$2",&src->"$2");\n")
	}
}
