#include "DeviceStore.h"
#include <QMessageBox>

#define CC(x) QString::fromLocal8Bit(x)

QSet<QString> DEV_TYPE_SET;
QSet<QString> ELETRIC_TYPE_SET;
QSet<QString> WAVE_TYPE_SET;

QMap<int, int> DEV_FACTORY_MAP;
QMap<int, QString> DEV_ID_MAP;
QMap<int, QString> FACTORY_ID_MAP;

DeviceStore::DeviceStore(QWidget *parent) : QMainWindow(parent)
{
	ui.setupUi(this);
	this->initForm();
	sql_connected = false;

	if (SQL_ConnectDatabase() && SQL_LoadDatabase()) {
		VecToTable();
	}
}

DeviceStore::~DeviceStore()
{
}

void DeviceStore::initForm()
{
	loadStyle(":/qss/flatgray.css");
	//  loadStyle(":/qss/lightblue.css");
	//  loadStyle(":/qss/blacksoft.css");

	ui.tabWidget_tables->setCurrentIndex(0);

	ui.pushButton_importFile->setEnabled(false);
	ui.pushButton_exportFile->setEnabled(false);


	ui.tabWidget_tables->setTabText(0, CC("厂家表"));
	ui.tabWidget_tables->setTabText(1, CC("设备表"));
	ui.tabWidget_tables->setTabText(2, CC("波形特征表"));


	ui.tableWidget_deviceType->setColumnCount(5);
	ui.tableWidget_deviceType->setRowCount(0);
	ui.tableWidget_deviceType->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
	QStringList deviceTypeHeaders, deviceTypeTmpData;
	deviceTypeHeaders << CC("型号代码") << CC("厂家代码") << CC("设备类型") << CC("额定功耗(KW)") << CC("电气类型");
	ui.tableWidget_deviceType->setHorizontalHeaderLabels(deviceTypeHeaders);


	ui.tableWidget_waveFeature->setColumnCount(5);
	ui.tableWidget_waveFeature->setRowCount(0);
	ui.tableWidget_waveFeature->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);

	QStringList waveFeatureHeaders, waveFeatureTmpData;
	waveFeatureHeaders << CC("波形代码") << CC("型号代码") << CC("波形类型") << CC("记录通道") << CC("波形文件");
	ui.tableWidget_waveFeature->setHorizontalHeaderLabels(waveFeatureHeaders);


	ui.tableWidget_factory->setColumnCount(3);
	ui.tableWidget_factory->setRowCount(0);
	ui.tableWidget_factory->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);

	QStringList factoryHeaders, factoryTmpData;
	factoryHeaders << CC("厂家代码") << CC("厂家名称") << CC("厂家经营范围");
	ui.tableWidget_factory->setHorizontalHeaderLabels(factoryHeaders);



	//---------------------------------------------------------- 信号槽 ---------------------------------------------------------------

	connect(ui.tabWidget_tables, SIGNAL(currentChanged(int)), this, SLOT(on_tabCurrentChanged(int)));
	connect(ui.tableWidget_factory, SIGNAL(itemChanged(QTableWidgetItem*)), this, SLOT(on_ItemChanged(QTableWidgetItem*)));
	connect(ui.tableWidget_deviceType, SIGNAL(itemChanged(QTableWidgetItem*)), this, SLOT(on_ItemChanged(QTableWidgetItem*)));
	connect(ui.tableWidget_waveFeature, SIGNAL(itemChanged(QTableWidgetItem*)), this, SLOT(on_ItemChanged(QTableWidgetItem*)));


	//---------------------------------------------------------- 信号槽 ---------------------------------------------------------------


}

void DeviceStore::loadStyle(const QString &qssFile)
{
	QString qss;
	QFile file(qssFile);
	if (file.open(QFile::ReadOnly)) {
		QStringList list;
		QTextStream in(&file);
		//in.setCodec("utf-8");
		while (!in.atEnd()) {
			QString line;
			in >> line;
			list << line;
		}

		file.close();
		qss = list.join("\n");
		QString paletteColor = qss.mid(20, 7);
		qApp->setPalette(QPalette(paletteColor));
		qApp->setStyleSheet(qss);
	}
}

void DeviceStore::on_tabCurrentChanged(int index) {
	if (index == 2) {
		ui.pushButton_importFile->setEnabled(true);
		ui.pushButton_exportFile->setEnabled(true);
	}
	else {
		ui.pushButton_importFile->setEnabled(false);
		ui.pushButton_exportFile->setEnabled(false);
	}
}

