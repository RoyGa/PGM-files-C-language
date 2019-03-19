//                                        ROY GAZIT  &  GAL MAZOR
#include "project_Functions.h"
void main()
{
	int input;// user's input

			  // case 1 variables:
	char fileName[maxFileName];// used for storing input file name
	grayImage* img = NULL;

	// case 2 + 3 varaibles:
	imgPosList *segments = NULL;
	grayImage* coloredImg = NULL;
	unsigned char threshold;
	int numOfSegments,i;


	// case 4+5 varaibles:
	char userBinFilename[100];
	char userPGMFilename[100];
	unsigned char user_max_gray_level;


	// printing menu options:
	printMenu();
	// getting user's input
	scanf("%d", &input);

	while (input != option_7)
	{
		switch (input)
		{
		case 1:
			if (img != NULL)
				freeGrayImage(img);
			// getting file name
			printf("\nPlease enter the PGM file name: ");
			scanf("%s", fileName);
			// sending file name to function that reads an image file in PGM format
			img = readPGM(fileName);
			if (img != NULL)
				printf("\nPGM file successfully loaded into memory\n");
			else
			{
				printf("\nNo image loaded yet. Load image.\n");
				input = option_1;
				break;
			}
			// printing option menu
			printMenu();
			// getting next user input
			scanf("%d", &input);
			break;

		case 2:
			if (img == NULL)// incase no image was loaded
			{
				printf("\nNo image loaded yet. Load image.\n");
				input = option_1;
				break;
			}
			else
			{
				// getting threshold
				printf("\nPlease enter the threshold for the segemnts: ");
				scanf("%hhu", &threshold);
				// finding all segments
				numOfSegments = findAllSegments(img, threshold, &segments);
				printf("\nAll the segments in the picture were found\n");

			}
			// printing option menu
			printMenu();
			// getting next user input
			scanf("%d", &input);
			break;
		case 3:
			if (segments == NULL)// incase no segments found yet
			{
				printf("\nNo segments were found yet. Search for segment.\n");
				input = option_2;
				break;
			}
			else
			{
				coloredImg = colorSegments(segments, numOfSegments);
				printf("\nColoring the segment in the picture was successfully completed!\n");
			}
			for (i = 0; i < numOfSegments; i++)
				freeSegmentsList(&segments[i]);

			// printing option menu
			printMenu();
			// getting next user input
			scanf("%d", &input);
			break;
		case 4:
			if (coloredImg == NULL)
			{
				printf("\nNo image created yet. Create image first.\n");
				input = option_3;
				break;
			}
			else
			{
				// asking user for filename 
				printf("\nPlease enter a binary file name to save the compressed colored image: ");
				scanf("%s", userBinFilename);
				printf("\nPlease enter the max gray level (0-127): ");
				scanf("%hhu", &user_max_gray_level);
				// saving the colored image in compressed format
				saveCompressed(userBinFilename, coloredImg, user_max_gray_level);
				printf("\nCompressing the colored picture was successfully completed!\n");

			}
			// printing option menu
			printMenu();
			// getting next user input
			scanf("%d", &input);
			break;
		case 5:

			// getting file name
			printf("\nPlease enter a PGM file name to compress: ");
			scanf("%s", userPGMFilename);
			// sending file name to function that reads an image file in PGM format
			img = readPGM(userPGMFilename);
			if (img != NULL)
				printf("\nPGM file successfully loaded into memory!\n");
			else
			{
				printf("\nNo image loaded yet. Load image.\n");
				input = option_5;
				break;
			}
			// getting max gray level
			printf("\nPlease enter the max gray level (0-127):");
			scanf("%hhu", &user_max_gray_level);
			// getting new binary filename
			printf("\nPlease enter a binary file name to save the compressed image: ");
			scanf("%s", userBinFilename);
			// compress image
			saveCompressed(userBinFilename, img, user_max_gray_level);
			printf("\nCompressing the  picture into file was successfully completed!\n");
			// printing option menu
			printMenu();
			// getting next user input
			scanf("%d", &input);
			break;

		case 6:
			// getting binary file name
			printf("\nPlease enter a binary file name that contains a compressed image: ");
			scanf("%s", userBinFilename);
			// getting PGM file name
			printf("\nPlease enter a PGM file name to save the compressed image: ");
			scanf("%s", userPGMFilename);
			// converting a compressed image to PGM format
			convertCompressedImageToPGM(userBinFilename, userPGMFilename);
			printf("\nConverting the compressed bianry file into PGM was successfully completed!\n");
			// printing option menu
			printMenu();
			// getting next user input
			scanf("%d", &input);
			break;

		default:
			printf("\ninvalid input, try again.\n");
			// printing option menu
			printMenu();
			// getting next user input
			scanf("%d", &input);
			break;
		}
	}
	freeGrayImage(img);
	freeGrayImage(coloredImg);
	for (i = 0; i < numOfSegments; i++)
		freeSegmentsList(&segments[i]);
}


