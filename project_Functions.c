//                                        ROY GAZIT  &  GAL MAZOR

#include "project_Functions.h"

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~Q01~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
grayImage *readPGM(char *fname)
{
	FILE *fp;
	grayImage *imagePGM;// returned value
	unsigned short rows, cols;
	int maxGrayScale;// aid varaible
	char isPGM[3];
	int i, j;
	BOOL filedOpen;
	// allocating memory for returned imagePGM
	imagePGM = (grayImage *)malloc(sizeof(grayImage)); 
	checkMemoryAllocation(imagePGM);
	// open given file
	fp = fopen(fname, "r");
	filedOpen=PGMFileCheckMemoryAllocation(fp);
	if (filedOpen == FALSE)
	{
		imagePGM = NULL;
		return (imagePGM);
	}
	// getting PGM filename
	fscanf(fp, "%s", isPGM);
	// getting matrix size
	fscanf(fp, "%hu", &cols);
	fscanf(fp, "%hu", &rows);
	// getting max gray scale number
	fscanf(fp, "%d", &maxGrayScale);
	// assigning rows and columns
	imagePGM->rows = rows;
	imagePGM->cols = cols;
	// creating matrix
	imagePGM->pixels = (unsigned char**)malloc(rows * sizeof(unsigned char*)); // allocation for the rows
	checkMemoryAllocation(imagePGM->pixels);
	// copying data from file to matrix
	for (i = 0; i < rows; i++)
	{
		// allocating memory
		imagePGM->pixels[i] = (unsigned char*)malloc((cols + 1) * sizeof(unsigned char));//added +1 for the '\0'
		checkMemoryAllocation(imagePGM->pixels[i]);
		// getting data
		for (j = 0; j < cols; j++)
			fscanf(fp, "%hhu", &imagePGM->pixels[i][j]);
		imagePGM->pixels[i][cols] = '\0';
	}
	fseek(fp, 0, SEEK_SET);
	fclose(fp);
	return (imagePGM);
}

//
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~Q02~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

Segment findSingleSegment(grayImage *img, imgPos start, unsigned char threshold)
{
	BOOL** mat;
	unsigned short rows, cols;
	unsigned short recRow, recCol;
	RANGE range; // hold the higher and lower range
	Segment resSeg;

	// getting range
	range = gettingRange(img->pixels[start[1]][start[0]], threshold);

	// getting number of row and columns
	rows = img->rows;
	cols = img->cols;
	// allocating memory for matrix and input FALSE to cells
	mat = createNewMatrix(rows, cols);
	// getting starting position
	recCol = start[0];
	recRow = start[1];

	resSeg.root = findSingleSegmentHelper(img, recRow, recCol, mat, range);
	freeMatrix(mat, rows);
	return (resSeg); 
}

// this function recieve the value of start point and the range to calculate and return the bottom and top range
RANGE gettingRange(BYTE startPoint, BYTE threshold)
{
	RANGE res;
	int startP = (int)startPoint;
	int plusMinus = (int)threshold;
	if ((startP - plusMinus) < 0) // startP<plusMinus so the left range is 0
		res.lowerRange = 0;
	else
		res.lowerRange = (startPoint - threshold);
	if ((startP + plusMinus) > 255) // unsigned char can hold only 0-255, if we pass 255 it is go back to count from 0
		res.higherRange = 255;
	else
		res.higherRange = (startPoint + threshold);
	return (res);
}



//
treeNode *findSingleSegmentHelper(grayImage *grayImg, unsigned short currRow, unsigned short currCol, BOOL** mat, RANGE range)
{
	treeNodeListCell *temp;
	unsigned short nextRow, nextCol;
	treeNode *root;
	mat[currRow][currCol] = TRUE;
	root = createNewTreeNode(currRow, currCol, NULL);
	root->next_possible_positions = findNeighborsInRange(grayImg, currRow, currCol, mat, range, root->next_possible_positions);
	temp = root->next_possible_positions;
	while (temp != NULL)
	{
		nextRow = temp->node->position[1];
		nextCol = temp->node->position[0];
		temp->node = findSingleSegmentHelper(grayImg, nextRow, nextCol, mat, range);
		temp = temp->next;
	}
	return (root);
}


