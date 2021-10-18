#include "MainForm.h"

using namespace System;
using namespace System::Windows::Forms;

[System::Runtime::InteropServices::DllImport("user32.dll")]
extern bool SetProcessDPIAware();

[STAThreadAttribute]
void main(array<String^>^ args) {
    SetProcessDPIAware();
    Application::EnableVisualStyles();
    Application::SetCompatibleTextRenderingDefault(false);
    NumericLab1::MainForm form;
    Application::Run(% form);
}