void DeviceStore::on_pushButton_addData_clicked()
{
	AddData *adddata;
	if (ui.tabWidget_tables->currentIndex() == 0) {
		adddata = new AddData(Factory, this);
	}
	if (ui.tabWidget_tables->currentIndex() == 1) {
		adddata = new AddData(Device_Type, this);
	}
	if (ui.tabWidget_tables->currentIndex() == 2) {
		adddata = new AddData(Wave_Feature, this);
	}
	connect(adddata, SIGNAL(SendRequest(AddData*, int, QVector<int>*, QVector<QString>*)), this, SLOT(on_RecvRequest(AddData*, int, QVector<int>*, QVector<QString>*)));
	adddata->show();
}
bool DeviceStore::SQL_ConnectDatabase() {
	if (sql_connected) {
		return true;
	}

	sql_database = QSqlDatabase::addDatabase("QSQLITE");
	sql_database.setDatabaseName("dev.db");
	if (sql_database.open()) {
		sql_connected = true;
		ui.textEdit_msg->append(CC("[√] 数据库连接成功"));
		sql_query = QSqlQuery(sql_database);
	}
	else {
		sql_connected = false;
		ui.textEdit_msg->append(CC("[×] 数据库连接失败"));
	}
	return sql_connected;
}
bool DeviceStore::SQL_LoadDatabase() {
	if (!sql_connected) return false;

	ELETRIC_TYPE_SET.clear();
	WAVE_TYPE_SET.clear();
	DEV_TYPE_SET.clear();

	DEV_FACTORY_MAP.clear();
	DEV_ID_MAP.clear();
	FACTORY_ID_MAP.clear();

	// Initial ELETRIC_TYPE_SET

	ELETRIC_TYPE_SET.insert(CC("变频"));
	ELETRIC_TYPE_SET.insert(CC("定频"));
	ELETRIC_TYPE_SET.insert(CC("感性"));
	ELETRIC_TYPE_SET.insert(CC("容性"));
	ELETRIC_TYPE_SET.insert(CC("阻性"));

	//-------------------------

	// Initial WAVE_TYPE_SET

	WAVE_TYPE_SET.insert(CC("启动"));
	WAVE_TYPE_SET.insert(CC("突变"));
	WAVE_TYPE_SET.insert(CC("稳态"));

	//-------------------------

	factory_vec.clear();
	device_vec.clear();
	feature_vec.clear();
	del_vec.clear();

	if (sql_query.exec("select * from factory")) {
		while (sql_query.next()) {
			int f_id = sql_query.value(0).toInt();
			QString f_name = sql_query.value(1).toString();
			QString f_sob = sql_query.value(2).toString();
			factory_vec.push_back({ ORIGINAL_DATA,f_id,f_name,f_sob });
			FACTORY_ID_MAP[f_id] = f_name;
		}
	}
	else {
		ui.textEdit_msg->append(CC("[×] 加载 厂家表 失败"));
		return false;
	}

	if (sql_query.exec("select * from device")) {
		while (sql_query.next()) {
			int d_id = sql_query.value(0).toInt();
			int f_id = sql_query.value(1).toInt();
			QString d_type = sql_query.value(2).toString();
			double d_pw = sql_query.value(3).toDouble();
			QString d_etype = sql_query.value(4).toString();
			device_vec.push_back({ ORIGINAL_DATA,d_id,f_id,d_type,d_pw,d_etype });

			DEV_TYPE_SET.insert(d_type);
			ELETRIC_TYPE_SET.insert(d_etype);

			DEV_FACTORY_MAP[d_id] = f_id;
			DEV_ID_MAP[d_id] = d_type;

		}

	}
	else {
		ui.textEdit_msg->append(CC("[×] 加载 设备型号表 失败"));
		return false;
	}

	if (sql_query.exec("select * from feature")) {
		while (sql_query.next()) {
			int w_id = sql_query.value(0).toInt();
			int d_id = sql_query.value(1).toInt();
			QString w_type = sql_query.value(2).toString();
			int r_tnl = sql_query.value(3).toInt();
			QString w_filename = sql_query.value(4).toString();
			feature_vec.push_back({ ORIGINAL_DATA,w_id,d_id,w_type,r_tnl,w_filename,"","" });
			WAVE_TYPE_SET.insert(w_type);
		}
	}
	else {
		ui.textEdit_msg->append(CC("[×] 加载 负荷特征波形记录表 失败"));
		return false;
	}

	ui.textEdit_msg->append(CC("[√] 加载数据表成功"));
	return true;
}

