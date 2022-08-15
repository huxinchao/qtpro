#include "adddata.h"
#include <QPushButton>
#include <QLayout>
#include <QLabel>
#include <QLineEdit>
#include <QDebug>
#include <QCloseEvent>
#include <QComboBox>
#include <QSpacerItem>
#include <QFileDialog>

#define CC(x) QString::fromLocal8Bit(x)

extern QSet<QString> DEV_TYPE_SET;
extern QSet<QString> ELETRIC_TYPE_SET;
extern QSet<QString> WAVE_TYPE_SET;

extern QMap<int, int> DEV_FACTORY_MAP;
extern QMap<int, QString> DEV_ID_MAP;
extern QMap<int, QString> FACTORY_ID_MAP;

AddData::AddData(int type, QWidget *parent) :
	QMainWindow(parent), local_type(type)
{
	ui.setupUi(this);
	this->setAttribute(Qt::WA_DeleteOnClose);
	this->setWindowModality(Qt::WindowModal);
	this->setWindowFlags(Qt::Dialog);
	
	QStringList labelList;
	
	if (type == Device_Type) {
		labelList << CC("厂家代码:       ") << CC("设备类型:       ") << CC("额定功耗(KW):") << CC("电气类型:       ");
		this->setWindowTitle(CC("设备型号表-添加数据"));
		for (int i = 0; i < 4; i++) {
			if (i == 0) {
				QHBoxLayout *layout = new QHBoxLayout();
				QLabel *label_prefix = new QLabel(labelList[0], this);
				layout->addWidget(label_prefix);
				QComboBox *comboBox = new QComboBox(this);
				QSizePolicy sp = comboBox->sizePolicy();
				sp.setHorizontalPolicy(QSizePolicy::Expanding);
				comboBox->setSizePolicy(sp);

				for (auto ite = FACTORY_ID_MAP.begin(); ite != FACTORY_ID_MAP.end(); ite++) {
					QString f_name = ite.value();
					int f_id = ite.key();
					comboBox->addItem(f_name + "(" + QString::number(f_id) + ")", f_id);
				}
				layout->addWidget(comboBox);
				combobox_vec.push_back(comboBox);
				ui.verticalLayout_input->addLayout(layout);
			}

			if (i == 1) {
				QHBoxLayout *layout = new QHBoxLayout();
				QLabel *label_prefix = new QLabel(labelList[1], this);
				layout->addWidget(label_prefix);

				QComboBox *comboBox = new QComboBox(this);
				QSizePolicy sp = comboBox->sizePolicy();
				sp.setHorizontalPolicy(QSizePolicy::Expanding);
				comboBox->setSizePolicy(sp);
				comboBox->setEditable(true);


				for (auto ite = DEV_TYPE_SET.begin(); ite != DEV_TYPE_SET.end(); ite++) {
					QString d_type = (*ite);
					comboBox->addItem(d_type);
				}
				layout->addWidget(comboBox);
				combobox_vec.push_back(comboBox);
				ui.verticalLayout_input->addLayout(layout);
			}

			if (i == 2) {
				QHBoxLayout *layout = new QHBoxLayout();
				QLabel *label_prefix = new QLabel(labelList[2], this);
				QLineEdit *lineedit = new QLineEdit(this);
				QDoubleValidator *validator = new QDoubleValidator(this);
				lineedit->setValidator(validator);

				layout->addWidget(label_prefix);
				layout->addWidget(lineedit);
				lineedit_vec.push_back(lineedit);
				ui.verticalLayout_input->addLayout(layout);
			}

			if (i == 3) {
				QHBoxLayout *layout = new QHBoxLayout();
				QLabel *label_prefix = new QLabel(labelList[3], this);
				layout->addWidget(label_prefix);

				QComboBox *comboBox = new QComboBox(this);
				QSizePolicy sp = comboBox->sizePolicy();
				sp.setHorizontalPolicy(QSizePolicy::Expanding);
				comboBox->setSizePolicy(sp);
				comboBox->setEditable(true);

				for (auto ite = ELETRIC_TYPE_SET.begin(); ite != ELETRIC_TYPE_SET.end(); ite++) {
					QString d_etype = (*ite);
					comboBox->addItem(d_etype);
				}

				layout->addWidget(comboBox);
				combobox_vec.push_back(comboBox);
				ui.verticalLayout_input->addLayout(layout);

			}
		}
	}
	if (type == Wave_Feature) {
		labelList << CC("设备类型:") << CC("波形类型:") << CC("记录通道:");
		this->setWindowTitle(CC("波形表-添加数据"));
		for (int i = 0; i < 3; i++) {
			if (i == 0) {
				QHBoxLayout *layout = new QHBoxLayout();
				QLabel *label_prefix = new QLabel(labelList[0], this);
				layout->addWidget(label_prefix);
				QComboBox *comboBox = new QComboBox(this);
				QSizePolicy sp = comboBox->sizePolicy();
				sp.setHorizontalPolicy(QSizePolicy::Expanding);
				comboBox->setSizePolicy(sp);
				for (int d_id = 1; d_id <= DEV_ID_MAP.size(); d_id++) {
					int f_id = DEV_FACTORY_MAP[d_id];
					QString d_type = DEV_ID_MAP[d_id];
					QString f_name = FACTORY_ID_MAP[f_id];
					QString dev_str = QString::number(d_id) + "(" + f_name + "," + d_type + ")";
					comboBox->addItem(dev_str, d_id);
				}
				layout->addWidget(comboBox);
				combobox_vec.push_back(comboBox);
				ui.verticalLayout_input->addLayout(layout);
			}

			if (i == 1) {
				QHBoxLayout *layout = new QHBoxLayout();
				QLabel *label_prefix = new QLabel(labelList[1], this);
				layout->addWidget(label_prefix);
				QComboBox *comboBox = new QComboBox(this);
				comboBox->setEditable(true);
				QSizePolicy sp = comboBox->sizePolicy();
				sp.setHorizontalPolicy(QSizePolicy::Expanding);
				comboBox->setSizePolicy(sp);
				for (auto ite = WAVE_TYPE_SET.begin(); ite != WAVE_TYPE_SET.end(); ite++) {
					comboBox->addItem(*ite);
				}
				layout->addWidget(comboBox);
				combobox_vec.push_back(comboBox);
				ui.verticalLayout_input->addLayout(layout);
			}

			if (i == 2) {
				QHBoxLayout *layout = new QHBoxLayout();
				QLabel *label_prefix = new QLabel(labelList[2], this);
				layout->addWidget(label_prefix);
				QLineEdit *lineedit = new QLineEdit(this);
				lineedit->setValidator(new QIntValidator(this));
				layout->addWidget(lineedit);
				lineedit_vec.push_back(lineedit);
				ui.verticalLayout_input->addLayout(layout);
			}
		}
		AddUploadFileLayout();
	}
	if (type == Factory) {
		labelList << CC("厂家名称:      ") << CC("厂家经营范围:");
		this->setWindowTitle(CC("厂家表-添加数据"));
		for (int i = 0; i < 2; i++) {
			QHBoxLayout *layout = new QHBoxLayout();
			QLabel *label_prefix = new QLabel(labelList[i], this);
			QLineEdit *lineedit = new QLineEdit(this);
			layout->addWidget(label_prefix);
			layout->addWidget(lineedit);
			lineedit_vec.push_back(lineedit);
			ui.verticalLayout_input->addLayout(layout);
		}
	}
	if (type == Import_File) {
		this->setWindowTitle(CC("波形表-导入文件"));
		QHBoxLayout *layout = new QHBoxLayout();
		QLabel *label_prefix = new QLabel(CC("文件:"), this);
		QLabel *label_filePath = new QLabel("/", this);

		layout->addWidget(label_prefix);
		layout->addWidget(label_filePath);

		layout->addSpacerItem(new QSpacerItem(20, 20, QSizePolicy::Expanding));

		QPushButton *button_select = new QPushButton(CC("选择"), this);

		connect(button_select, SIGNAL(clicked()), this, SLOT(on_import_selectFile()));
		layout->addWidget(button_select);
		ui.verticalLayout_input->addLayout(layout);
	}
	if (type == Export_File) {
		this->setWindowTitle(CC("波形表-导出文件"));
		QHBoxLayout *layout = new QHBoxLayout();
		QLabel *label_prefix = new QLabel(CC("输出目录:"), this);
		QLabel *label_filePath = new QLabel("C:/", this);

		layout->addWidget(label_prefix);
		layout->addWidget(label_filePath);

		layout->addSpacerItem(new QSpacerItem(20, 20, QSizePolicy::Expanding));

		QPushButton *button_select = new QPushButton(CC("选择"), this);

		connect(button_select, SIGNAL(clicked()), this, SLOT(on_export_selectDir()));
		layout->addWidget(button_select);
		ui.verticalLayout_input->addLayout(layout);
	}
}

