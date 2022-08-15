#pragma once
#include "ui_AddData.h"
#include <QLayout>
#include <QComboBox>
enum {
	Device_Type,
	Wave_Feature,
	Factory,
	Wave_File,
	Import_File,
	Export_File
};

enum {
	ADD_FACTORY_OK,
	ADD_FACTORY_FAILED_NAME,
	ADD_DEVICE_OK,
	READ_CFG_FILE_FAILED,
	READ_DAT_FILE_FAILED,
	OUTPUT_CFG_FILE_FAILED,
	OUTPUT_DAT_FILE_FAILED,
	ADD_WAVEFEATURE_OK,
	IMPORT_FILE_OK,
	EXPORT_FILE_OK
};


namespace Ui {
	class AddDataClass;
}

class AddData : public QMainWindow
{
	Q_OBJECT

public:
	explicit AddData(int type, QWidget *parent = nullptr);
	~AddData();

	void ProcessRet(int arg_1, int arg_2 = 0);



private:
	Ui::AddDataClass ui;
	int local_type;

	QVector<QHBoxLayout*> filelayout_vec;

	QVector<QLineEdit*> lineedit_vec;
	QVector<QComboBox*> combobox_vec;

private:
	void AddUploadFileLayout();
	void DelUploadFileLayout(int);


signals:
	void SendRequest(AddData*, int, QVector<int>*, QVector<QString>*);

private slots:
	void on_pushButton_ok_clicked();
	void on_pushButton_cancel_clicked();
	void on_selectFile();
	void on_import_selectFile();
	void on_export_selectDir();
	void on_removeFile();
};