#include <stdlib.h>
#include <stdio.h>
#include "extmem.h"
#include<time.h>

#define MAX 1000

typedef struct TempArray
{
	int c;
	int d;
	int blk;
}TempArray;

typedef struct index  //����
{
	int a;
	int blk;
}index;



//ȫ�ֱ�������
int bucketR[5], bucketS[5];
int index_countR = 0, index_countS = 0;
int temp_count = 0, tempflag = 0;

TempArray **BucketR;
TempArray **BucketS;
int Random(int low, int high);                            //���������
void OutputData(int beginAddr, int n, unsigned int *blk, Buffer *buf, int flag); //��ӡ����
void storeData(unsigned int * blk, Buffer *buf);          //�洢����
TempArray *loadData(unsigned int * blk, Buffer *buf, int len, int addr, int templen);  //��ȡ����

TempArray *sort(TempArray *temp, int len);  //����

void SelectRelationship(unsigned int * blk, Buffer *buf, TempArray *tempR, TempArray *tempS);   //ѡ���ϵ
void LineSearch(unsigned int * blk, Buffer *buf, TempArray *tempR, TempArray *tempS);      // ���Բ���
int Binary(int num, TempArray *temp, int len);
void BinarySearch(unsigned int * blk, Buffer *buf, TempArray *tempR, TempArray *tempS);   // ��������
index * CreateIndex(unsigned int *blk, Buffer *buf, TempArray *temp, int addr, int len);
void IndexSearch(unsigned int * blk, Buffer *buf, TempArray *tempR, TempArray *tempS);    // �����㷨

void MappingRelationship(unsigned int * blk, Buffer *buf);  //ͶӰ��ϵ
unsigned int Merge(unsigned int left, int ln, unsigned int right, int rn, Buffer *buf, int addr);
unsigned int Exsort(unsigned int saddr, int n, Buffer *buf, int addr);

void ConnectRelationship(unsigned int * blk, Buffer *buf, TempArray *tempR, TempArray *tempS);   //���ӹ�ϵ
void Nst_Loop_Join(unsigned int * blk, Buffer *buf);   //Nst_Loop_Join
void Sort_Merge_Join(unsigned int * blk, Buffer *buf, TempArray *tempR, TempArray *tempS); //sort_merge_join
void Hash_Join(unsigned int *blk, Buffer *buf, TempArray *tempR, TempArray *tempS, int addr); //hash_join

void CollectionRelationship(unsigned int * blk, Buffer *buf);  //���ϲ���
void CalculationIntersection(unsigned int *blk, Buffer *buf, TempArray *temp);   //����R S�Ľ���
void UnionOfRelation(unsigned int * blk, Buffer *buf, TempArray *temp);       //����
void IntersectionOfRelation(unsigned int *blk, Buffer *buf, TempArray *temp); //����
void DifferenceOfRelation(unsigned int *blk, Buffer *buf, TempArray *temp);   //�


int main()
{
	srand((unsigned)time(NULL));
	Buffer buf; /* A buffer */
	//unsigned char *blk; /* A pointer to a block */
	unsigned int *blk = NULL;
	int i = 0;
	int sel;
	/* Initialize the buffer */
	if (!initBuffer(520, 64, &buf))
	{
		perror("Buffer Initialization Failed!\n");
		return -1;
	}
	storeData(blk, &buf);
	TempArray *tempR = loadData(blk, &buf, 16, 0, 112);
	TempArray *tempS = loadData(blk, &buf, 32, 20, 224);
	tempR = sort(tempR, 112);
	tempS = sort(tempS, 224);
	while (1)
	{
		printf("***** 1 ѡ��\t2 ͶӰ\t3 ����\t 4 ���ϲ��� *****\n ");
		scanf("%d", &sel);
		switch (sel)
		{
		case 1:  //ѡ���ϵ
		{
			SelectRelationship(blk, &buf, tempR, tempS);
			continue;
		}
		case 2: //ͶӰ��ϵ
		{
			MappingRelationship(blk, &buf);
			continue;
		}
		case 3:  //���ӹ�ϵ
		{
			ConnectRelationship(blk, &buf, tempR, tempS);
			continue;
		}
		case 4: //���ϲ���
		{
			CollectionRelationship(blk, &buf);
			continue;
		}
		default:
		{
			printf("\tѡ����Ŵ�����������!\n\n");
			break;
		}
		}
		//printf("# of IO's is %d\n", buf.numIO); /* Check the number of IO's */
	}
	printf("\n");
	printf("# of IO's is %d\n", buf.numIO); /* Check the number of IO's */
	system("pause");
	return 0;
}
/** \brief  ���������
*  Random ����low-high֮��������
* \param int low
* \param  int high
* \return  int
*
*/
int Random(int low, int high)  //���������
{
	return rand() % (high - low) + low + 1;
}
/** \brief  �洢����
*     storeData ������ɵ����ݴ洢 R 0-15  S 20-51
* \param unsigned int *blk
* \param  Buffer * buf
* \return void
*
*/
void storeData(unsigned int * blk, Buffer *buf) //�洢����
{
	unsigned int a, b, c, d;
	int Ri, Rj, Si, Sj;
	for (Ri = 0; Ri<16; Ri++)
	{
		

		blk = (unsigned int *)getNewBlockInBuffer(buf);
		for (Rj = 0; Rj<7; Rj++)
		{
			a = Random(1, 40);
			b = Random(1, 1000);
			*(blk + 2 * Rj) = a;
			*(blk + 2 * Rj + 1) = b;
		}
		*(blk + 2 * Rj + 1) = Ri + 1;
		if (Ri == 15)
		{
			*(blk + 2 * Rj + 1) = 0;
		}
		//printf("R %d,%d\n",*blk,*(blk+1));
		if (writeBlockToDisk((unsigned char *)blk, Ri, buf) != 0)
		{
			perror("Writing Block Failed!\n");
			return;
		}
		freeBlockInBuffer((unsigned char *)blk, buf);
	}
	for (Si = 0; Si<32; Si++)
	{
		blk = (unsigned int *)getNewBlockInBuffer(buf);
		for (Sj = 0; Sj<7; Sj++)
		{
			c = Random(20, 60);
			d = Random(1, 1000);
			*(blk + 2 * Sj) = c;
			*(blk + 2 * Sj + 1) = d;
		}
		*(blk + 2 * Sj + 1) = Si + 21; //����һ���Ŀ��
		if (Si == 31)
		{
			*(blk + 2 * Sj + 1) = 0;
		}
		//printf("S %d,%d\n",*blk,*(blk+1));
		if (writeBlockToDisk((unsigned char *)blk, Si + 20, buf) != 0)
		{
			perror("Writing Block Failed!\n");
			return;
		}
		freeBlockInBuffer((unsigned char *)blk, buf);
	}
}
/** \brief OutputData
* ��ӡ�洢������
* \param int beginAddr  �����ʼ��ַ
* \param int n          ����Ŀ
* \param unsigned int *blk
* \param Buffer *buf
* \param int flag   �����R\S
* \return void
*
*/

