#ifndef __TEXTURE_H
#define __TEXTURE_H

#include <malloc.h>

typedef struct Texture
{
	int width, height;
	
	UINT32 * pRGBABuffer;

	WORD * ARGBBuffer;

	BYTE * pAlpha;
}Texture;

//void ablend_565(BYTE *lpAlpha,DWORD iAlpPitch,
//				BYTE *lpSrc,DWORD iSrcX, DWORD iSrcY,
//				DWORD iSrcPitch, BYTE *lpDst,
//				DWORD iDstX, DWORD iDstY,
//				DWORD iDstW, DWORD iDstH,
//				DWORD iDstPitch);

Texture * newTexture( int width, int height, UINT32 * pRGBABuffer )
{
	Texture * texture;
	WORD * ARGBBuffer;
	int lSize = 0, i = 0, j = 0;
	//BYTE * p;
	//BYTE * alpha;
	//WORD * color;
	DWORD dwPixel;
	DWORD a,r,g,b;

	if( ( texture = ( Texture * )malloc( sizeof( Texture ) ) ) == NULL )
	{
		exit( TRUE );
	}

	texture->width = width;
	texture->height = height;
	texture->pRGBABuffer = pRGBABuffer;

	lSize = width * height;

	//if use normal mode
	if( ( ARGBBuffer = ( WORD * )malloc( sizeof( WORD ) * lSize * 4 ) ) == NULL ) exit( TRUE );

	//if( ( alpha = ( BYTE * )malloc( sizeof( BYTE ) * lSize ) ) == NULL ) exit( TRUE );

	//if( ( p = ( BYTE * )malloc( sizeof( BYTE ) * lSize * 2 + 8 ) ) == NULL ) exit( TRUE );

	//else
	/*p += (DWORD)p%8;

	color = (WORD*)p;*/

	for ( ; i < lSize; i ++, j += 4 )
	{
		dwPixel = pRGBABuffer[i];

		/*r = ((dwPixel>>24)&0x000000ff);
		g = ((dwPixel>>16)&0x000000ff);
		b = ((dwPixel>> 8)&0x000000ff);
		a = ((dwPixel>> 0)&0x000000ff);*/

		a = ((dwPixel>>24) & 0xff);
		r = ((dwPixel>>16) & 0xff);
		g = ((dwPixel>>8) & 0xff);
		b = ((dwPixel) & 0xff);

		//if use normal mode
		ARGBBuffer[j] = a;
		ARGBBuffer[j+1] = r;
		ARGBBuffer[j+2] = g;
		ARGBBuffer[j+3] = b;

		//else
		//color[i] = RGBTo16( r, g, b );
		//alpha[i] = a;
	}

	//if use normal mode
	texture->ARGBBuffer = ARGBBuffer;
	//else
	//texture->ARGBBuffer = color;
	//texture->pAlpha = alpha;

	return texture;
}

void texture_dispose( Texture * texture )
{
	free( texture->pRGBABuffer );
	texture->pRGBABuffer = NULL;
}

