#include "BitmapManager.h"

void BitmapManager::loadBMP(const char* filename)
{
    //Otwórz plik
    FILE* file = fopen(filename, "rb");
    if (!file) {
        std::cout << "Could not load file." << std::endl;
        return;
    }
    this->isFileLoaded = true;

    //Odczytaj dane do nag³ówków
    fread(&(this->fileHeader), sizeof(BitmapFileHeader), 1, file);
    fread(&(this->infoHeader), sizeof(BitmapInfoHeader), 1, file);

    auto dataSize = this->infoHeader.biSizeImage;
    unsigned char* imageData1 = new unsigned char[dataSize];

    //Odczytaj dane obrazu
    fseek(file, this->fileHeader.bfOffBits, SEEK_SET);
    fread(imageData1, sizeof(unsigned char), dataSize, file);

    this->imageData = imageData1;
    this->blurredImageData = new unsigned char[dataSize];
}

BitmapManager::BitmapManager(const char* filename)
{
    //Za³aduj bitmapê
    this->loadBMP(filename);

    //Wygeneruj uchwyty do DLL
    this->hinstLibAsm = LoadLibrary(TEXT("BlurAsmDll.dll"));
    this->hinstLibC = LoadLibrary(TEXT("BlurCDll.dll"));

    //Jeœli uchwyt wskazuje na bibliotekê, wygeneruj uchwyt do odpowiedniej procedury
    if (this->hinstLibAsm != NULL) {  
        this->handleToAsmBlur = (ASM_PROC)GetProcAddress(this->hinstLibAsm, "BlurProc");
    }
    if (this->hinstLibC != NULL) {
        this->handleToCBlur = (MYPROC)GetProcAddress(this->hinstLibC, "BlurProc");
    }
   
}

void BitmapManager::printImageOnConsole()
{
    //Oblicz, ile bajtów w jednym rzêdzie
    auto bytesPerRow = this->infoHeader.biSizeImage / this->infoHeader.biHeight;
    std::cout << "Bytes in row: " << bytesPerRow << std::endl;

    auto start = std::chrono::high_resolution_clock::now();

    //Wypisz ca³y obraz
    for (int y = 0; y < this->infoHeader.biSizeImage; y += bytesPerRow) {
        for (int x = 0; x < bytesPerRow; x++) {
       //     if (int(*(this->imageData + x)) != 0) {
            std::cout << std::setw(3) << float(*(this->imageData + x));
            std::cout << " ";
       //     }
        }
        std::cout << std::endl;
    }

    auto finish = std::chrono::high_resolution_clock::now();

    std::chrono::duration<double> duration = finish - start;

    //Wypisz w konsoli czas wyœwietlania obrazu
    std::cout << "========================================================" << std::endl;
    std::cout << "Displaying image on the screen took " << duration.count() << " seconds." << std::endl;
    std::cout << "========================================================" << std::endl;
}

void BitmapManager::printBytes(int numberOfBytes, bool choice)
{
    std::cout << "First " << numberOfBytes << " bytes:" << std::endl;
    if (!choice) {
        for (int i = 0; i < numberOfBytes; i++) {
            std::cout << float(this->imageData[i]) << " ";
        }
    }
    else {
        for (int i = 0; i < numberOfBytes; i++) {
            std::cout << float(this->blurredImageData[i]) << " ";
        }
    }
    std::cout << std::endl;
}