void OutputData(int beginAddr, int n, unsigned int *blk, Buffer *buf, int flag)
{
	int i, j;
	for (i = beginAddr; i<beginAddr + n; i++)
	{
		blk = (unsigned int *)readBlockFromDisk(i, buf);
		if (flag == 3)
		{
			for (j = 0; j<15; j = j + 3)
			{
				printf("(%d, %d, %d) ", *(blk + j), *(blk + j + 1), *(blk + j + 2));
			}
			printf("\n");
		}
		if (flag == 2)
		{
			for (j = 0; j<7; j++)
			{
				printf("(%d ,%d) ", *(blk + 2 * j), *(blk + 2 * j + 1));
			}
			printf("\n");
		}
		if (flag == 1)
		{
			for (j = 0; j<15; j++)
			{
				printf("%d ", *(blk + j));
			}
			printf("\n");
		}
		freeBlockInBuffer((unsigned char *)blk, buf);

	}
}
/** \brief ��ȡ����
*   loadData  ��ȡ�������ݵ�TempArray
* \param  unsigned int * blk
* \param  Buffer * buf
* \param  int len �����
* \param  int addr ��ʼ���
* \param  int templen Ԫ�������Ŀ
* \return TempArray *
*
*/
TempArray * loadData(unsigned int * blk, Buffer *buf, int len, int addr, int templen)
{
	unsigned int a, b;
	TempArray *temp;
	temp = (TempArray *)malloc(sizeof(TempArray)*templen);
	int i, j, temp_num = 0;
	for (i = 0; i<len; i++)
	{
		if ((blk = (unsigned int *)readBlockFromDisk(addr + i, buf)) == NULL)
		{
			perror("Reading Block Failed!\n");
			return (TempArray *)-1;
		}
		for (j = 0; j<7; j++)
		{
			a = *(blk + 2 * j);
			b = *(blk + 2 * j + 1);
			//printf("R = %d,%d\n",a,b);
			temp[temp_num].c = a;
			temp[temp_num].d = b;
			temp[temp_num].blk = addr + i;
			temp_num++;
		}
		freeBlockInBuffer((unsigned char *)blk, buf);
	}
	return temp;
}
/** \brief ����
* sort��ð������
* \param  TempArray *temp ��temp����
* \param  int len ����Ԫ����Ŀ
* \return TempArray *
*
*/
TempArray *sort(TempArray *temp, int len)
{
	int i, j;
	TempArray tempp;
	for (i = 0; i<len; i++)
	{
		for (j = len - 1; j >= i + 1; j--)
		{
			if (temp[j].c < temp[j - 1].c)
			{
				tempp = temp[j];
				temp[j] = temp[j - 1];
				temp[j - 1] = tempp;
			}
		}
	}
	return temp;
}

/** \brief BinarySearch
* ��������
* \param int num  ���ҵ�keyֵ
* \param TempArray *temp ����
* \param int len    Ԫ����Ŀ
* \return int ���ز���Ԫ�����ڵ�λ��
*
*/
int Binary(int num, TempArray *temp, int len)
{
	int left = 0, right = len, mid;
	while (left <= right)
	{
		mid = (left + right) / 2;
		if (temp[mid].c == num)
		{
			return mid;
		}
		if (temp[mid].c < num)
		{
			left = mid + 1;
		}
		else
			right = mid - 1;
	}
	return -1;
}
/** \brief BinarySearch
*  ��������
* \param unsigned int * blk
* \param Buffer *buf
* \param TempArray *tempR RԪ��
* \param TempArray *tempS SԪ��
* \return
*
*/