////unsigned char *lpAlpha, // 256 级alpha通道
////unsigned int iAlpPitch, // alpha通道的pitch
////unsigned char *lpSrc, // 原色彩缓冲
////unsigned int iSrcX, //
////unsigned int iSrcY, // 原色彩位置
////unsigned int iSrcPitch, // 原色彩pitch
////unsigned char *lpDst, // 目标缓冲
////unsigned int iDstX,
////unsigned int iDstY, // 目标位置
////unsigned int iDstW,
////unsigned int iDstH, // 目标缓冲的尺寸
////unsigned int iDstPitch // 目标缓冲的pitch
//
//void ablend_565(BYTE *lpAlpha,DWORD iAlpPitch,
//				BYTE *lpSrc,DWORD iSrcX, DWORD iSrcY,
//				DWORD iSrcPitch, BYTE *lpDst,
//				DWORD iDstX, DWORD iDstY,
//				DWORD iDstW, DWORD iDstH,
//				DWORD iDstPitch)
//{
//	//Mask for isolating the red,green, and blue components
//
//	static __int64 MASKB=0x001F001F001F001F;
//
//	static __int64 MASKG=0x07E007E007E007E0;
//
//	static __int64 MASKSHIFTG=0x03F003F003F003F0;
//
//	static __int64 MASKR=0xF800F800F800F800;
//
//	//constants used by the integer alpha blending equation
//
//	static __int64 SIXTEEN=0x0010001000100010;
//
//	static __int64 FIVETWELVE=0x0200020002000200;
//
//	static __int64 SIXONES=0x003F003F003F003F;
//
//	unsigned char *lpLinearDstBp=(iDstX<<1)+(iDstY*iDstPitch)+lpDst; //base pointer for linear destination
//
//	unsigned char *lpLinearSrcBp=(iSrcX<<1)+(iSrcY*iSrcPitch)+lpSrc; //base pointer for linear source
//
//	unsigned char *lpLinearAlpBp=iSrcX+(iSrcY*iAlpPitch)+lpAlpha; //base pointer for linear alpha
//
//	_asm{
//
//		mov esi,lpLinearSrcBp; //src
//
//		mov edi,lpLinearDstBp; //dst
//
//		mov eax,lpLinearAlpBp; //alpha
//
//		mov ecx,iDstH; //ecx=number of lines to copy
//
//		mov ebx,iDstW; //ebx=span width to copy
//
//		test esi,6; //check if source address is qword aligned
//
//		//since addr coming in is always word aligned(16bit)
//
//		jnz done; //if not qword aligned we don't do anything
//
//primeloop:
//
//		movd mm1,[eax]; //mm1=00 00 00 00 a3 a2 a1 a0
//
//		pxor mm2,mm2; //mm2=0;
//
//		movq mm4,[esi]; //g1: mm4=src3 src2 src1 src0
//
//		punpcklbw mm1,mm2; //mm1=00a3 00a2 00a1 00a0
//
//loopqword:
//
//		mov edx,[eax];
//
//		test ebx,0xFFFFFFFC; //check if only 3 pixels left
//
//		jz checkback; //3 or less pixels left
//
//		//early out tests
//
//		cmp edx,0xffffffff; //test for alpha value of 1
//
//		je copyback; //if 1's copy the source pixels to the destination
//
//		test edx,0xffffffff; //test for alpha value of 0
//
//		jz leavefront; //if so go to the next 4 pixels
//
//		//the alpha blend starts
//
//		//green
//
//		//i=a*sg+(63-a)*dg;
//
//		//i=(i+32)+((i+32)>>6)>>6;
//
//		//red
//
//		//i=a*sr+(31-a)*dr;
//
//		//i=(i+16)+((i+16)>>5)>>5;
//
//		movq mm5,[edi]; //g2: mm5=dst3 dst2 dst1 dst0
//
//		psrlw mm1,2; //mm1=a?>>2 nuke out lower 2 bits
//
//		movq mm7,MASKSHIFTG; //g3: mm7=1 bit shifted green mask
//
//		psrlw mm4,1; //g3a: move src green down by 1 so that we won't overflow
//
//		movq mm0,mm1; //mm0=00a3 00a2 00a1 00a0
//
//		psrlw mm5,1; //g3b: move dst green down by 1 so that we won't overflow
//
//		psrlw mm1,1; //mm1=a?>>1 nuke out lower 1 bits
//
//		pand mm4,mm7; //g5: mm4=sg3 sg2 sg1 sg0
//
//		movq mm2,SIXONES;//g4: mm2=63
//
//		pand mm5,mm7; //g7: mm5=dg3 dg2 dg1 dg0
//
//		movq mm3,[esi]; //b1: mm3=src3 src2 src1 src0
//
//		psubsb mm2,mm0; //g6: mm2=63-a3 63-a2 63-a1 63-a0
//
//		movq mm7,MASKB; //b2: mm7=BLUE MASK
//
//		pmullw mm4,mm0; //g8: mm4=sg?*a?
//
//		movq mm0,[edi]; //b3: mm0=dst3 dst2 dst1 dst0
//
//		pmullw mm5,mm2; //g9: mm5=dg?*(1-a?)
//
//		movq mm2,mm7; //b4: mm2=fiveones
//
//		pand mm3,mm7; //b4: mm3=sb3 sb2 sb1 sb0
//
//		pmullw mm3,mm1; //b6: mm3=sb?*a?
//
//		pand mm0,mm7; //b5: mm0=db3 db2 db1 db0
//
//		movq mm7,[esi]; //r1: mm7=src3 src2 src1 src0
//
//		paddw mm4,mm5; //g10: mm4=sg?*a?+dg?*(1-a?)
//
//		pand mm7,MASKR; //r2: mm7=sr3 sr2 sr1 sr0　
//
//		psubsb mm2,mm1; //b5a: mm2=31-a3 31-a2 31-a1 31-a0
//
//		paddw mm4,FIVETWELVE; //g11: mm4=(mm4+512) green
//
//		pmullw mm0,mm2; //b7: mm0=db?*(1-a?)
//
//		movq mm5,mm4; //g12: mm5=mm4 green
//
//		psrlw mm7,11; //r4: shift src red down to position 0
//
//		psrlw mm4,6; //g13: mm4=mm4>>6
//
//		paddw mm4,mm5; //g14: mm4=mm4+mm5 green
//
//		paddw mm0,mm3; //b8: mm0=sb?*a?+db?*(1-a?)
//
//		movq mm5,[edi]; //r3: mm5=dst3 dst2 dst1 dst0
//
//		paddw mm0,SIXTEEN; //b9: mm0=(mm0+16) blue
//
//		pand mm5,MASKR; //r5: mm5=dr3 dr2 dr1 dr0
//
//		psrlw mm4,5; //g15: mm4=0?g0 0?g0 0?g0 0?g0 green
//
//		movq mm3,mm0; //b10: mm3=mm0 blue
//
//		psrlw mm0,5; //b11: mm0=mm0>>5 blue
//
//		psrlw mm5,11; //r6: shift dst red down to position 0
//
//		paddw mm0,mm3; //b12: mm0=mm3+mm0 blue
//
//		psrlw mm0,5; //b13: mm0=000b 000b 000b 000b blue
//
//		pmullw mm7,mm1; //mm7=sr?*a?
//
//		pand mm4,MASKG; //g16: mm4=00g0 00g0 00g0 00g0 green
//
//		pmullw mm5,mm2; //r7: mm5=dr?*(31-a?)
//
//		por mm0,mm4; //mm0=00gb 00gb 00gb 00gb
//
//		add eax,4; //move to next 4 alphas
//
//		add esi,8; //move to next 4 pixels in src
//
//		add edi,8; //move to next 4 pixels in dst
//
//		movd mm1,[eax]; //mm1=00 00 00 00 a3 a2 a1 a0
//
//		paddw mm5,mm7; //r8: mm5=sr?*a?+dr?*(31-a?)
//
//		paddw mm5,SIXTEEN; //r9: mm5=(mm5+16) red
//
//		pxor mm2,mm2; //mm2=0;
//
//		movq mm7,mm5; //r10: mm7=mm5 red
//
//		psrlw mm5,5; //r11: mm5=mm5>>5 red
//
//		movq mm4,[esi]; //g1: mm4=src3 src2 src1 src0
//
//		paddw mm5,mm7; //r12: mm5=mm7+mm5 red
//
//		punpcklbw mm1,mm2; //mm1=00a3 00a2 00a1 00a0
//
//		psrlw mm5,5; //r13: mm5=mm5>>5 red
//
//		psllw mm5,11; //r14: mm5=mm5<<10 red
//
//		por mm0,mm5; //mm0=0rgb 0rgb 0rgb 0rgb
//
//		sub ebx,4; //polished off 4 pixels
//
//		movq [edi-8],mm0; //dst=0rgb 0rgb 0rgb 0rgb
//
//		jmp loopqword; //go back to start
//
//copyback:
//
//		movq [edi],mm4; //copy source to destination
//
//leavefront:
//
//		add edi,8; //advance destination by 4 pixels
//
//		add eax,4; //advance alpha by 4
//
//		add esi,8; //advance source by 4 pixels
//
//		sub ebx,4; //decrease pixel count by 4
//
//		jmp primeloop;
//
//checkback:
//
//		test ebx,0xFF; //check if 0 pixels left
//
//		jz nextline; //done with this span
//
//		//backalign: //work out back end pixels
//
//		movq mm5,[edi]; //g2: mm5=dst3 dst2 dst1 dst0
//
//		psrlw mm1,2; //mm1=a?>>2 nuke out lower 2 bits
//
//		movq mm7,MASKSHIFTG; //g3: mm7=shift 1 bit green mask
//
//		psrlw mm4,1; //g3a: move src green down by 1 so that we won't overflow
//
//		movq mm0,mm1; //mm0=00a3 00a2 00a1 00a0
//
//		psrlw mm5,1; //g3b: move dst green down by 1 so that we won't overflow
//
//		psrlw mm1,1; //mm1=a?>>1 nuke out lower 1 bits
//
//		pand mm4,mm7; //g5: mm4=sg3 sg2 sg1 sg0
//
//		movq mm2,SIXONES;//g4: mm2=63
//
//		pand mm5,mm7; //g7: mm5=dg3 dg2 dg1 dg0
//
//		movq mm3,[esi]; //b1: mm3=src3 src2 src1 src0
//
//		psubsb mm2,mm0; //g6: mm2=63-a3 63-a2 63-a1 63-a0
//
//		movq mm7,MASKB; //b2: mm7=BLUE MASK
//
//		pmullw mm4,mm0; //g8: mm4=sg?*a?
//
//		movq mm0,[edi]; //b3: mm0=dst3 dst2 dst1 dst0
//
//		pmullw mm5,mm2; //g9: mm5=dg?*(1-a?)
//
//		movq mm2,mm7; //b4: mm2=fiveones
//
//		pand mm3,mm7; //b4: mm3=sr3 sr2 sr1 sr0
//
//		pmullw mm3,mm1; //b6: mm3=sb?*a?
//
//		pand mm0,mm7; //b5: mm0=db3 db2 db1 db0
//
//		movq mm7,[esi]; //r1: mm7=src3 src2 src1 src0
//
//		paddw mm4,mm5; //g10: mm4=sg?*a?+dg?*(1-a?)
//
//		pand mm7,MASKR; //r2: mm7=sr3 sr2 sr1 sr0
//
//		psubsb mm2,mm1; //b5a: mm2=31-a3 31-a2 31-a1 31-a0
//
//		paddw mm4,FIVETWELVE; //g11: mm4=(i+512) green
//
//		pmullw mm0,mm2; //b7: mm0=db?*(1-a?)
//
//		movq mm5,mm4; //g12: mm5=(i+512) green
//
//		psrlw mm7,11; //r4: shift src red down to position 0
//
//		psrlw mm4,6; //g13: mm4=(i+512)>>6
//
//		paddw mm4,mm5; //g14: mm4=(i+512)+((i+512)>>6) green
//
//		paddw mm0,mm3; //b8: mm0=sb?*a?+db?*(1-a?)
//
//		movq mm5,[edi]; //r3: mm5=dst3 dst2 dst1 dst0
//
//		paddw mm0,SIXTEEN; //b9: mm0=(i+16) blue
//
//		pand mm5,MASKR; //r5: mm5=dr3 dr2 dr1 dr0
//
//		psrlw mm4,5; //g15: mm4=0?g0 0?g0 0?g0 0?g0 green
//
//		movq mm3,mm0; //b10: mm3=(i+16) blue
//
//		psrlw mm0,5; //b11: mm0=(i+16)>>5 blue
//
//		psrlw mm5,11; //r6: shift dst red down to position 0
//
//		paddw mm0,mm3; //b12: mm0=(i+16)+(i+16)>>5 blue
//
//		psrlw mm0,5; //b13: mm0=000r 000r 000r 000r blue
//
//		pmullw mm7,mm1; //mm7=sr?*a?
//
//		pand mm4,MASKG; //g16: mm4=00g0 00g0 00g0 00g0 green
//
//		pmullw mm5,mm2; //r7: mm5=dr?*(31-a?)
//
//		por mm0,mm4; //mm0=00gb 00gb 00gb 00gb
//
//		add eax,4; //move to next 4 alphas
//
//		//stall
//
//		paddw mm5,mm7; //r8: mm5=sr?*a?+dr?*(31-a?)
//
//		paddw mm5,SIXTEEN; //r9: mm5=(i+16) red
//
//		movq mm7,mm5; //r10: mm7=(i+16) red
//
//		psrlw mm5,5; //r11: mm5=(i+16)>>5 red
//
//		paddw mm5,mm7; //r12: mm5=(i+16)+((i+16)>>5) red
//
//		psrlw mm5,5; //r13: mm5=(i+16)+((i+16)>>5)>>5 red
//
//		psllw mm5,11; //r14: mm5=mm5<<10 red
//
//		por mm0,mm5; //mm0=0rgb 0rgb 0rgb 0rgb
//
//		test ebx,2; //check if there are 2 pixels
//
//		jz oneendpixel; //goto one pixel if that's it
//
//		movd [edi],mm0; //dst=0000 0000 0rgb 0rgb
//
//		psrlq mm0,32; //mm0>>32
//
//		add edi,4; //edi=edi+4
//
//		sub ebx,2; //saved 2 pixels
//
//		jz nextline; //all done goto next line
//
//oneendpixel: //work on last pixel
//
//		movd edx,mm0; //edx=0rgb
//
//		mov [edi],dx; //dst=0rgb
//
//nextline: //goto next line
//
//		dec ecx; //nuke one line
//
//		jz done; //all done
//
//		mov eax,lpLinearAlpBp; //alpha
//
//		mov esi,lpLinearSrcBp; //src
//
//		mov edi,lpLinearDstBp; //dst
//
//		add eax,iAlpPitch; //inc alpha ptr by 1 line
//
//		add esi,iSrcPitch; //inc src ptr by 1 line
//
//		add edi,iDstPitch; //inc dst ptr by 1 line
//
//		mov lpLinearAlpBp,eax; //save new alpha base ptr
//
//		mov ebx,iDstW; //ebx=span width to copy
//
//		mov lpLinearSrcBp,esi; //save new src base ptr
//
//		mov lpLinearDstBp,edi; //save new dst base ptr
//
//		jmp primeloop; //start the next span
//
//done:
//
//		emms
//	}
//}
//
////lpSrc:物体图像数据16位
////lpdest:目标内存位置16位,保存混合后的数据
////lpalpha:alpha通道数据8位
////其中lpalpha与lpSrc的图象数据大小一致
////nSrcWidth, nDestWidth: 为各自表面或内存图像的宽度
////nSrcXFrom, nSrcYFrom,
////nDestXFrom, nDestYFrom 以上是在混合时,从每个图像数据的第Y行的第X个像素开始混合
////int nBlendPixel:每行混合多少个像素
////int nBlendLine:总共混合多少行
//void AlphaBlendTo565New(LPWORD lpDest,
//						LPWORD lpSrc,
//						LPBYTE lpAlpha,
//						int nSrcWidth, int nDestWidth,
//						int nSrcXFrom, int nSrcYFrom,
//						int nDestXFrom, int nDestYFrom,
//						int nBlendPixel, int nBlendLine)
//{
//	DWORD dwSrcPitch, dwDestPitch, dwAlphaPitch;
//	LPWORD lpSrc0, lpDest0;
//	LPBYTE lpAlpha0;
//
//	const __int64 MMX_BLUE_MASK_565 = 0x001F001F001F001F;
//
//	if (nBlendPixel % 4 > 0)
//		nBlendPixel -= nBlendPixel % 4;
//
//	if (nBlendPixel <= 0 || nBlendLine <= 0)
//		return;
//
//	//每行数据处理结束后, 应跳过的距离
//	dwSrcPitch = (nSrcWidth - nBlendPixel) << 1;
//	dwDestPitch = (nDestWidth - nBlendPixel) << 1;
//	dwAlphaPitch = (nSrcWidth - nBlendPixel);
//
//	//数据开始的位置
//	lpSrc0 = lpSrc + nSrcWidth * nSrcYFrom + nSrcXFrom;
//	lpAlpha0 = lpAlpha + nSrcWidth * nSrcYFrom + nSrcXFrom;
//	lpDest0 = lpDest + nDestWidth * nDestYFrom + nDestXFrom;
//	nBlendPixel >>= 2;
//	__asm
//	{
//		push edi;
//		push esi;
//		push eax;
//		push ebx;
//		push ecx;
//		push edx;
//
//		mov esi, lpSrc0;//放入源数据的内存地址lpSource，
//		mov edi, lpDest0;//放入目的数据的内存地址lpDest
//		mov ebx, lpAlpha0; //放入Alpha的内存地址lpAlpha
//
//		//外层循环开始
//		mov ecx, nBlendLine;//循环变量,混合行数
//		pxor mm7, mm7;
//		//内层循环开始
//L1: mov edx, nBlendPixel;//循环变量,混合每行的多少像素
//		//shr edx, 2; //一次处理4个点,所以为宽度除以4
//L2: mov eax, [ebx];//Alpha值
//		//cmpblack: 比较Alpha值,如果是0x1F则直接使用前景色,如果是0,则直接使用背景色
//		cmp eax, 0;
//		jz blendend;
//		//cmpwhite://比较Alpha值是否是白色
//		cmp eax, 0x1F1F1F1F;
//		jnz beginblend; //不相等,则需要混合
//
//		movq mm0, [esi]; //是白色直接使用源数据
//		movq [edi], mm0;
//
//		jmp blendend;
//beginblend:
//		movd mm6, eax;
//		punpcklbw mm6, mm7;
//		//开始混合
//		//混合公式
//		//混合后的颜色=(( src-dest ) * Alpha ) >> 5 + dest
//		movq mm0, [esi];//源数据放在mm0-2
//		movq mm3, [edi];//背景数据放在mm3-5
//
//		movq mm1, mm0;
//		movq mm4, mm3;
//		movq mm2, mm0;
//		movq mm5, mm3;
//
//		psrlw mm0, 11; //右移11位,保留足够的空间给乘法和加法公式
//		psrlw mm3, 11; //右移11位,保持同步
//		psrlw mm1, 6; //右移6位,保留足够的空间给乘法和加法公式
//		psrlw mm4, 6; //右移6位,保持同步,顺变转换成555
//
//		pand mm1, MMX_BLUE_MASK_565; //分色
//		pand mm2, MMX_BLUE_MASK_565;
//		pand mm4, MMX_BLUE_MASK_565;
//		pand mm5, MMX_BLUE_MASK_565;
//
//		psubsw mm0, mm3; //src - dest
//		psubsw mm1, mm4;
//		psubsw mm2, mm5;
//
//		pmullw mm0, mm6; //(src-dest)*alpha
//		pmullw mm1, mm6;
//		pmullw mm2, mm6;
//
//		psraw mm0, 5; //((src-dest)*alpha >> 5)
//		psraw mm1, 5;
//		psraw mm2, 5;
//
//		paddsw mm0, mm3; //((src-dest)*alpha >> 5) + dest
//		paddsw mm1, mm4;
//		paddsw mm2, mm5;
//
//		psllw mm0, 10; //左移5位,恢复到最高5位
//		psllw mm1, 5; //左移5位,恢复到高5位, 555
//
//		por mm0, mm1;//合成颜色
//		// Is565?
//		// cmp bIs565, 0;
//		// jz _555;
//		psllw mm0, 1;//565才需要这一步
//		//_555:
//		por mm0, mm2;
//		movq [edi],mm0; //混合完毕,保存到目标内存中
//
//blendend:
//		//4个点混合完毕,移动内存指针(每个点2个字节,故为4*2)
//		//以及移动Alpha内存指针
//		add esi, 8;
//		add edi, 8;
//		add ebx, 4;
//
//		dec edx; //判断内层循环是否结束
//		jnz L2;
//
//		//内层循环结束
//		add esi, dwSrcPitch; //移动内存指针到下一行, Src
//		add edi, dwDestPitch; //Dest
//		add ebx, dwAlphaPitch; //Alpha
//
//		dec ecx; //判断外层循环是否结束
//		jnz L1;
//
//		//外层循环结束
//		//清除mmx标记
//		emms;
//
//		//复原寄存器
//		pop edx;
//		pop ecx;
//		pop ebx;
//		pop eax;
//		pop esi;
//		pop edi;
//	}
//}

#endif