AddData::~AddData()
{
}

void AddData::ProcessRet(int arg_1, int arg_2) {
	if (arg_1 == ADD_FACTORY_OK) {
		this->close();
	}
	if (arg_1 == ADD_FACTORY_FAILED_NAME) {
		ui.label_retmsg->setText(CC("[-] 厂家名重复!"));
	}
	if (arg_1 == ADD_DEVICE_OK) {
		this->close();
	}
	if (arg_1 == READ_CFG_FILE_FAILED) {
		ui.label_retmsg->setText(CC("[-] 读取 文件") + QString::number(arg_2) + CC("(cfg) 失败!"));
	}
	if (arg_1 == READ_DAT_FILE_FAILED) {
		ui.label_retmsg->setText(CC("[-] 读取 文件") + QString::number(arg_2) + CC("(dat) 失败"));
	}
	if (arg_1 == ADD_WAVEFEATURE_OK) {
		this->close();
	}
	if (arg_1 == IMPORT_FILE_OK) {
		this->close();
	}
	if (arg_1 == OUTPUT_CFG_FILE_FAILED) {
		ui.label_retmsg->setText(CC("[-] 导出cfg文件失败!"));
	}
	if (arg_1 == OUTPUT_DAT_FILE_FAILED) {
		ui.label_retmsg->setText(CC("[-] 导出dat文件失败"));
	}
	if (arg_1 == EXPORT_FILE_OK) {
		this->close();
	}

}