void BinarySearch(unsigned int * blk, Buffer *buf, TempArray *tempR, TempArray *tempS)
{
	int Rnum = Binary(40, tempR, 112);
	int snum = Binary(60, tempS, 224);
	int i, j, Ri = Rnum, Si = snum, num = 300, t_count;
	TempArray *temp_ = (TempArray *)malloc(sizeof(TempArray) * 336);
	if (Rnum != -1)
	{
		i = 0;
		temp_[i] = tempR[Rnum];
		while ((tempR[Rnum + 1].c == 40) && (Rnum <= 112)) //�����ظ���
		{

			temp_[++i] = tempR[++Rnum];
		}
		while ((tempR[Ri - 1].c == 40) && (Ri >= 1)) //�����ظ���
		{

			temp_[++i] = tempR[--Ri];
		}
	}
	if (snum != -1)
	{
		temp_[++i] = tempS[snum];
		while (tempS[snum + 1].c == 60 && snum <= 224)
		{

			temp_[++i] = tempS[++snum];
		}
		while (tempS[Si - 1].c == 60 && Si >= 0)
		{
			temp_[++i] = tempS[--Si];
		}
		blk = (unsigned int *)getNewBlockInBuffer(buf);
		t_count = 0;
		for (j = 0; j <= i; j++)
		{
			printf("(%d, %d) \n", temp_[j].c, temp_[j].d);
			*(blk + 2 * t_count) = temp_[j].c;
			*(blk + 2 * t_count + 1) = temp_[j].d;
			if (t_count == 7)
			{
				*(blk + 2 * t_count) = num + 1;
				writeBlockToDisk((unsigned char *)blk, num, buf);
				freeBlockInBuffer((unsigned char *)blk, buf);
				num++;
				t_count = 0;
			}
			if (j == i&&t_count<7) //���һ���鲻��
			{
				writeBlockToDisk((unsigned char *)blk, num, buf);
				freeBlockInBuffer((unsigned char *)blk, buf);
			}
		}
	}
	else
	{
		printf("û��Ҫ���Ԫ��\n");
	}
}
/** \brief ��������
* CreateIndex
* \param TempArray *temp ��temp��������
* \param int addr �����洢�Ŀ��
* \param int len  ����
* \param unsigned int *blk
* \param Buffer *buf
* \return index* ���ش���������
*
*/

index * CreateIndex(unsigned int *blk, Buffer *buf, TempArray *temp, int addr, int len) //��temp��������
{
	index *myIndex = (index *)malloc(sizeof(index) * 40);
	int i, index_count = 0, w_count = 0;
	myIndex[index_count].a = temp[0].c;
	myIndex[index_count].blk = addr;
	blk = (unsigned int *)getNewBlockInBuffer(buf);
	for (i = 0; i<len; i++)
	{
		*(blk + 2 * w_count) = temp[i].c;
		*(blk + 2 * w_count + 1) = temp[i].d;
		w_count++;
		if (w_count == 7)           //�����洢
		{
			if (i == len - 1)  *(blk + 2 * w_count + 1) = 0;//���һ����ĺ�̴��̺���0
			else *(blk + 2 * w_count + 1) = addr + 1;
			writeBlockToDisk((unsigned char *)blk, addr, buf);
			freeBlockInBuffer((unsigned char *)blk, buf);
			addr++;
			w_count = 0;
		}
		if (temp[i].c > temp[i - 1].c && i>0) //��һ����ǰһ�����ʱ�򣬴������ظ��еĵ�һ��
		{                           //��������
			index_count++;
			myIndex[index_count].a = temp[i].c;
			if (w_count == 0) myIndex[index_count].blk = addr - 1;
			else myIndex[index_count].blk = addr; //�洢��ǰ�Ŀ��
		}

	}
	if (len == 112) index_countR = index_count;
	else index_countS = index_count;
	return myIndex;
}
/** \brief �����㷨
* IndexSearch  ��������
* \param unsigned int * blk
* \param Buffer *buf
* \param TempArray *tempR RԪ��
* \param TempArray *tempS SԪ��
* \return void
*
*/