void BitmapManager::runBlur(int threadNumber, bool choice)
{
    std::vector<std::thread> threads;

    if (threadNumber > this->infoHeader.biHeight)
        threadNumber = this->infoHeader.biHeight;

    int linesPerThread = this->infoHeader.biHeight / threadNumber;
    int moduloCounter = this->infoHeader.biHeight % threadNumber;
 //   std::cout << linesPerThread << std::endl;
 //   std::cout << "Modulo: " << moduloCounter << std::endl;

    auto bytesPerRow = this->infoHeader.biSizeImage / this->infoHeader.biHeight;
    //W zale¿noœci od wyboru u¿yj odpowiedniego uchwytu procedury
        if (NULL != this->handleToAsmBlur) {
            for (int i = 0; i < moduloCounter; i++) {
                threads.push_back(std::thread([this, i, linesPerThread, bytesPerRow, choice]() {
                    if (!choice) {
                        this->handleToAsmBlur(this->imageData + i * (linesPerThread + 1) * bytesPerRow,
                            this->blurredImageData + i * (linesPerThread + 1) * bytesPerRow,
                            bytesPerRow, linesPerThread + 1);
                    }
                    else {
                        this->handleToCBlur(this->imageData + i * (linesPerThread + 1) * bytesPerRow,
                            this->blurredImageData + i * (linesPerThread + 1) * bytesPerRow,
                            bytesPerRow, linesPerThread + 1);
                    }
              //      std::cout << i * (linesPerThread + 1) * bytesPerRow << std::endl;
                    }));
            }

            int moduloShift = moduloCounter * (linesPerThread + 1) * bytesPerRow;

            for (int i = 0; i < threadNumber - moduloCounter; i++) //Utwórz tyle w¹tków, ile zosta³o podane
                threads.push_back(std::thread([this, bytesPerRow, i, linesPerThread, moduloShift, choice]() {
                if (!choice) {
                    this->handleToAsmBlur(this->imageData + moduloShift + i * linesPerThread * bytesPerRow,
                        this->blurredImageData + moduloShift + i * linesPerThread * bytesPerRow,
                        bytesPerRow, linesPerThread);
                }
                else {
                    this->handleToCBlur(this->imageData + moduloShift + i * linesPerThread * bytesPerRow,
                        this->blurredImageData + moduloShift + i * linesPerThread * bytesPerRow,
                        bytesPerRow, linesPerThread);
                }
                    }));

            for (auto& t : threads) //Zaczekaj, a¿ wszystkie w¹tki zakoñcz¹ pracê
                t.join();

            //for (int i = 0; i < threadNumber; i++) //Utwórz tyle w¹tków, ile zosta³o podane
            //    threads.push_back(std::thread([this, bytesPerRow, i, linesPerThread]() {
            //    this->handleToAsmBlur(this->imageData + i * bytesPerRow,
            //        this->blurredImageData + i * bytesPerRow, bytesPerRow, linesPerThread);
            //        }));
            //for (auto& t : threads) //Zaczekaj, a¿ wszystkie w¹tki zakoñcz¹ pracê
            //    t.join();
        }
        else {
            std::cout << "Error: Have not found the proper function." << std::endl;
        }
   // }
  /*  else {
        if (NULL != this->handleToCBlur) {
            for (int i = 0; i < threadNumber; i++)
                threads.push_back(std::thread([this](int elem1, int elem2) {
                std::cout << this->handleToCBlur(elem1, elem2) << " ";
                    }, 3, 5));
            for (auto& t : threads)
                t.join();
        }
        else {
            std::cout << "Error: Have not found the proper function." << std::endl;
        }
    }*/
}

