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
        this->handleToCBlur = (MYPROC)GetProcAddress(this->hinstLibC, "MyFunc");
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
            std::cout << std::setw(3) << int(*(this->imageData + x));
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
    std::vector<std::thread> threads;

    auto bytesPerRow = this->infoHeader.biSizeImage / this->infoHeader.biHeight;

    //W zale¿noœci od wyboru u¿yj odpowiedniego uchwytu procedury
    if (!choice) {
        if (NULL != this->handleToAsmBlur) {
            for (int i = 0; i < threadNumber; i++) //Utwórz tyle w¹tków, ile zosta³o podane
                threads.push_back(std::thread([this, bytesPerRow, i]() {
                this->handleToAsmBlur(this->imageData + i * bytesPerRow,
                    this->blurredImageData + i * bytesPerRow, bytesPerRow, 1);
                    }));
            for (auto& t : threads) //Zaczekaj, a¿ wszystkie w¹tki zakoñcz¹ pracê
                t.join();
        }
        else {
            std::cout << "Error: Have not found the proper function." << std::endl;
        }
    }
    else {
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
    }
}

BitmapManager::~BitmapManager()
{
    //Zwolnij uchwyty do bibliotek
    FreeLibrary(this->hinstLibAsm);
    FreeLibrary(this->hinstLibC); 
    delete[] this->imageData;
    delete[] this->blurredImageData;
}