void IndexSearch(unsigned int * blk, Buffer *buf, TempArray *tempR, TempArray *tempS)
{
	index *indexR = CreateIndex(blk, buf, tempR, 5000, 112);
	index *indexS = CreateIndex(blk, buf, tempS, 6000, 224);
	int i, block = 0, j;
	for (i = 0; i <= index_countR; i++)
	{
		if (indexR[i].a == 40)
		{
			block = indexR[i].blk; //��ѯ��һ���������ڵĴ��̿�
			printf("block %d\n", block);
			while (block != -1)
			{
				blk = (unsigned int *)readBlockFromDisk(block, buf);
				for (j = 0; j<7; j++)
				{
					if (*(blk + 2 * j) == 40)
						printf("(%d,%d) \n", *(blk + 2 * j), *(blk + 2 * j + 1));
					if (*(blk + 2 * j)>40)
						block = -1;
				}
				if (*(blk + 2 * j + 1) == 0) //���һ��
				{
					block = -1;
				}
				freeBlockInBuffer((unsigned char *)blk, buf);
				printf("\n");
			}

		}
	}
	for (i = 0; i <= index_countS; i++)
	{
		if (indexS[i].a == 60)
		{
			block = indexS[i].blk; //��ѯ��һ���������ڵĴ��̿�
			printf("block %d\n", block);
			while (block != -1)
			{
				blk = (unsigned int *)readBlockFromDisk(block, buf);
				for (j = 0; j<7; j++)
				{
					if (*(blk + 2 * j) == 60)
						printf("(%d,%d) \n", *(blk + 2 * j), *(blk + 2 * j + 1));
					if (*(blk + 2 * j)>60)
						block = -1;
				}
				if (*(blk + 2 * j + 1) == 0) //���һ��
				{
					block = -1;
				}
				freeBlockInBuffer((unsigned char *)blk, buf);
				printf("\n");
			}

		}
	}
}
/** 
* LineSearch  ��������
* \param unsigned int * blk
* \param Buffer *buf
* \param TempArray *tempR RԪ��
* \param TempArray *tempS SԪ��
* \return void
*
*/
void LineSearch(unsigned int * blk, Buffer *buf, TempArray *tempR, TempArray *tempS)
{
	int i, j, a, b, c, d;
	int line_count = 60;  //60 ��ʼ�洢
	unsigned int *disk_blk;
	for (i = 0; i<16; i++)  //��R
	{
		blk = (unsigned int *)readBlockFromDisk(i, buf);
		for (j = 0; j<7; j++)
		{
			if (*(blk + 2 * j) == 40)
			{
				a = 40;
				b = *(blk + 2 * j + 1);
				printf("a = %d, b = %d\n", a, b);
				disk_blk = (unsigned int *)getNewBlockInBuffer(buf);
				*disk_blk = a;
				*(disk_blk + 1) = b;
				writeBlockToDisk((unsigned char *)disk_blk, line_count, buf);
				line_count++;
			}
		}
		freeBlockInBuffer((unsigned char *)blk, buf);
	}
	line_count += 10;   //�м���10��blk������ R.A=40 ��S.C=60��Ԫ��
	for (i = 0; i<32; i++)
	{
		blk = (unsigned int *)readBlockFromDisk(i + 20, buf);
		for (j = 0; j<7; j++)
		{
			if (*(blk + 2 * j) == 60)
			{
				c = 60;
				d = *(blk + 2 * j + 1);
				printf("c = %d, d = %d\n", c, d);
				disk_blk = (unsigned int *)getNewBlockInBuffer(buf);
				*disk_blk = c;
				*(disk_blk + 1) = d;
				writeBlockToDisk((unsigned char *)disk_blk, line_count, buf);
				line_count++;
			}
		}
		freeBlockInBuffer((unsigned char *)blk, buf);
	}
	printf("\n");
}
/** \brief ѡ���ϵ
* �������� �������� �����㷨
* �����Ŵ���
* \param unsigned int * blk
* \param Buffer *buf
* \param TempArray *tempR
* \param TempArray *tempS
* \return void
*
*/
void SelectRelationship(unsigned int * blk, Buffer *buf, TempArray *tempR, TempArray *tempS)
{
	printf("ѡ���ѯ�Ӳ˵�\n");
	printf("1 ��������\n");
	printf("2 ��������\n");
	printf("3 �����㷨\n");
	printf("-1 ����\n");
	int sel = 0;
	int i, j, a, b, c, d;
	int line_count = 60;  //60 ��ʼ�洢
	unsigned int *disk_blk;
	while (sel != -1)
	{
		scanf("%d", &sel);
		switch (sel)
		{
		case 1: //��������
		{
			LineSearch(blk, buf, tempR, tempS);
			break;
		}
		case 2: //��������
		{
			BinarySearch(blk, buf, tempR, tempS);
			break;
		}
		case 3: //�����㷨
		{
			IndexSearch(blk, buf, tempR, tempS);
			break;
		}
		default:
		{
			break;
		}
		}
	}

}
/** \brief �鲢
* Merge
* \param unsigned int left
* \param int ln
* \param unsigned int right
* \param int rn
* \param Buffer *buf
* \param int addr
* \return unsigned int
*
*/

unsigned int Merge(unsigned int left, int ln, unsigned int right, int rn, Buffer *buf, int addr)
{
	int l = 0, r = 0;//����
	int offset = 0, i;
	unsigned int tmpaddr = left + addr;//�������ĵ�ַ
	unsigned int *blk1 = NULL, *blk2 = NULL;
	while (l<ln || r<rn)
	{
		if (l == ln)//��߽���
			*blk1 = MAX;
		else
			blk1 = (unsigned int *)readBlockFromDisk(left + l, buf);
		if (r == rn)
			*blk2 = MAX;//�ұ߽���
		else
			blk2 = (unsigned int *)readBlockFromDisk(right + r, buf);
		if (*blk1<*blk2)
		{
			writeBlockToDisk((unsigned char *)blk1, tmpaddr + offset, buf);
			l++;//���ָ�����
			offset++;//д��ĵ�ַ����ƶ�
		}
		else
		{
			writeBlockToDisk((unsigned char *)blk2, tmpaddr + offset, buf);
			r++;
			offset++;
		}
		freeBlockInBuffer((unsigned char *)blk1, buf);
		freeBlockInBuffer((unsigned char *)blk2, buf);
	}
	for (i = 0; i<offset; i++)//д��ԭ����λ��
	{
		blk1 = (unsigned int*)readBlockFromDisk(tmpaddr + i, buf);
		writeBlockToDisk((unsigned char *)blk1, left + i, buf);
		freeBlockInBuffer((unsigned char *)blk1, buf);
	}
	return left;
}
/** \brief �鲢
* Exsort
* \param unsigned int saddr
* \param int n
* \param Buffer *buf
* \param int addr
* \return unsigned int
*
*/

