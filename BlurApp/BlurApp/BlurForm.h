#pragma once
#include "BitmapManager.h";
#include <atlstr.h>


namespace BlurApp {

	using namespace System;
	using namespace System::ComponentModel;
	using namespace System::Collections;
	using namespace System::Windows::Forms;
	using namespace System::Data;
	using namespace System::Drawing;
	using namespace System::Runtime::InteropServices;


	/// <summary>
	/// Podsumowanie informacji o BlurForm
	/// </summary>
	public ref class BlurForm : public System::Windows::Forms::Form
	{
		//Mened¿er bitmapy
		BitmapManager* blurManager;
	public:
		BlurForm(void)
		{
			InitializeComponent();
			//
			//TODO: W tym miejscu dodaj kod konstruktora
			//
			// 
			//this->blurManager = new BitmapManager();
			//this->blurManager->loadBMP("land2.bmp");
			//this->blurManager->runBlur(64, true);
			//this->blurManager->exportImage("TESTFORM.bmp");

			this->blurManager = new BitmapManager();
		}

	protected:
		/// <summary>
		/// Wyczyœæ wszystkie u¿ywane zasoby.
		/// </summary>
		~BlurForm()
		{
			if (components)
			{
				delete components;
			}
		}
	private: System::Windows::Forms::TrackBar^ threadNumberTrackbar;
	protected:

	private: System::Windows::Forms::Label^ label1;
	private: System::Windows::Forms::Button^ blurButton;
	private: System::Windows::Forms::RadioButton^ asmRadioButton;


	private: System::Windows::Forms::GroupBox^ chooseDllGroupBox;
	private: System::Windows::Forms::RadioButton^ cRadioButton;
	private: System::Windows::Forms::PictureBox^ originalImagePictureBox;
	private: System::Windows::Forms::PictureBox^ blurredImagePictureBox;




	private: System::Windows::Forms::OpenFileDialog^ FileDialog;
	private: System::Windows::Forms::Button^ chooseImageButton;
	private: System::Windows::Forms::Label^ threadNumberLabel;
	private: System::Windows::Forms::Label^ funcInfoLabel;


	protected:

	private:
		/// <summary>
		/// Wymagana zmienna projektanta.
		/// </summary>
		System::ComponentModel::Container ^components;

#pragma region Windows Form Designer generated code
		/// <summary>
		/// Metoda wymagana do obs³ugi projektanta — nie nale¿y modyfikowaæ
		/// jej zawartoœci w edytorze kodu.
		/// </summary>
		void InitializeComponent(void)
		{
			this->blurButton = (gcnew System::Windows::Forms::Button());
			this->threadNumberTrackbar = (gcnew System::Windows::Forms::TrackBar());
			this->label1 = (gcnew System::Windows::Forms::Label());
			this->asmRadioButton = (gcnew System::Windows::Forms::RadioButton());
			this->chooseDllGroupBox = (gcnew System::Windows::Forms::GroupBox());
			this->cRadioButton = (gcnew System::Windows::Forms::RadioButton());
			this->originalImagePictureBox = (gcnew System::Windows::Forms::PictureBox());
			this->blurredImagePictureBox = (gcnew System::Windows::Forms::PictureBox());
			this->FileDialog = (gcnew System::Windows::Forms::OpenFileDialog());
			this->chooseImageButton = (gcnew System::Windows::Forms::Button());
			this->threadNumberLabel = (gcnew System::Windows::Forms::Label());
			this->funcInfoLabel = (gcnew System::Windows::Forms::Label());
			(cli::safe_cast<System::ComponentModel::ISupportInitialize^>(this->threadNumberTrackbar))->BeginInit();
			this->chooseDllGroupBox->SuspendLayout();
			(cli::safe_cast<System::ComponentModel::ISupportInitialize^>(this->originalImagePictureBox))->BeginInit();
			(cli::safe_cast<System::ComponentModel::ISupportInitialize^>(this->blurredImagePictureBox))->BeginInit();
			this->SuspendLayout();
			// 
			// blurButton
			// 
			this->blurButton->Location = System::Drawing::Point(215, 151);
			this->blurButton->Name = L"blurButton";
			this->blurButton->Size = System::Drawing::Size(110, 43);
			this->blurButton->TabIndex = 0;
			this->blurButton->Text = L"Blur image";
			this->blurButton->UseVisualStyleBackColor = true;
			this->blurButton->Click += gcnew System::EventHandler(this, &BlurForm::blurButton_Click);
			// 
			// threadNumberTrackbar
			// 
			this->threadNumberTrackbar->Location = System::Drawing::Point(12, 28);
			this->threadNumberTrackbar->Maximum = 64;
			this->threadNumberTrackbar->Minimum = 1;
			this->threadNumberTrackbar->Name = L"threadNumberTrackbar";
			this->threadNumberTrackbar->Size = System::Drawing::Size(345, 56);
			this->threadNumberTrackbar->TabIndex = 1;
			this->threadNumberTrackbar->Value = 1;
			this->threadNumberTrackbar->Scroll += gcnew System::EventHandler(this, &BlurForm::threadNumberTrackbar_Scroll);
			// 
			// label1
			// 
			this->label1->AutoSize = true;
			this->label1->Location = System::Drawing::Point(12, 9);
			this->label1->Name = L"label1";
			this->label1->Size = System::Drawing::Size(82, 16);
			this->label1->TabIndex = 2;
			this->label1->Text = L"Iloœæ w¹tków";
			// 
			// asmRadioButton
			// 
			this->asmRadioButton->AutoSize = true;
			this->asmRadioButton->Checked = true;
			this->asmRadioButton->Location = System::Drawing::Point(0, 20);
			this->asmRadioButton->Name = L"asmRadioButton";
			this->asmRadioButton->Size = System::Drawing::Size(57, 20);
			this->asmRadioButton->TabIndex = 3;
			this->asmRadioButton->TabStop = true;
			this->asmRadioButton->Text = L"ASM";
			this->asmRadioButton->UseVisualStyleBackColor = true;
			// 
			// chooseDllGroupBox
			// 
			this->chooseDllGroupBox->Controls->Add(this->cRadioButton);
			this->chooseDllGroupBox->Controls->Add(this->asmRadioButton);
			this->chooseDllGroupBox->Location = System::Drawing::Point(15, 90);
			this->chooseDllGroupBox->Name = L"chooseDllGroupBox";
			this->chooseDllGroupBox->Size = System::Drawing::Size(139, 104);
			this->chooseDllGroupBox->TabIndex = 4;
			this->chooseDllGroupBox->TabStop = false;
			this->chooseDllGroupBox->Text = L"Biblioteka DLL";
			// 
			// cRadioButton
			// 
			this->cRadioButton->AutoSize = true;
			this->cRadioButton->Location = System::Drawing::Point(0, 46);
			this->cRadioButton->Name = L"cRadioButton";
			this->cRadioButton->Size = System::Drawing::Size(51, 20);
			this->cRadioButton->TabIndex = 4;
			this->cRadioButton->Text = L"C++";
			this->cRadioButton->UseVisualStyleBackColor = true;
			// 
			// originalImagePictureBox
			// 
			this->originalImagePictureBox->Anchor = static_cast<System::Windows::Forms::AnchorStyles>(((System::Windows::Forms::AnchorStyles::Top | System::Windows::Forms::AnchorStyles::Bottom)
				| System::Windows::Forms::AnchorStyles::Left));
			this->originalImagePictureBox->BorderStyle = System::Windows::Forms::BorderStyle::FixedSingle;
			this->originalImagePictureBox->Location = System::Drawing::Point(415, 12);
			this->originalImagePictureBox->Name = L"originalImagePictureBox";
			this->originalImagePictureBox->Size = System::Drawing::Size(485, 494);
			this->originalImagePictureBox->SizeMode = System::Windows::Forms::PictureBoxSizeMode::Zoom;
			this->originalImagePictureBox->TabIndex = 5;
			this->originalImagePictureBox->TabStop = false;
			// 
			// blurredImagePictureBox
			// 
			this->blurredImagePictureBox->Anchor = static_cast<System::Windows::Forms::AnchorStyles>(((System::Windows::Forms::AnchorStyles::Top | System::Windows::Forms::AnchorStyles::Bottom)
				| System::Windows::Forms::AnchorStyles::Right));
			this->blurredImagePictureBox->BorderStyle = System::Windows::Forms::BorderStyle::FixedSingle;
			this->blurredImagePictureBox->Location = System::Drawing::Point(906, 12);
			this->blurredImagePictureBox->Name = L"blurredImagePictureBox";
			this->blurredImagePictureBox->Size = System::Drawing::Size(485, 494);
			this->blurredImagePictureBox->SizeMode = System::Windows::Forms::PictureBoxSizeMode::Zoom;
			this->blurredImagePictureBox->TabIndex = 6;
			this->blurredImagePictureBox->TabStop = false;
			// 
			// FileDialog
			// 
			this->FileDialog->FileName = L"openFileDialog1";
			this->FileDialog->Filter = L"\"BMP|*.bmp|All Files|*.*\"";
			// 
			// chooseImageButton
			// 
			this->chooseImageButton->Location = System::Drawing::Point(215, 90);
			this->chooseImageButton->Name = L"chooseImageButton";
			this->chooseImageButton->Size = System::Drawing::Size(110, 43);
			this->chooseImageButton->TabIndex = 7;
			this->chooseImageButton->Text = L"Choose image";
			this->chooseImageButton->UseVisualStyleBackColor = true;
			this->chooseImageButton->Click += gcnew System::EventHandler(this, &BlurForm::chooseImageButton_Click);
			// 
			// threadNumberLabel
			// 
			this->threadNumberLabel->AutoSize = true;
			this->threadNumberLabel->Location = System::Drawing::Point(363, 28);
			this->threadNumberLabel->Name = L"threadNumberLabel";
			this->threadNumberLabel->Size = System::Drawing::Size(14, 16);
			this->threadNumberLabel->TabIndex = 8;
			this->threadNumberLabel->Text = L"1";
			// 
			// funcInfoLabel
			// 
			this->funcInfoLabel->AutoSize = true;
			this->funcInfoLabel->Location = System::Drawing::Point(12, 490);
			this->funcInfoLabel->Name = L"funcInfoLabel";
			this->funcInfoLabel->Size = System::Drawing::Size(0, 16);
			this->funcInfoLabel->TabIndex = 9;
			// 
			// BlurForm
			// 
			this->AutoScaleDimensions = System::Drawing::SizeF(8, 16);
			this->AutoScaleMode = System::Windows::Forms::AutoScaleMode::Font;
			this->ClientSize = System::Drawing::Size(1403, 521);
			this->Controls->Add(this->funcInfoLabel);
			this->Controls->Add(this->threadNumberLabel);
			this->Controls->Add(this->chooseImageButton);
			this->Controls->Add(this->blurredImagePictureBox);
			this->Controls->Add(this->originalImagePictureBox);
			this->Controls->Add(this->chooseDllGroupBox);
			this->Controls->Add(this->label1);
			this->Controls->Add(this->threadNumberTrackbar);
			this->Controls->Add(this->blurButton);
			this->Name = L"BlurForm";
			this->Text = L"BlurForm";
			(cli::safe_cast<System::ComponentModel::ISupportInitialize^>(this->threadNumberTrackbar))->EndInit();
			this->chooseDllGroupBox->ResumeLayout(false);
			this->chooseDllGroupBox->PerformLayout();
			(cli::safe_cast<System::ComponentModel::ISupportInitialize^>(this->originalImagePictureBox))->EndInit();
			(cli::safe_cast<System::ComponentModel::ISupportInitialize^>(this->blurredImagePictureBox))->EndInit();
			this->ResumeLayout(false);
			this->PerformLayout();

		}
#pragma endregion
	private: System::Void blurButton_Click(System::Object^ sender, System::EventArgs^ e) {
		if (this->blurManager->isFileLoaded) {
			if (this->asmRadioButton->Checked) {
				this->blurManager->runBlur(this->threadNumberTrackbar->Value, false);
				this->funcInfoLabel->Text = "Ran function with ASM DLL with "
					+ this->threadNumberTrackbar->Value
					+ (this->threadNumberTrackbar->Value == 1 ? " thread" : " threads");
			}
			else {
				this->blurManager->runBlur(this->threadNumberTrackbar->Value, true);
				this->funcInfoLabel->Text = "Ran function with C++ DLL with " 
					+ this->threadNumberTrackbar->Value
					+ (this->threadNumberTrackbar->Value == 1 ? " thread" : " threads");
			}
			this->blurManager->exportImage("output.bmp");
			this->blurredImagePictureBox->ImageLocation = "output.bmp";
			
		}
		
		
		//this->blurredImagePictureBox->Image = Image::FromFile("output.bmp");
	}
	
	private: System::Void chooseImageButton_Click(System::Object^ sender, System::EventArgs^ e) {
		if (this->FileDialog->ShowDialog() == System::Windows::Forms::DialogResult::OK) {
			this->originalImagePictureBox->ImageLocation = this->FileDialog->FileName;
			this->blurredImagePictureBox->Image = nullptr;
			//const char* str = (const char*)(void*)
			//	Marshal::StringToHGlobalAnsi(this->FileDialog->FileName->ToString());
			// use str here for the ofstream filename
			//Marshal::FreeHGlobal(str);

			IntPtr tmpHandle = Marshal::StringToHGlobalAnsi(this->FileDialog->FileName->ToString());
			const char* str = static_cast<char*> (tmpHandle.ToPointer());

			this->blurManager->loadBMP(str);

			Marshal::FreeHGlobal(tmpHandle);
			
		}
	}
private: System::Void threadNumberTrackbar_Scroll(System::Object^ sender, System::EventArgs^ e) {
	this->threadNumberLabel->Text = this->threadNumberTrackbar->Value.ToString();
}
};
}
