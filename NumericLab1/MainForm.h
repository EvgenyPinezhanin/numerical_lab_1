#pragma once

#include<cmath>
#include<typeinfo>
#include<vector>
#include"numerical.h"

namespace NumericLab1 {

	using namespace System;
	using namespace System::ComponentModel;
	using namespace System::Collections;
	using namespace System::Windows::Forms;
	using namespace System::Data;
	using namespace System::Drawing;
	using namespace System::Reflection;

	const double maxD = 79228162514264337593543950335.0;
	const double minD = 0.00000000000000000000000000001;
	const int startSizeArray = 10001;
	int currSizeArray = startSizeArray;

	// функция для установки Control двойной буфферизации
	void SetDoubleBuffered(Control^ c, bool value) {
		PropertyInfo ^pi = (Control::typeid)->GetProperty("DoubleBuffered", BindingFlags::SetProperty | BindingFlags::Instance | BindingFlags::NonPublic);
		if (pi != nullptr) {
			pi->SetValue(c, value, nullptr);
		}
	}

	// Функция для проверки, что переменная типа double лежит в диапазоне чисел decimal
	// Нужна при рисовании точек на графике
	bool rangeOfDec(double d) {
		if (abs(d) >= minD && abs(d) <= maxD) {
			return true;
		}
		return false;
	}

	template<typename T>
	int max(T *arr, int N) {
		T m = arr[0];
		int ind = 0;
		for (int i = 1; i < N; i++) {
			if (arr[i] > m) {
				m = arr[i];
				ind = i;
			}
		}
		return ind;
	}

	template<typename T>
	int min(T* arr, int N) {
		T m = arr[0];
		int ind = 0;
		for (int i = 1; i < N; i++) {
			if (arr[i] < m) {
				m = arr[i];
				ind = i;
			}
		}
		return ind;
	}

	template<typename T>
	int sum(T* arr, int N) {
		T sum = T(0);
		for (int i = 0; i < N; i++) {
			sum += arr[i];
		}
		return sum;
	}

	template<typename T>
	void abs_arr(T* arr, int N) {
		for (int i = 0; i < N; i++) {
			arr[i] = abs(arr[i]);
		}
	}

	double x0, u0, A, B, ud0;

	double f_test(double x, double u) {
		return -1.5 * u;
	}

	double func_test(double x) {
		return u0 * exp(1.5 * x0) * exp(-1.5 * x);
	}

	double f(double x, double u) {
		return 1 / pow(1 + x * x, 1.0 / 3.0) * u * u + u - u * u * u * sin(10.0 * x);
	}

	double f1(double x, double u1, double u2) {
		return u2;
	}

	double f2(double x, double u1, double u2) {
		return -A * u2 * u2 + B * u1;
	}