unsigned int Exsort(unsigned int saddr, int n, Buffer *buf, int addr)
{
	unsigned int left, right;
	if (n == 1)   ///complete sorting
		return saddr;
	left = Exsort(saddr, n / 2, buf, addr);
	right = Exsort(saddr + n / 2, n - n / 2, buf, addr);
	return Merge(left, n / 2, right, n - n / 2, buf, addr);
}
/** \brief ͶӰ��ϵ
* ʵ�ֹ�ϵͶӰ�����洢�ڴ�����
* \param unsigned int * blk
* \param Buffer *buf
* \return void
*
*/
void MappingRelationship(unsigned int * blk, Buffer *buf)  //��R.AͶӰ
{
	printf("\t ͶӰ��ϵ   \n");
	int ri, rj, a, basic = 200, num = 0, counta = 0, count = 0;
	unsigned int *disk_blk, tmp;
	printf("\t����ǰ R.A :\n");
	for (ri = 0; ri<16; ri++)
	{
		if ((blk = (unsigned int *)readBlockFromDisk(ri, buf)) == NULL)
		{
			perror("Reading Block Failed!\n");
			return;
		}
		for (rj = 0; rj<7; rj++)
		{
			a = *(blk + 2 * rj);
			printf("%d ", a);
			disk_blk = (unsigned int *)getNewBlockInBuffer(buf);
			*disk_blk = a;
			writeBlockToDisk((unsigned char *)disk_blk, basic + num, buf); //200-311.blk
			num++;
			freeBlockInBuffer((unsigned char *)disk_blk, buf);
		}
		freeBlockInBuffer((unsigned char *)blk, buf);
	}
	printf("\n");
	printf("\t����� R.A:\n");
	basic = Exsort(basic, num, buf, 500);
	//printf("%d\n",basic);
	for (ri = 0; ri<num; ri++)
	{
		blk = (unsigned int *)readBlockFromDisk(basic + ri, buf);
		printf("%d ", *blk);
		freeBlockInBuffer((unsigned char *)blk, buf);
	}
	printf("\n\tȥ�غ� R.A :\n");
	blk = (unsigned int*)readBlockFromDisk(basic, buf);
	disk_blk = (unsigned int *)getNewBlockInBuffer(buf); //������� 350-�� R.A
	tmp = *blk;
	*(disk_blk + count) = tmp;
	writeBlockToDisk((unsigned char *)disk_blk,basic+150+counta,buf);
	printf("%d ", tmp);
	for (ri = 1; ri<num; ri++)
	{
		while (*blk == tmp)
		{
			freeBlockInBuffer((unsigned char *)blk, buf);
			ri++;
			if (ri<num)
				blk = (unsigned int*)readBlockFromDisk(basic + ri, buf);
			else
			{
				goto END;
			}
		}
		disk_blk = (unsigned int *)getNewBlockInBuffer(buf); //������� 350-390��
		tmp = *blk;
		count++;
		*(disk_blk + count) = tmp;
		if (ri == num - 1)
		{
			writeBlockToDisk((unsigned char *)disk_blk, basic + 150 + counta, buf);
			freeBlockInBuffer((unsigned char *)disk_blk, buf);
		}
		else
		{
			if (count == 15)
			{
				*(disk_blk + count + 1) = basic + 150 + counta + 1; //����һ�����
				writeBlockToDisk((unsigned char *)disk_blk, basic + 150 + counta, buf);
				freeBlockInBuffer((unsigned char *)disk_blk, buf);
				count = -1;
				counta++;
			}
		}
		printf("%d ", tmp);
		freeBlockInBuffer((unsigned char *)blk, buf);
	}
	printf("\n�Ѿ�д�뵽���� 350-390�Ŀ���\n");
END:    printf("\n\n");
}
/** \brief Nst_Loop_Join
*
* \param unsigned int * blk
* \param Buffer *buf
* \return void
*
*/

void Nst_Loop_Join(unsigned int * blk, Buffer *buf)
{
	int ri, rj, si, sj, num = 1000, blk_count = 0, count = 0;
	int a, b, c, d;
	unsigned int *desk_blk = NULL, *wr_blk = NULL;
	for (ri = 0; ri<16; ri++)
	{
		blk = (unsigned int *)readBlockFromDisk(ri, buf);  //R ÿ��block
		for (rj = 0; rj<7; rj++)
		{
			a = *(blk + 2 * rj);
			for (si = 0; si<32; si++)
			{
				desk_blk = (unsigned int *)readBlockFromDisk(si + 20, buf);//s ÿ��block
				for (sj = 0; sj<7; sj++)
				{
					c = *(desk_blk + 2 * sj);
					if (a == c)
					{
						b = *(blk + 2 * rj + 1);
						d = *(desk_blk + 2 * sj + 1);
						if (blk_count == 0)
						{
							wr_blk = (unsigned int *)getNewBlockInBuffer(buf);
						}
						*(wr_blk + blk_count) = a;
						*(wr_blk + blk_count + 1) = b;
						*(wr_blk + blk_count + 2) = d;
						count++;
						blk_count += 3;
						if (ri == 15 && si == 31)   //��������һ���飨��������Ҳ���д洢
						{
							writeBlockToDisk((unsigned char *)wr_blk, num, buf);
							freeBlockInBuffer((unsigned char *)wr_blk, buf);
						}
						else
						{
							if (blk_count == 15)   //������������
							{
								*(wr_blk + blk_count) = num + 1;  //����ĸ��ֽ� �洢��һ��Ŀ��
								writeBlockToDisk((unsigned char *)wr_blk, num, buf);
								num++;
								freeBlockInBuffer((unsigned char *)wr_blk, buf);
								blk_count = 0;
							}
						}
					}

				}
				freeBlockInBuffer((unsigned char *)desk_blk, buf);
			}
		}
		freeBlockInBuffer((unsigned char *)blk, buf);
	}
	OutputData(1000, num - 1000, blk, buf, 3);
	printf("\ncount = %d\n", count);
}
/** \brief Sort_Merge_Join
* Sort_Merge_Join
* \param unsigned int * blk
* \param Buffer *buf
* \param TempArray *tempR
* \param TempArray *tempS
* \return void
*
*/