bool DeviceStore::SQL_SaveToDatabase() {
	if (!sql_connected) {
		ui.textEdit_msg->append(CC("[×] 数据库未连接!"));
		return false;
	}
	else {
		QTime time = QTime::currentTime();
		ui.textEdit_msg->append("[" + time.toString() + CC("] ------------- 准备将修改写入数据库 ------------- "));
	}

	for (int i = 0; i < del_vec.size(); i++) {
		int table = del_vec[i].table;
		int id = del_vec[i].id;
		QString table_str;
		QString id_str;
		if (table == Factory) {
			table_str = "factory";
			id_str = "f_id";
		}
		if (table == Device_Type) {
			table_str = "device";
			id_str = "d_id";
		}
		if (table == Wave_Feature) {
			table_str = "feature";
			id_str = "w_id";
		}

		QString sql_str = "delete from " + table_str + " where " + id_str + " = " + QString::number(id);
		if (sql_query.exec(sql_str)) {
			QTime time = QTime::currentTime();
			ui.textEdit_msg->append("[" + time.toString() + CC("] 删除表 ") + table_str + CC(" 记录") + id_str + " = " + QString::number(id));
		}
		else {
			ui.textEdit_msg->append("[-] sql exec error - del_vec part");
			return false;
		}
	}
	del_vec.clear();
	for (int i = 0; i < factory_vec.size(); i++) {
		if (factory_vec[i].status == CHANGED_DATA) {
			QString sql_str = "update factory set f_name = '" \
				+ factory_vec[i].f_name + "',f_sob = '" \
				+ factory_vec[i].f_sob + "' where f_id = "\
				+ QString::number(factory_vec[i].f_id);
			if (sql_query.exec(sql_str)) {
				factory_vec[i].status = ORIGINAL_DATA;
				QTime time = QTime::currentTime();
				ui.textEdit_msg->append("[" + time.toString() + CC("] 修改表 factory 记录 id = ") + QString::number(factory_vec[i].f_id));
			}
			else {
				ui.textEdit_msg->append("[-] sql exec error - factory part - A");
				return false;
			}
		}
		if (factory_vec[i].status == NEWADDED_DATA) {
			QString sql_str = "insert into factory(f_id,f_name,f_sob) values(" \
				+ QString::number(factory_vec[i].f_id) + ",'" \
				+ factory_vec[i].f_name + "','" \
				+ factory_vec[i].f_sob + "')";
			if (sql_query.exec(sql_str)) {
				factory_vec[i].status = ORIGINAL_DATA;
				QTime time = QTime::currentTime();
				ui.textEdit_msg->append("[" + time.toString() + CC("] 在表 factory 添加记录 id = ") + QString::number(factory_vec[i].f_id));
			}
			else {
				ui.textEdit_msg->append("[-] sql exec error - factory part - B");
				return false;
			}
		}
	}

	for (int i = 0; i < device_vec.size(); i++) {
		if (device_vec[i].status == CHANGED_DATA) {
			QString sql_str = "update device set f_id = " + QString::number(device_vec[i].f_id) \
				+ ",d_type='" + device_vec[i].d_type \
				+ "',d_pw=" + QString::number(device_vec[i].d_pw) \
				+ ",d_etype='" + device_vec[i].d_etype + "' where d_id = " \
				+ QString::number(device_vec[i].d_id);

			if (sql_query.exec(sql_str)) {
				device_vec[i].status = ORIGINAL_DATA;
				QTime time = QTime::currentTime();
				ui.textEdit_msg->append("[" + time.toString() + CC("] 修改表 device 记录 id = ") + QString::number(device_vec[i].d_id));
			}
			else {
				ui.textEdit_msg->append("[-] sql exec error - device part - A");
				return false;
			}
		}

	
		if (device_vec[i].status == NEWADDED_DATA) {
			QString sql_str = "insert into device(d_id,f_id,d_type,d_pw,d_etype) values(" \
				+ QString::number(device_vec[i].d_id) + "," \
				+ QString::number(device_vec[i].f_id) + ",'" \
				+ device_vec[i].d_type + "'," \
				+ QString::number(device_vec[i].d_pw) + ",'" \
				+ device_vec[i].d_etype + "')";
			if (sql_query.exec(sql_str)) {
				device_vec[i].status = ORIGINAL_DATA;
				QTime time = QTime::currentTime();
				ui.textEdit_msg->append("[" + time.toString() + CC("] 在表 device 添加记录 id = ") + QString::number(device_vec[i].d_id));
			}
			else {
				ui.textEdit_msg->append("[-] sql exec error - device part - B");
				return false;
			}
		}
	}

	for (int i = 0; i < feature_vec.size(); i++) {
		if (feature_vec[i].status == CHANGED_DATA) {
			QString sql_str = "update feature set d_id = " + QString::number(feature_vec[i].d_id) \
				+ ",w_type='" + feature_vec[i].w_type
				+ "',r_tnl=" + QString::number(feature_vec[i].r_tnl)
				+ ",w_filename='" + feature_vec[i].w_filename
				+ "' where w_id = " + QString::number(feature_vec[i].w_id);
			
			if (sql_query.exec(sql_str)) {
				feature_vec[i].status = ORIGINAL_DATA;
				QTime time = QTime::currentTime();
				ui.textEdit_msg->append("[" + time.toString() + CC("] 修改表 feature 记录 id = ") + QString::number(feature_vec[i].w_id));
			}
			else {
				ui.textEdit_msg->append("[-] sql exec error - feature part - A1");
				return false;
			}
		}

		if (feature_vec[i].status == CHANGED_DATAFILE) {
			QString sql_str = "update feature set d_id = " + QString::number(feature_vec[i].d_id) \
				+ ",w_type='" + feature_vec[i].w_type
				+ "',r_tnl=" + QString::number(feature_vec[i].r_tnl)
				+ ",w_filename='" + feature_vec[i].w_filename
				+ "' where w_id = " + QString::number(feature_vec[i].w_id);
			if (sql_query.exec(sql_str)) {
				sql_str = "update feature set cfg_file=:cfg_file where w_id = " + QString::number(feature_vec[i].w_id);
				sql_query.prepare(sql_str);
				sql_query.bindValue(":cfg_file", feature_vec[i].cfg_file);

				if (sql_query.exec()) {
					ui.textEdit_msg->append("insert cfg file successed");
				}
				else {
					ui.textEdit_msg->append("insert cfg file failed");
					return false;
				}

				sql_str = "update feature set dat_file=:dat_file where w_id = " + QString::number(feature_vec[i].w_id);
				sql_query.prepare(sql_str);
				sql_query.bindValue(":dat_file", feature_vec[i].dat_file);

				if (sql_query.exec()) {
					ui.textEdit_msg->append("insert dat file successed");
				}
				else {
					ui.textEdit_msg->append("insert dat file failed");
					return false;
				}

				feature_vec[i].status = ORIGINAL_DATA;
				QTime time = QTime::currentTime();
				ui.textEdit_msg->append("[" + time.toString() + CC("] 修改表 feature 记录 id = ") + QString::number(feature_vec[i].w_id));
			}
			else {
				ui.textEdit_msg->append("[-] sql exec error - feature part - A2");
				return false;
			}
		}


		if (feature_vec[i].status == NEWADDED_DATA) {
			QString sql_str = "insert into feature(w_id,d_id,w_type,r_tnl,w_filename) values(" \
				+ QString::number(feature_vec[i].w_id) + "," \
				+ QString::number(feature_vec[i].d_id) + ",'" \
				+ feature_vec[i].w_type + "'," \
				+ QString::number(feature_vec[i].r_tnl) + ",'" \
				+ feature_vec[i].w_filename + "')";
			if (sql_query.exec(sql_str)) {
				sql_str = "update feature set cfg_file=:cfg_file where w_id = " + QString::number(feature_vec[i].w_id);
				sql_query.prepare(sql_str);
				sql_query.bindValue(":cfg_file", feature_vec[i].cfg_file);

				if (sql_query.exec()) {
					ui.textEdit_msg->append("insert cfg file successed");
				}
				else {
					ui.textEdit_msg->append("insert cfg file failed");
					return false;
				}

				sql_str = "update feature set dat_file=:dat_file where w_id = " + QString::number(feature_vec[i].w_id);
				sql_query.prepare(sql_str);
				sql_query.bindValue(":dat_file", feature_vec[i].dat_file);

				if (sql_query.exec()) {
					ui.textEdit_msg->append("insert dat file successed");
				}
				else {
					ui.textEdit_msg->append("insert dat file failed");
					return false;
				}

				feature_vec[i].status = ORIGINAL_DATA;
				QTime time = QTime::currentTime();
				ui.textEdit_msg->append("[" + time.toString() + CC("] 在表 feature 添加记录 id = ") + QString::number(feature_vec[i].w_id));


			}
			else {
				ui.textEdit_msg->append("[-] sql exec error - feature part - B");
				return false;
			}
		}
	}



	QTime time = QTime::currentTime();
	ui.textEdit_msg->append("[" + time.toString() + CC("] --------------- 所有修改写入完成 --------------- "));
	return true;
}