/*  type = Factory / p_numvec = NULL / p_strvec[0] = factory name
 *                                     p_strvec[1] = factory scope of business
 *
 *  type = Device_Tpe / p_numvec[0] = factory id / p_strvec[0] = device type
 *                                                 p_strvec[1] = device PW
 *                                                 p_strvec[2] = eletric type
 *
 *  type = Wave_Feature / p_numvec[0] = d_id   / p_strvec[0] = wave type
 *                                               p_strvec[1~N] = wave files path
 *                        p_numvec[1] = r_tnl
 ***/

void AddData::on_pushButton_ok_clicked()
{
	if (local_type == Factory) {
		QVector<QString> str_vec;
		for (int i = 0; i < lineedit_vec.size(); i++) {
			str_vec.push_back(lineedit_vec[i]->text());
		}
		if (str_vec[0].size() == 0) {
			ui.label_retmsg->setText(CC("[-] 厂家名为空!"));
			return;
		}
		if (str_vec[1].size() == 0) {
			ui.label_retmsg->setText(CC("[-] 经营范围为空"));
			return;
		}
		ui.label_retmsg->setText("");
		for (int i = 0; i < lineedit_vec.size(); i++) {
			str_vec.push_back(lineedit_vec[i]->text());
		}

		emit SendRequest(this, local_type, NULL, &str_vec);

	}

	if (local_type == Device_Type) {
		QVector<int> num_vec;
		QVector<QString> str_vec;
		int f_id = combobox_vec[0]->currentData().toInt();
		QString d_type = combobox_vec[1]->currentText();
		QString d_pw = lineedit_vec[0]->text();
		QString d_etype = combobox_vec[2]->currentText();

		if (combobox_vec[0]->currentText().size() == 0) {
			ui.label_retmsg->setText(CC("[-] 厂家名为空,请先添加厂家数据!"));
			return;
		}
		if (d_type.size() == 0) {
			ui.label_retmsg->setText(CC("[-] 设备类型为空!"));
			return;
		}
		if (lineedit_vec[0]->text().size() == 0) {
			ui.label_retmsg->setText(CC("[-] 功耗为空!"));
			return;
		}
		if (d_etype.size() == 0) {
			ui.label_retmsg->setText(CC("[-] 电气类型为空!"));
			return;
		}
		ui.label_retmsg->setText("");
		num_vec.push_back(f_id);
		str_vec.push_back(d_type);
		str_vec.push_back(d_pw);
		str_vec.push_back(d_etype);
		emit SendRequest(this, local_type, &num_vec, &str_vec);
	}

	if (local_type == Wave_Feature) {
		QVector<int> num_vec;
		QVector<QString> str_vec;

		int d_id = combobox_vec[0]->currentData().toInt();
		QString w_type = combobox_vec[1]->currentText();
		int r_tnl = lineedit_vec[0]->text().toInt();

		if (combobox_vec[0]->currentText().size() == 0) {
			ui.label_retmsg->setText(CC("[-] 型号代码为空,请先添加设备数据!"));
			return;
		}
		if (w_type.size() == 0) {
			ui.label_retmsg->setText(CC("[-] 波形类型为空!"));
			return;
		}
		if (lineedit_vec[0]->text().size() == 0) {
			ui.label_retmsg->setText(CC("[-] 记录通道为空!"));
			return;
		}
		int fileCount = filelayout_vec.size() - 1;
		if (fileCount == 0) {
			ui.label_retmsg->setText(CC("[-] 未选择波形文件!"));
			return;
		}

		num_vec.push_back(d_id);
		num_vec.push_back(r_tnl);

		str_vec.push_back(w_type);

		for (int i = 0; i < fileCount; i++) {
			str_vec.push_back(((QLabel*)(filelayout_vec[i]->itemAt(1)->widget()))->text());
		}

		emit SendRequest(this, local_type, &num_vec, &str_vec);
	}

	if (local_type == Import_File) {
		QVector<QString> str_vec;
		QHBoxLayout *layout = (QHBoxLayout*)ui.verticalLayout_input->itemAt(0);
		QLabel *label = (QLabel*)layout->itemAt(1)->widget();

		if (label->text().size() > 1) {
			str_vec.push_back(label->text());
			emit SendRequest(this, local_type, NULL, &str_vec);

		}
		else {
			ui.label_retmsg->setText(CC("[-] 未选择波形文件!"));
		}
	}

	if (local_type == Export_File) {
		QVector<QString> str_vec;
		QHBoxLayout *layout = (QHBoxLayout*)ui.verticalLayout_input->itemAt(0);
		QLabel *label = (QLabel*)layout->itemAt(1)->widget();
		QString output_path = label->text();
		if (output_path[output_path.size() - 1] != '/') {
			output_path.push_back('/');
		}
		str_vec.push_back(output_path);
		emit SendRequest(this, local_type, NULL, &str_vec);
	}

}