void Sort_Merge_Join(unsigned int * blk, Buffer *buf, TempArray *tempR, TempArray *tempS)
{
	int i = 0, j = 0, count = 0, flag = 0, num = 4000, w_count = 0;
	blk = (unsigned int *)getNewBlockInBuffer(buf); //����
	while (i<112 && j<224)
	{
		if (tempR[i].c == tempS[j].c)
		{
			printf("(%d,%d,%d) ", tempR[i].c, tempR[i].d, tempS[j].d);
			*(blk + w_count) = tempR[i].c;
			*(blk + w_count + 1) = tempR[i].d;
			*(blk + w_count + 2) = tempS[j].d;
			w_count += 3;
			if (tempR[i - 1].c < tempR[i].c || i == 0) flag = i;
			count++;
			if (count % 6 == 0) printf("\n");
			if (i == 111) //��Ե���⴦�����Ҳ�����i���һ�������,j��������
			{
				i = flag;
				j++;
			}
			else i++;
		}
		else if (tempR[i].c < tempS[j].c) i++;
		else
		{
			j++;
			if (tempS[j].c == tempS[j - 1].c)  i = flag;
		}
		if (w_count == 15) //��һ��洢
		{
			*(blk + w_count) = num + 1;
			writeBlockToDisk((unsigned char *)blk, num, buf);
			freeBlockInBuffer((unsigned char *)blk, buf);
			num++;
			w_count = 0;
		}
	}
	if (w_count != 0 && w_count<15) //�����һ���ʱ��ֱ�Ӵ洢
	{
		writeBlockToDisk((unsigned char *)blk, num, buf);
		freeBlockInBuffer((unsigned char *)blk, buf);
	}
	printf("\ncount = %d \n", count);
}
/** \brief FillBucket
* ���Ͱ
* \param unsigned int *blk
* \param Buffer *buf
* \param int addr  ����ʼ��ַ
* \param int block_len ����Ŀ
* \return TempArray **
*
*/

TempArray ** FillBucket(unsigned int *blk, Buffer *buf, int addr, int block_len)
{
	int i, j, sel, p;
	//TempArray bucket[5][7*block_len];
	TempArray **bucket;
	bucket = (TempArray **)malloc(sizeof(TempArray*) * 5);
	for (i = 0; i<5; i++)
		bucket[i] = (TempArray *)malloc(sizeof(TempArray*)*block_len * 7);
	int bucketCon[5] = { 0 };
	for (i = 0; i<block_len; i++)
	{
		blk = (unsigned int *)readBlockFromDisk(addr + i, buf);
		for (j = 0; j<7; j++)
		{
			sel = (*(blk + 2 * j)) % 5;
			p = bucketCon[sel];
			bucket[sel][p].c = *(blk + 2 * j);
			bucket[sel][p].d = *(blk + 2 * j + 1);
			//printf("%d-%d,%d,%d \n",sel,p, bucket[sel][p].c, bucket[sel][p].d);
			bucketCon[sel]++;
		}
		freeBlockInBuffer((unsigned char *)blk, buf);
	}
	for (i = 0; i<5; i++)
	{
		if (addr == 0) //R
		{
			bucketR[i] = bucketCon[i];
		}
		else
		{
			bucketS[i] = bucketCon[i];
		}
	}
	return bucket;
}
/** \brief Hash_Join
* Hash_Join
* \param unsigned int *blk
* \param Buffer *buf
* \param TempArray *tempR RԪ��
* \param TempArray *tempS SԪ��
* \param int addr hash_join����洢����ʼ��ַ
* \return void
*
*/

