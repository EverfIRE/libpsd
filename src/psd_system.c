// created: 2007-01-27
#include <stdio.h>
#include <stdlib.h>
#include "libpsd.h"
#include "psd_system.h"

/*
����malloc()��calloc()������������̬�����ڴ�ռ�,��������������
malloc()������һ������,��Ҫ������ڴ�ռ�Ĵ�С:
void*malloc(size_tsize);
calloc()��������������,�ֱ�ΪԪ�ص���Ŀ��ÿ��Ԫ�صĴ�С,�����������ĳ˻�����Ҫ������ڴ�ռ�Ĵ�С��
void*calloc(size_tnumElements,size_tsizeOfElement);
������óɹ�,����malloc()�ͺ���calloc()����������������ڴ�ռ���׵�ַ��
����malloc()�ͺ���calloc() ����Ҫ������ǰ�߲��ܳ�ʼ����������ڴ�ռ�,�������ܡ������malloc()����������ڴ�ռ�ԭ��û�б�ʹ�ù��������е�ÿһλ���ܶ���0;��֮, ����ⲿ���ڴ������������,�����п��������и��ָ��������ݡ�Ҳ����˵��ʹ��malloc()�����ĳ���ʼʱ(�ڴ�ռ仹û�б����·���)�������� ��,������һ��ʱ��(�ڴ�ռ仹�Ѿ������·���)���ܻ�������⡣
����calloc() �Ὣ��������ڴ�ռ��е�ÿһλ����ʼ��Ϊ��,Ҳ����˵,�������Ϊ�ַ����ͻ��������͵�Ԫ�ط����ڴ�,������ЩԪ�ؽ���֤�ᱻ��ʼ��Ϊ0;�������Ϊָ �����͵�Ԫ�ط����ڴ�,������ЩԪ��ͨ���ᱻ��ʼ��Ϊ��ָ��;�����Ϊʵ�����ݷ����ڴ�,����ЩԪ�ػᱻ��ʼ��Ϊ�����͵��㡣
*/
void * psd_malloc(psd_int size)
{
	void *pMalloc=malloc(size);
	memset(pMalloc,0,size);		//add by yingcai, like calloc()
	return pMalloc;
}

void * psd_realloc(void * block, psd_int size)
{
	return realloc(block, size);
}

void psd_free(void * block)
{
	free(block);
	//block = NULL:		//add by freeman
}

void psd_freeif(void * block)
{
	if (block != NULL)
	{
		psd_free(block);
	//	block = NULL:		//add by freeman
	}
}

void * psd_fopen(psd_char * file_name)
{
	return (void *)fopen(file_name, "rb+");		//need allow to write
}

psd_int psd_fsize(void * file)
{
	psd_int offset, size;

	offset = ftell((FILE *)file);
	fseek((FILE *)file, 0, SEEK_END);
	size = ftell(file);
	fseek((FILE *)file, 0, SEEK_SET);
	fseek((FILE *)file, offset, SEEK_CUR);

	return size;
}

//we only read byte one by one. do NOT read word!!!
psd_int psd_fread(psd_uchar * buffer, psd_int count, void * file)
{
	return fread(buffer, 1, count, (FILE *)file);
}

psd_int psd_fseek(void * file, psd_int length)
{
	return fseek((FILE *)file, length, SEEK_CUR);
}

void psd_fclose(void * file)
{
	fclose((FILE *)file);
}
/*
����ԭ��:
size_t fwrite(   const void *buffer,   size_t size,   size_t count,   FILE *stream ); 
�����б�:
buffer    // ��д���ݻ��������׵�ַ,Pointer to data to be written    
size  //һ��д�����ݿ�Ĵ�С,Item size in bytes
count  // д�����ݿ�Ĵ���,Maximum number of items  to be written
stream  // �ļ��ṹָ��,Pointer to FILE structure
eg: 
    fwrite(pBuffer, sizeof(unsigned long), 0x20000, fp_dma);
//��pBuffer ���0X20000��unsigned long���͵����ݵ�fp_dma�ļ�.
*/
psd_int psd_fwrite(psd_uchar * buffer, psd_int count, void * file)
{
	return fwrite(buffer, 1, count, (FILE *)file);
}
/*
ftell������������ȡ�ļ��ĵ�ǰ��дλ��;
����ԭ��: long ftell(FILE *fp)
��������:�õ���ʽ�ļ��ĵ�ǰ��дλ��,�䷵��ֵ�ǵ�ǰ��дλ��ƫ���ļ�ͷ�����ֽ���.
*/
psd_int psd_ftell(void * file)
{
	return ftell((FILE *)file);
}
void *psd_fnew(psd_char * file_name)
{
	return (void *)fopen(file_name, "wb+");
}

//copy file if it exists, add by freeman
static int psd_fCopy(psd_char * filename)
{
	FILE *fpSrc=NULL,*fpDst=NULL;
	int ret=-1;
	psd_int offset, size;
	psd_uchar *buffer;
//	char DstFileName[128];

	//open source file to read
	fpSrc = fopen(filename, "rb");
	if (fpSrc == NULL)
	{
		ret = -1;
		goto End;
	}

	//create a tmp psd file in current dir to write
	fpDst = fopen("tmp.psd", "wb+");
	//get file size
	offset = ftell((FILE *)fpSrc);
	fseek((FILE *)fpSrc, 0, SEEK_END);
	size = ftell(fpSrc);
	fseek((FILE *)fpSrc, 0, SEEK_SET);
	fseek((FILE *)fpSrc, offset, SEEK_CUR);
	//malloc buffer
	buffer = malloc(size);
	if (buffer == NULL)
	{
		ret = -2;
		goto End;
	}
	fread(buffer, 1, size, (FILE *)fpSrc);	//read source file by byte
	fwrite(buffer, 1, size, (FILE *)fpDst);	//write to dst file by byte
	ret=0;
	
End:
	if (fpSrc)	fclose(fpSrc);
	if (fpDst)	fclose(fpDst);
	return ret;
}