void AddData::on_pushButton_cancel_clicked()
{
	this->close();
}

void AddData::AddUploadFileLayout() {
	QHBoxLayout *layout = new QHBoxLayout();
	int file_count = filelayout_vec.size() + 1;
	QLabel *label_prefix = new QLabel(CC("文件") + QString::number(file_count) + ":", this);
	QLabel *label_filePath = new QLabel("/", this);

	layout->addWidget(label_prefix);
	layout->addWidget(label_filePath);

	layout->addSpacerItem(new QSpacerItem(20, 20, QSizePolicy::Expanding));

	QPushButton *button_select = new QPushButton(CC("选择"), this);
	QPushButton *button_remove = new QPushButton(CC("删除"), this);

	connect(button_select, SIGNAL(clicked()), this, SLOT(on_selectFile()));
	connect(button_remove, SIGNAL(clicked()), this, SLOT(on_removeFile()));
	layout->addWidget(button_select);
	layout->addWidget(button_remove);

	filelayout_vec.push_back(layout);
	ui.verticalLayout_input->addLayout(layout);
}

void AddData::on_selectFile() {
	QPushButton *from = (QPushButton*)sender();
	int ti;
	for (int i = 0; i < filelayout_vec.size(); i++) {
		if (from == (QPushButton*)filelayout_vec[i]->itemAt(3)->widget()) {
			ti = i;
			break;
		}
	}


	QFileDialog *fileDialog = new QFileDialog(this);
	fileDialog->setWindowTitle(CC("选择文件") + QString::number(ti + 1));
	fileDialog->setDirectory("c:/");
	fileDialog->setNameFilter("File(*.cfg)");
	fileDialog->setFileMode(QFileDialog::ExistingFile);
	fileDialog->setViewMode(QFileDialog::Detail);
	QStringList filePathList;
	if (fileDialog->exec()) {
		filePathList = fileDialog->selectedFiles();
	}
	if (filePathList.size()) {
		QString cfgFilePath = filePathList.first();
		for (int i = 0; i < filelayout_vec.size(); i++) {
			if (cfgFilePath == ((QLabel*)filelayout_vec[i]->itemAt(1)->widget())->text()) {
				ui.label_retmsg->setText(CC("[-] 该文件已被选择!"));
				return;
			}
		}
		((QLabel*)filelayout_vec[ti]->itemAt(1)->widget())->setText(cfgFilePath);
		((QPushButton*)filelayout_vec[ti]->itemAt(4)->widget())->show();
		AddUploadFileLayout();
	}
}