treeNodeListCell* findNeighborsInRange(grayImage *img, unsigned short currRow, unsigned short currCol, BOOL** mat, RANGE range, treeNodeListCell *currList)
{
	treeNodeListCell *nextList = NULL;
	int i, j;
	unsigned short lastRow, lastCol;
	lastRow = img->rows;
	lastCol = img->cols;

	for (i = (currRow - 1); i <= (currRow + 1); i++)
	{
		if ((i >= 0) && (i < lastRow)) // check if we are in the matrix territory
		{
			for (j = (currCol - 1); j <= (currCol + 1); j++)
			{
				if ((j >= 0) && (j < lastCol)) // check if we are in the matrix territory
				{
					if ((mat[i][j] == FALSE) && (img->pixels[i][j] >= range.lowerRange) && (img->pixels[i][j] <= range.higherRange))
					{
						mat[i][j] = TRUE;
						if (currList == NULL)// the list is empty
						{
							currList = createNewTreeNodeListCell(i, j, NULL);
							nextList = currList;
						}
						else
						{
							nextList->next = createNewTreeNodeListCell(i, j, NULL);
							nextList = nextList->next;
						}
					}
				}
			}
		}
	}
	return(currList);
}

treeNode* createNewTreeNode(unsigned short currRow, unsigned short currCol, treeNodeListCell *curr)
{
	treeNode *resNew;
	resNew = (treeNode *)malloc(sizeof(treeNode));
	checkMemoryAllocation(resNew);

	resNew->position[0] = currCol;
	resNew->position[1] = currRow;
	resNew->next_possible_positions = curr;
	return (resNew);
}


treeNodeListCell* createNewTreeNodeListCell(unsigned short currRow, unsigned short currCol, treeNodeListCell *next)
{
	treeNodeListCell *resNew;
	resNew = (treeNodeListCell *)malloc(sizeof(treeNodeListCell));
	checkMemoryAllocation(resNew);
	resNew->node = createNewTreeNode(currRow, currCol, NULL);
	resNew->next = next;
	return (resNew);
}




//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~Q03~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

int findAllSegments(grayImage *img, unsigned char threshold, imgPosList **segments)
{
	imgPosList *res;
	int sizeRes = 0;
	int logSizePosList = 1;
	Segment currSeg;
	imgPos currStart;
	BOOL** mat;
	unsigned short rows, cols;
	unsigned short i, j;
	res = (imgPosList *)malloc(logSizePosList * sizeof(imgPosList));
	checkMemoryAllocation(res);

	// getting number of row and columns
	rows = img->rows;
	cols = img->cols;
	// allocating memory for matrix and input FALSE to cells
	mat = createNewMatrix(rows, cols);

	for (i = 0; i < rows; i++)
	{
		for (j = 0; j < cols; j++)
		{
			if (mat[i][j] == FALSE)
			{
				currStart[0] = j;
				currStart[1] = i;
				currSeg = findSingleSegment(img, currStart, threshold);
				makeEmptyList(&res[sizeRes]);
				createNewList(&res[sizeRes], currSeg.root, mat);
				sizeRes++;
				if (logSizePosList == sizeRes)
				{
					logSizePosList = logSizePosList * 2;
					res = (imgPosList *)realloc(res, logSizePosList * sizeof(imgPosList));
					checkMemoryAllocation(res);
				}
				freeSegment(currSeg.root);
			}
		}
	}
	res = (imgPosList *)realloc(res, sizeRes * sizeof(imgPosList));
	checkMemoryAllocation(res);
	freeMatrix(mat, rows);
	*segments = res;
	return (sizeRes);
}


void createNewList(imgPosList *lst, treeNode *root, BOOL **mat)
{

	imgPos currPos;
	treeNodeListCell *temp;
	currPos[0] = root->position[0];
	currPos[1] = root->position[1];
	temp = root->next_possible_positions;
	if (mat[currPos[1]][currPos[0]] != TRUE)
	{
		insertDataToEndList(lst, currPos);
		mat[root->position[1]][root->position[0]] = TRUE;
		while (temp != NULL)
		{
			createNewList(lst, temp->node, mat);
			temp = temp->next;
		}
	}
	return;
}


