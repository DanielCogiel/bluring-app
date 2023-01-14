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

    //Otw�rz plik
    FILE* file = fopen(filename, "rb");
    if (!file) {
        std::cout << "Could not load file." << std::endl;
        return;
    }
    this->isFileLoaded = true;

    //Odczytaj dane do nag��wk�w
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
    //Za�aduj bitmap�
 //   this->loadBMP(filename);
    
    //Zaalokowanie ma�ego obszaru pami�ci, aby delete na pocz�tku metody loadBMP
    //nie spowodowa� b��du
    //this->imageData = new unsigned char[1];
    //this->blurredImageData = new unsigned char[1];

    this->lastRuntime = 0;
    this->isFileLoaded = false;

    //Wygeneruj uchwyty do DLL
    this->hinstLibAsm = LoadLibrary(TEXT("BlurAsmDll.dll"));
    this->hinstLibC = LoadLibrary(TEXT("BlurCDll.dll"));

    //Je�li uchwyt wskazuje na bibliotek�, wygeneruj uchwyt do odpowiedniej procedury
    if (this->hinstLibAsm != NULL) {  
        this->handleToAsmBlur = (ASM_PROC)GetProcAddress(this->hinstLibAsm, "BlurProc");
    }
    if (this->hinstLibC != NULL) {
        this->handleToCBlur = (MYPROC)GetProcAddress(this->hinstLibC, "BlurProc");
    }
   
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

    int bytesPerLine = this->infoHeader.biSizeImage / this->infoHeader.biHeight;
    int linesPerThread = this->infoHeader.biHeight / threadNumber;
    int additionalLines = this->infoHeader.biHeight % threadNumber;

    std::chrono::steady_clock::time_point start;
    if (!choice) {
        start = std::chrono::high_resolution_clock::now();

        for (int i = 0; i < additionalLines; i++) {
            threads.push_back(std::thread(this->handleToAsmBlur,
                this->imageData + i * bytesPerLine * (linesPerThread + 1),
                this->blurredImageData + i * bytesPerLine * (linesPerThread + 1),
                bytesPerLine, linesPerThread + 1));
        }
        for (int i = 0; i < threadNumber - additionalLines; i++) {
            threads.push_back(std::thread(this->handleToAsmBlur,
                this->imageData + additionalLines * bytesPerLine * (linesPerThread + 1) + 
            i * bytesPerLine * linesPerThread,
                this->blurredImageData + additionalLines * bytesPerLine * (linesPerThread + 1) + 
            i * bytesPerLine * linesPerThread,
                bytesPerLine, linesPerThread));
        }
    }
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

       /* for (int i = 0; i < threadNumber; i++) {
            threads.push_back(std::thread(this->handleToCBlur,
                this->imageData + i * bytesPerLine * linesPerThread,
                this->blurredImageData + i * bytesPerLine * linesPerThread,
                bytesPerLine, linesPerThread));
        }

        if (startAdditionalThread) {
            threads.push_back(std::thread(this->handleToCBlur, 
                this->imageData + threadNumber * bytesPerLine * linesPerThread,
                this->blurredImageData + threadNumber * bytesPerLine * linesPerThread,
                bytesPerLine, additionalLines));
        }*/
    }

    //auto start = std::chrono::high_resolution_clock::now();
    //
    //for (int i = 0; i < threadNumber; i++) {
    //    threads.push_back(std::thread([this, choice, bytesPerLine, linesPerThread, i]() {
    //        //if (i == 1) {
    //            if (!choice) {
    //                this->handleToAsmBlur(this->imageData + i * bytesPerLine * linesPerThread + 9,
    //                    this->blurredImageData + i * bytesPerLine * linesPerThread,
    //                    bytesPerLine, linesPerThread);
    //            }
    //            else {
    //                this->handleToCBlur(this->imageData + i * bytesPerLine * linesPerThread,
    //                    this->blurredImageData + i * bytesPerLine * linesPerThread,
    //                    bytesPerLine, linesPerThread);
    //            }
    //        //}
    //    }   ));
    //  
    //}

    //if (startAdditionalThread) {
    //    threads.push_back(std::thread([this, choice, bytesPerLine, linesPerThread,
    //        threadNumber, additionalLines]() {
    //        if (!choice) {
    //            this->handleToAsmBlur(this->imageData + threadNumber * bytesPerLine * linesPerThread + 9,
    //                this->blurredImageData + threadNumber * bytesPerLine * linesPerThread,
    //                bytesPerLine, additionalLines);
    //        }
    //        else {
    //            this->handleToCBlur(this->imageData + threadNumber * bytesPerLine * linesPerThread,
    //                this->blurredImageData + threadNumber * bytesPerLine * linesPerThread,
    //                bytesPerLine, additionalLines);
    //        }
    //        }));
    //}

    for (auto& t : threads)
        t.join();
    
    auto finish = std::chrono::high_resolution_clock::now();
    auto duration = std::chrono::duration_cast<std::chrono::microseconds>(finish - start);
    this->setLastRuntime(duration.count());

    //------------------------------------------------------------------
    //int bytesPerThread = this->infoHeader.biSizeImage / threadNumber;
    //int moduloPerThread = bytesPerThread % 4;
    //bool startAdditionalThread = false;
    //int additionalThreadBytes = 0;
    //
    //if (moduloPerThread > 0) {
    //    bytesPerThread -= moduloPerThread;
    //    startAdditionalThread = true;
    //    additionalThreadBytes = threadNumber * moduloPerThread;
    //}
    //
    //auto start = std::chrono::high_resolution_clock::now();
    //
    //for (int i = 0; i < threadNumber; i++) {
    //    //threads.push_back(std::thread([this, choice, i, bytesPerThread]() {
    //    //    if (!choice) {
    //    //        this->handleToAsmBlur(this->imageData + 9 + i * bytesPerThread, 
    //    //            this->blurredImageData + i * bytesPerThread, bytesPerThread); 
    //    //    }
    //    //    else {
    //    //        this->handleToCBlur(this->imageData + 9 + i * bytesPerThread,
    //    //            this->blurredImageData + i * bytesPerThread, bytesPerThread);  
    //    //    }
    //    //})
    //    //);
    //    
    //    if (!choice) {
    //        threads.push_back(std::thread(this->handleToAsmBlur, this->imageData + 9 + i * bytesPerThread,
    //            this->blurredImageData + i * bytesPerThread, bytesPerThread));
    //    }
    //    else {
    //        threads.push_back(std::thread(this->handleToCBlur, this->imageData + 9 + i * bytesPerThread,
    //            this->blurredImageData + i * bytesPerThread, bytesPerThread));
    //    }
    //}
    //if (startAdditionalThread) {
    //    //threads.push_back(std::thread([this, choice, additionalThreadBytes,
    //    //    bytesPerThread, threadNumber]() {
    //    //    if (!choice) {
    //    //        this->handleToAsmBlur(this->imageData + 6 + threadNumber * bytesPerThread,
    //    //            this->blurredImageData + threadNumber * bytesPerThread, additionalThreadBytes);
    //    //    }
    //    //    else {
    //    //        this->handleToCBlur(this->imageData + 6 + threadNumber * bytesPerThread,
    //    //            this->blurredImageData + threadNumber * bytesPerThread, additionalThreadBytes);
    //    //    }
    //    //}));
    //    if (!choice)
    //        threads.push_back(std::thread(this->handleToAsmBlur, 
    //            this->imageData + 9 + threadNumber * bytesPerThread,
    //            this->blurredImageData + threadNumber * bytesPerThread, additionalThreadBytes));
    //    else 
    //        threads.push_back(std::thread(this->handleToCBlur,
    //            this->imageData + 9 + threadNumber * bytesPerThread,
    //            this->blurredImageData + threadNumber * bytesPerThread, additionalThreadBytes));
    //}
    //
    //for (auto& t : threads)
    //    t.join();
    //
    //auto finish = std::chrono::high_resolution_clock::now();
    //auto duration = std::chrono::duration_cast<std::chrono::microseconds>(finish - start);
    //this->setLastRuntime(duration.count());
    //GOOD


 //   if (threadNumber > this->infoHeader.biHeight)
 //       threadNumber = this->infoHeader.biHeight;
 //
 //   int linesPerThread = this->infoHeader.biHeight / threadNumber;
 //   int moduloCounter = this->infoHeader.biHeight % threadNumber;
 //
 //
 //   auto bytesPerRow = this->infoHeader.biSizeImage / this->infoHeader.biHeight;
 //   //W zale�no�ci od wyboru u�yj odpowiedniego uchwytu procedury
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
 //           for (int i = 0; i < threadNumber - moduloCounter; i++) //Utw�rz tyle w�tk�w, ile zosta�o podane
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
 //           for (auto& t : threads) //Zaczekaj, a� wszystkie w�tki zako�cz� prac�
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
   //     for (int i = 0; i < threadNumber - moduloCounter; i++) //Utw�rz tyle w�tk�w, ile zosta�o podane
   //         threads.push_back(std::thread([this, bytesPerRow, i, linesPerThread, moduloShift, testLoadingAsmHandle]() {
   //         testLoadingAsmHandle(this->imageData + moduloShift + i * linesPerThread * bytesPerRow,
   //             this->blurredImageData + moduloShift + i * linesPerThread * bytesPerRow,
   //             bytesPerRow, linesPerThread);
   //             }));
   //     for (auto& t : threads) //Zaczekaj, a� wszystkie w�tki zako�cz� prac�
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
   //     for (int i = 0; i < threadNumber - moduloCounter; i++) //Utw�rz tyle w�tk�w, ile zosta�o podane
   //         threads2.push_back(std::thread([this, bytesPerRow, i, linesPerThread, moduloShift, testLoadingCHandle]() {
   //         testLoadingCHandle(this->imageData + moduloShift + i * linesPerThread * bytesPerRow,
   //             this->blurredImageData + moduloShift + i * linesPerThread * bytesPerRow,
   //             bytesPerRow, linesPerThread);
   //             }));
   //     for (auto& t : threads2) //Zaczekaj, a� wszystkie w�tki zako�cz� prac�
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

bool BitmapManager::test(int threadNumber)
{
    bool result = true;
    int c = 0;
    unsigned char badMatchAsm = 0;
    unsigned char badMatchC = 0;

    this->runBlur(threadNumber, false);
    auto dataSize = this->infoHeader.biSizeImage;
    unsigned char* asmBlurredData = new unsigned char [dataSize];
    std::copy(this->blurredImageData, this->blurredImageData + dataSize, asmBlurredData);
    this->runBlur(threadNumber, true);
    unsigned char* cBlurredData = new unsigned char[dataSize];
    std::copy(this->blurredImageData, this->blurredImageData + dataSize, cBlurredData);
    for (int i = 0; i < dataSize; i++) {
        if (cBlurredData[i] != asmBlurredData[i]) {
            result = false;
            badMatchAsm = asmBlurredData[i];
            badMatchC = cBlurredData[i];
            c = 1;
        }
            
    }

    delete[] asmBlurredData;
    delete[] cBlurredData;


    return result;
}

BitmapManager::~BitmapManager()
{
    //Zwolnij uchwyty do bibliotek
    FreeLibrary(this->hinstLibAsm);
    FreeLibrary(this->hinstLibC);
    delete[] this->imageData;
    delete[] this->blurredImageData;
   
}