void Hash_Join(unsigned int *blk, Buffer *buf, TempArray *tempR, TempArray *tempS, int addr)
{
	BucketR = FillBucket(blk, buf, 0, 16);    //RͰ
	BucketS = FillBucket(blk, buf, 20, 32);    //SͰ
	int i, rj = 0, sj = 0;
	int numR, numS, w_count = 0, count = 0;
	blk = (unsigned int *)getNewBlockInBuffer(buf);
	for (i = 0; i<5; i++)
	{
		numR = bucketR[i];
		numS = bucketS[i];
		for (rj = 0; rj<numR; rj++)
		{
			for (sj = 0; sj<numS; sj++)
			{
				if (BucketR[i][rj].c == BucketS[i][sj].c)
				{
					printf("(%d, %d, %d) ", BucketR[i][rj].c, BucketR[i][rj].d, BucketS[i][sj].d);
					*(blk + w_count) = BucketR[i][rj].c;
					*(blk + w_count + 1) = BucketR[i][rj].d;
					*(blk + w_count + 2) = BucketS[i][sj].d;
					w_count += 3;
					count++;
				}
				if (w_count == 15)
				{
					printf("\n");
					*(blk + w_count) = addr + 1;
					writeBlockToDisk((unsigned char *)blk, addr, buf);
					addr++;
					freeBlockInBuffer((unsigned char *)blk, buf);
					w_count = 0;
				}
				if (rj == numR - 1 && sj == numS - 1 && w_count<15)
				{
					*(blk + w_count) = 0;
					writeBlockToDisk((unsigned char *)blk, addr, buf);
					freeBlockInBuffer((unsigned char *)blk, buf);
				}
			}
		}
	}
	printf("\n count =%d \n", count);
}
/** \brief ���ӹ�ϵ
* Nst_Loop_Join�㷨 Sort_Merge_Join�㷨 Hash_Join�㷨
*�����Ŵ���
* \param unsigned int * blk
* \param Buffer *buf
* \return void
*
*/
void ConnectRelationship(unsigned int * blk, Buffer *buf, TempArray *tempR, TempArray *tempS)
{
	printf("\t1 Nst_Loop_Join \t2 Sort_Merge_Join \t3 Hash_Join \t��������\n");
	int sel = 0;
	while (sel != -1)
	{
		scanf("%d", &sel);
		switch (sel)
		{
		case 1: //Nst_Loop_Join�㷨
		{
			Nst_Loop_Join(blk, buf);
			continue;
		}
		case 2: //Sort_Merge_Join�㷨
		{
			Sort_Merge_Join(blk, buf, tempR, tempS);
			continue;
		}
		case 3: //Hash_Join�㷨
		{
			Hash_Join(blk, buf, tempR, tempS, 6000);
			continue;
		}
		default:
			break;
		}
	}
}
/** \brief CalculationIntersection
* ���ϵĽ���
* \param unsigned int *blk
* \param Buffer *buf
* \return TempArray *temp
*
*/

void CalculationIntersection(unsigned int *blk, Buffer *buf, TempArray *temp)
{
	int ri, rj, si, sj;
	int a, b, c, d;
	unsigned int *desk_blk;
	for (ri = 0; ri<16; ri++)
	{
		blk = (unsigned int *)readBlockFromDisk(ri, buf);
		for (rj = 0; rj<7; rj++)
		{
			a = *(blk + 2 * rj);
			b = *(blk + 2 * rj + 1);
			for (si = 0; si<32; si++)
			{
				desk_blk = (unsigned int *)readBlockFromDisk(si + 20, buf);
				for (sj = 0; sj<7; sj++)
				{
					c = *(desk_blk + 2 * sj);
					d = *(desk_blk + 2 * sj + 1);
					if (a == c && b == d) //�ظ�Ԫ��
					{
						temp[temp_count].c = c;
						temp[temp_count].d = d;
						temp_count++;
					}
				}
				freeBlockInBuffer((unsigned char *)desk_blk, buf);
			}

		}
		freeBlockInBuffer((unsigned char *)blk, buf);
	}

}
/** \brief  UnionOfRelation
* R S�Ĳ���ȥ��
* \param unsigned int * blk
* \param Buffer *buf
* \return void
*
*/
void UnionOfRelation(unsigned int * blk, Buffer *buf, TempArray *temp)
{
	int ri, rj, si, sj, num = 1500, blk_count = 0, i, snum = 1516, s_count = 0;
	int a, b, c, d, flag = 0, num_count = 16;
	unsigned int *desk_blk, *wrA_blk, *wr_blk, *s_blk;
	for (si = 0; si<32; si++)
	{
		desk_blk = (unsigned int *)readBlockFromDisk(si + 20, buf);
		wr_blk = (unsigned int *)getNewBlockInBuffer(buf);
		for (sj = 0; sj<7; sj++)
		{
			tempflag = 0;
			c = *(desk_blk + 2 * sj);
			d = *(desk_blk + 2 * sj + 1);
			for (i = 0; i<temp_count; i++)
			{
				if (c == temp[i].c && d == temp[i].d)
				{
					tempflag = 1;
					break;
				}
			}
			if (tempflag == 0) //�����ظ��б���,�Ŵ�����
			{
				*(wr_blk + 2 * s_count) = c;
				*(wr_blk + 2 * s_count + 1) = d;
				s_count++;
			}
			if (s_count == 7)
			{
				*(wr_blk + 2 * s_count + 1) = snum + 1;
				writeBlockToDisk((unsigned char *)wr_blk, snum, buf);
				freeBlockInBuffer((unsigned char *)wr_blk, buf);
				s_count = 0;
				snum++;

			}
		}
		if (si == 31 && sj == 6) //���ģ�����ҲҪ�洢
		{
			writeBlockToDisk((unsigned char *)wr_blk, snum, buf);
			freeBlockInBuffer((unsigned char *)wr_blk, buf);
		}
		freeBlockInBuffer((unsigned char *)desk_blk, buf);
	}
	if (temp_count == 0)
	{
		printf("���ϲ���û���ظ�\n");
	}
	printf("���ϲ�����֮��ȥ�ع���,�ظ�����%d\n", temp_count);
	OutputData(0, 16, blk, buf, 2);
	OutputData(1516, snum - 1516, blk, buf, 2);
}

/** \brief IntersectionOfRelation
* R S�Ľ�
* \param unsigned int *blk
* \param Buffer *buf
* \return void
*
*/