void makeEmptyList(imgPosList * lst)
{
	lst->head = NULL;
	lst->tail = NULL;
}

void insertDataToEndList(imgPosList * lst, imgPos curr)
{
	imgPosCell* newTail;
	newTail = createNewimgPosCell(curr, NULL, lst->tail);
	insertImgPosCellToEndList(lst, newTail);
}

imgPosCell* createNewimgPosCell(imgPos curr, imgPosCell* next, imgPosCell* prev)
{
	imgPosCell* res;
	res = (imgPosCell*)malloc(sizeof(imgPosCell));
	checkMemoryAllocation(res);

	res->position[0] = curr[0];
	res->position[1] = curr[1];
	res->next = next;
	res->prev = prev;
	return res;
}


void insertImgPosCellToEndList(imgPosList * lst, imgPosCell* newTail)
{
	if (isEmptyList(lst) == TRUE)
	{
		newTail->next = NULL;
		newTail->prev = NULL;
		lst->head = newTail;
		lst->tail = newTail;
	}
	else
	{
		newTail->next = NULL;
		newTail->prev = lst->tail;
		lst->tail->next = newTail;
		lst->tail = newTail;
	}
}

BOOL isEmptyList(imgPosList *lst)
{
	if (lst->head == NULL)
		return TRUE;
	else
		return FALSE;
}


//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~Q04~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

grayImage *colorSegments(imgPosList *segments, unsigned int size)
{
	grayImage *res;
	unsigned int i;
	int currGrayColor;
	imgPosCell *currCell;

	res = createGrayImage(segments, size);
	// coloring the matrix according to segments
	for (i = 0; i < size; i++)
	{
		currCell = segments[i].head;
		if (size == 1)
			currGrayColor = 0;
		else
			currGrayColor = (i*(GRAY_SCALE_RANGE / (size - 1)));
		while (currCell != NULL)
		{
			res->pixels[currCell->position[1]][currCell->position[0]] = (BYTE)currGrayColor;
			currCell = currCell->next;
		}
	}
	return (res);
}

grayImage *createGrayImage(imgPosList *segments, unsigned int size)
{
	grayImage *res;
	unsigned int i;
	imgPosCell *currCell;
	unsigned int maxRow = 0, maxCol = 0;
	res = (grayImage *)malloc(sizeof(grayImage));
	checkMemoryAllocation(res);
	// finding the maximum columns and rows for the pixels matrix
	for (i = 0; i < size; i++)
	{
		currCell = segments[i].head;
		while (currCell != NULL)
		{
			if ((currCell->position[0]) > maxCol)
				maxCol = currCell->position[0];
			if ((currCell->position[1]) > maxRow)
				maxRow = currCell->position[1];
			currCell = currCell->next;
		}
	}
	maxRow = (maxRow + 1);
	maxCol = (maxCol + 1);
	res->rows = maxRow;
	res->cols = maxCol;
	// creating matrix
	res->pixels = (unsigned char**)malloc(maxRow * sizeof(unsigned char*)); // allocation for the rows
	checkMemoryAllocation(res->pixels);
	for (i = 0; i < maxRow; i++)
	{		// allocating memory
		res->pixels[i] = (unsigned char*)malloc((maxCol + 1) * sizeof(unsigned char));//added +1 for the '\0'
		checkMemoryAllocation(res->pixels[i]);
	}
	return (res);
}



