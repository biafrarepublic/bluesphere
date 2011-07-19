/* arch/arm/mach-msm/lge/lge_ats_eta.c
 *
 * Copyright (C) 2010 LGE, Inc.
 *
 * This software is licensed under the terms of the GNU General Public
 * License version 2, as published by the Free Software Foundation, and
 * may be copied, distributed, and modified under those terms.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 */
#include <linux/module.h>
#include <linux/kernel.h>
#include <mach/lge_base64.h>

int base64_decode(char *, unsigned char *, int);
int base64_encode(char *, int, char *);

/*------ Base64 Encoding Table ------*/
const char MimeBase64[] = {
	'A', 'B', 'C', 'D', 'E', 'F', 'G', 'H',
	'I', 'J', 'K', 'L', 'M', 'N', 'O', 'P',
	'Q', 'R', 'S', 'T', 'U', 'V', 'W', 'X',
	'Y', 'Z', 'a', 'b', 'c', 'd', 'e', 'f',
	'g', 'h', 'i', 'j', 'k', 'l', 'm', 'n',
	'o', 'p', 'q', 'r', 's', 't', 'u', 'v',
	'w', 'x', 'y', 'z', '0', '1', '2', '3',
	 '4', '5', '6', '7', '8', '9', '+', '/'
};

/*------ Base64 Decoding Table ------*/
static int DecodeMimeBase64[256] = {
	-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1, /* 00-0F */
	-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1, /* 10-1F */
	-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,62,-1,-1,-1,63, /* 20-2F */ 
	52,53,54,55,56,57,58,59,60,61,-1,-1,-1,-1,-1,-1, /* 30-3F */ 
	-1, 0, 1, 2, 3, 4, 5, 6, 7, 8, 9,10,11,12,13,14, /* 40-4F */ 
	15,16,17,18,19,20,21,22,23,24,25,-1,-1,-1,-1,-1, /* 50-5F */ 
	-1,26,27,28,29,30,31,32,33,34,35,36,37,38,39,40, /* 60-6F */ 
	41,42,43,44,45,46,47,48,49,50,51,-1,-1,-1,-1,-1, /* 70-7F */ 
	-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1, /* 80-8F */ 
	-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1, /* 90-9F */ 
	-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1, /* A0-AF */ 
	-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1, /* B0-BF */ 
	-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1, /* C0-CF */ 
	-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1, /* D0-DF */ 
	-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1, /* E0-EF */ 
	-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1  /* F0-FF */ 
};

int base64_decode(char *text, unsigned char *dst, int numBytes)
{
	const char* cp; 
	int space_idx = 0, phase; 
	int d, prev_d = 0; 
	unsigned char c;

	printk(KERN_INFO "[ETA] text: 0x%X, dst: 0x%X, size: %d\n",  (unsigned int)text, (unsigned int)dst, numBytes);
		
	space_idx = 0; 
	phase = 0;
	
	for ( cp = text; *cp != '\0'; ++cp ) { 
		d = DecodeMimeBase64[(int) *cp]; 
	    if ( d != -1 ) { 
	    	switch ( phase ) { 
	        	case 0: 
		        	++phase; 
	        		break; 
	        	case 1: 
	          		c = ( ( prev_d << 2 ) | ( ( d & 0x30 ) >> 4 ) );
				printk(KERN_INFO "[ETA] space_idx: 0x%X, char: 0x%X\n",  space_idx, c);
	          		if ( space_idx < numBytes )
		            dst[space_idx++] = c; 
			        ++phase; 
	    		    break; 
		        case 2: 
		        	c = ( ( ( prev_d & 0xf ) << 4 ) | ( ( d & 0x3c ) >> 2 ) );
				printk(KERN_INFO "[ETA] space_idx: 0x%X, char: 0x%X\n",  space_idx, c);
			        if ( space_idx < numBytes ) 
	         		dst[space_idx++] = c; 
	          		++phase; 
	          		break; 
	        	case 3: 
	          		c = ( ( ( prev_d & 0x03 ) << 6 ) | d ); 
				printk(KERN_INFO "[ETA] space_idx: 0x%X, char: 0x%X\n",  space_idx, c);
	          		if ( space_idx < numBytes ) 
	            	dst[space_idx++] = c; 
	          		phase = 0; 
	          		break; 
			} 	
	      	prev_d = d; 
		}
	}
	printk(KERN_INFO "[ETA] Complete..\n");
	return space_idx;
}
EXPORT_SYMBOL(base64_decode);

int base64_encode(char *text, int numBytes, char *encodedText)
{
	unsigned char input[3] = {0,0,0}; 
	unsigned char output[4] = {0,0,0,0}; 
	int  index, i, j; 
	char *p, *plen; 

	plen = text + numBytes - 1; 

	j = 0;

	for (i = 0, p = text;p <= plen; i++, p++) { 
	    index = i % 3; 
	    input[index] = *p;

		if (index == 2 || p == plen) { 
	    	output[0] = ((input[0] & 0xFC) >> 2); 
			output[1] = ((input[0] & 0x3) << 4) | ((input[1] & 0xF0) >> 4); 
			output[2] = ((input[1] & 0xF) << 2) | ((input[2] & 0xC0) >> 6); 
			output[3] = (input[2] & 0x3F);

			encodedText[j++] = MimeBase64[output[0]]; 
			encodedText[j++] = MimeBase64[output[1]]; 
			encodedText[j++] = index == 0? '=' : MimeBase64[output[2]]; 
			encodedText[j++] = index < 2? '=' : MimeBase64[output[3]];

			input[0] = input[1] = input[2] = 0; 
		} 
	}
	encodedText[j] = '\0';

	return strlen(encodedText); 	
}
EXPORT_SYMBOL(base64_encode);