void IntersectionOfRelation(unsigned int *blk, Buffer *buf, TempArray *temp)
{
	int i, j, itemp = 0, num = 2000;
	if (temp_count != 0)
	{
		printf("R �� S�Ľ���\n");
		for (i = 0; i<temp_count; i++)
		{
			blk = (unsigned int *)getNewBlockInBuffer(buf);
			for (j = 0; j<7 && itemp<temp_count; j++)
			{
				*(blk + 2 * j) = temp[itemp].c;
				*(blk + 2 * j + 1) = temp[itemp].d;
				printf("(%d, %d) ", temp[itemp].c, temp[itemp].d);
				itemp++;
			}
			printf("\n");
			*(blk + 2 * j + 1) = num + 1;
			writeBlockToDisk((unsigned char *)blk, num, buf);
			num++;
			freeBlockInBuffer((unsigned char *)blk, buf);
		}
		//OutputData(2000,num-2000,blk,buf,2);
	}
	else
	{
		printf("R S�Ľ�Ϊ��\n");
	}

}
/** \brief DifferenceOfRelation
*  R��S�Ĳ
* \param unsigned int *blk
* \param Buffer *buf
* \param TempArray *temp
* \return void
*
*/
void DifferenceOfRelation(unsigned int *blk, Buffer *buf, TempArray *temp)
{
	printf("\t 1 R-S  \t 2 S-R\n");
	int sel, ri, rj, si, sj, a, b, c, d, i, flag = 0, num = 0, blk_num = 2500, blk_num1 = 3000;
	unsigned int *wr_blk;
	scanf("%d", &sel);
	if (sel == 1)
	{
		printf("R-S�Ĳ\n");
		for (ri = 0; ri<16; ri++)
		{
			blk = (unsigned int *)readBlockFromDisk(ri, buf);
			wr_blk = (unsigned int *)getNewBlockInBuffer(buf);
			for (rj = 0; rj<7; rj++)
			{
				flag = 0;
				a = *(blk + 2 * rj);
				b = *(blk + 2 * rj + 1);
				for (i = 0; i<temp_count; i++)
				{
					if (a == temp[i].c && b == temp[i].d)
					{
						flag = 1;
						break;
					}
				}
				if (flag == 0)
				{
					*(wr_blk + 2 * num) = a;
					*(wr_blk + 2 * num + 1) = b;
					printf("(%d, %d) ", a, b);
					num++;
				}
				if (ri == 15 && rj == 6)  //���һ���鲻����ʱ�򣬴洢
				{
					printf("\n");
					*(wr_blk + 2 * num + 1) = 0;
					writeBlockToDisk((unsigned char *)wr_blk, blk_num, buf);
					freeBlockInBuffer((unsigned char *)wr_blk, buf);
				}
			}
			printf("\n");
			if (num == 7)
			{
				*(wr_blk + 2 * num + 1) = blk_num + 1;
				writeBlockToDisk((unsigned char *)wr_blk, blk_num, buf);
				blk_num++;
				num = 0;
				freeBlockInBuffer((unsigned char *)wr_blk, buf);
			}
			freeBlockInBuffer((unsigned char *)blk, buf);
		}
		//OutputData(2500,blk_num-2500,blk,buf,2);
	}
	else if (sel == 2)
	{
		printf("S-R�Ĳ\n");
		for (si = 0; si<32; si++)
		{
			blk = (unsigned int *)readBlockFromDisk(si + 20, buf);
			wr_blk = (unsigned int *)getNewBlockInBuffer(buf);
			for (sj = 0; sj<7; sj++)
			{
				flag = 0;
				c = *(blk + 2 * sj);
				d = *(blk + 2 * sj + 1);
				for (i = 0; i<temp_count; i++)
				{
					if (c == temp[i].c && d == temp[i].d)
					{
						flag = 1;
						break;
					}
				}
				if (flag == 0)
				{
					*(wr_blk + 2 * num) = c;
					*(wr_blk + 2 * num + 1) = d;
					printf("(%d, %d) ", c, d);
					num++;
				}
				if (si == 31 && sj == 6)  //���һ���鲻����ʱ�򣬴洢
				{
					printf("\n");
					*(wr_blk + 2 * num + 1) = 0;
					writeBlockToDisk((unsigned char *)wr_blk, blk_num1, buf);
					freeBlockInBuffer((unsigned char *)wr_blk, buf);
				}
			}
			printf("\n");
			if (num == 7)
			{
				*(wr_blk + 2 * num + 1) = blk_num1 + 1;
				writeBlockToDisk((unsigned char *)wr_blk, blk_num1, buf);
				blk_num1++;
				num = 0;
				freeBlockInBuffer((unsigned char *)wr_blk, buf);
			}
			freeBlockInBuffer((unsigned char *)blk, buf);
		}
	}
	else
	{
		printf("�������\n");
	}
}
/** \brief ���ϲ���
* ����������
* \param unsigned int * blk
* \param Buffer *buf
* \return void
*
*/
void CollectionRelationship(unsigned int * blk, Buffer *buf)
{
	printf("\t 1 �� \t 2 �� \t 3 �� \t ��������\n");
	struct TempArray *temp = (struct TempArray *)malloc(sizeof(TempArray) * 32);
	int sel = 0;
	CalculationIntersection(blk, buf, temp);
	while (sel != -1)
	{
		scanf("%d", &sel);
		switch (sel)
		{
		case 1: //��  ȥ��
		{
			UnionOfRelation(blk, buf, temp);
			break;
		}
		case 2: //��
		{
			IntersectionOfRelation(blk, buf, temp);
			break;;
		}
		case 3://��
		{
			DifferenceOfRelation(blk, buf, temp);
			break;;
		}
		default:
			break;
		}
	}
}