void BitmapManager::TestLoading(int threadNumber)
{
    ASM_PROC testLoadingAsmHandle = (ASM_PROC)GetProcAddress(this->hinstLibAsm, "TestLoading");
    MYPROC  testLoadingCHandle = (MYPROC)GetProcAddress(this->hinstLibC, "TestLoading");
    
    std::vector<std::thread> threads;

    if (threadNumber > this->infoHeader.biHeight)
        threadNumber = this->infoHeader.biHeight;

    int linesPerThread = this->infoHeader.biHeight / threadNumber;
    int moduloCounter = this->infoHeader.biHeight % threadNumber;

    auto bytesPerRow = this->infoHeader.biSizeImage / this->infoHeader.biHeight;
    
    std::chrono::duration<double> durationASM;
    std::chrono::duration<double> durationC;

    if (NULL != testLoadingAsmHandle) {

        auto start = std::chrono::high_resolution_clock::now(); //START CLOCK

        for (int i = 0; i < moduloCounter; i++) {
            threads.push_back(std::thread([this, i, linesPerThread, bytesPerRow, testLoadingAsmHandle]() {
                testLoadingAsmHandle(this->imageData + i * (linesPerThread + 1) * bytesPerRow,
                    this->blurredImageData + i * (linesPerThread + 1) * bytesPerRow,
                    bytesPerRow, linesPerThread + 1);
                }));
        }

        int moduloShift = moduloCounter * (linesPerThread + 1) * bytesPerRow;

        for (int i = 0; i < threadNumber - moduloCounter; i++) //Utwórz tyle w¹tków, ile zosta³o podane
            threads.push_back(std::thread([this, bytesPerRow, i, linesPerThread, moduloShift, testLoadingAsmHandle]() {
            testLoadingAsmHandle(this->imageData + moduloShift + i * linesPerThread * bytesPerRow,
                this->blurredImageData + moduloShift + i * linesPerThread * bytesPerRow,
                bytesPerRow, linesPerThread);
                }));
        for (auto& t : threads) //Zaczekaj, a¿ wszystkie w¹tki zakoñcz¹ pracê
            t.join();

        auto finish = std::chrono::high_resolution_clock::now(); //FINISH CLOCK
        durationASM = finish - start;
        std::cout << "ASM loading time is: " << durationASM.count() << std::endl;
    }
    else {
        std::cout << "Error: Have not found the proper ASM function." << std::endl;
    }

    std::vector<std::thread> threads2;

    if (NULL != testLoadingCHandle) {

        auto start = std::chrono::high_resolution_clock::now(); //START CLOCK

        for (int i = 0; i < moduloCounter; i++) {
            threads2.push_back(std::thread([this, i, linesPerThread, bytesPerRow, testLoadingCHandle]() {
                testLoadingCHandle(this->imageData + i * (linesPerThread + 1) * bytesPerRow,
                    this->blurredImageData + i * (linesPerThread + 1) * bytesPerRow,
                    bytesPerRow, linesPerThread + 1);
                }));
        }

        int moduloShift = moduloCounter * (linesPerThread + 1) * bytesPerRow;

        for (int i = 0; i < threadNumber - moduloCounter; i++) //Utwórz tyle w¹tków, ile zosta³o podane
            threads2.push_back(std::thread([this, bytesPerRow, i, linesPerThread, moduloShift, testLoadingCHandle]() {
            testLoadingCHandle(this->imageData + moduloShift + i * linesPerThread * bytesPerRow,
                this->blurredImageData + moduloShift + i * linesPerThread * bytesPerRow,
                bytesPerRow, linesPerThread);
                }));
        for (auto& t : threads2) //Zaczekaj, a¿ wszystkie w¹tki zakoñcz¹ pracê
            t.join();

        auto finish = std::chrono::high_resolution_clock::now(); //FINISH CLOCK
        std::chrono::duration<double> durationC = finish - start;
        std::cout << "C loading time is: " << durationC.count() << std::endl;
    }
    else {
        std::cout << "Error: Have not found the proper C function." << std::endl;
    }
    
    if (durationC.count() > durationASM.count()) {
        std::cout << "Loading C proc takes more time." << std::endl;
    }
    else if (durationC.count() < durationASM.count()) {
        std::cout << "Loading ASM proc takes more time." << std::endl;
    }
}

void BitmapManager::exportImage(const char * filename)
{
    FILE* outputFile = fopen(filename, "w");
    if (!outputFile)
        return;
    auto dataSize = this->infoHeader.biSizeImage;
    fwrite(&(this->fileHeader), sizeof(BitmapFileHeader), 1, outputFile);
    fwrite(&(this->infoHeader), sizeof(BitmapInfoHeader), 1, outputFile);
    fwrite(this->blurredImageData, sizeof(unsigned char), dataSize, outputFile);
}

BitmapManager::~BitmapManager()
{
    //Zwolnij uchwyty do bibliotek
    FreeLibrary(this->hinstLibAsm);
    FreeLibrary(this->hinstLibC); 
    delete[] this->imageData;
    delete[] this->blurredImageData;
}
