#include "BitmapManager.h"

void BitmapManager::setLastRuntime(long long duration)
{
    this->lastRuntime = duration;
}

void BitmapManager::loadBMP(const char* filename)
{
   if (this->isFileLoaded) {
       delete[] this->imageData;
       this->imageData = nullptr;
       delete[] this->blurredImageData;
       this->blurredImageData = nullptr;
   }

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
    unsigned char* imageData1 = new unsigned char[dataSize + 18];

    //Odczytaj dane obrazu
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
    //Za³aduj bitmapê
 //   this->loadBMP(filename);
    
    //Zaalokowanie ma³ego obszaru pamiêci, aby delete na pocz¹tku metody loadBMP
    //nie spowodowa³ b³êdu
    //this->imageData = new unsigned char[1];
    //this->blurredImageData = new unsigned char[1];

    this->lastRuntime = 0;
    this->isFileLoaded = false;

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
            std::cout << std::setw(3) << float(*(this->imageData + x));
            std::cout << " ";
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
    if (NULL == this->handleToAsmBlur || NULL == this->handleToCBlur)
        return;

    std::vector<std::thread> threads;

    int bytesPerThread = this->infoHeader.biSizeImage / threadNumber;
    int moduloPerThread = bytesPerThread % 4;
    bool startAdditionalThread = false;
    int additionalThreadBytes = 0;

    if (moduloPerThread > 0) {
        bytesPerThread -= moduloPerThread;
        startAdditionalThread = true;
        additionalThreadBytes = threadNumber * moduloPerThread;
    }

    auto start = std::chrono::high_resolution_clock::now();

    for (int i = 0; i < threadNumber; i++) {
        //threads.push_back(std::thread([this, choice, i, bytesPerThread]() {
        //    if (!choice) {
        //        this->handleToAsmBlur(this->imageData + 9 + i * bytesPerThread, 
        //            this->blurredImageData + i * bytesPerThread, bytesPerThread); 
        //    }
        //    else {
        //        this->handleToCBlur(this->imageData + 9 + i * bytesPerThread,
        //            this->blurredImageData + i * bytesPerThread, bytesPerThread);  
        //    }
        //})
        //);
        
        if (!choice) {
            threads.push_back(std::thread(this->handleToAsmBlur, this->imageData + 9 + i * bytesPerThread,
                this->blurredImageData + i * bytesPerThread, bytesPerThread));
        }
        else {
            threads.push_back(std::thread(this->handleToCBlur, this->imageData + 9 + i * bytesPerThread,
                this->blurredImageData + i * bytesPerThread, bytesPerThread));
        }
    }
    if (startAdditionalThread) {
        //threads.push_back(std::thread([this, choice, additionalThreadBytes,
        //    bytesPerThread, threadNumber]() {
        //    if (!choice) {
        //        this->handleToAsmBlur(this->imageData + 6 + threadNumber * bytesPerThread,
        //            this->blurredImageData + threadNumber * bytesPerThread, additionalThreadBytes);
        //    }
        //    else {
        //        this->handleToCBlur(this->imageData + 6 + threadNumber * bytesPerThread,
        //            this->blurredImageData + threadNumber * bytesPerThread, additionalThreadBytes);
        //    }
        //}));
        if (!choice)
            threads.push_back(std::thread(this->handleToAsmBlur, 
                this->imageData + 9 + threadNumber * bytesPerThread,
                this->blurredImageData + threadNumber * bytesPerThread, additionalThreadBytes));
        else 
            threads.push_back(std::thread(this->handleToCBlur,
                this->imageData + 9 + threadNumber * bytesPerThread,
                this->blurredImageData + threadNumber * bytesPerThread, additionalThreadBytes));
    }

    for (auto& t : threads)
        t.join();

    auto finish = std::chrono::high_resolution_clock::now();
    auto duration = std::chrono::duration_cast<std::chrono::microseconds>(finish - start);
    this->setLastRuntime(duration.count());

 //   if (threadNumber > this->infoHeader.biHeight)
 //       threadNumber = this->infoHeader.biHeight;
 //
 //   int linesPerThread = this->infoHeader.biHeight / threadNumber;
 //   int moduloCounter = this->infoHeader.biHeight % threadNumber;
 //
 //
 //   auto bytesPerRow = this->infoHeader.biSizeImage / this->infoHeader.biHeight;
 //   //W zale¿noœci od wyboru u¿yj odpowiedniego uchwytu procedury
 //       if (NULL != this->handleToAsmBlur) {
 //         
 //
 //           for (int i = 0; i < moduloCounter; i++) {
 //               threads.push_back(std::thread([this, i, linesPerThread, bytesPerRow, choice]() {
 //                   if (!choice) {
 //                       this->handleToAsmBlur(this->imageData + i * (linesPerThread + 1) * bytesPerRow,
 //                           this->blurredImageData + i * (linesPerThread + 1) * bytesPerRow,
 //                           bytesPerRow, linesPerThread + 1);
 //                   }
 //                   else {
 //                       this->handleToCBlur(this->imageData + i * (linesPerThread + 1) * bytesPerRow,
 //                           this->blurredImageData + i * (linesPerThread + 1) * bytesPerRow,
 //                           bytesPerRow, linesPerThread + 1);
 //                   }
 //           
 //                   }));
 //           }
 //           
 //           int moduloShift = moduloCounter * (linesPerThread + 1) * bytesPerRow;
 //           
 //           for (int i = 0; i < threadNumber - moduloCounter; i++) //Utwórz tyle w¹tków, ile zosta³o podane
 //               threads.push_back(std::thread([this, bytesPerRow, i, linesPerThread, moduloShift, choice]() {
 //               if (!choice) {
 //                   this->handleToAsmBlur(this->imageData + moduloShift + i * linesPerThread * bytesPerRow,
 //                       this->blurredImageData + moduloShift + i * linesPerThread * bytesPerRow,
 //                       bytesPerRow, linesPerThread);
 //               }
 //               else {
 //                   this->handleToCBlur(this->imageData + moduloShift + i * linesPerThread * bytesPerRow,
 //                       this->blurredImageData + moduloShift + i * linesPerThread * bytesPerRow,
 //                       bytesPerRow, linesPerThread);
 //               }
 //                   }));
 //           
 //           for (auto& t : threads) //Zaczekaj, a¿ wszystkie w¹tki zakoñcz¹ pracê
 //               t.join();
 //
 //
 //           
 //       }
 //       else {
 //           std::cout << "Error: Have not found the proper function." << std::endl;
 //       }
 

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
   // ASM_PROC testLoadingAsmHandle = (ASM_PROC)GetProcAddress(this->hinstLibAsm, "TestLoading");
   // MYPROC  testLoadingCHandle = (MYPROC)GetProcAddress(this->hinstLibC, "TestLoading");
   // 
   // std::vector<std::thread> threads;
   //
   // if (threadNumber > this->infoHeader.biHeight)
   //     threadNumber = this->infoHeader.biHeight;
   //
   // int linesPerThread = this->infoHeader.biHeight / threadNumber;
   // int moduloCounter = this->infoHeader.biHeight % threadNumber;
   //
   // auto bytesPerRow = this->infoHeader.biSizeImage / this->infoHeader.biHeight;
   // 
   // std::chrono::duration<double> durationASM;
   // std::chrono::duration<double> durationC;
   //
   // if (NULL != testLoadingAsmHandle) {
   //
   //     auto start = std::chrono::high_resolution_clock::now(); //START CLOCK
   //
   //     for (int i = 0; i < moduloCounter; i++) {
   //         threads.push_back(std::thread([this, i, linesPerThread, bytesPerRow, testLoadingAsmHandle]() {
   //             testLoadingAsmHandle(this->imageData + i * (linesPerThread + 1) * bytesPerRow,
   //                 this->blurredImageData + i * (linesPerThread + 1) * bytesPerRow,
   //                 bytesPerRow, linesPerThread + 1);
   //             }));
   //     }
   //
   //     int moduloShift = moduloCounter * (linesPerThread + 1) * bytesPerRow;
   //
   //     for (int i = 0; i < threadNumber - moduloCounter; i++) //Utwórz tyle w¹tków, ile zosta³o podane
   //         threads.push_back(std::thread([this, bytesPerRow, i, linesPerThread, moduloShift, testLoadingAsmHandle]() {
   //         testLoadingAsmHandle(this->imageData + moduloShift + i * linesPerThread * bytesPerRow,
   //             this->blurredImageData + moduloShift + i * linesPerThread * bytesPerRow,
   //             bytesPerRow, linesPerThread);
   //             }));
   //     for (auto& t : threads) //Zaczekaj, a¿ wszystkie w¹tki zakoñcz¹ pracê
   //         t.join();
   //
   //     auto finish = std::chrono::high_resolution_clock::now(); //FINISH CLOCK
   //     durationASM = finish - start;
   //     std::cout << "ASM loading time is: " << durationASM.count() << std::endl;
   // }
   // else {
   //     std::cout << "Error: Have not found the proper ASM function." << std::endl;
   // }
   //
   // std::vector<std::thread> threads2;
   //
   // if (NULL != testLoadingCHandle) {
   //
   //     auto start = std::chrono::high_resolution_clock::now(); //START CLOCK
   //
   //     for (int i = 0; i < moduloCounter; i++) {
   //         threads2.push_back(std::thread([this, i, linesPerThread, bytesPerRow, testLoadingCHandle]() {
   //             testLoadingCHandle(this->imageData + i * (linesPerThread + 1) * bytesPerRow,
   //                 this->blurredImageData + i * (linesPerThread + 1) * bytesPerRow,
   //                 bytesPerRow, linesPerThread + 1);
   //             }));
   //     }
   //
   //     int moduloShift = moduloCounter * (linesPerThread + 1) * bytesPerRow;
   //
   //     for (int i = 0; i < threadNumber - moduloCounter; i++) //Utwórz tyle w¹tków, ile zosta³o podane
   //         threads2.push_back(std::thread([this, bytesPerRow, i, linesPerThread, moduloShift, testLoadingCHandle]() {
   //         testLoadingCHandle(this->imageData + moduloShift + i * linesPerThread * bytesPerRow,
   //             this->blurredImageData + moduloShift + i * linesPerThread * bytesPerRow,
   //             bytesPerRow, linesPerThread);
   //             }));
   //     for (auto& t : threads2) //Zaczekaj, a¿ wszystkie w¹tki zakoñcz¹ pracê
   //         t.join();
   //
   //     auto finish = std::chrono::high_resolution_clock::now(); //FINISH CLOCK
   //     std::chrono::duration<double> durationC = finish - start;
   //     std::cout << "C loading time is: " << durationC.count() << std::endl;
   // }
   // else {
   //     std::cout << "Error: Have not found the proper C function." << std::endl;
   // }
   // 
   // if (durationC.count() > durationASM.count()) {
   //     std::cout << "Loading C proc takes more time." << std::endl;
   // }
   // else if (durationC.count() < durationASM.count()) {
   //     std::cout << "Loading ASM proc takes more time." << std::endl;
   // }
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

long long BitmapManager::getLastRuntime()
{
    return this->lastRuntime;
}

BitmapManager::~BitmapManager()
{
    //Zwolnij uchwyty do bibliotek
    FreeLibrary(this->hinstLibAsm);
    FreeLibrary(this->hinstLibC); 
    delete[] this->imageData;
    delete[] this->blurredImageData;
}
