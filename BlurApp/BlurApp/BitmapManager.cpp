#include "BitmapManager.h"

void BitmapManager::setLastRuntime(long long duration)
{
    this->lastRuntime = duration;
}

void BitmapManager::loadBMP(const char* filename)
{
   //if file is already loaded, delete allocated data
   if (this->isFileLoaded) {
       delete[] this->imageData;
       this->imageData = nullptr;
       delete[] this->blurredImageData;
       this->blurredImageData = nullptr;
   }

    //open file
    FILE* file = fopen(filename, "rb");
    if (!file) {
        std::cout << "Could not load file." << std::endl;
        return;
    }
    this->isFileLoaded = true;

    //get data from headers
    fread(&(this->fileHeader), sizeof(BitmapFileHeader), 1, file);
    fread(&(this->infoHeader), sizeof(BitmapInfoHeader), 1, file);

    //allocate image data
    auto dataSize = this->infoHeader.biSizeImage;
    unsigned char* imageData1 = new unsigned char[dataSize + 18];

    //read image data from bitmap
    fseek(file, this->fileHeader.bfOffBits, SEEK_SET);
    fread(imageData1 + 9, sizeof(unsigned char), dataSize, file);
    fclose(file);

    this->imageData = imageData1;
    this->blurredImageData = new unsigned char[dataSize + 4];

    for (int i = 0; i < 9; i++) {
        this->imageData[i] = 0;
        this->imageData[dataSize + 18 - i - 1] = 0;
        this->blurredImageData[dataSize + 4 - i - 1] = 0;
    }
}

BitmapManager::BitmapManager()
{
    this->lastRuntime = 0;
    this->isFileLoaded = false;

    //generate DLL handles
    this->hinstLibAsm = LoadLibrary(TEXT("BlurAsmDll.dll"));
    this->hinstLibC = LoadLibrary(TEXT("BlurCDll.dll"));

    //generate procedure handles, if DLL handles are valid
    if (this->hinstLibAsm != NULL) {  
        this->handleToAsmBlur = (ASM_PROC)GetProcAddress(this->hinstLibAsm, "BlurProc");
    }
    if (this->hinstLibC != NULL) {
        this->handleToCBlur = (MYPROC)GetProcAddress(this->hinstLibC, "BlurProc");
    }
}

void BitmapManager::runBlur(int threadNumber, bool choice)
{
    //check if procedure handles are valid
    if (NULL == this->handleToAsmBlur || NULL == this->handleToCBlur)
        return;

    std::vector<std::thread> threads;

    //count how many bytes per line to process
    int bytesPerLine = this->infoHeader.biSizeImage / this->infoHeader.biHeight;
    //count how many lines to process per thread
    int linesPerThread = this->infoHeader.biHeight / threadNumber;
    //count how many threads should print additional line
    int additionalLines = this->infoHeader.biHeight % threadNumber;

    std::chrono::steady_clock::time_point start;
    //if ASM procedure was chosen
    if (!choice) {
        start = std::chrono::high_resolution_clock::now();

        //start threads processing an additional line
        for (int i = 0; i < additionalLines; i++) {
            threads.push_back(std::thread(this->handleToAsmBlur,
                this->imageData + i * bytesPerLine * (linesPerThread + 1),
                this->blurredImageData + i * bytesPerLine * (linesPerThread + 1),
                bytesPerLine, linesPerThread + 1));
        }
        //start threads processing normal value of lines
        for (int i = 0; i < threadNumber - additionalLines; i++) {
            threads.push_back(std::thread(this->handleToAsmBlur,
                this->imageData + additionalLines * bytesPerLine * (linesPerThread + 1) + 
            i * bytesPerLine * linesPerThread,
                this->blurredImageData + additionalLines * bytesPerLine * (linesPerThread + 1) + 
            i * bytesPerLine * linesPerThread,
                bytesPerLine, linesPerThread));
        }
    }
    //if C++ procedure was chosen
    else {
        start = std::chrono::high_resolution_clock::now();

        for (int i = 0; i < additionalLines; i++) {
            threads.push_back(std::thread(this->handleToCBlur, 
                this->imageData + i * bytesPerLine * (linesPerThread + 1),
                this->blurredImageData + i * bytesPerLine * (linesPerThread + 1),
                bytesPerLine, linesPerThread + 1));
        }
        for (int i = 0; i < threadNumber - additionalLines; i++) {
            threads.push_back(std::thread(this->handleToCBlur,
                this->imageData + additionalLines * bytesPerLine * (linesPerThread + 1) +
                i * bytesPerLine * linesPerThread,
                this->blurredImageData + additionalLines * bytesPerLine * (linesPerThread + 1) +
                i * bytesPerLine * linesPerThread,
                bytesPerLine, linesPerThread));
        }
    }
    for (auto& t : threads)
        t.join();
    
    auto finish = std::chrono::high_resolution_clock::now();
    auto duration = std::chrono::duration_cast<std::chrono::microseconds>(finish - start);
    this->setLastRuntime(duration.count());
}

void BitmapManager::exportImage(const char * filename)
{
    FILE* outputFile = fopen(filename, "wb");
    if (!outputFile)
        return;
    auto dataSize = this->infoHeader.biSizeImage;
    fwrite(&(this->fileHeader), sizeof(BitmapFileHeader), 1, outputFile);
    fwrite(&(this->infoHeader), sizeof(BitmapInfoHeader), 1, outputFile);
    fseek(outputFile, this->fileHeader.bfOffBits, SEEK_SET);
    fwrite(this->blurredImageData, sizeof(unsigned char), dataSize, outputFile);
    fclose(outputFile);
}

void BitmapManager::runSpeedTests(const char* filename)
{
    FILE* fpt;
    fpt = fopen("tests.csv", "w+");
    this->loadBMP(filename);

    this->runBlur(1, false);

    std::vector<int> threadNumbers = { 1, 2, 4, 8, 16, 32, 64 };

    fprintf(fpt, filename);
    fprintf(fpt, "\n");
    for (int i : threadNumbers) {
        fprintf(fpt, "ASM (%d);", i);
        for (int j = 0; j < 20; j++) {
            this->runBlur(i, false);
            fprintf(fpt, "%lld;", this->getLastRuntime());
        }
        fprintf(fpt, "\n");
    }
    for (int i : threadNumbers) {
        fprintf(fpt, "C++ (%d);", i);
        for (int j = 0; j < 20; j++) {
            this->runBlur(i, true);
            fprintf(fpt, "%lld;", this->getLastRuntime());
        }
        fprintf(fpt, "\n");
    }

    fclose(fpt);
}

long long BitmapManager::getLastRuntime()
{
    return this->lastRuntime;
}

BitmapManager::~BitmapManager()
{
    //release DLL handles
    FreeLibrary(this->hinstLibAsm);
    FreeLibrary(this->hinstLibC);
    delete[] this->imageData;
    delete[] this->blurredImageData;
}