//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~Q05~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
void saveCompressed(char* file_name, grayImage *image, unsigned char max_gray_level)
{
	FILE *fp;
	unsigned short rows, cols; // getting from image rows and columns
	int new_max = (int)max_gray_level; // new max_gray_scale, range->[0,127]
	int maxBitsForGray; // hold maximum number of bits necessery to present number in the new range
	BYTE grayScale = max_gray_level;
	int i, j;
	int currentGray; // hold each time the number from the matrix
	BYTE cuurGray;
	BYTE toPrint = 0;
	BOOL compress = COMPRESS;
	int currGrayLoc, toPrintLocBit = 7;
	BOOL lastPix = FALSE;
	// update rows and columns
	rows = image->rows;
	cols = image->cols;

	// creating a new binary file to write
	fp = fopen(file_name, "wb");
	checkMemoryAllocation(fp);
	// write to the file rows, columns and max gray scale
	fwrite(&image->cols, sizeof(unsigned short), 1, fp);
	fwrite(&image->rows, sizeof(unsigned short), 1, fp);
	fwrite(&grayScale, sizeof(BYTE), 1, fp);

	//calculate maximum bits for max_gray_level (between 1-7)
	maxBitsForGray = (countBitsForPixel(new_max) - 1);

	for (i = 0; i < rows; i++)
	{
		for (j = 0; j < cols; j++)
		{
			currentGray = (int)image->pixels[i][j]; // getting next pixel
			currentGray = updateGrayScale(currentGray, new_max, compress); // update currentGray according new_max
			cuurGray = (BYTE)currentGray;
			if ((i == (rows - 1)) && (j == (cols - 1)))
				lastPix = TRUE;
			currGrayLoc = maxBitsForGray; // update starting bit in BYTE to copy from 
			compressBYTE(cuurGray, &toPrint, &currGrayLoc, &toPrintLocBit, fp, lastPix);
		}
	}
	fclose(fp);
}

//this function compress as much as she can the BYTES and every time thre is a full BYTE to print we write it
// into the binary file
void compressBYTE(BYTE cuurGray, BYTE *toPrint, int *currGrayLoc, int *toPrintLocBit, FILE *fp, BOOL last)
{
	BYTE temp = 0; // use to check if the bit is on
	int bitsToMove; // calculate how many bits we need to move if we copy to new empty BYTE
	while ((*currGrayLoc) != END_OF_BYTE)
	{
		if ((*toPrintLocBit) == 7) // toPrint=0, empty BYTE ,we can copy the entire BYTE from cuurGray
		{
			bitsToMove = (*toPrintLocBit - *currGrayLoc);
			(*toPrint) = (*toPrint | cuurGray) << bitsToMove;
			(*toPrintLocBit) = (bitsToMove - 1);
			(*currGrayLoc) = END_OF_BYTE; // we copy the entire BYTE 
			if (last == TRUE)
				(*toPrintLocBit) = (*currGrayLoc);
		}

		else // toPrint is not empty
		{
			temp = isBitISet(cuurGray, *currGrayLoc); // check if the bit in currGrayLoc is '1' or '0'
			if (temp > 0)
				(*toPrint) = setBitI(*toPrint, *toPrintLocBit); // we set bit I in BYTE toPrint
			// else-> no need to copy because the value of the bit is 0 so we just skip to the next bit
			(*currGrayLoc)--;
			(*toPrintLocBit)--;
		}
		if ((*toPrintLocBit) == PRINT_BYTE) // we have a full BUTE to print
		{
			fwrite(toPrint, sizeof(BYTE), 1, fp);
			(*toPrint) = 0;
			(*toPrintLocBit) = 7;
		}
	}
}

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~Q06~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

