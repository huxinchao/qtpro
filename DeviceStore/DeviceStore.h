#pragma once

#include <QtWidgets/QMainWindow>
#include <QtSql>
#include "ui_DeviceStore.h"
#include "adddata.h"

enum RECORD_STATUS {
	ORIGINAL_DATA,
	CHANGED_DATA,
	CHANGED_DATAFILE,
	NEWADDED_DATA
};


namespace Ui {
	class DeviceStoreClass;
}

struct stu_factory {
	int status;
	int f_id;
	QString f_name;
	QString f_sob;
};

struct stu_device {
	int status;
	int d_id;
	int f_id;
	QString d_type;
	double d_pw;
	QString d_etype;
};

struct stu_feature {
	int status;
	int w_id;
	int d_id;
	QString w_type;
	int r_tnl;
	QString w_filename;
	QByteArray cfg_file;
	QByteArray dat_file;
};

struct stu_del {
	int table;
	int id;
};

class DeviceStore : public QMainWindow
{
    Q_OBJECT

public:
    DeviceStore(QWidget *parent = nullptr);
    ~DeviceStore();

private:
    Ui::DeviceStoreClass ui;

	bool sql_connected;
	QSqlDatabase sql_database;
	QSqlQuery sql_query;

	QVector<stu_factory> factory_vec;
	QVector<stu_device> device_vec;
	QVector<stu_feature> feature_vec;
	QVector<stu_del> del_vec;

private:
	bool SQL_ConnectDatabase();
	bool SQL_LoadDatabase();
	bool SQL_SaveToDatabase();
	bool SQL_LoadFile(int i);
	void VecToTable();

	int GetNewFactoryID();
	int GetNewDeviceID();
	int GetNewFileID();

	void DelRecord(int type, int id);

private slots:
	void initForm();
	void loadStyle(const QString &qssFile);
	void on_tabCurrentChanged(int);

	void on_pushButton_addData_clicked();

	void on_RecvRequest(AddData* from, int type, QVector<int> *p_numvec, QVector<QString> *p_strvec);
	void on_pushButton_undoChanged_clicked();
	void on_pushButton_delData_clicked();
	void on_pushButton_saveChanged_clicked();
	void on_pushButton_importFile_clicked();
	void on_ItemChanged(QTableWidgetItem *item);
	void on_pushButton_exportFile_clicked();
};
