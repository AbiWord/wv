/* Contributed by Sergey V. Udaltsov <svu@pop.convey.ru> */

#include <stdlib.h>
#include <stdio.h>
#include "wv.h"


U16 wvConvertUnicodeToKOI8_R(U16 char16)
{
	/*wvError(("character is %d\n",char16));*/
	switch (char16)
		{
		case 9472: return 128;
		case 9474: return 129;
		case 9484: return 130;
		case 9488: return 131;
		case 9492: return 132;
		case 9496: return 133;
		case 9500: return 134;
		case 9508: return 135;
		case 9516: return 136;
		case 9524: return 137;
		case 9532: return 138;
		case 9600: return 139;
		case 9604: return 140;
		case 9608: return 141;
		case 9612: return 142;
		case 9616: return 143;
		case 9617: return 144;
		case 9618: return 145;
		case 9619: return 146;
		case 8992: return 147;
		case 9632: return 148;
		case 8226: return 149;
		case 8730: return 150;
		case 8776: return 151;
		case 8804: return 152;
		case 8805: return 153;
		case 160: return 154;
		case 8993: return 155;
		case 176: return 156;
		case 178: return 157;
		case 183: return 158;
		case 247: return 159;
		case 9552: return 160;
		case 9553: return 161;
		case 9554: return 162;
		case 1105: return 163;
		case 9555: return 164;
		case 9556: return 165;
		case 9557: return 166;
		case 9558: return 167;
		case 9559: return 168;
		case 9560: return 169;
		case 9561: return 170;
		case 9562: return 171;
		case 9563: return 172;
		case 9564: return 173;
		case 9565: return 174;
		case 9566: return 175;
		case 9567: return 176;
		case 9568: return 177;
		case 9569: return 178;
		case 1025: return 179;
		case 9570: return 180;
		case 9571: return 181;
		case 9572: return 182;
		case 9573: return 183;
		case 9574: return 184;
		case 9575: return 185;
		case 9576: return 186;
		case 9577: return 187;
		case 9578: return 188;
		case 9579: return 189;
		case 9580: return 190;
		case 169: return 191;
		case 1102: return 192;
		case 1072: return 193;
		case 1073: return 194;
		case 1094: return 195;
		case 1076: return 196;
		case 1077: return 197;
		case 1092: return 198;
		case 1075: return 199;
		case 1093: return 200;
		case 1080: return 201;
		case 1081: return 202;
		case 1082: return 203;
		case 1083: return 204;
		case 1084: return 205;
		case 1085: return 206;
		case 1086: return 207;
		case 1087: return 208;
		case 1103: return 209;
		case 1088: return 210;
		case 1089: return 211;
		case 1090: return 212;
		case 1091: return 213;
		case 1078: return 214;
		case 1074: return 215;
		case 1100: return 216;
		case 1099: return 217;
		case 1079: return 218;
		case 1096: return 219;
		case 1101: return 220;
		case 1097: return 221;
		case 1095: return 222;
		case 1098: return 223;
		case 1070: return 224;
		case 1040: return 225;
		case 1041: return 226;
		case 1062: return 227;
		case 1044: return 228;
		case 1045: return 229;
		case 1060: return 230;
		case 1043: return 231;
		case 1061: return 232;
		case 1048: return 233;
		case 1049: return 234;
		case 1050: return 235;
		case 1051: return 236;
		case 1052: return 237;
		case 1053: return 238;
		case 1054: return 239;
		case 1055: return 240;
		case 1071: return 241;
		case 1056: return 242;
		case 1057: return 243;
		case 1058: return 244;
		case 1059: return 245;
		case 1046: return 246;
		case 1042: return 247;
		case 1068: return 248;
		case 1067: return 249;
		case 1047: return 250;
		case 1064: return 251;
		case 1069: return 252;
		case 1065: return 253;
		case 1063: return 254;
		case 1066: return 255;
/* long and short '-' - just transfer to '-'. */
		case 8212: return 45;
		case 8211: return 45;
/* another '>>' */
		case 8221: return 187;
/* another '<<' */
		case 8220: return 171;
/* russian '#' - like 'N' */
		case 8470: return 78;
/* another '''  */
		case 8217: return 39;
/* another '`' */
		case 8216: return 96;
		default:
			if (char16 <= 0xFF)
				return(char16);
			else
				return(0xffff);
			break;
		}
	}