void AddData::on_import_selectFile() {
	QFileDialog *fileDialog = new QFileDialog(this);
	fileDialog->setWindowTitle(CC("选择文件"));
	fileDialog->setDirectory("c:/");
	fileDialog->setNameFilter("File(*.cfg)");
	fileDialog->setFileMode(QFileDialog::ExistingFile);
	fileDialog->setViewMode(QFileDialog::Detail);
	QStringList filePathList;
	if (fileDialog->exec()) {
		filePathList = fileDialog->selectedFiles();
	}
	if (filePathList.size()) {
		QString filepath = filePathList.first();
		QHBoxLayout *layout = (QHBoxLayout*)ui.verticalLayout_input->itemAt(0);
		QLabel *label = (QLabel*)layout->itemAt(1)->widget();
		label->setText(filepath);
	}
}

void AddData::on_export_selectDir() {
	QFileDialog *fileDialog = new QFileDialog(this);
	fileDialog->setWindowTitle(CC("选择文件夹"));
	fileDialog->setDirectory("c:/");
	fileDialog->setFileMode(QFileDialog::DirectoryOnly);
	fileDialog->setViewMode(QFileDialog::Detail);
	QStringList filePathList;
	if (fileDialog->exec()) {
		filePathList = fileDialog->selectedFiles();
	}
	if (filePathList.size()) {
		QString filepath = filePathList.first();
		QHBoxLayout *layout = (QHBoxLayout*)ui.verticalLayout_input->itemAt(0);
		QLabel *label = (QLabel*)layout->itemAt(1)->widget();
		label->setText(filepath);
	}
}

void AddData::on_removeFile() {
	QPushButton *from = (QPushButton*)sender();
	int ti;
	for (int i = 0; i < filelayout_vec.size(); i++) {
		if (from == (QPushButton*)filelayout_vec[i]->itemAt(4)->widget()) {
			ti = i;
			break;
		}
	}

	for (int i = 0; i < 5; i++) {
		QLayoutItem *item = filelayout_vec[ti]->itemAt(0);
		delete item->widget();
		filelayout_vec[ti]->removeItem(item);
	}
	ui.verticalLayout_input->removeItem(filelayout_vec[ti]);

	for (; ti < filelayout_vec.size() - 1; ti++) {
		filelayout_vec[ti] = filelayout_vec[ti + 1];
		((QLabel*)filelayout_vec[ti]->itemAt(0)->widget())->setText(CC("文件") + QString::number(ti + 1));
	}

	filelayout_vec.pop_back();
}