void DeviceStore::VecToTable() {
	ui.tableWidget_factory->blockSignals(true);
	ui.tableWidget_deviceType->blockSignals(true);
	ui.tableWidget_waveFeature->blockSignals(true);

	ui.tableWidget_factory->setRowCount(factory_vec.size());
	ui.tableWidget_deviceType->setRowCount(device_vec.size());
	ui.tableWidget_waveFeature->setRowCount(feature_vec.size());


	for (int i = 0; i < factory_vec.size(); i++) {
		QTableWidgetItem *item_fid = new QTableWidgetItem(QString::number(factory_vec[i].f_id));
		QTableWidgetItem *item_fname = new QTableWidgetItem(factory_vec[i].f_name);
		QTableWidgetItem *item_fsob = new QTableWidgetItem(factory_vec[i].f_sob);


		ui.tableWidget_factory->setItem(i, 0, item_fid);
		ui.tableWidget_factory->setItem(i, 1, item_fname);
		ui.tableWidget_factory->setItem(i, 2, item_fsob);

		for (int j = 0; j < 3; j++) { ui.tableWidget_factory->item(i, j)->setTextAlignment(Qt::AlignCenter); }
	}


	for (int i = 0; i < device_vec.size(); i++) {
		QTableWidgetItem *item_did = new QTableWidgetItem(QString::number(device_vec[i].d_id));
		QTableWidgetItem *item_fid = new QTableWidgetItem(QString::number(device_vec[i].f_id));
		QTableWidgetItem *item_dtype = new QTableWidgetItem(device_vec[i].d_type);
		QTableWidgetItem *item_dpw = new QTableWidgetItem(QString::number(device_vec[i].d_pw));
		QTableWidgetItem *item_detype = new QTableWidgetItem(device_vec[i].d_etype);

		ui.tableWidget_deviceType->setItem(i, 0, item_did);
		ui.tableWidget_deviceType->setItem(i, 1, item_fid);
		ui.tableWidget_deviceType->setItem(i, 2, item_dtype);
		ui.tableWidget_deviceType->setItem(i, 3, item_dpw);
		ui.tableWidget_deviceType->setItem(i, 4, item_detype);


		for (int j = 0; j < 5; j++) { ui.tableWidget_deviceType->item(i, j)->setTextAlignment(Qt::AlignCenter); }
	}

	for (int i = 0; i < feature_vec.size(); i++) {
		QTableWidgetItem *item_fid = new QTableWidgetItem(QString::number(feature_vec[i].w_id));
		QTableWidgetItem *item_did = new QTableWidgetItem(QString::number(feature_vec[i].d_id));
		QTableWidgetItem *item_wtype = new QTableWidgetItem(feature_vec[i].w_type);
		QTableWidgetItem *item_rtnl = new QTableWidgetItem(QString::number(feature_vec[i].r_tnl));
		QTableWidgetItem *item_wfilename = new QTableWidgetItem(feature_vec[i].w_filename);

		ui.tableWidget_waveFeature->setItem(i, 0, item_fid);
		ui.tableWidget_waveFeature->setItem(i, 1, item_did);
		ui.tableWidget_waveFeature->setItem(i, 2, item_wtype);
		ui.tableWidget_waveFeature->setItem(i, 3, item_rtnl);
		ui.tableWidget_waveFeature->setItem(i, 4, item_wfilename);

		for (int j = 0; j < 5; j++) { ui.tableWidget_waveFeature->item(i, j)->setTextAlignment(Qt::AlignCenter); }

	}

	ui.tableWidget_factory->blockSignals(false);
	ui.tableWidget_deviceType->blockSignals(false);
	ui.tableWidget_waveFeature->blockSignals(false);

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


void DeviceStore::on_RecvRequest(AddData* from, int type, QVector<int> *p_numvec, QVector<QString> *p_strvec) {
	if (type == Factory) {
		ui.tableWidget_factory->blockSignals(true);

		qDebug() << "Factory Request";
		QString f_name = (*p_strvec)[0];
		QString f_sob = (*p_strvec)[1];

		bool availFlag = false;
		for (auto ite = FACTORY_ID_MAP.begin(); ite != FACTORY_ID_MAP.end(); ite++) {
			if (ite.value() == f_name) {
				availFlag = true;
				break;
			}
		}

		if (!availFlag) {
			int f_id = GetNewFactoryID();
			factory_vec.push_back({ NEWADDED_DATA,f_id, f_name, f_sob });
			FACTORY_ID_MAP[f_id] = f_name;
			int rowCount = factory_vec.size();
			ui.tableWidget_factory->setRowCount(rowCount);

			ui.tableWidget_factory->setItem(rowCount - 1, 0, new QTableWidgetItem(QString::number(factory_vec.back().f_id)));
			ui.tableWidget_factory->setItem(rowCount - 1, 1, new QTableWidgetItem(factory_vec.back().f_name));
			ui.tableWidget_factory->setItem(rowCount - 1, 2, new QTableWidgetItem(factory_vec.back().f_sob));

			for (int i = 0; i < 3; i++) { ui.tableWidget_factory->item(rowCount - 1, i)->setTextAlignment(Qt::AlignCenter); }
			from->ProcessRet(ADD_FACTORY_OK);
		}
		else {
			from->ProcessRet(ADD_FACTORY_FAILED_NAME);
		}

		ui.tableWidget_factory->blockSignals(false);
	}

	if (type == Device_Type) {
		ui.tableWidget_deviceType->blockSignals(true);

		qDebug() << "Device Request";
		int d_id = GetNewDeviceID();
		int f_id = (*p_numvec)[0];
		QString d_type = (*p_strvec)[0];
		double d_pw = (*p_strvec)[1].toDouble();
		QString d_etype = (*p_strvec)[2];

		DEV_TYPE_SET.insert(d_type);
		ELETRIC_TYPE_SET.insert(d_etype);
		DEV_FACTORY_MAP[d_id] = f_id;
		DEV_ID_MAP[d_id] = d_type;


		device_vec.push_back({ NEWADDED_DATA,d_id,f_id,d_type,d_pw,d_etype });

		int rowCount = device_vec.size();
		ui.tableWidget_deviceType->setRowCount(rowCount);

		ui.tableWidget_deviceType->setItem(rowCount - 1, 0, new QTableWidgetItem(QString::number(device_vec.back().d_id)));
		ui.tableWidget_deviceType->setItem(rowCount - 1, 1, new QTableWidgetItem(QString::number(device_vec.back().f_id)));
		ui.tableWidget_deviceType->setItem(rowCount - 1, 2, new QTableWidgetItem(device_vec.back().d_type));
		ui.tableWidget_deviceType->setItem(rowCount - 1, 3, new QTableWidgetItem(QString::number(device_vec.back().d_pw)));
		ui.tableWidget_deviceType->setItem(rowCount - 1, 4, new QTableWidgetItem(device_vec.back().d_etype));
		for (int i = 0; i < 5; i++) { ui.tableWidget_deviceType->item(rowCount - 1, i)->setTextAlignment(Qt::AlignCenter); }

		from->ProcessRet(ADD_DEVICE_OK);
		ui.tableWidget_deviceType->blockSignals(false);
	}

	if (type == Wave_Feature) {
		ui.tableWidget_waveFeature->blockSignals(true);
		qDebug() << "Wave Request";
		int d_id = (*p_numvec)[0];
		int r_tnl = (*p_numvec)[1];

		QString w_type = (*p_strvec)[0];
		QVector<QString> filePath_vec;

		QVector<stu_feature> tmp_task_vec;
		int w_id = GetNewFileID();

		for (int i = 1; i < p_strvec->size(); i++) {
			QString cfg_filePath = (*p_strvec)[i];
			QString dat_filePath = cfg_filePath.mid(0, cfg_filePath.lastIndexOf('.')) + ".dat";


			QString w_filename = cfg_filePath.mid(cfg_filePath.lastIndexOf('/') + 1, cfg_filePath.lastIndexOf('.') - cfg_filePath.lastIndexOf('/') - 1);
			QByteArray cfg_file, dat_file;
			QFile cfgFile(cfg_filePath);

			if (cfgFile.open(QIODevice::ReadOnly)) {
				cfg_file.append(cfgFile.readAll());
			}
			else {
				from->ProcessRet(READ_CFG_FILE_FAILED, i);
				return;
			}

			QFile datFile(dat_filePath);
			if (datFile.open(QIODevice::ReadOnly)) {
				dat_file.append(datFile.readAll());
			}
			else {
				from->ProcessRet(READ_DAT_FILE_FAILED, i);
				return;
			}
			tmp_task_vec.push_back({ NEWADDED_DATA, w_id++, d_id, w_type, r_tnl, w_filename, cfg_file, dat_file });
		}

		int basicRowCount = feature_vec.size();
		int rowCount = feature_vec.size() + tmp_task_vec.size();
		ui.tableWidget_waveFeature->setRowCount(rowCount);

		for (int i = 0; i < tmp_task_vec.size(); i++) {
			int ti = i + basicRowCount;
			ui.tableWidget_waveFeature->setItem(ti, 0, new QTableWidgetItem(QString::number(tmp_task_vec[i].w_id)));
			ui.tableWidget_waveFeature->setItem(ti, 1, new QTableWidgetItem(QString::number(tmp_task_vec[i].d_id)));
			ui.tableWidget_waveFeature->setItem(ti, 2, new QTableWidgetItem(tmp_task_vec[i].w_type));
			WAVE_TYPE_SET.insert(tmp_task_vec[i].w_type);
			ui.tableWidget_waveFeature->setItem(ti, 3, new QTableWidgetItem(QString::number(tmp_task_vec[i].r_tnl)));
			ui.tableWidget_waveFeature->setItem(ti, 4, new QTableWidgetItem(tmp_task_vec[i].w_filename));


			for (int j = 0; j < 5; j++) { ui.tableWidget_waveFeature->item(ti, j)->setTextAlignment(Qt::AlignCenter); }
			feature_vec.push_back(tmp_task_vec[i]);
		}

		from->ProcessRet(ADD_WAVEFEATURE_OK);

		ui.tableWidget_waveFeature->blockSignals(false);
	}

	if (type == Import_File) {
		QString cfg_filePath = (*p_strvec)[0];
		QString dat_filePath = cfg_filePath.mid(0, cfg_filePath.lastIndexOf('.')) + ".dat";


		QString w_filename = cfg_filePath.mid(cfg_filePath.lastIndexOf('/') + 1, cfg_filePath.lastIndexOf('.') - cfg_filePath.lastIndexOf('/') - 1);
		QByteArray cfg_file, dat_file;
		QFile cfgFile(cfg_filePath);

		if (cfgFile.open(QIODevice::ReadOnly)) {
			cfg_file.append(cfgFile.readAll());
		}
		else {
			from->ProcessRet(READ_CFG_FILE_FAILED, 1);
			return;
		}

		QFile datFile(dat_filePath);
		if (datFile.open(QIODevice::ReadOnly)) {
			dat_file.append(datFile.readAll());
		}
		else {
			from->ProcessRet(READ_DAT_FILE_FAILED, 1);
			return;
		}

		int row = ui.tableWidget_waveFeature->currentRow();
		feature_vec[row].cfg_file = cfg_file;
		feature_vec[row].dat_file = dat_file;
		feature_vec[row].w_filename = w_filename;
		ui.tableWidget_waveFeature->item(row, 4)->setText(w_filename);

		if (feature_vec[row].status != NEWADDED_DATA) feature_vec[row].status = CHANGED_DATAFILE;

		from->ProcessRet(IMPORT_FILE_OK);
	}

	if (type == Export_File) {
		int row = ui.tableWidget_waveFeature->currentRow();
		if (SQL_LoadFile(row)) {
			QString filename;

			int d_id = feature_vec[row].d_id;
			int f_id = DEV_FACTORY_MAP[d_id];
			int d_row;
			for (int i = 0; i < device_vec.size(); i++) {
				if (device_vec[i].d_id == d_id) {
					d_row = i;
					break;
				}
			}

			QString d_type = device_vec[d_id].d_type;
			QString d_etype = device_vec[d_id].d_etype;
			QString w_type = feature_vec[row].w_type;
			int r_tnl = feature_vec[row].r_tnl;
			int d_pw = device_vec[d_id].d_pw;
			filename = QString("%1_%2_%3_%4_%5_%6_%7").arg(d_type).arg(f_id).arg(d_id).arg(d_pw).arg(d_etype).arg(w_type).arg(r_tnl);

			QString cfg_filename = filename + ".cfg";
			QString dat_filename = filename + ".dat";
			QString output_path = (*p_strvec)[0];

			QString cfg_output_path = output_path + cfg_filename;
			QString dat_output_path = output_path + dat_filename;

			QFile cfg_file(cfg_output_path);
			QFile dat_file(dat_output_path);
			if (cfg_file.open(QIODevice::WriteOnly)) {
				cfg_file.write(feature_vec[row].cfg_file);
			}
			else {
				from->ProcessRet(OUTPUT_CFG_FILE_FAILED);
				QFile::remove(cfg_output_path);
				return;
			}
			if (dat_file.open(QIODevice::WriteOnly)) {
				dat_file.write(feature_vec[row].dat_file);
			}
			else {
				from->ProcessRet(OUTPUT_DAT_FILE_FAILED);
				QFile::remove(dat_output_path);
				return;
			}
			from->ProcessRet(EXPORT_FILE_OK);
		}
	}
}

int DeviceStore::GetNewFactoryID() {
	int *ar = new int[factory_vec.size()];
	memset(ar, 0, factory_vec.size() * sizeof(int));
	for (int i = 0; i < factory_vec.size(); i++) {
		ar[factory_vec[i].f_id - 1] = 1;
	}
	int res = factory_vec.size() + 1;
	for (int i = 0; i < factory_vec.size(); i++) {
		if (!ar[i]) {
			res = i + 1;
			delete[] ar;
			return res;
		}
	}
	delete[] ar;
	return res;
}
int DeviceStore::GetNewDeviceID() {
	int *ar = new int[device_vec.size()];
	memset(ar, 0, device_vec.size() * sizeof(int));
	for (int i = 0; i < device_vec.size(); i++) {
		ar[device_vec[i].d_id - 1] = 1;
	}
	int res = device_vec.size() + 1;
	for (int i = 0; i < device_vec.size(); i++) {
		if (!ar[i]) {
			res = i + 1;
			delete[] ar;
			return res;
		}
	}
	delete[] ar;
	return res;
}
int DeviceStore::GetNewFileID() {
	int *ar = new int[feature_vec.size()];
	memset(ar, 0, feature_vec.size() * sizeof(int));
	for (int i = 0; i < feature_vec.size(); i++) {
		ar[feature_vec[i].w_id - 1] = 1;
	}
	int res = feature_vec.size() + 1;
	for (int i = 0; i < feature_vec.size(); i++) {
		if (!ar[i]) {
			res = i + 1;
			delete[] ar;
			return res;
		}
	}
	delete[] ar;
	return res;
}

void DeviceStore::on_pushButton_undoChanged_clicked()
{
	QMessageBox::StandardButton result = QMessageBox::question(this, CC("提示"), CC("确认撤销修改吗?"));
	if (result == QMessageBox::Yes) {
		SQL_LoadDatabase();
		VecToTable();
	}
}



void DeviceStore::on_pushButton_delData_clicked()
{
	if (ui.tabWidget_tables->currentIndex() == 0) { // Factory
		int selectedRow = ui.tableWidget_factory->currentRow();
		if (selectedRow >= 0) {
			int id = factory_vec[selectedRow].f_id;

			QVector<int> related_dev_vec;
			QVector<int> related_wave_vec;

			for (int i = 0; i < device_vec.size(); i++) {
				if (device_vec[i].f_id == id) {
					related_dev_vec.push_back(device_vec[i].d_id);
				}
			}

			for (int i = 0; i < related_dev_vec.size(); i++) {
				for (int j = 0; j < feature_vec.size(); j++) {
					if (feature_vec[j].d_id == related_dev_vec[i]) {
						related_wave_vec.push_back(feature_vec[j].w_id);
					}
				}
			}

			QMessageBox::StandardButton result = QMessageBox::question(this, CC("删除 厂家") + QString::number(id) + " " + factory_vec[selectedRow].f_name, CC("此记录关联 ") \
				+ QString::number(related_dev_vec.size()) + CC(" 条设备记录, ") \
				+ QString::number(related_wave_vec.size()) + CC(" 条波形记录。 \n确认删除?"));

			if (result == QMessageBox::StandardButton::Yes) {
				qDebug() << "Start Delete Factory...";
				DelRecord(Factory, id);
				for (int i = 0; i < related_dev_vec.size(); i++) {
					DelRecord(Device_Type, related_dev_vec[i]);
				}
				for (int i = 0; i < related_wave_vec.size(); i++) {
					DelRecord(Wave_Feature, related_wave_vec[i]);
				}
			}
		}
	}
	if (ui.tabWidget_tables->currentIndex() == 1) { // Device
		int selectedRow = ui.tableWidget_deviceType->currentRow();
		if (selectedRow >= 0) {
			int id = device_vec[selectedRow].d_id;
			QVector<int> related_wave_vec;

			for (int i = 0; i < feature_vec.size(); i++) {
				if (feature_vec[i].d_id == id) {
					related_wave_vec.push_back(feature_vec[i].w_id);
				}
			}

			QMessageBox::StandardButton result = QMessageBox::question(this, CC("删除 设备") + QString::number(id) + " " + device_vec[selectedRow].d_type, \
				CC("此纪录关联 ") + QString::number(related_wave_vec.size()) + CC(" 条波形记录。 \n确认删除?"));

			if (result == QMessageBox::StandardButton::Yes) {
				qDebug() << "Start Delete Device";
				DelRecord(Device_Type, id);
				for (int i = 0; i < related_wave_vec.size(); i++) {
					DelRecord(Wave_Feature, related_wave_vec[i]);
				}
			}
		}
	}
	if (ui.tabWidget_tables->currentIndex() == 2) { // Wave Feature
		int selectedRow = ui.tableWidget_waveFeature->currentRow();
		if (selectedRow >= 0) {
			int id = feature_vec[selectedRow].w_id;
			QMessageBox::StandardButton result = QMessageBox::question(this, CC("删除 波形记录") + QString::number(id), CC("确认删除?"));
			if (result == QMessageBox::StandardButton::Yes) {
				qDebug() << "Start Delete Wave-Record";
				DelRecord(Wave_Feature, id);
			}
		}

	}
}

void DeviceStore::DelRecord(int type, int id) {
	if (type == Factory) {
		FACTORY_ID_MAP.remove(id);
		int ti = -1;
		for (int i = 0; i < factory_vec.size(); i++) {
			if (factory_vec[i].f_id == id) {
				ti = i;
				break;
			}
		}
		if (ti == -1) return;
		if (factory_vec[ti].status == ORIGINAL_DATA || factory_vec[ti].status == CHANGED_DATA) {
			del_vec.push_back({ Factory,id });
		}
		ui.tableWidget_factory->removeRow(ti);
		for (int i = ti; i < factory_vec.size() - 1; i++) {
			factory_vec[i] = factory_vec[i + 1];
		}
		factory_vec.pop_back();

	}

	if (type == Device_Type) {
		DEV_FACTORY_MAP.remove(id);
		DEV_ID_MAP.remove(id);

		int ti = -1;
		for (int i = 0; i < device_vec.size(); i++) {
			if (device_vec[i].d_id == id) {
				ti = i;
				break;
			}
		}
		if (ti == -1) return;

		if (device_vec[ti].status == ORIGINAL_DATA || device_vec[ti].status == CHANGED_DATA || device_vec[ti].status == CHANGED_DATAFILE) {
			del_vec.push_back({ Device_Type,id });
		}
		ui.tableWidget_deviceType->removeRow(ti);
		for (int i = ti; i < device_vec.size() - 1; i++) {
			device_vec[i] = device_vec[i + 1];
		}
		device_vec.pop_back();
	}

	if (type == Wave_Feature) {
		int ti = -1;
		for (int i = 0; i < feature_vec.size(); i++) {
			if (feature_vec[i].w_id == id) {
				ti = i;
				break;
			}
		}
		if (ti == -1) return;
		if (feature_vec[ti].status == ORIGINAL_DATA || feature_vec[ti].status == CHANGED_DATA) {
			del_vec.push_back({ Wave_Feature,id });
		}
		ui.tableWidget_waveFeature->removeRow(ti);
		for (int i = ti; i < feature_vec.size() - 1; i++) {
			feature_vec[i] = feature_vec[i + 1];
		}
		feature_vec.pop_back();
	}
}

void DeviceStore::on_pushButton_saveChanged_clicked()
{
	QMessageBox::StandardButton result = QMessageBox::question(this, CC("提示"), CC("确认将修改写入数据库?"));
	if (result == QMessageBox::Yes) {
		SQL_SaveToDatabase();
	}
}

void DeviceStore::on_pushButton_importFile_clicked()
{
	if (ui.tableWidget_waveFeature->currentRow() >= 0) {
		AddData *adddata = new AddData(Import_File, this);
		connect(adddata, SIGNAL(SendRequest(AddData*, int, QVector<int>*, QVector<QString>*)), this, SLOT(on_RecvRequest(AddData*, int, QVector<int>*, QVector<QString>*)));
		adddata->show();
	}
	else {
		ui.textEdit_msg->append(CC("[×] 未选择波形条目"));
	}
}

void DeviceStore::on_ItemChanged(QTableWidgetItem *item) {
	if (sender() == ui.tableWidget_factory) {
		int row = item->row();
		int col = item->column();
		if (col == 0) item->setText(QString::number(factory_vec[row].f_id));
		if (col == 1) {
			if (item->text().size() == 0) {
				item->setText(factory_vec[row].f_name);
			}
			else {
				factory_vec[row].f_name = item->text();
				if (factory_vec[row].status == ORIGINAL_DATA) factory_vec[row].status = CHANGED_DATA;
				FACTORY_ID_MAP[factory_vec[row].f_id] = item->text();
			}
		}
		if (col == 2) {
			if (item->text().size() == 0) {
				item->setText(factory_vec[row].f_sob);
			}
			else {
				factory_vec[row].f_sob = item->text();
				if (factory_vec[row].status == ORIGINAL_DATA) factory_vec[row].status = CHANGED_DATA;
			}
		}
	}

	if (sender() == ui.tableWidget_deviceType) {
		int row = item->row();
		int col = item->column();
		if (col == 0) item->setText(QString::number(device_vec[row].d_id));
		if (col == 1) {
			if (item->text().size() == 0) {
				item->setText(QString::number(device_vec[row].f_id));
			}
			else {
				int cf_id = item->text().toInt();
				if (FACTORY_ID_MAP.find(cf_id) != FACTORY_ID_MAP.end()) {
					device_vec[row].f_id = cf_id;
					if (device_vec[row].status == ORIGINAL_DATA) device_vec[row].status = CHANGED_DATA;
				}
				else {
					item->setText(QString::number(device_vec[row].f_id));
				}
			}
		}
		if (col == 2) {
			if (item->text().size() == 0) {
				item->setText(device_vec[row].d_type);
			}
			else {
				QString cd_type = item->text();
				DEV_TYPE_SET.insert(cd_type);
				device_vec[row].d_type = cd_type;
				if (device_vec[row].status == ORIGINAL_DATA) device_vec[row].status = CHANGED_DATA;
			}
		}
		if (col == 3) {
			if (item->text().size() == 0) {
				item->setText(QString::number(device_vec[row].d_pw));
			}
			else {
				int cd_pw = item->text().toInt();
				device_vec[row].d_pw = cd_pw;
				item->setText(QString::number(cd_pw));
				if (device_vec[row].status == ORIGINAL_DATA) device_vec[row].status = CHANGED_DATA;
			}
		}
		if (col == 4) {
			if (item->text().size() == 0) {
				item->setText(device_vec[row].d_etype);
			}
			else {
				QString cd_etype = item->text();
				ELETRIC_TYPE_SET.insert(cd_etype);
				device_vec[row].d_etype = cd_etype;
				if (device_vec[row].status == ORIGINAL_DATA) device_vec[row].status = CHANGED_DATA;
			}
		}
	}

	if (sender() == ui.tableWidget_waveFeature) {
		int row = item->row();
		int col = item->column();
		if (col == 0) item->setText(QString::number(feature_vec[row].w_id));
		if (col == 1) {
			if (item->text().size() == 0) {
				item->setText(QString::number(feature_vec[row].d_id));
			}
			else {
				int cd_id = item->text().toInt();
				if (DEV_ID_MAP.find(cd_id) != DEV_ID_MAP.end()) {
					feature_vec[row].d_id = cd_id;
					if (feature_vec[row].status == ORIGINAL_DATA) device_vec[row].status = CHANGED_DATA;
				}
				else {
					item->setText(QString::number(feature_vec[row].d_id));
				}
			}
		}
		if (col == 2) {
			if (item->text().size() == 0) {
				item->setText(feature_vec[row].w_type);
			}
			else {
				QString cw_type = item->text();
				feature_vec[row].w_type = cw_type;
				WAVE_TYPE_SET.insert(cw_type);
				if (feature_vec[row].status == ORIGINAL_DATA) feature_vec[row].status = CHANGED_DATA;
			}
		}
		if (col == 3) {
			if (item->text().size() == 0) {
				item->setText(QString::number(feature_vec[row].r_tnl));
			}
			else {
				int cr_tnl = item->text().toInt();
				feature_vec[row].r_tnl = cr_tnl;
				item->setText(QString::number(cr_tnl));
				if (feature_vec[row].status == ORIGINAL_DATA) feature_vec[row].status = CHANGED_DATA;
			}
		}
		if (col == 4) {
			if (item->text().size() == 0) {
				item->setText(feature_vec[row].w_filename);
			}
			else {
				QString cw_filename = item->text();
				feature_vec[row].w_filename = cw_filename;
				if (feature_vec[row].status == ORIGINAL_DATA) feature_vec[row].status = CHANGED_DATA;
			}
		}
	}


}

void DeviceStore::on_pushButton_exportFile_clicked()
{
	if (ui.tableWidget_waveFeature->currentRow() >= 0) {
		AddData *adddata = new AddData(Export_File, this);
		connect(adddata, SIGNAL(SendRequest(AddData*, int, QVector<int>*, QVector<QString>*)), this, SLOT(on_RecvRequest(AddData*, int, QVector<int>*, QVector<QString>*)));
		adddata->show();

	}
	else {
		ui.textEdit_msg->append(CC("[×] 未选择波形条目"));
	}
}

bool DeviceStore::SQL_LoadFile(int i) {
	if (!sql_connected) {
		ui.textEdit_msg->append(CC("[×] 数据库未连接!"));
		return false;
	}
	int w_id = feature_vec[i].w_id;

	QString sql_str = QString("select cfg_file,dat_file from feature where w_id = %1").arg(w_id);

	if (sql_query.exec(sql_str)) {
		sql_query.next();
		feature_vec[i].cfg_file = sql_query.value(0).toByteArray();
		feature_vec[i].dat_file = sql_query.value(1).toByteArray();
	}
	else {

		return false;
	}
	return true;
}