void convertCompressedImageToPGM(char * compressed_file_name, char *pgm_file_name)
{
	FILE *fpBIN, *fpPGM;
	unsigned short rows, cols; // getting from image rows and columns
	int maxBitsForGray; // hold maximum number of bits necessery to present number in the new range
	BYTE maxGrayScale, currByte;
	BYTE toPrint = 0;
	int maxGray, newMaxGray = 0;
	long int pixelsStart, endPixelsBINFile, locToPrintNewMaxGray;
	int toPrintIndex; // hold each time the number from the matrix
	int pixelsCounter = 0; // counter for each pixel we find
	int totalPixels; // hold the number of pixels in the PGM

	// opening a new binary file to read
	fpBIN = fopen(compressed_file_name, "rb");
	FileCheckMemoryAllocation(fpBIN);
	fseek(fpBIN, 0, SEEK_SET);

	// opening a new pgm file to write PGM
	fpPGM = fopen(pgm_file_name, "w+");
	checkMemoryAllocation(fpPGM);
	fseek(fpPGM, 0, SEEK_SET);

	// getting from the file rows, columns and max_Gray_Scale
	fread(&cols, sizeof(unsigned short), 1, fpBIN);
	fread(&rows, sizeof(unsigned short), 1, fpBIN);
	fread(&maxGrayScale, sizeof(BYTE), 1, fpBIN);
	pixelsStart = ftell(fpBIN);
	totalPixels = cols*rows; // calculate pixels in the picture
							 // writing to PGM file first 3 lines
	fprintf(fpPGM, "%s\n", "P2");
	fprintf(fpPGM, "%hu %hu\n", cols, rows);
	locToPrintNewMaxGray = ftell(fpPGM); // save the location to back and print the max gray scale
	fprintf(fpPGM, "     \n"); // print 5 spaces to the file so we will have where to write the max gray scale number
							   //calculate size of binary file
	fseek(fpBIN, 0, SEEK_END);
	endPixelsBINFile = ftell(fpBIN);
	fseek(fpBIN, pixelsStart, SEEK_SET); // return to first pixel

	maxGray = (int)maxGrayScale;
	//calculate maximum bits for max_gray_level (between 1-7)
	maxBitsForGray = countBitsForPixel((int)maxGrayScale);
	toPrintIndex = (maxBitsForGray - 1);
	while (ftell(fpBIN) != endPixelsBINFile)
	{
		fread(&currByte, sizeof(BYTE), 1, fpBIN);
		unCompressBYTE(currByte, &toPrint, &toPrintIndex, fpPGM, maxBitsForGray, maxGray, &pixelsCounter, cols, &newMaxGray, totalPixels);
	}
	// return to newMaxGray location and print the maxGrayScale
	fseek(fpPGM, locToPrintNewMaxGray, SEEK_SET);
	fprintf(fpPGM, "%d", newMaxGray);
	fclose(fpBIN);
	fclose(fpPGM);
}

void unCompressBYTE(BYTE cuurGray, BYTE *toPrint, int *toPrintIndex, FILE *fp, int maxBitsForGray, int maxGray, int *pixelsCounter, int cols, int *newMaxGray, int lastPix)
{
	BYTE temp;
	int currByteIndex = 7;
	int numToPrint;
	BOOL uncompress = UNCOMPRESS;
	BOOL once = FALSE;
	while (currByteIndex != END_OF_BYTE)
	{
		temp = 0;
		temp = isBitISet(cuurGray, currByteIndex); // check if the bit in currGrayLoc is '1' or '0'
		if (temp > 0)
			(*toPrint) = setBitI(*toPrint, *toPrintIndex); // we set bit I in BYTE toPrint
		(*toPrintIndex)--;
		currByteIndex--; // next bit to check
		if ((*toPrintIndex) == PRINT_BYTE) // case we have a full BYTE to print
		{
			numToPrint = (int)(*toPrint); // getting the number from BYTE to int
			numToPrint = updateGrayScale(numToPrint, maxGray, uncompress); // return the compressed number to original
			fprintf(fp, "%d ", numToPrint); // write number to text file

											//updating new max gray if neccesery
			if ((*newMaxGray) < numToPrint)
				(*newMaxGray) = numToPrint;
			(*pixelsCounter)++; // count to next pixel
			if (((*pixelsCounter) == (lastPix - 1)) && (currByteIndex>(maxBitsForGray - 1))) // case we are in the last pixel
				currByteIndex = (maxBitsForGray - 1);
			if (((*pixelsCounter) % cols) == 0) // end of line print ENTER
				fprintf(fp, "\n");
			(*toPrint) = 0;
			(*toPrintIndex) = (maxBitsForGray - 1);
		}
	}
}

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~  GENERAL functions  ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

// print menu function
void printMenu()
{
	// printing menu options:
	printf("\nMENU:\n");
	printf("1. Read an image file in PGM format\n");
	printf("2. Find all segments\n");
	printf("3. Color the segments\n");
	printf("4. Save the colored image in a compressed format\n");
	printf("5. Compress and save the original image in a compressed format\n");
	printf("6. Convert a compressed image to PGM format\n");
	printf("7. Exit\n");
	printf("Enter your selection: ");
}

// this function check if the allocation succeeded
void checkMemoryAllocation(void *ptr)
{
	if (ptr == NULL)
	{
		printf("Memory allocation failed\n");
		exit(1);
	}
}

