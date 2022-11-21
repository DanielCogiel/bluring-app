#include "BlurForm.h"

using namespace System;
using namespace System::Windows::Forms;
void main(array<String^>^ args) {
	Application::EnableVisualStyles();
	Application::SetCompatibleTextRenderingDefault(false);
	BlurApp::BlurForm form;
	Application::Run(% form);
}