	public ref class MainForm : public System::Windows::Forms::Form {
	public:
		double* X;
		double* V, *V_1;
		double* V2, *V2_1;
		double* OLP_Arr;
		double* H;
		int* C1_Arr;
		int* C2_Arr;
		double* U;
		double* U_V;

		double x, b, eps, h;
		int Nmax, N;
		bool local_error_control;

		Resources::ResourceManager^ s_pxResourceManager;
		String^ strTask;

		MainForm(void)
		{
			InitializeComponent();
			strTask = TaskComboBox->Text;

			SetDoubleBuffered(Table, true);
			SetDoubleBuffered(Chart, true);
			SetDoubleBuffered(Chart1, true);
			SetDoubleBuffered(Chart2, true);
			SetDoubleBuffered(Chart3, true);

			X = new double[startSizeArray];
			V = new double[startSizeArray];
			V2 = new double[startSizeArray];
			OLP_Arr = new double[startSizeArray];
			H = new double[startSizeArray];
			C1_Arr = new int[startSizeArray];
			C2_Arr = new int[startSizeArray];
			U = new double[startSizeArray];
			U_V = new double[startSizeArray];
			V_1 = new double[startSizeArray];
			V2_1 = new double[startSizeArray];

			// Нужно для получения картинок из resource.resx
			Reflection::Assembly^ pxAssembly = Reflection::Assembly::GetExecutingAssembly();
			String^ pxResName = pxAssembly->GetName()->Name + ".resource";
			s_pxResourceManager = (gcnew Resources::ResourceManager(pxResName, pxAssembly));
			PictureDU->Image = (cli::safe_cast<Drawing::Bitmap^>(s_pxResourceManager->GetObject("DU_test")));
		}

	protected:
		~MainForm()
		{
			if (components)
			{
				delete components;
			}
		}

#pragma region declaration

	private: System::Windows::Forms::TableLayoutPanel^ MainTableLayoutPanel;
	private: System::Windows::Forms::Panel^ MainPanel;
	private: System::Windows::Forms::GroupBox^ TaskBox;
	private: System::Windows::Forms::ComboBox^ TaskComboBox;
	private: System::Windows::Forms::GroupBox^ DuBox;
	private: System::Windows::Forms::Button^ StartButton;
	private: System::Windows::Forms::Panel^ ParametersPanel;
	private: System::Windows::Forms::TextBox^ BorderTextBox;
	private: System::Windows::Forms::Label^ StepLabel;
	private: System::Windows::Forms::Label^ CountMaxLabel;
	private: System::Windows::Forms::Label^ ParametrLabel;
	private: System::Windows::Forms::Label^ BorderLabel;
	private: System::Windows::Forms::TextBox^ ErrorTextBox;
	private: System::Windows::Forms::TextBox^ CountMaxTextBox;
	private: System::Windows::Forms::TextBox^ U0TextBox;
	private: System::Windows::Forms::TextBox^ XTextBox;
	private: System::Windows::Forms::Label^ XLabel;
	private: System::Windows::Forms::TextBox^ X0TextBox;
	private: System::Windows::Forms::Label^ U0Label;
	private: System::Windows::Forms::Label^ X0Label;
	private: System::Windows::Forms::CheckBox^ ErrorCheckBox;
	private: System::Windows::Forms::TextBox^ StepTextBox;
	private: System::Windows::Forms::GroupBox^ ReferenceBox;
	private: System::Windows::Forms::Label^ NResLabel;
	private: System::Windows::Forms::Label^ NLabel;
	private: System::Windows::Forms::Label^ B_xnResLabel;
	private: System::Windows::Forms::Label^ B_xnLabel;
	private: System::Windows::Forms::DataVisualization::Charting::Chart^ Chart;
	private: System::Windows::Forms::SplitContainer^ TCContainer;
	private: System::Windows::Forms::DataGridView^ Table;
	private: System::Windows::Forms::DataGridViewTextBoxColumn^ i;
	private: System::Windows::Forms::DataGridViewTextBoxColumn^ Xi;
	private: System::Windows::Forms::DataGridViewTextBoxColumn^ Vi;
	private: System::Windows::Forms::DataGridViewTextBoxColumn^ Vi1;
	private: System::Windows::Forms::DataGridViewTextBoxColumn^ Vi2;
	private: System::Windows::Forms::DataGridViewTextBoxColumn^ V2i;
	private: System::Windows::Forms::DataGridViewTextBoxColumn^ V2i1;
	private: System::Windows::Forms::DataGridViewTextBoxColumn^ V2i2;
	private: System::Windows::Forms::DataGridViewTextBoxColumn^ Vi_V2i;
	private: System::Windows::Forms::DataGridViewTextBoxColumn^ Vi1_V2i1;
	private: System::Windows::Forms::DataGridViewTextBoxColumn^ Vi2_V2i2;
	private: System::Windows::Forms::DataGridViewTextBoxColumn^ OLP;
	private: System::Windows::Forms::DataGridViewTextBoxColumn^ Hi;
	private: System::Windows::Forms::DataGridViewTextBoxColumn^ C1;
	private: System::Windows::Forms::DataGridViewTextBoxColumn^ C2;
	private: System::Windows::Forms::DataGridViewTextBoxColumn^ Ui;
	private: System::Windows::Forms::DataGridViewTextBoxColumn^ Ui_Vi;
	private: System::Windows::Forms::GroupBox^ ChartBox;
	private: System::Windows::Forms::DataVisualization::Charting::Chart^ Chart3;
	private: System::Windows::Forms::RadioButton^ radioButton3;
	private: System::Windows::Forms::RadioButton^ radioButton2;
	private: System::Windows::Forms::RadioButton^ radioButton1;
	private: System::Windows::Forms::Label^ MaxOLPResLabel;
	private: System::Windows::Forms::Label^ MaxOLPLabel;
	private: System::Windows::Forms::Label^ PriX3ResLabel;
	private: System::Windows::Forms::Label^ PriX3Label;
	private: System::Windows::Forms::Label^ MaxU_VResLabel;
	private: System::Windows::Forms::Label^ MinHLabel;
	private: System::Windows::Forms::Label^ PriX2ResLabel;
	private: System::Windows::Forms::Label^ MaxHResLabel;
	private: System::Windows::Forms::Label^ MaxU_VLabel;
	private: System::Windows::Forms::Label^ MaxHLabel;
	private: System::Windows::Forms::Label^ PriX1ResLabel;
	private: System::Windows::Forms::Label^ PriX1Label;
	private: System::Windows::Forms::Label^ C1ResLabel;
	private: System::Windows::Forms::Label^ C1Label;
	private: System::Windows::Forms::Label^ C2ResLabel;
	private: System::Windows::Forms::Label^ C2Label;
	private: System::Windows::Forms::Label^ PriX2Label;
	private: System::Windows::Forms::Label^ MinHResLabel;
	private: System::Windows::Forms::TextBox^ ATextBox;
	private: System::Windows::Forms::Label^ BLabel;
	private: System::Windows::Forms::Label^ ALabel;
	private: System::Windows::Forms::TextBox^ Ud0TextBox;
	private: System::Windows::Forms::Label^ Ud0Label;
	private: System::Windows::Forms::TextBox^ BTextBox;
	private: System::Windows::Forms::DataVisualization::Charting::Chart^ Chart2;
	private: System::Windows::Forms::DataVisualization::Charting::Chart^ Chart1;
	private: System::Windows::Forms::PictureBox^ PictureDU;
#pragma endregion

	private:
		/// <summary>
		/// Обязательная переменная конструктора.
		/// </summary>
		System::ComponentModel::Container ^components;

#pragma region Windows Form Designer generated code
		/// <summary>
		/// Требуемый метод для поддержки конструктора — не изменяйте 
		/// содержимое этого метода с помощью редактора кода.
		/// </summary>
		void InitializeComponent(void)
		{
			System::Windows::Forms::DataVisualization::Charting::ChartArea^ chartArea1 = (gcnew System::Windows::Forms::DataVisualization::Charting::ChartArea());
			System::Windows::Forms::DataVisualization::Charting::Legend^ legend1 = (gcnew System::Windows::Forms::DataVisualization::Charting::Legend());
			System::Windows::Forms::DataVisualization::Charting::Series^ series1 = (gcnew System::Windows::Forms::DataVisualization::Charting::Series());
			System::Windows::Forms::DataVisualization::Charting::Series^ series2 = (gcnew System::Windows::Forms::DataVisualization::Charting::Series());
			System::Windows::Forms::DataVisualization::Charting::ChartArea^ chartArea2 = (gcnew System::Windows::Forms::DataVisualization::Charting::ChartArea());
			System::Windows::Forms::DataVisualization::Charting::Legend^ legend2 = (gcnew System::Windows::Forms::DataVisualization::Charting::Legend());
			System::Windows::Forms::DataVisualization::Charting::Series^ series3 = (gcnew System::Windows::Forms::DataVisualization::Charting::Series());
			System::Windows::Forms::DataVisualization::Charting::ChartArea^ chartArea3 = (gcnew System::Windows::Forms::DataVisualization::Charting::ChartArea());
			System::Windows::Forms::DataVisualization::Charting::Legend^ legend3 = (gcnew System::Windows::Forms::DataVisualization::Charting::Legend());
			System::Windows::Forms::DataVisualization::Charting::Series^ series4 = (gcnew System::Windows::Forms::DataVisualization::Charting::Series());
			System::Windows::Forms::DataVisualization::Charting::ChartArea^ chartArea4 = (gcnew System::Windows::Forms::DataVisualization::Charting::ChartArea());
			System::Windows::Forms::DataVisualization::Charting::Legend^ legend4 = (gcnew System::Windows::Forms::DataVisualization::Charting::Legend());
			System::Windows::Forms::DataVisualization::Charting::Series^ series5 = (gcnew System::Windows::Forms::DataVisualization::Charting::Series());
			this->MainPanel = (gcnew System::Windows::Forms::Panel());
			this->BTextBox = (gcnew System::Windows::Forms::TextBox());
			this->ATextBox = (gcnew System::Windows::Forms::TextBox());
			this->BLabel = (gcnew System::Windows::Forms::Label());
			this->ALabel = (gcnew System::Windows::Forms::Label());
			this->Ud0TextBox = (gcnew System::Windows::Forms::TextBox());
			this->Ud0Label = (gcnew System::Windows::Forms::Label());
			this->XTextBox = (gcnew System::Windows::Forms::TextBox());
			this->XLabel = (gcnew System::Windows::Forms::Label());
			this->U0TextBox = (gcnew System::Windows::Forms::TextBox());
			this->X0TextBox = (gcnew System::Windows::Forms::TextBox());
			this->U0Label = (gcnew System::Windows::Forms::Label());
			this->X0Label = (gcnew System::Windows::Forms::Label());
			this->ErrorCheckBox = (gcnew System::Windows::Forms::CheckBox());
			this->StartButton = (gcnew System::Windows::Forms::Button());
			this->DuBox = (gcnew System::Windows::Forms::GroupBox());
			this->PictureDU = (gcnew System::Windows::Forms::PictureBox());
			this->TaskBox = (gcnew System::Windows::Forms::GroupBox());
			this->TaskComboBox = (gcnew System::Windows::Forms::ComboBox());
			this->ParametersPanel = (gcnew System::Windows::Forms::Panel());
			this->StepTextBox = (gcnew System::Windows::Forms::TextBox());
			this->CountMaxTextBox = (gcnew System::Windows::Forms::TextBox());
			this->ErrorTextBox = (gcnew System::Windows::Forms::TextBox());
			this->BorderTextBox = (gcnew System::Windows::Forms::TextBox());
			this->StepLabel = (gcnew System::Windows::Forms::Label());
			this->CountMaxLabel = (gcnew System::Windows::Forms::Label());
			this->ParametrLabel = (gcnew System::Windows::Forms::Label());
			this->BorderLabel = (gcnew System::Windows::Forms::Label());
			this->ReferenceBox = (gcnew System::Windows::Forms::GroupBox());
			this->PriX2Label = (gcnew System::Windows::Forms::Label());
			this->MinHResLabel = (gcnew System::Windows::Forms::Label());
			this->PriX3ResLabel = (gcnew System::Windows::Forms::Label());
			this->PriX3Label = (gcnew System::Windows::Forms::Label());
			this->MaxU_VResLabel = (gcnew System::Windows::Forms::Label());
			this->MinHLabel = (gcnew System::Windows::Forms::Label());
			this->PriX2ResLabel = (gcnew System::Windows::Forms::Label());
			this->MaxHResLabel = (gcnew System::Windows::Forms::Label());
			this->MaxU_VLabel = (gcnew System::Windows::Forms::Label());
			this->MaxHLabel = (gcnew System::Windows::Forms::Label());
			this->PriX1ResLabel = (gcnew System::Windows::Forms::Label());
			this->PriX1Label = (gcnew System::Windows::Forms::Label());
			this->C1ResLabel = (gcnew System::Windows::Forms::Label());
			this->C1Label = (gcnew System::Windows::Forms::Label());
			this->C2ResLabel = (gcnew System::Windows::Forms::Label());
			this->C2Label = (gcnew System::Windows::Forms::Label());
			this->MaxOLPResLabel = (gcnew System::Windows::Forms::Label());
			this->MaxOLPLabel = (gcnew System::Windows::Forms::Label());
			this->B_xnResLabel = (gcnew System::Windows::Forms::Label());
			this->B_xnLabel = (gcnew System::Windows::Forms::Label());
			this->NResLabel = (gcnew System::Windows::Forms::Label());
			this->NLabel = (gcnew System::Windows::Forms::Label());
			this->Table = (gcnew System::Windows::Forms::DataGridView());
			this->i = (gcnew System::Windows::Forms::DataGridViewTextBoxColumn());
			this->Xi = (gcnew System::Windows::Forms::DataGridViewTextBoxColumn());
			this->Vi = (gcnew System::Windows::Forms::DataGridViewTextBoxColumn());
			this->Vi1 = (gcnew System::Windows::Forms::DataGridViewTextBoxColumn());
			this->Vi2 = (gcnew System::Windows::Forms::DataGridViewTextBoxColumn());
			this->V2i = (gcnew System::Windows::Forms::DataGridViewTextBoxColumn());
			this->V2i1 = (gcnew System::Windows::Forms::DataGridViewTextBoxColumn());
			this->V2i2 = (gcnew System::Windows::Forms::DataGridViewTextBoxColumn());
			this->Vi_V2i = (gcnew System::Windows::Forms::DataGridViewTextBoxColumn());
			this->Vi1_V2i1 = (gcnew System::Windows::Forms::DataGridViewTextBoxColumn());
			this->Vi2_V2i2 = (gcnew System::Windows::Forms::DataGridViewTextBoxColumn());
			this->OLP = (gcnew System::Windows::Forms::DataGridViewTextBoxColumn());
			this->Hi = (gcnew System::Windows::Forms::DataGridViewTextBoxColumn());
			this->C1 = (gcnew System::Windows::Forms::DataGridViewTextBoxColumn());
			this->C2 = (gcnew System::Windows::Forms::DataGridViewTextBoxColumn());
			this->Ui = (gcnew System::Windows::Forms::DataGridViewTextBoxColumn());
			this->Ui_Vi = (gcnew System::Windows::Forms::DataGridViewTextBoxColumn());
			this->Chart = (gcnew System::Windows::Forms::DataVisualization::Charting::Chart());
			this->TCContainer = (gcnew System::Windows::Forms::SplitContainer());
			this->ChartBox = (gcnew System::Windows::Forms::GroupBox());
			this->radioButton3 = (gcnew System::Windows::Forms::RadioButton());
			this->radioButton2 = (gcnew System::Windows::Forms::RadioButton());
			this->radioButton1 = (gcnew System::Windows::Forms::RadioButton());
			this->Chart3 = (gcnew System::Windows::Forms::DataVisualization::Charting::Chart());
			this->Chart2 = (gcnew System::Windows::Forms::DataVisualization::Charting::Chart());
			this->Chart1 = (gcnew System::Windows::Forms::DataVisualization::Charting::Chart());
			this->MainTableLayoutPanel = (gcnew System::Windows::Forms::TableLayoutPanel());
			this->MainPanel->SuspendLayout();
			this->DuBox->SuspendLayout();
			(cli::safe_cast<System::ComponentModel::ISupportInitialize^>(this->PictureDU))->BeginInit();
			this->TaskBox->SuspendLayout();
			this->ParametersPanel->SuspendLayout();
			this->ReferenceBox->SuspendLayout();
			(cli::safe_cast<System::ComponentModel::ISupportInitialize^>(this->Table))->BeginInit();
			(cli::safe_cast<System::ComponentModel::ISupportInitialize^>(this->Chart))->BeginInit();
			(cli::safe_cast<System::ComponentModel::ISupportInitialize^>(this->TCContainer))->BeginInit();
			this->TCContainer->Panel1->SuspendLayout();
			this->TCContainer->Panel2->SuspendLayout();
			this->TCContainer->SuspendLayout();
			this->ChartBox->SuspendLayout();
			(cli::safe_cast<System::ComponentModel::ISupportInitialize^>(this->Chart3))->BeginInit();
			(cli::safe_cast<System::ComponentModel::ISupportInitialize^>(this->Chart2))->BeginInit();
			(cli::safe_cast<System::ComponentModel::ISupportInitialize^>(this->Chart1))->BeginInit();
			this->MainTableLayoutPanel->SuspendLayout();
			this->SuspendLayout();
			// 
			// MainPanel
			// 
			this->MainPanel->BorderStyle = System::Windows::Forms::BorderStyle::FixedSingle;
			this->MainPanel->Controls->Add(this->BTextBox);
			this->MainPanel->Controls->Add(this->ATextBox);
			this->MainPanel->Controls->Add(this->BLabel);
			this->MainPanel->Controls->Add(this->ALabel);
			this->MainPanel->Controls->Add(this->Ud0TextBox);
			this->MainPanel->Controls->Add(this->Ud0Label);
			this->MainPanel->Controls->Add(this->XTextBox);
			this->MainPanel->Controls->Add(this->XLabel);
			this->MainPanel->Controls->Add(this->U0TextBox);
			this->MainPanel->Controls->Add(this->X0TextBox);
			this->MainPanel->Controls->Add(this->U0Label);
			this->MainPanel->Controls->Add(this->X0Label);
			this->MainPanel->Controls->Add(this->ErrorCheckBox);
			this->MainPanel->Controls->Add(this->StartButton);
			this->MainPanel->Controls->Add(this->DuBox);
			this->MainPanel->Controls->Add(this->TaskBox);
			this->MainPanel->Dock = System::Windows::Forms::DockStyle::Fill;
			this->MainPanel->Location = System::Drawing::Point(3, 3);
			this->MainPanel->Name = L"MainPanel";
			this->MainPanel->Size = System::Drawing::Size(500, 189);
			this->MainPanel->TabIndex = 0;
			// 
			// BTextBox
			// 
			this->BTextBox->Enabled = false;
			this->BTextBox->Location = System::Drawing::Point(146, 137);
			this->BTextBox->Name = L"BTextBox";
			this->BTextBox->Size = System::Drawing::Size(83, 22);
			this->BTextBox->TabIndex = 16;
			this->BTextBox->Text = L"1";
			// 
			// ATextBox
			// 
			this->ATextBox->Enabled = false;
			this->ATextBox->Location = System::Drawing::Point(34, 137);
			this->ATextBox->Name = L"ATextBox";
			this->ATextBox->Size = System::Drawing::Size(79, 22);
			this->ATextBox->TabIndex = 15;
			this->ATextBox->Text = L"1";
			// 
			// BLabel
			// 
			this->BLabel->AutoSize = true;
			this->BLabel->Enabled = false;
			this->BLabel->Location = System::Drawing::Point(119, 140);
			this->BLabel->Name = L"BLabel";
			this->BLabel->Size = System::Drawing::Size(16, 17);
			this->BLabel->TabIndex = 14;
			this->BLabel->Text = L"b";
			// 
			// ALabel
			// 
			this->ALabel->AutoSize = true;
			this->ALabel->Enabled = false;
			this->ALabel->Location = System::Drawing::Point(12, 140);
			this->ALabel->Name = L"ALabel";
			this->ALabel->Size = System::Drawing::Size(16, 17);
			this->ALabel->TabIndex = 13;
			this->ALabel->Text = L"a";
			// 
			// Ud0TextBox
			// 
			this->Ud0TextBox->Enabled = false;
			this->Ud0TextBox->Location = System::Drawing::Point(381, 99);
			this->Ud0TextBox->Name = L"Ud0TextBox";
			this->Ud0TextBox->Size = System::Drawing::Size(83, 22);
			this->Ud0TextBox->TabIndex = 12;
			this->Ud0TextBox->Text = L"1";
			// 
			// Ud0Label
			// 
			this->Ud0Label->AutoSize = true;
			this->Ud0Label->Enabled = false;
			this->Ud0Label->Location = System::Drawing::Point(349, 102);
			this->Ud0Label->Name = L"Ud0Label";
			this->Ud0Label->Size = System::Drawing::Size(26, 17);
			this->Ud0Label->TabIndex = 11;
			this->Ud0Label->Text = L"U\'₀";
			// 
			// XTextBox
			// 
			this->XTextBox->Location = System::Drawing::Point(146, 99);
			this->XTextBox->Name = L"XTextBox";
			this->XTextBox->Size = System::Drawing::Size(83, 22);
			this->XTextBox->TabIndex = 10;
			this->XTextBox->Text = L"1";
			// 
			// XLabel
			// 
			this->XLabel->AutoSize = true;
			this->XLabel->Location = System::Drawing::Point(119, 102);
			this->XLabel->Name = L"XLabel";
			this->XLabel->Size = System::Drawing::Size(17, 17);
			this->XLabel->TabIndex = 9;
			this->XLabel->Text = L"X";
			// 
			// U0TextBox
			// 
			this->U0TextBox->Location = System::Drawing::Point(264, 99);
			this->U0TextBox->Name = L"U0TextBox";
			this->U0TextBox->Size = System::Drawing::Size(79, 22);
			this->U0TextBox->TabIndex = 8;
			this->U0TextBox->Text = L"1";
			// 
			// X0TextBox
			// 
			this->X0TextBox->Location = System::Drawing::Point(34, 99);
			this->X0TextBox->Name = L"X0TextBox";
			this->X0TextBox->Size = System::Drawing::Size(79, 22);
			this->X0TextBox->TabIndex = 7;
			this->X0TextBox->Text = L"0";
			// 
			// U0Label
			// 
			this->U0Label->AutoSize = true;
			this->U0Label->Location = System::Drawing::Point(235, 102);
			this->U0Label->Name = L"U0Label";
			this->U0Label->Size = System::Drawing::Size(23, 17);
			this->U0Label->TabIndex = 6;
			this->U0Label->Text = L"U₀";
			// 
			// X0Label
			// 
			this->X0Label->AutoSize = true;
			this->X0Label->Location = System::Drawing::Point(6, 102);
			this->X0Label->Name = L"X0Label";
			this->X0Label->Size = System::Drawing::Size(22, 17);
			this->X0Label->TabIndex = 5;
			this->X0Label->Text = L"X₀";
			// 
			// ErrorCheckBox
			// 
			this->ErrorCheckBox->AutoSize = true;
			this->ErrorCheckBox->Location = System::Drawing::Point(6, 75);
			this->ErrorCheckBox->Name = L"ErrorCheckBox";
			this->ErrorCheckBox->Size = System::Drawing::Size(257, 21);
			this->ErrorCheckBox->TabIndex = 4;
			this->ErrorCheckBox->Text = L"Контроль локальной погрешности";
			this->ErrorCheckBox->UseVisualStyleBackColor = true;
			this->ErrorCheckBox->CheckedChanged += gcnew System::EventHandler(this, &MainForm::ErrorCheckBox_CheckedChanged);
			// 
			// StartButton
			// 
			this->StartButton->Font = (gcnew System::Drawing::Font(L"Microsoft Sans Serif", 13.8F, System::Drawing::FontStyle::Regular, System::Drawing::GraphicsUnit::Point,
				static_cast<System::Byte>(204)));
			this->StartButton->Location = System::Drawing::Point(297, 127);
			this->StartButton->Name = L"StartButton";
			this->StartButton->Size = System::Drawing::Size(127, 48);
			this->StartButton->TabIndex = 2;
			this->StartButton->Text = L"ЗАПУСК";
			this->StartButton->UseVisualStyleBackColor = true;
			this->StartButton->Click += gcnew System::EventHandler(this, &MainForm::StartButton_Click);
			// 
			// DuBox
			// 
			this->DuBox->Controls->Add(this->PictureDU);
			this->DuBox->Location = System::Drawing::Point(160, 3);
			this->DuBox->Name = L"DuBox";
			this->DuBox->Size = System::Drawing::Size(322, 66);
			this->DuBox->TabIndex = 1;
			this->DuBox->TabStop = false;
			this->DuBox->Text = L"ДУ";
			// 
			// PictureDU
			// 
			this->PictureDU->Dock = System::Windows::Forms::DockStyle::Fill;
			this->PictureDU->Location = System::Drawing::Point(3, 18);
			this->PictureDU->Name = L"PictureDU";
			this->PictureDU->Size = System::Drawing::Size(316, 45);
			this->PictureDU->SizeMode = System::Windows::Forms::PictureBoxSizeMode::StretchImage;
			this->PictureDU->TabIndex = 0;
			this->PictureDU->TabStop = false;
			// 
			// TaskBox
			// 
			this->TaskBox->Controls->Add(this->TaskComboBox);
			this->TaskBox->Location = System::Drawing::Point(3, 3);
			this->TaskBox->Name = L"TaskBox";
			this->TaskBox->Size = System::Drawing::Size(151, 66);
			this->TaskBox->TabIndex = 0;
			this->TaskBox->TabStop = false;
			this->TaskBox->Text = L"Задача";
			// 
			// TaskComboBox
			// 
			this->TaskComboBox->FormattingEnabled = true;
			this->TaskComboBox->Items->AddRange(gcnew cli::array< System::Object^  >(3) { L"Тестовая", L"Основная №1", L"Основная №2" });
			this->TaskComboBox->Location = System::Drawing::Point(12, 25);
			this->TaskComboBox->Name = L"TaskComboBox";
			this->TaskComboBox->Size = System::Drawing::Size(123, 24);
			this->TaskComboBox->TabIndex = 0;
			this->TaskComboBox->Text = L"Тестовая";
			this->TaskComboBox->SelectedIndexChanged += gcnew System::EventHandler(this, &MainForm::TaskComboBox_SelectedIndexChanged);
			// 
			// ParametersPanel
			// 
			this->ParametersPanel->BorderStyle = System::Windows::Forms::BorderStyle::FixedSingle;
			this->ParametersPanel->Controls->Add(this->StepTextBox);
			this->ParametersPanel->Controls->Add(this->CountMaxTextBox);
			this->ParametersPanel->Controls->Add(this->ErrorTextBox);
			this->ParametersPanel->Controls->Add(this->BorderTextBox);
			this->ParametersPanel->Controls->Add(this->StepLabel);
			this->ParametersPanel->Controls->Add(this->CountMaxLabel);
			this->ParametersPanel->Controls->Add(this->ParametrLabel);
			this->ParametersPanel->Controls->Add(this->BorderLabel);
			this->ParametersPanel->Dock = System::Windows::Forms::DockStyle::Fill;
			this->ParametersPanel->Location = System::Drawing::Point(509, 3);
			this->ParametersPanel->Name = L"ParametersPanel";
			this->ParametersPanel->Size = System::Drawing::Size(463, 189);
			this->ParametersPanel->TabIndex = 1;
			// 
			// StepTextBox
			// 
			this->StepTextBox->Location = System::Drawing::Point(314, 87);
			this->StepTextBox->Name = L"StepTextBox";
			this->StepTextBox->Size = System::Drawing::Size(144, 22);
			this->StepTextBox->TabIndex = 7;
			this->StepTextBox->Text = L"0.001";
			// 
			// CountMaxTextBox
			// 
			this->CountMaxTextBox->Location = System::Drawing::Point(314, 31);
			this->CountMaxTextBox->Name = L"CountMaxTextBox";
			this->CountMaxTextBox->Size = System::Drawing::Size(144, 22);
			this->CountMaxTextBox->TabIndex = 6;
			this->CountMaxTextBox->Text = L"10000";
			// 
			// ErrorTextBox
			// 
			this->ErrorTextBox->Enabled = false;
			this->ErrorTextBox->Location = System::Drawing::Point(314, 59);
			this->ErrorTextBox->Name = L"ErrorTextBox";
			this->ErrorTextBox->Size = System::Drawing::Size(144, 22);
			this->ErrorTextBox->TabIndex = 5;
			this->ErrorTextBox->Text = L"0.0000001";
			// 
			// BorderTextBox
			// 
			this->BorderTextBox->Location = System::Drawing::Point(314, 2);
			this->BorderTextBox->Name = L"BorderTextBox";
			this->BorderTextBox->Size = System::Drawing::Size(144, 22);
			this->BorderTextBox->TabIndex = 4;
			this->BorderTextBox->Text = L"0.00000001";
			// 
			// StepLabel
			// 
			this->StepLabel->AutoSize = true;
			this->StepLabel->Location = System::Drawing::Point(3, 90);
			this->StepLabel->Name = L"StepLabel";
			this->StepLabel->Size = System::Drawing::Size(111, 17);
			this->StepLabel->TabIndex = 3;
			this->StepLabel->Text = L"Начальный шаг";
			// 
			// CountMaxLabel
			// 
			this->CountMaxLabel->AutoSize = true;
			this->CountMaxLabel->Location = System::Drawing::Point(3, 34);
			this->CountMaxLabel->Name = L"CountMaxLabel";
			this->CountMaxLabel->Size = System::Drawing::Size(289, 17);
			this->CountMaxLabel->TabIndex = 2;
			this->CountMaxLabel->Text = L"Максимально допустимое число итераций";
			// 
			// ParametrLabel
			// 
			this->ParametrLabel->AutoSize = true;
			this->ParametrLabel->Location = System::Drawing::Point(3, 62);
			this->ParametrLabel->Name = L"ParametrLabel";
			this->ParametrLabel->Size = System::Drawing::Size(155, 17);
			this->ParametrLabel->TabIndex = 1;
			this->ParametrLabel->Text = L"Параметр контроля, ε";
			// 
			// BorderLabel
			// 
			this->BorderLabel->AutoSize = true;
			this->BorderLabel->Location = System::Drawing::Point(3, 5);
			this->BorderLabel->Name = L"BorderLabel";
			this->BorderLabel->Size = System::Drawing::Size(266, 17);
			this->BorderLabel->TabIndex = 0;
			this->BorderLabel->Text = L"Контроль выхода на правую границу, b";
			// 
			// ReferenceBox
			// 
			this->ReferenceBox->Controls->Add(this->PriX2Label);
			this->ReferenceBox->Controls->Add(this->MinHResLabel);
			this->ReferenceBox->Controls->Add(this->PriX3ResLabel);
			this->ReferenceBox->Controls->Add(this->PriX3Label);
			this->ReferenceBox->Controls->Add(this->MaxU_VResLabel);
			this->ReferenceBox->Controls->Add(this->MinHLabel);
			this->ReferenceBox->Controls->Add(this->PriX2ResLabel);
			this->ReferenceBox->Controls->Add(this->MaxHResLabel);
			this->ReferenceBox->Controls->Add(this->MaxU_VLabel);
			this->ReferenceBox->Controls->Add(this->MaxHLabel);
			this->ReferenceBox->Controls->Add(this->PriX1ResLabel);
			this->ReferenceBox->Controls->Add(this->PriX1Label);
			this->ReferenceBox->Controls->Add(this->C1ResLabel);
			this->ReferenceBox->Controls->Add(this->C1Label);
			this->ReferenceBox->Controls->Add(this->C2ResLabel);
			this->ReferenceBox->Controls->Add(this->C2Label);
			this->ReferenceBox->Controls->Add(this->MaxOLPResLabel);
			this->ReferenceBox->Controls->Add(this->MaxOLPLabel);
			this->ReferenceBox->Controls->Add(this->B_xnResLabel);
			this->ReferenceBox->Controls->Add(this->B_xnLabel);
			this->ReferenceBox->Controls->Add(this->NResLabel);
			this->ReferenceBox->Controls->Add(this->NLabel);
			this->ReferenceBox->Dock = System::Windows::Forms::DockStyle::Fill;
			this->ReferenceBox->Location = System::Drawing::Point(978, 3);
			this->ReferenceBox->Name = L"ReferenceBox";
			this->ReferenceBox->Size = System::Drawing::Size(477, 189);
			this->ReferenceBox->TabIndex = 0;
			this->ReferenceBox->TabStop = false;
			this->ReferenceBox->Text = L"Справка";
			// 
			// PriX2Label
			// 
			this->PriX2Label->AutoSize = true;
			this->PriX2Label->Location = System::Drawing::Point(228, 113);
			this->PriX2Label->Name = L"PriX2Label";
			this->PriX2Label->Size = System::Drawing::Size(54, 17);
			this->PriX2Label->TabIndex = 21;
			this->PriX2Label->Text = L"при x =";
			// 
			// MinHResLabel
			// 
			this->MinHResLabel->AutoSize = true;
			this->MinHResLabel->Location = System::Drawing::Point(69, 113);
			this->MinHResLabel->Name = L"MinHResLabel";
			this->MinHResLabel->Size = System::Drawing::Size(33, 17);
			this->MinHResLabel->TabIndex = 20;
			this->MinHResLabel->Text = L"*****";
			// 
			// PriX3ResLabel
			// 
			this->PriX3ResLabel->AutoSize = true;
			this->PriX3ResLabel->Location = System::Drawing::Point(338, 132);
			this->PriX3ResLabel->Name = L"PriX3ResLabel";
			this->PriX3ResLabel->Size = System::Drawing::Size(38, 17);
			this->PriX3ResLabel->TabIndex = 19;
			this->PriX3ResLabel->Text = L"******";
			// 
			// PriX3Label
			// 
			this->PriX3Label->AutoSize = true;
			this->PriX3Label->Location = System::Drawing::Point(278, 132);
			this->PriX3Label->Name = L"PriX3Label";
			this->PriX3Label->Size = System::Drawing::Size(54, 17);
			this->PriX3Label->TabIndex = 18;
			this->PriX3Label->Text = L"при x =";
			// 
			// MaxU_VResLabel
			// 
			this->MaxU_VResLabel->AutoSize = true;
			this->MaxU_VResLabel->Location = System::Drawing::Point(101, 132);
			this->MaxU_VResLabel->Name = L"MaxU_VResLabel";
			this->MaxU_VResLabel->Size = System::Drawing::Size(33, 17);
			this->MaxU_VResLabel->TabIndex = 17;
			this->MaxU_VResLabel->Text = L"*****";
			// 
			// MinHLabel
			// 
			this->MinHLabel->AutoSize = true;
			this->MinHLabel->Location = System::Drawing::Point(6, 113);
			this->MinHLabel->Name = L"MinHLabel";
			this->MinHLabel->Size = System::Drawing::Size(56, 17);
			this->MinHLabel->TabIndex = 16;
			this->MinHLabel->Text = L"min hᵢ =";
			// 
			// PriX2ResLabel
			// 
			this->PriX2ResLabel->AutoSize = true;
			this->PriX2ResLabel->Location = System::Drawing::Point(288, 113);
			this->PriX2ResLabel->Name = L"PriX2ResLabel";
			this->PriX2ResLabel->Size = System::Drawing::Size(33, 17);
			this->PriX2ResLabel->TabIndex = 15;
			this->PriX2ResLabel->Text = L"*****";
			// 
			// MaxHResLabel
			// 
			this->MaxHResLabel->AutoSize = true;
			this->MaxHResLabel->Location = System::Drawing::Point(72, 94);
			this->MaxHResLabel->Name = L"MaxHResLabel";
			this->MaxHResLabel->Size = System::Drawing::Size(33, 17);
			this->MaxHResLabel->TabIndex = 14;
			this->MaxHResLabel->Text = L"*****";
			// 
			// MaxU_VLabel
			// 
			this->MaxU_VLabel->AutoSize = true;
			this->MaxU_VLabel->Location = System::Drawing::Point(6, 132);
			this->MaxU_VLabel->Name = L"MaxU_VLabel";
			this->MaxU_VLabel->Size = System::Drawing::Size(87, 17);
			this->MaxU_VLabel->TabIndex = 13;
			this->MaxU_VLabel->Text = L"max|Uᵢ - Vᵢ| =";
			// 
			// MaxHLabel
			// 
			this->MaxHLabel->AutoSize = true;
			this->MaxHLabel->Location = System::Drawing::Point(6, 94);
			this->MaxHLabel->Name = L"MaxHLabel";
			this->MaxHLabel->Size = System::Drawing::Size(59, 17);
			this->MaxHLabel->TabIndex = 12;
			this->MaxHLabel->Text = L"max hᵢ =";
			// 
			// PriX1ResLabel
			// 
			this->PriX1ResLabel->AutoSize = true;
			this->PriX1ResLabel->Location = System::Drawing::Point(288, 94);
			this->PriX1ResLabel->Name = L"PriX1ResLabel";
			this->PriX1ResLabel->Size = System::Drawing::Size(33, 17);
			this->PriX1ResLabel->TabIndex = 11;
			this->PriX1ResLabel->Text = L"*****";
			// 
			// PriX1Label
			// 
			this->PriX1Label->AutoSize = true;
			this->PriX1Label->Location = System::Drawing::Point(228, 94);
			this->PriX1Label->Name = L"PriX1Label";
			this->PriX1Label->Size = System::Drawing::Size(54, 17);
			this->PriX1Label->TabIndex = 10;
			this->PriX1Label->Text = L"при x =";
			// 
			// C1ResLabel
			// 
			this->C1ResLabel->AutoSize = true;
			this->C1ResLabel->Location = System::Drawing::Point(210, 75);
			this->C1ResLabel->Name = L"C1ResLabel";
			this->C1ResLabel->Size = System::Drawing::Size(33, 17);
			this->C1ResLabel->TabIndex = 9;
			this->C1ResLabel->Text = L"*****";
			// 
			// C1Label
			// 
			this->C1Label->AutoSize = true;
			this->C1Label->Location = System::Drawing::Point(6, 75);
			this->C1Label->Name = L"C1Label";
			this->C1Label->Size = System::Drawing::Size(205, 17);
			this->C1Label->TabIndex = 8;
			this->C1Label->Text = L"Общее число делений шага =";
			// 
			// C2ResLabel
			// 
			this->C2ResLabel->AutoSize = true;
			this->C2ResLabel->Location = System::Drawing::Point(238, 56);
			this->C2ResLabel->Name = L"C2ResLabel";
			this->C2ResLabel->Size = System::Drawing::Size(33, 17);
			this->C2ResLabel->TabIndex = 7;
			this->C2ResLabel->Text = L"*****";
			// 
			// C2Label
			// 
			this->C2Label->AutoSize = true;
			this->C2Label->Location = System::Drawing::Point(6, 56);
			this->C2Label->Name = L"C2Label";
			this->C2Label->Size = System::Drawing::Size(226, 17);
			this->C2Label->TabIndex = 6;
			this->C2Label->Text = L"Общее число удваиваний шага =";
			// 
			// MaxOLPResLabel
			// 
			this->MaxOLPResLabel->AutoSize = true;
			this->MaxOLPResLabel->Location = System::Drawing::Point(94, 37);
			this->MaxOLPResLabel->Name = L"MaxOLPResLabel";
			this->MaxOLPResLabel->Size = System::Drawing::Size(33, 17);
			this->MaxOLPResLabel->TabIndex = 5;
			this->MaxOLPResLabel->Text = L"*****";
			// 
			// MaxOLPLabel
			// 
			this->MaxOLPLabel->AutoSize = true;
			this->MaxOLPLabel->Location = System::Drawing::Point(6, 37);
			this->MaxOLPLabel->Name = L"MaxOLPLabel";
			this->MaxOLPLabel->Size = System::Drawing::Size(82, 17);
			this->MaxOLPLabel->TabIndex = 4;
			this->MaxOLPLabel->Text = L"max|ОЛП| =";
			// 
			// B_xnResLabel
			// 
			this->B_xnResLabel->AutoSize = true;
			this->B_xnResLabel->Location = System::Drawing::Point(171, 18);
			this->B_xnResLabel->Name = L"B_xnResLabel";
			this->B_xnResLabel->Size = System::Drawing::Size(33, 17);
			this->B_xnResLabel->TabIndex = 3;
			this->B_xnResLabel->Text = L"*****";
			// 
			// B_xnLabel
			// 
			this->B_xnLabel->AutoSize = true;
			this->B_xnLabel->Location = System::Drawing::Point(107, 18);
			this->B_xnLabel->Name = L"B_xnLabel";
			this->B_xnLabel->Size = System::Drawing::Size(55, 17);
			this->B_xnLabel->TabIndex = 2;
			this->B_xnLabel->Text = L"b - Xₙ =";
			// 
			// NResLabel
			// 
			this->NResLabel->AutoSize = true;
			this->NResLabel->Location = System::Drawing::Point(40, 18);
			this->NResLabel->Name = L"NResLabel";
			this->NResLabel->Size = System::Drawing::Size(33, 17);
			this->NResLabel->TabIndex = 1;
			this->NResLabel->Text = L"*****";
			// 
			// NLabel
			// 
			this->NLabel->AutoSize = true;
			this->NLabel->Location = System::Drawing::Point(6, 18);
			this->NLabel->Name = L"NLabel";
			this->NLabel->Size = System::Drawing::Size(28, 17);
			this->NLabel->TabIndex = 0;
			this->NLabel->Text = L"n =";
			// 
			// Table
			// 
			this->Table->AllowUserToAddRows = false;
			this->Table->AllowUserToDeleteRows = false;
			this->Table->ColumnHeadersHeightSizeMode = System::Windows::Forms::DataGridViewColumnHeadersHeightSizeMode::AutoSize;
			this->Table->Columns->AddRange(gcnew cli::array< System::Windows::Forms::DataGridViewColumn^  >(17) {
				this->i, this->Xi, this->Vi,
					this->Vi1, this->Vi2, this->V2i, this->V2i1, this->V2i2, this->Vi_V2i, this->Vi1_V2i1, this->Vi2_V2i2, this->OLP, this->Hi, this->C1,
					this->C2, this->Ui, this->Ui_Vi
			});
			this->Table->Dock = System::Windows::Forms::DockStyle::Fill;
			this->Table->Location = System::Drawing::Point(0, 0);
			this->Table->Name = L"Table";
			this->Table->ReadOnly = true;
			this->Table->RowHeadersWidth = 51;
			this->Table->RowTemplate->Height = 24;
			this->Table->Size = System::Drawing::Size(796, 593);
			this->Table->TabIndex = 0;
			// 
			// i
			// 
			this->i->HeaderText = L"i";
			this->i->MinimumWidth = 6;
			this->i->Name = L"i";
			this->i->ReadOnly = true;
			this->i->Width = 125;
			// 
			// Xi
			// 
			this->Xi->HeaderText = L"Xᵢ";
			this->Xi->MinimumWidth = 6;
			this->Xi->Name = L"Xi";
			this->Xi->ReadOnly = true;
			this->Xi->Width = 145;
			// 
			// Vi
			// 
			this->Vi->HeaderText = L"Vᵢ";
			this->Vi->MinimumWidth = 6;
			this->Vi->Name = L"Vi";
			this->Vi->ReadOnly = true;
			this->Vi->Width = 145;
			// 
			// Vi1
			// 
			this->Vi1->HeaderText = L"Vᵢ1";
			this->Vi1->MinimumWidth = 6;
			this->Vi1->Name = L"Vi1";
			this->Vi1->ReadOnly = true;
			this->Vi1->Visible = false;
			this->Vi1->Width = 145;
			// 
			// Vi2
			// 
			this->Vi2->HeaderText = L"Vᵢ2";
			this->Vi2->MinimumWidth = 6;
			this->Vi2->Name = L"Vi2";
			this->Vi2->ReadOnly = true;
			this->Vi2->Visible = false;
			this->Vi2->Width = 145;
			// 
			// V2i
			// 
			this->V2i->HeaderText = L"V2ᵢ";
			this->V2i->MinimumWidth = 6;
			this->V2i->Name = L"V2i";
			this->V2i->ReadOnly = true;
			this->V2i->Width = 145;
			// 
			// V2i1
			// 
			this->V2i1->HeaderText = L"V2ᵢ1";
			this->V2i1->MinimumWidth = 6;
			this->V2i1->Name = L"V2i1";
			this->V2i1->ReadOnly = true;
			this->V2i1->Visible = false;
			this->V2i1->Width = 145;
			// 
			// V2i2
			// 
			this->V2i2->HeaderText = L"V2ᵢ2";
			this->V2i2->MinimumWidth = 6;
			this->V2i2->Name = L"V2i2";
			this->V2i2->ReadOnly = true;
			this->V2i2->Visible = false;
			this->V2i2->Width = 145;
			// 
			// Vi_V2i
			// 
			this->Vi_V2i->HeaderText = L"Vᵢ - V2ᵢ";
			this->Vi_V2i->MinimumWidth = 6;
			this->Vi_V2i->Name = L"Vi_V2i";
			this->Vi_V2i->ReadOnly = true;
			this->Vi_V2i->Width = 145;
			// 
			// Vi1_V2i1
			// 
			this->Vi1_V2i1->HeaderText = L"Vᵢ1-V2ᵢ1";
			this->Vi1_V2i1->MinimumWidth = 6;
			this->Vi1_V2i1->Name = L"Vi1_V2i1";
			this->Vi1_V2i1->ReadOnly = true;
			this->Vi1_V2i1->Visible = false;
			this->Vi1_V2i1->Width = 145;
			// 
			// Vi2_V2i2
			// 
			this->Vi2_V2i2->HeaderText = L"Vᵢ2-V2ᵢ2";
			this->Vi2_V2i2->MinimumWidth = 6;
			this->Vi2_V2i2->Name = L"Vi2_V2i2";
			this->Vi2_V2i2->ReadOnly = true;
			this->Vi2_V2i2->Visible = false;
			this->Vi2_V2i2->Width = 145;
			// 
			// OLP
			// 
			this->OLP->HeaderText = L"ОЛП";
			this->OLP->MinimumWidth = 6;
			this->OLP->Name = L"OLP";
			this->OLP->ReadOnly = true;
			this->OLP->Visible = false;
			this->OLP->Width = 145;
			// 
			// Hi
			// 
			this->Hi->HeaderText = L"hᵢ";
			this->Hi->MinimumWidth = 6;
			this->Hi->Name = L"Hi";
			this->Hi->ReadOnly = true;
			this->Hi->Visible = false;
			this->Hi->Width = 145;
			// 
			// C1
			// 
			this->C1->HeaderText = L"C1";
			this->C1->MinimumWidth = 6;
			this->C1->Name = L"C1";
			this->C1->ReadOnly = true;
			this->C1->Visible = false;
			this->C1->Width = 125;
			// 
			// C2
			// 
			this->C2->HeaderText = L"C2";
			this->C2->MinimumWidth = 6;
			this->C2->Name = L"C2";
			this->C2->ReadOnly = true;
			this->C2->Visible = false;
			this->C2->Width = 125;
			// 
			// Ui
			// 
			this->Ui->HeaderText = L"Uᵢ";
			this->Ui->MinimumWidth = 6;
			this->Ui->Name = L"Ui";
			this->Ui->ReadOnly = true;
			this->Ui->Width = 145;
			// 
			// Ui_Vi
			// 
			this->Ui_Vi->HeaderText = L"|Uᵢ - Vᵢ|";
			this->Ui_Vi->MinimumWidth = 6;
			this->Ui_Vi->Name = L"Ui_Vi";
			this->Ui_Vi->ReadOnly = true;
			this->Ui_Vi->Width = 145;
			// 
			// Chart
			// 
			chartArea1->AxisX->Title = L"X";
			chartArea1->AxisY->Title = L"V";
			chartArea1->Name = L"ChartArea1";
			this->Chart->ChartAreas->Add(chartArea1);
			this->Chart->Dock = System::Windows::Forms::DockStyle::Fill;
			legend1->Name = L"Legend1";
			this->Chart->Legends->Add(legend1);
			this->Chart->Location = System::Drawing::Point(0, 0);
			this->Chart->Name = L"Chart";
			series1->ChartArea = L"ChartArea1";
			series1->ChartType = System::Windows::Forms::DataVisualization::Charting::SeriesChartType::Line;
			series1->Color = System::Drawing::Color::Red;
			series1->Legend = L"Legend1";
			series1->Name = L"Истинное решение";
			series2->ChartArea = L"ChartArea1";
			series2->ChartType = System::Windows::Forms::DataVisualization::Charting::SeriesChartType::Line;
			series2->Color = System::Drawing::Color::Blue;
			series2->Legend = L"Legend1";
			series2->Name = L"Численное решение";
			this->Chart->Series->Add(series1);
			this->Chart->Series->Add(series2);
			this->Chart->Size = System::Drawing::Size(657, 593);
			this->Chart->TabIndex = 0;
			this->Chart->Text = L"Chart";
			// 
			// TCContainer
			// 
			this->TCContainer->Anchor = static_cast<System::Windows::Forms::AnchorStyles>((((System::Windows::Forms::AnchorStyles::Top | System::Windows::Forms::AnchorStyles::Bottom)
				| System::Windows::Forms::AnchorStyles::Left)
				| System::Windows::Forms::AnchorStyles::Right));
			this->TCContainer->Location = System::Drawing::Point(12, 207);
			this->TCContainer->Name = L"TCContainer";
			// 
			// TCContainer.Panel1
			// 
			this->TCContainer->Panel1->Controls->Add(this->Table);
			// 
			// TCContainer.Panel2
			// 
			this->TCContainer->Panel2->Controls->Add(this->ChartBox);
			this->TCContainer->Panel2->Controls->Add(this->Chart);
			this->TCContainer->Panel2->Controls->Add(this->Chart3);
			this->TCContainer->Panel2->Controls->Add(this->Chart2);
			this->TCContainer->Panel2->Controls->Add(this->Chart1);
			this->TCContainer->Size = System::Drawing::Size(1463, 593);
			this->TCContainer->SplitterDistance = 796;
			this->TCContainer->SplitterWidth = 10;
			this->TCContainer->TabIndex = 3;
			// 
			// ChartBox
			// 
			this->ChartBox->Anchor = static_cast<System::Windows::Forms::AnchorStyles>((System::Windows::Forms::AnchorStyles::Top | System::Windows::Forms::AnchorStyles::Right));
			this->ChartBox->Controls->Add(this->radioButton3);
			this->ChartBox->Controls->Add(this->radioButton2);
			this->ChartBox->Controls->Add(this->radioButton1);
			this->ChartBox->Location = System::Drawing::Point(469, 119);
			this->ChartBox->Name = L"ChartBox";
			this->ChartBox->Size = System::Drawing::Size(135, 100);
			this->ChartBox->TabIndex = 6;
			this->ChartBox->TabStop = false;
			this->ChartBox->Text = L"График";
			this->ChartBox->Visible = false;
			// 
			// radioButton3
			// 
			this->radioButton3->AutoSize = true;
			this->radioButton3->Location = System::Drawing::Point(6, 75);
			this->radioButton3->Name = L"radioButton3";
			this->radioButton3->Size = System::Drawing::Size(88, 21);
			this->radioButton3->TabIndex = 2;
			this->radioButton3->TabStop = true;
			this->radioButton3->Text = L"V\' = V\'(V)";
			this->radioButton3->UseVisualStyleBackColor = true;
			this->radioButton3->CheckedChanged += gcnew System::EventHandler(this, &MainForm::radioButton3_CheckedChanged);
			// 
			// radioButton2
			// 
			this->radioButton2->AutoSize = true;
			this->radioButton2->Location = System::Drawing::Point(6, 48);
			this->radioButton2->Name = L"radioButton2";
			this->radioButton2->Size = System::Drawing::Size(88, 21);
			this->radioButton2->TabIndex = 1;
			this->radioButton2->TabStop = true;
			this->radioButton2->Text = L"V\' = V\'(X)";
			this->radioButton2->UseVisualStyleBackColor = true;
			this->radioButton2->CheckedChanged += gcnew System::EventHandler(this, &MainForm::radioButton2_CheckedChanged);
			// 
			// radioButton1
			// 
			this->radioButton1->AutoSize = true;
			this->radioButton1->Checked = true;
			this->radioButton1->Location = System::Drawing::Point(6, 21);
			this->radioButton1->Name = L"radioButton1";
			this->radioButton1->Size = System::Drawing::Size(82, 21);
			this->radioButton1->TabIndex = 0;
			this->radioButton1->TabStop = true;
			this->radioButton1->Text = L"V = V(X)";
			this->radioButton1->UseVisualStyleBackColor = true;
			this->radioButton1->CheckedChanged += gcnew System::EventHandler(this, &MainForm::radioButton1_CheckedChanged);
			// 
			// Chart3
			// 
			chartArea2->AxisX->Title = L"V";
			chartArea2->AxisY->Title = L"V\'";
			chartArea2->Name = L"ChartArea1";
			this->Chart3->ChartAreas->Add(chartArea2);
			this->Chart3->Dock = System::Windows::Forms::DockStyle::Fill;
			legend2->Name = L"Legend1";
			this->Chart3->Legends->Add(legend2);
			this->Chart3->Location = System::Drawing::Point(0, 0);
			this->Chart3->Name = L"Chart3";
			series3->ChartArea = L"ChartArea1";
			series3->ChartType = System::Windows::Forms::DataVisualization::Charting::SeriesChartType::Line;
			series3->Legend = L"Legend1";
			series3->Name = L"Численное решение";
			this->Chart3->Series->Add(series3);
			this->Chart3->Size = System::Drawing::Size(657, 593);
			this->Chart3->TabIndex = 7;
			this->Chart3->Text = L"Chart3";
			this->Chart3->Visible = false;
			// 
			// Chart2
			// 
			chartArea3->AxisX->Title = L"X";
			chartArea3->AxisY->Title = L"V\'";
			chartArea3->Name = L"ChartArea1";
			this->Chart2->ChartAreas->Add(chartArea3);
			this->Chart2->Dock = System::Windows::Forms::DockStyle::Fill;
			legend3->Name = L"Legend1";
			this->Chart2->Legends->Add(legend3);
			this->Chart2->Location = System::Drawing::Point(0, 0);
			this->Chart2->Name = L"Chart2";
			series4->ChartArea = L"ChartArea1";
			series4->ChartType = System::Windows::Forms::DataVisualization::Charting::SeriesChartType::Line;
			series4->Legend = L"Legend1";
			series4->Name = L"Численное решение";
			this->Chart2->Series->Add(series4);
			this->Chart2->Size = System::Drawing::Size(657, 593);
			this->Chart2->TabIndex = 2;
			this->Chart2->Text = L"Chart2";
			this->Chart2->Visible = false;
			// 
			// Chart1
			// 
			chartArea4->AxisX->Title = L"X";
			chartArea4->AxisY->Title = L"V";
			chartArea4->Name = L"ChartArea1";
			this->Chart1->ChartAreas->Add(chartArea4);
			this->Chart1->Dock = System::Windows::Forms::DockStyle::Fill;
			legend4->Name = L"Legend1";
			this->Chart1->Legends->Add(legend4);
			this->Chart1->Location = System::Drawing::Point(0, 0);
			this->Chart1->Name = L"Chart1";
			series5->ChartArea = L"ChartArea1";
			series5->ChartType = System::Windows::Forms::DataVisualization::Charting::SeriesChartType::Line;
			series5->Legend = L"Legend1";
			series5->Name = L"Численное решение";
			series5->YValuesPerPoint = 2;
			this->Chart1->Series->Add(series5);
			this->Chart1->Size = System::Drawing::Size(657, 593);
			this->Chart1->TabIndex = 1;
			this->Chart1->Text = L"Chart1";
			this->Chart1->Visible = false;
			// 
			// MainTableLayoutPanel
			// 
			this->MainTableLayoutPanel->Anchor = static_cast<System::Windows::Forms::AnchorStyles>(((System::Windows::Forms::AnchorStyles::Top | System::Windows::Forms::AnchorStyles::Left)
				| System::Windows::Forms::AnchorStyles::Right));
			this->MainTableLayoutPanel->ColumnCount = 3;
			this->MainTableLayoutPanel->ColumnStyles->Add((gcnew System::Windows::Forms::ColumnStyle(System::Windows::Forms::SizeType::Percent,
				34.71075F)));
			this->MainTableLayoutPanel->ColumnStyles->Add((gcnew System::Windows::Forms::ColumnStyle(System::Windows::Forms::SizeType::Percent,
				32.23141F)));
			this->MainTableLayoutPanel->ColumnStyles->Add((gcnew System::Windows::Forms::ColumnStyle(System::Windows::Forms::SizeType::Percent,
				33.05785F)));
			this->MainTableLayoutPanel->Controls->Add(this->ReferenceBox, 2, 0);
			this->MainTableLayoutPanel->Controls->Add(this->ParametersPanel, 1, 0);
			this->MainTableLayoutPanel->Controls->Add(this->MainPanel, 0, 0);
			this->MainTableLayoutPanel->Location = System::Drawing::Point(12, 9);
			this->MainTableLayoutPanel->Name = L"MainTableLayoutPanel";
			this->MainTableLayoutPanel->RowCount = 1;
			this->MainTableLayoutPanel->RowStyles->Add((gcnew System::Windows::Forms::RowStyle(System::Windows::Forms::SizeType::Percent,
				100)));
			this->MainTableLayoutPanel->Size = System::Drawing::Size(1458, 195);
			this->MainTableLayoutPanel->TabIndex = 0;
			// 
			// MainForm
			// 
			this->AutoScaleDimensions = System::Drawing::SizeF(120, 120);
			this->AutoScaleMode = System::Windows::Forms::AutoScaleMode::Dpi;
			this->BackgroundImageLayout = System::Windows::Forms::ImageLayout::None;
			this->ClientSize = System::Drawing::Size(1482, 803);
			this->Controls->Add(this->MainTableLayoutPanel);
			this->Controls->Add(this->TCContainer);
			this->DoubleBuffered = true;
			this->Name = L"MainForm";
			this->StartPosition = System::Windows::Forms::FormStartPosition::CenterScreen;
			this->Text = L"Задача Коши для ОДУ. Вариант №3";
			this->MainPanel->ResumeLayout(false);
			this->MainPanel->PerformLayout();
			this->DuBox->ResumeLayout(false);
			(cli::safe_cast<System::ComponentModel::ISupportInitialize^>(this->PictureDU))->EndInit();
			this->TaskBox->ResumeLayout(false);
			this->ParametersPanel->ResumeLayout(false);
			this->ParametersPanel->PerformLayout();
			this->ReferenceBox->ResumeLayout(false);
			this->ReferenceBox->PerformLayout();
			(cli::safe_cast<System::ComponentModel::ISupportInitialize^>(this->Table))->EndInit();
			(cli::safe_cast<System::ComponentModel::ISupportInitialize^>(this->Chart))->EndInit();
			this->TCContainer->Panel1->ResumeLayout(false);
			this->TCContainer->Panel2->ResumeLayout(false);
			(cli::safe_cast<System::ComponentModel::ISupportInitialize^>(this->TCContainer))->EndInit();
			this->TCContainer->ResumeLayout(false);
			this->ChartBox->ResumeLayout(false);
			this->ChartBox->PerformLayout();
			(cli::safe_cast<System::ComponentModel::ISupportInitialize^>(this->Chart3))->EndInit();
			(cli::safe_cast<System::ComponentModel::ISupportInitialize^>(this->Chart2))->EndInit();
			(cli::safe_cast<System::ComponentModel::ISupportInitialize^>(this->Chart1))->EndInit();
			this->MainTableLayoutPanel->ResumeLayout(false);
			this->ResumeLayout(false);

		}
#pragma endregion

	void clearHelp() {
		NResLabel->Text = "     ";
		B_xnResLabel->Text = "     ";
		MaxOLPResLabel->Text = "     ";
		C1ResLabel->Text = "     ";
		C2ResLabel->Text = "     ";
		MaxHResLabel->Text = "     ";
		PriX1ResLabel->Text = "     ";
		MinHResLabel->Text = "     ";
		PriX2ResLabel->Text = "     ";
		MaxU_VResLabel->Text = "     ";
		PriX3ResLabel->Text = "     ";
	}

	void clearTable() {
		for (int i = 0; i < Table->Rows->Count; i++) {
			for (int j = 0; j < 17; j++) {
				Table->Rows[i]->Cells[j]->Value = "";
			}
		}
	}

	void clearChart() {
		Chart->Series["Истинное решение"]->Points->Clear();
		Chart->Series["Численное решение"]->Points->Clear();
		Chart1->Series["Численное решение"]->Points->Clear();
		Chart2->Series["Численное решение"]->Points->Clear();
		Chart3->Series["Численное решение"]->Points->Clear();
	}

	private: System::Void StartButton_Click(System::Object^ sender, System::EventArgs^ e) {
		clearChart();
		clearHelp();
		clearTable();

		if (!Double::TryParse(X0TextBox->Text, System::Globalization::NumberStyles::Float, System::Globalization::CultureInfo::InvariantCulture, x0)) {
			MessageBox::Show("X0 не число", "Ошибка", MessageBoxButtons::OK, MessageBoxIcon::Error,
				MessageBoxDefaultButton::Button1, MessageBoxOptions::RightAlign);
			return;
		}
		if (!Double::TryParse(U0TextBox->Text, System::Globalization::NumberStyles::Float, System::Globalization::CultureInfo::InvariantCulture, u0)) {
			MessageBox::Show("U0 не число", "Ошибка", MessageBoxButtons::OK, MessageBoxIcon::Error,
				MessageBoxDefaultButton::Button1, MessageBoxOptions::RightAlign);
			return;
		}
		if (!Double::TryParse(XTextBox->Text, System::Globalization::NumberStyles::Float, System::Globalization::CultureInfo::InvariantCulture, x)) {
			MessageBox::Show("X не число", "Ошибка", MessageBoxButtons::OK, MessageBoxIcon::Error,
				MessageBoxDefaultButton::Button1, MessageBoxOptions::RightAlign);
			return;
		} else if (x <= x0) {
			MessageBox::Show("X меньше либо равен X0", "Ошибка", MessageBoxButtons::OK, MessageBoxIcon::Error,
				MessageBoxDefaultButton::Button1, MessageBoxOptions::RightAlign);
			return;
		}
		if (!Double::TryParse(BorderTextBox->Text, System::Globalization::NumberStyles::Float, System::Globalization::CultureInfo::InvariantCulture, b)) {
			MessageBox::Show("Параметр контроля выхода на правую границу не число", "Ошибка", MessageBoxButtons::OK, MessageBoxIcon::Error,
				MessageBoxDefaultButton::Button1, MessageBoxOptions::RightAlign);
			return;
		} else if (b < 0) {
			MessageBox::Show("Параметр контроля выхода на правую границу меньше нуля", "Ошибка", MessageBoxButtons::OK, MessageBoxIcon::Error,
				MessageBoxDefaultButton::Button1, MessageBoxOptions::RightAlign);
			return;
		}
		if (!Double::TryParse(StepTextBox->Text, System::Globalization::NumberStyles::Float, System::Globalization::CultureInfo::InvariantCulture, h)) {
			MessageBox::Show("Начальный шаг не число", "Ошибка", MessageBoxButtons::OK, MessageBoxIcon::Error,
				MessageBoxDefaultButton::Button1, MessageBoxOptions::RightAlign);
			return;
		} else if (h <= 0) {
			MessageBox::Show("Начальный шаг меньше либо равен нулю", "Ошибка", MessageBoxButtons::OK, MessageBoxIcon::Error,
				MessageBoxDefaultButton::Button1, MessageBoxOptions::RightAlign);
			return;
		}
		if (!Int32::TryParse(CountMaxTextBox->Text, Nmax)) {
			MessageBox::Show("Максимально допустимое число итераций не число", "Ошибка", MessageBoxButtons::OK, MessageBoxIcon::Error,
				MessageBoxDefaultButton::Button1, MessageBoxOptions::RightAlign);
			return;
		} else if (Nmax <= 0) {
			MessageBox::Show("Максимально допустимое число итераций меньше либо равно нулю", "Ошибка", MessageBoxButtons::OK, MessageBoxIcon::Error,
				MessageBoxDefaultButton::Button1, MessageBoxOptions::RightAlign);
			return;
		}
		local_error_control = ErrorCheckBox->Checked;
		if (local_error_control) {
			if (!Double::TryParse(ErrorTextBox->Text, System::Globalization::NumberStyles::Float, System::Globalization::CultureInfo::InvariantCulture, eps)) {
				MessageBox::Show("Параметр контроля не число", "Ошибка", MessageBoxButtons::OK, MessageBoxIcon::Error,
					MessageBoxDefaultButton::Button1, MessageBoxOptions::RightAlign);
				return;
			} else if (eps <= 0) {
				MessageBox::Show("Параметр контроля меньше либо равен нулю", "Ошибка", MessageBoxButtons::OK, MessageBoxIcon::Error,
					MessageBoxDefaultButton::Button1, MessageBoxOptions::RightAlign);
				return;
			}
		}
		if (TaskComboBox->Text == "Основная №2") {
			if (!Double::TryParse(Ud0TextBox->Text, System::Globalization::NumberStyles::Float, System::Globalization::CultureInfo::InvariantCulture, ud0)) {
				MessageBox::Show("U'0 не число", "Ошибка", MessageBoxButtons::OK, MessageBoxIcon::Error,
					MessageBoxDefaultButton::Button1, MessageBoxOptions::RightAlign);
				return;
			}
			if (!Double::TryParse(ATextBox->Text, System::Globalization::NumberStyles::Float, System::Globalization::CultureInfo::InvariantCulture, A)) {
				MessageBox::Show("a не число", "Ошибка", MessageBoxButtons::OK, MessageBoxIcon::Error,
					MessageBoxDefaultButton::Button1, MessageBoxOptions::RightAlign);
				return;
			}
			if (!Double::TryParse(BTextBox->Text, System::Globalization::NumberStyles::Float, System::Globalization::CultureInfo::InvariantCulture, B)) {
				MessageBox::Show("b не число", "Ошибка", MessageBoxButtons::OK, MessageBoxIcon::Error,
					MessageBoxDefaultButton::Button1, MessageBoxOptions::RightAlign);
				return;
			}
		}

		if (Nmax + 1 > currSizeArray) {
			currSizeArray = Nmax + 1;
			delete[] X; X = new double[currSizeArray];
			delete[] V; V = new double[currSizeArray];
			delete[] V2; V2 = new double[currSizeArray];
			if (local_error_control) {
				delete[] OLP_Arr; OLP_Arr = new double[currSizeArray];
				delete[] H; H = new double[currSizeArray];
				delete[] C1_Arr; C1_Arr = new int[currSizeArray];
				delete[] C2_Arr; C2_Arr = new int[currSizeArray];
			}
			if (TaskComboBox->Text == "Тестовая") {
				delete[] U; U = new double[currSizeArray];
				delete[] U_V; U_V = new double[currSizeArray];
			}
			if (TaskComboBox->Text == "Основная №2") {
				delete[] V_1; V_1 = new double[currSizeArray];
				delete[] V2_1; V2_1 = new double[currSizeArray];
			}
		}
		
		if (TaskComboBox->Text == "Тестовая") {
			if (local_error_control) {
				N = rk4_method_test(h, Nmax, x0, u0, x, eps, b, &f_test, &func_test, X, V, V2, OLP_Arr, H, C1_Arr, C2_Arr, U, U_V);
			} else {
				N = rk4_method_test(h, Nmax, x0, u0, x, b, &f_test, &func_test, X, V, V2, U, U_V);
			}
			//рисование графика
			for (double i = x0; i < x; i += 0.001) {
				Chart->Series["Истинное решение"]->Points->AddXY(i, func_test(i));
			}

			// заполнение последней строки справки
			int mInd = max(U_V, N);
			MaxU_VResLabel->Text = U_V[mInd].ToString();
			PriX3ResLabel->Text = X[mInd].ToString();

		} else if (TaskComboBox->Text == "Основная №1") {
			if (local_error_control) {
				N = rk4_method(h, Nmax, x0, u0, x, eps, b, &f, X, V, V2, OLP_Arr, H, C1_Arr, C2_Arr);
			} else {
				N = rk4_method(h, Nmax, x0, u0, x, b, &f, X, V, V2);
			}
		} else if (TaskComboBox->Text == "Основная №2") {
			if (local_error_control) {
				N = rk4_method(h, Nmax, x0, u0, ud0, x, eps, b, &f1, &f2, X, V, V_1, V2, V2_1, OLP_Arr, H, C1_Arr, C2_Arr);
			} else {
				N = rk4_method(h, Nmax, x0, u0, ud0, x, b, &f1, &f2, X, V, V_1, V2, V2_1);
			}
		}

		//рисование графика
		if (TaskComboBox->Text == "Тестовая") {
			for (int i = 0; i < N + 1; i++) {
				if (!Double::IsNaN(V[i]) && !Double::IsNaN(X[i]) && rangeOfDec(V[i]) && rangeOfDec(X[i])) {
					Chart->Series["Численное решение"]->Points->AddXY(X[i], V[i]);
				}
			}
		} else {
			for (int i = 0; i < N + 1; i++) {
				if (!Double::IsNaN(V[i]) && !Double::IsNaN(X[i]) && rangeOfDec(V[i]) && rangeOfDec(X[i])) {
					Chart1->Series["Численное решение"]->Points->AddXY(X[i], V[i]);
				}
			}
			if (TaskComboBox->Text == "Основная №2") {
				for (int i = 0; i < N + 1; i++) {
					if (!Double::IsNaN(V_1[i]) && !Double::IsNaN(X[i]) && rangeOfDec(V_1[i]) && rangeOfDec(X[i])) {
						Chart2->Series["Численное решение"]->Points->AddXY(X[i], V_1[i]);
					}
				}
				for (int i = 0; i < N + 1; i++) {
					if (!Double::IsNaN(V[i]) && !Double::IsNaN(V_1[i]) && rangeOfDec(V[i]) && rangeOfDec(V_1[i])) {
						Chart3->Series["Численное решение"]->Points->AddXY(V[i], V_1[i]);
					}
				}
			}
		}

		//заполнение таблицы
		if (Table->Rows->Count < N + 1) {
			int k = N + 1 - Table->Rows->Count;
			for (int i = 0; i < k; i++) {
				Table->Rows->Add();
			}
		} else if (Table->Rows->Count > 2 * N) {
			int k = Table->Rows->Count - 1;
			for (int i = k; i > N; i--) {
				Table->Rows->RemoveAt(i);
			}
		}
		Table->Rows[0]->Cells[0]->Value = "0";
		Table->Rows[0]->Cells[1]->Value = X[0].ToString();
		if (TaskComboBox->Text == "Основная №2") {
			Table->Rows[0]->Cells[3]->Value = V[0].ToString();
			Table->Rows[0]->Cells[4]->Value = V_1[0].ToString();
		} else {
			Table->Rows[0]->Cells[2]->Value = V[0].ToString();
		}
		for (int i = 1; i < N + 1; i++) {
			Table->Rows[i]->Cells[0]->Value = i.ToString();
			Table->Rows[i]->Cells[1]->Value = X[i].ToString();
			if (TaskComboBox->Text == "Основная №2") {
				Table->Rows[i]->Cells[3]->Value = V[i].ToString();
				Table->Rows[i]->Cells[4]->Value = V_1[i].ToString();
				Table->Rows[i]->Cells[6]->Value = V2[i].ToString();
				Table->Rows[i]->Cells[7]->Value = V2_1[i].ToString();
				Table->Rows[i]->Cells[9]->Value = (V[i] - V2[i]).ToString();
				Table->Rows[i]->Cells[10]->Value = (V_1[i] - V2_1[i]).ToString();
			} else {
				Table->Rows[i]->Cells[0]->Value = i.ToString();
				Table->Rows[i]->Cells[1]->Value = X[i].ToString();
				Table->Rows[i]->Cells[2]->Value = V[i].ToString();
				Table->Rows[i]->Cells[5]->Value = V2[i].ToString();
				Table->Rows[i]->Cells[8]->Value = (V[i] - V2[i]).ToString();
			}
			if (TaskComboBox->Text == "Тестовая") {
				Table->Rows[i]->Cells[15]->Value = U[i].ToString();
				Table->Rows[i]->Cells[16]->Value = abs(U_V[i]).ToString();
			}
			if (local_error_control) {
				Table->Rows[i]->Cells[11]->Value = OLP_Arr[i].ToString();
				Table->Rows[i]->Cells[12]->Value = H[i - 1].ToString();
				Table->Rows[i]->Cells[13]->Value = C1_Arr[i].ToString();
				Table->Rows[i]->Cells[14]->Value = C2_Arr[i].ToString();
			}
		}

		//Заполнение справки
		NResLabel->Text = N.ToString();
		B_xnResLabel->Text = (b - X[N]).ToString();
		if (local_error_control) {
			C1ResLabel->Text = sum(C1_Arr, N + 1).ToString();
			C2ResLabel->Text = sum(C2_Arr, N + 1).ToString();
			abs_arr(OLP_Arr, N + 1);
			MaxOLPResLabel->Text = OLP_Arr[max(OLP_Arr, N + 1)].ToString();
			int mInd = max(H, N + 1);
			MaxHResLabel->Text = H[mInd].ToString();
			PriX1ResLabel->Text = X[mInd].ToString();
			mInd = min(H, N + 1);
			MinHResLabel->Text = H[mInd].ToString();
			PriX2ResLabel->Text = X[mInd].ToString();
		}
	}

	private: System::Void TaskComboBox_SelectedIndexChanged(System::Object^ sender, System::EventArgs^ e) {
		if (TaskComboBox->Text == "Тестовая") {
			if (strTask != TaskComboBox->Text) {
				// Картинка задания
				PictureDU->Image = (cli::safe_cast<Drawing::Bitmap^>(s_pxResourceManager->GetObject("DU_test")));
			
				// Строка в справке
				MaxU_VLabel->Visible = true; MaxU_VResLabel->Visible = true;
				PriX3Label->Visible = true; PriX3ResLabel->Visible = true;

				// Два столбца в таблице
				Ui->Visible = true; Ui_Vi->Visible = true;

				// График
				Chart->Visible = true;
			}
		} else {
			// Строка в справке
			MaxU_VLabel->Visible = false; MaxU_VResLabel->Visible = false;
			PriX3Label->Visible = false; PriX3ResLabel->Visible = false;

			// Два столбца в таблице
			Ui->Visible = false; Ui_Vi->Visible = false;

			// График
			Chart->Visible = false;
		}
		if (TaskComboBox->Text == "Основная №1") {
			// Картинка задания
			PictureDU->Image = (cli::safe_cast<Drawing::Bitmap^>(s_pxResourceManager->GetObject("DU_1")));

			// График
			Chart1->Visible = true;
		} else {
			// График
			Chart1->Visible = false;
		}
		if (TaskComboBox->Text == "Основная №2") {
			if (TaskComboBox->Text != strTask) {
				// Картинка задания
				PictureDU->Image = (cli::safe_cast<Drawing::Bitmap^>(s_pxResourceManager->GetObject("DU_2")));

				// Дополнительные параметры для системы
				Ud0Label->Enabled = true; Ud0TextBox->Enabled = true;
				ALabel->Enabled = true; ATextBox->Enabled = true;
				BLabel->Enabled = true; BTextBox->Enabled = true;

				// Дополнительные столбцы
				Vi1->Visible = true; Vi2->Visible = true;
				V2i1->Visible = true; V2i2->Visible = true;
				Vi1_V2i1->Visible = true; Vi2_V2i2->Visible = true;

				// Убрать ненужные столбцы
				Vi->Visible = false;
				V2i->Visible = false;
				Vi_V2i->Visible = false;

				// График
				Chart1->Visible = true;

				// Показать панель переключения
				ChartBox->Visible = true;
			}
		} else {
			// Дополнительные параметры для системы
			Ud0Label->Enabled = false; Ud0TextBox->Enabled = false;
			ALabel->Enabled = false; ATextBox->Enabled = false;
			BLabel->Enabled = false; BTextBox->Enabled = false;

			// Дополнительные столбцы
			Vi1->Visible = false; Vi2->Visible = false;
			V2i1->Visible = false; V2i2->Visible = false;
			Vi1_V2i1->Visible = false; Vi2_V2i2->Visible = false;

			// Вернуть ненужные столбцы
			Vi->Visible = true;
			V2i->Visible = true;
			Vi_V2i->Visible = true;

			// График
			Chart2->Visible = false;
			Chart3->Visible = false;

			// Убрать панель переключения
			ChartBox->Visible = false;
		}

		if (strTask != TaskComboBox->Text) {
			clearChart();
			clearHelp();
			clearTable();

			strTask = TaskComboBox->Text;
		}
	}

	private: System::Void ErrorCheckBox_CheckedChanged(System::Object^ sender, System::EventArgs^ e) {
		bool b = ErrorCheckBox->Checked;
		ErrorTextBox->Enabled = b;
		OLP->Visible = b;
		Hi->Visible = b;
		C1->Visible = b;
		C2->Visible = b;
	}
	private: System::Void radioButton1_CheckedChanged(System::Object^ sender, System::EventArgs^ e) {
		Chart1->Visible = true;
		Chart2->Visible = false;
		Chart3->Visible = false;
	}
	private: System::Void radioButton2_CheckedChanged(System::Object^ sender, System::EventArgs^ e) {
		Chart1->Visible = false;
		Chart2->Visible = true;
		Chart3->Visible = false;
	}
	private: System::Void radioButton3_CheckedChanged(System::Object^ sender, System::EventArgs^ e) {
		Chart1->Visible = false;
		Chart2->Visible = false;
		Chart3->Visible = true;
	}
};
}