// this function check if the allocation succeeded
void FileCheckMemoryAllocation(void *ptr)
{
	char ch;
	if (ptr == NULL)
	{
		printf("\nFile memory allocation failed:\n");
		printf("The binary file you entered doesn't exist.\n");
		printf("Press ENTER for the menu.\n");
		ch = getchar(); // for the first ENTER from the last input
		ch = getchar(); // the ENTER to the menu
		main();
	}	
}

// this function check if the allocation succeeded
BOOL PGMFileCheckMemoryAllocation(void *ptr)
{
	BOOL isOpen=TRUE;
	if (ptr == NULL)
		isOpen = FALSE;
	return (isOpen);
}


// this function calculate the size of the file
long int fileSize(FILE *fp)
{
	long int res, saver;
	saver = ftell(fp);
	fseek(fp, 0, SEEK_END);
	res = ftell(fp);
	fseek(fp, saver, SEEK_SET);
	return res;
}

//this function calculate how many digits in the number
int NumOfDigits(int num)
{
	int counter = 0;
	while (num > 0)
	{
		counter++;
		num = num / 10;
	}
	return counter;
}

// this function check if the bit 'i' is set
BYTE isBitISet(BYTE ch, int i)
{
	BYTE mask = 1 << i;
	return (ch&mask);
}

// this function set bit 'i' 
BYTE setBitI(BYTE ch, int i)
{
	BYTE mask = 1 << i;
	return (ch | mask);
}

// this function create a matrix of BOOL and input 'FALSE' to all cells 
BOOL **createNewMatrix(unsigned short rows, unsigned short cols)
{
	BOOL **mat;
	int i, j;
	mat = (BOOL**)malloc(rows * sizeof(BOOL*));
	checkMemoryAllocation(mat);
	for (i = 0; i < rows; i++)
	{
		mat[i] = (BOOL*)malloc(cols * sizeof(BOOL));
		checkMemoryAllocation(mat[i]);
		for (j = 0; j < cols; j++)
			mat[i][j] = FALSE;
	}
	return (mat);
}


void freeMatrix(BOOL **mat, unsigned short rows)
{
	int i;
	for (i = 0; i < rows; i++)
		free(mat[i]);
	free(mat);
}

// this function recieve the current gray scale value and the max gray allowed
//  and return a new number in the correct range
int updateGrayScale(int current, int maxGray, BOOL WHATTODO)
{
	int res;
	if (WHATTODO == COMPRESS)
	{
		res = (current*maxGray);
		res = (res / GRAY_SCALE_RANGE);
	}
	else //(WHATTODO = UNCOMPRESS)
	{
		res = current*GRAY_SCALE_RANGE;
		res = (res / maxGray);
	}
	return (res);
}


// this function calculate the number of bits that need to "present" the binary value of the number
int countBitsForPixel(int maxGrayValue)
{
	int counter = 0; // count number of bits that necessary for the binary code
	int currentNum = maxGrayValue;
	while (currentNum > 0)
	{
		currentNum = currentNum / 2;
		counter++;
	}
	return (counter);
}


//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~  FREE allocation  ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

void freeGrayImage(grayImage *image)
{

	unsigned short rows;
	unsigned short i;
	if (image == NULL)
		return;
	// getting number of row and columns
	rows = image->rows;

	for (i = 0; i < rows; i++)
	{
		free(image->pixels[i]);
	}
	free(image->pixels);
	free(image);
}



void freeSegmentsList(imgPosList *segments)
{
	imgPosCell *next, *curr;
	if (segments == NULL)
		return;
	else
	{
		curr = segments->head;
		while (curr != NULL)
		{
			next = curr->next;
			free(curr);
			curr = next;
		}
		// empty the LIST
		segments->head = NULL;
		segments->tail = NULL;
	
	}	
}

void freeSegment(treeNode* curr)
{
	if (curr->next_possible_positions == NULL)
	{
		free(curr);
		return;
	}
	else
	{
		freeSegment(curr->next_possible_positions->node);
		free(curr->next_possible_positions);
		free(curr);
	}
}