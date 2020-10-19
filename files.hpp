#ifndef FILES_H_
#define FILES_H_

/*
void Save()
{
	char fileName[] = "wanted_savedata";

	std::cout << scores[0] << " " << scores[1] << " " << scores[2] << " " << scores[3] << " " << scores[4] << std::endl;

	//SaveFileData(const char *fileName, void *data, unsigned int bytesToWrite);
	SaveFileData(fileName, scores, sizeof(unsigned short int) * 5);
}
*/

/*
void Load()
{
	char fileName[] = "wanted_savedata";
	unsigned int bytesRead;
	unsigned char * data = LoadFileData(fileName, &bytesRead);
	
	unsigned short a,b;
	for (int i = 0; i < 5; i++)
	{
		a = (unsigned short) data[2*i];
		b = (unsigned short) data[2*i + 1];
		if (a == 0 && b != 0)
			scores[i] = b * 256;
		else
			scores[i] = (a + 1 ) * ( b + 1) - 1;
		std::cout << scores[i] << std::endl;
	}
}
*/

void SaveScores()
{
	SaveStorageValue(HI_SCORE, scores[0]);
	SaveStorageValue(HI_SCORE2, scores[1]);
	SaveStorageValue(HI_SCORE3, scores[2]);
	SaveStorageValue(HI_SCORE4, scores[3]);
	SaveStorageValue(HI_SCORE5, scores[4]);
}

void LoadScores()
{
	scores[0] = LoadStorageValue(HI_SCORE);
	scores[1] = LoadStorageValue(HI_SCORE2);
	scores[2] = LoadStorageValue(HI_SCORE3);
	scores[3] = LoadStorageValue(HI_SCORE4);
	scores[4] = LoadStorageValue(HI_SCORE5);
}

#endif
