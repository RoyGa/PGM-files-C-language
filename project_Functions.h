#ifndef __project_Functions_H
#define __project_Functions_H

#define _CRT_SECURE_NO_WARNINGS
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

//                                        ROY GAZIT  &  GAL MAZOR

typedef int BOOL;
#define FALSE 0
#define TRUE 1

#define GRAY_SCALE_RANGE 255

// main:
#define option_1 1
#define option_2 2
#define option_3 3
#define option_4 4
#define option_5 5
#define option_6 6
#define option_7 7


#define END_OF_BYTE -1
#define PRINT_BYTE -1

#define maxFileName 100

#define COMPRESS 422
#define UNCOMPRESS 421




typedef unsigned char BYTE;

typedef struct _grayImage {
	unsigned short rows, cols;
	unsigned char **pixels;
}grayImage;

typedef unsigned short imgPos[2];

//forward decleration:
typedef struct _treeNodeListCell;

typedef struct _treeNode {
	imgPos position;
	struct _treeNodeListCell *next_possible_positions;
}treeNode;

typedef struct _treeNodeListCell {
	treeNode *node;
	struct _treeNodeListCell *next;

}treeNodeListCell;

typedef struct _segment {
	treeNode *root;
}Segment;


typedef struct _imgPosCell {
	imgPos position;
	struct _imgPosCell *next, *prev;
}imgPosCell;

typedef struct _imgPosList {
	imgPosCell *head, *tail;
}imgPosList;


typedef struct _range {
	unsigned char lowerRange;
	unsigned char higherRange;
}RANGE;
//~~~~~~~~~~~~~~~~~Q01~~~~~~~~~~~~~~~~~~~~~

grayImage *readPGM(char *fname);

//~~~~~~~~~~~~~~~~~Q02~~~~~~~~~~~~~~~~~~~~~

Segment findSingleSegment(grayImage *img, imgPos start, unsigned char threshold);
RANGE gettingRange(BYTE startPoint, BYTE threshold);

treeNode *findSingleSegmentHelper(grayImage *grayImg, unsigned short currRow, unsigned short currCol, BOOL** mat, RANGE range);
treeNodeListCell* findNeighborsInRange(grayImage *img, unsigned short currRow, unsigned short currCol, BOOL** mat, RANGE range, treeNodeListCell *currList);
treeNode* createNewTreeNode(unsigned short currRow, unsigned short currCol, treeNodeListCell *curr);
treeNodeListCell* createNewTreeNodeListCell(unsigned short currRow, unsigned short currCol, treeNodeListCell *next);

//~~~~~~~~~~~~~~~~~Q03~~~~~~~~~~~~~~~~~~~~~
int findAllSegments(grayImage *img, unsigned char threshold, imgPosList **segments);
void createNewList(imgPosList *lst, treeNode *root, BOOL **mat);
void makeEmptyList(imgPosList * lst);
void insertDataToEndList(imgPosList * lst, imgPos curr);
imgPosCell* createNewimgPosCell(imgPos curr, imgPosCell* next, imgPosCell* prev);
void insertImgPosCellToEndList(imgPosList * lst, imgPosCell* newTail);
BOOL isEmptyList(imgPosList *lst);

//~~~~~~~~~~~~~~~~~Q04~~~~~~~~~~~~~~~~~~~~~

grayImage *colorSegments(imgPosList *segments, unsigned int size);
grayImage *createGrayImage(imgPosList *segments, unsigned int size);

//~~~~~~~~~~~~~~~~~Q05~~~~~~~~~~~~~~~~~~~~~
void saveCompressed(char* file_name, grayImage *image, unsigned char max_gray_level);
void compressBYTE(BYTE cuurGray, BYTE *toPrint, int *currGrayLoc, int *toPrintLocBit, FILE *fp, BOOL last);
//~~~~~~~~~~~~~~~~~Q06~~~~~~~~~~~~~~~~~~~~~
void convertCompressedImageToPGM(char * compressed_file_name, char *pgm_file_name);
void unCompressBYTE(BYTE cuurGray, BYTE *toPrint, int *toPrintLocBit, FILE *fp, int maxBitsForGray, int maxGray, int *colIndex, int cols, int *newMaxGray, int lastPix);

//~~~~~~~~~~~~~~~~~GENERAL~~~~~~~~~~~~~~~~~~~~~

void printMenu();
int NumOfDigits(int num);
void checkMemoryAllocation(void *ptr);
void FileCheckMemoryAllocation(void *ptr);
BOOL PGMFileCheckMemoryAllocation(void *ptr);
long int fileSize(FILE *fp);
BOOL **createNewMatrix(unsigned short rows, unsigned short cols);
void freeMatrix(BOOL **mat, unsigned short rows);
int countBitsForPixel(int maxGrayValue);
int updateGrayScale(int current, int maxGray, BOOL WHATTODO);
BYTE setBitI(BYTE ch, int i);
BYTE isBitISet(BYTE ch, int i);

//~~~~~~~~~~~~~~~~  FREE allocation  ~~~~~~~~~~~~~
void freeGrayImage(grayImage *image);
void freeSegmentsList(imgPosList *segments);
void freeSegment(treeNode* curr);



#endif