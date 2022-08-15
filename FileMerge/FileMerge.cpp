#include "FileMerge.h"
#include <QFileDialog>
#include <QDateTime>
#include <QDebug>
#include <QValidator>

#define CC(x) QString::fromLocal8Bit(x)

constexpr int LIMIT_GRANULARIRY = 10;
constexpr int BUCKET_SIZE = 256;

FileMerge::FileMerge(QWidget *parent)
	: QMainWindow(parent)
{
	
	ui.setupUi(this);

	action_configure = new QAction(CC("零点偏移设置(OFF)"), this);
	action_switch = new QAction(CC("功能切换(波形处理)"), this);

	zeroPintSetFlag = false;
	HideZeroPointSet();

	currentFunction = 0; // 0:文件合并 1:波形处理
	ui.pushButton_process->hide();

	process_output_path = QCoreApplication::applicationDirPath() + "/output_process/";
	merge_output_path = QCoreApplication::applicationDirPath() + "/output_merge/";
	ui.label_outputPath->setText(merge_output_path);


	QIntValidator *intvali = new QIntValidator(this);
	ui.lineEdit_f1zero->setValidator(intvali);
	ui.lineEdit_f2zero->setValidator(intvali);

	ui.menuBar->addAction(action_switch);
	ui.menuBar->addAction(action_configure);

	connect(action_configure, SIGNAL(triggered()), this, SLOT(on_setZeroPoint()));
	connect(action_switch, SIGNAL(triggered()), this, SLOT(on_switchFunction()));

}

FileMerge::~FileMerge()
{
}

void FileMerge::on_pushButton_f1choose_clicked()
{
	QFileDialog *fileDialog = new QFileDialog(this);
	fileDialog->setWindowTitle(CC("选择文件1"));
	fileDialog->setDirectory("c:/");
	fileDialog->setNameFilter(("File(*.dat)"));
	fileDialog->setFileMode(QFileDialog::ExistingFile);
	fileDialog->setViewMode(QFileDialog::Detail);

	QStringList filePathList;
	if (fileDialog->exec()) {
		filePathList = fileDialog->selectedFiles();
	}

	if (filePathList.size()) {
		ui.label_f1path->setText(filePathList.first());
	}
}

void FileMerge::on_pushButton_f2choose_clicked()
{
	QFileDialog *fileDialog = new QFileDialog(this);
	fileDialog->setWindowTitle(CC("选择文件2"));
	fileDialog->setDirectory("c:/");
	fileDialog->setNameFilter(("File(*.dat)"));
	fileDialog->setFileMode(QFileDialog::ExistingFile);
	fileDialog->setViewMode(QFileDialog::Detail);

	QStringList filePathList;
	if (fileDialog->exec()) {
		filePathList = fileDialog->selectedFiles();
	}
	if (filePathList.size()) {
		ui.label_f2path->setText(filePathList.first());
	}
}

void FileMerge::on_pushButton_merge_clicked()
{
	f1vec_zp_index = -1;
	f2vec_zp_index = -1;
	f1pack_vec.clear();
	f2pack_vec.clear();

	QString f1path = ui.label_f1path->text();
	QString f2path = ui.label_f2path->text();

	ui.textEdit_msg->append(" -------- " + QDateTime::currentDateTime().time().toString() + " File Merge" + " -------- ");

	if (f1path == "/") {
		ui.textEdit_msg->append(CC("[-] 文件1未选择!"));
		return;
	}
	if (f2path == "/") {
		ui.textEdit_msg->append(CC("[-] 文件2未选择!"));
		return;
	}

	if (f1path == f2path) {
		ui.textEdit_msg->append(CC("[-] 文件选择重复!"));
		return;
	}

	int packSize = (int)sizeof(stuWavePoint);

	QFile file1(f1path);
	QFile file2(f2path);

	int f1_pack_n;
	int f2_pack_n;

	if (zeroPintSetFlag) {
		if (ui.lineEdit_f1zero->text().size() == 0) {
			f1zp_set = 1;
		}
		else {
			f1zp_set = ui.lineEdit_f1zero->text().toInt();
		}

		if (ui.lineEdit_f2zero->text().size() == 0) {
			f2zp_set = 1;
		}
		else {
			f2zp_set = ui.lineEdit_f2zero->text().toInt();
		}
		ui.textEdit_msg->append(CC("[*] 文件2 合并点偏移量设置为:") + QString::number(f1zp_set));
		ui.textEdit_msg->append(CC("[*] 文件2 合并点偏移量设置为:") + QString::number(f2zp_set));
	}
	else {
		f1zp_set = 1;
		f2zp_set = 1;
	}

	if (!file1.open(QIODevice::ReadOnly)) {
		ui.textEdit_msg->append(CC("[-] 文件1 打开失败!"));
		return;
	}

	if (file1.size() % packSize == 0) {
		f1_pack_n = file1.size() / packSize;
		ui.textEdit_msg->append(CC("[+] 文件1 数据集大小 : ") + QString::number(f1_pack_n));
	}
	else {
		ui.textEdit_msg->append(CC("[-] 文件1 数据集解析错误 : ") + QString::number(file1.size()));
		return;
	}

	for (int i = 0; i < f1_pack_n; i++) {
		stuWavePoint tmp_wp;
		file1.read((char*)&tmp_wp, packSize);
		f1pack_vec.push_back(tmp_wp);
	}

	for (int i = 0, f1zp_count = 0; i < f1_pack_n; i++) {
		if (i == 0) {
			if (f1pack_vec[0].data[0] == 0 && f1pack_vec[1].data[0] < 0 && ++f1zp_count == f1zp_set) {
				f1vec_zp_index = 0;
				ui.textEdit_msg->append(CC("[+] 文件1 合并偏移点 : ") + QString::number(f1vec_zp_index));
			}
		}
		else {
			if (i != f1_pack_n - 1 && f1pack_vec[i - 1].data[0] > 0 && f1pack_vec[i].data[0] == 0 && f1pack_vec[i + 1].data[0] < 0 && ++f1zp_count == f1zp_set) {
				f1vec_zp_index = i - 1;
				ui.textEdit_msg->append(CC("[+] 文件1 合并偏移点 : ") + QString::number(f1vec_zp_index));
			}
			if (f1pack_vec[i - 1].data[0] > 0 && f1pack_vec[i].data[0] < 0 && ++f1zp_count == f1zp_set) {
				f1vec_zp_index = i - 1;
				ui.textEdit_msg->append(CC("[+] 文件1 合并偏移点 : ") + QString::number(f1vec_zp_index));
			}
		}
	}

	if (f1vec_zp_index == -1) {
		ui.textEdit_msg->append(CC("[-] 文件1 合并点查找失败!"));
		return;
	}

	//----------------------------------- File1 complete

	if (!file2.open(QIODevice::ReadOnly)) {
		ui.textEdit_msg->append(CC("[-] 文件2 打开失败!"));
		return;
	}

	if (file2.size() % packSize == 0) {
		f2_pack_n = file2.size() / packSize;
		ui.textEdit_msg->append(CC("[+] 文件2 数据集大小 : ") + QString::number(f2_pack_n));
	}
	else {
		ui.textEdit_msg->append(CC("[-] 文件2 数据集解析错误 : ") + QString::number(file2.size()));
		return;
	}

	for (int i = 0; i < f2_pack_n; i++) {
		stuWavePoint tmp_wp;
		file2.read((char*)&tmp_wp, packSize);
		f2pack_vec.push_back(tmp_wp);
	}

	for (int i = 0, f2zp_count = 0; i < f2_pack_n; i++) {
		if (i == 0) {
			if (f2pack_vec[0].data[0] == 0 && f2pack_vec[1].data[0] < 0 && ++f2zp_count == f2zp_set) {
				f2vec_zp_index = 0;
				ui.textEdit_msg->append(CC("[+] 文件2 合并偏移点 : ") + QString::number(f2vec_zp_index));
			}
		}
		else {
			if (i != f2_pack_n - 1 && f2pack_vec[i - 1].data[0] > 0 && f2pack_vec[i].data[0] == 0 && f2pack_vec[i + 1].data[0] < 0 && ++f2zp_count == f2zp_set) {
				f2vec_zp_index = i - 1;
				ui.textEdit_msg->append(CC("[+] 文件2 合并偏移点 : ") + QString::number(f2vec_zp_index));
			}
			if (f2pack_vec[i - 1].data[0] > 0 && f2pack_vec[i].data[0] < 0 && ++f2zp_count == f2zp_set) {
				f2vec_zp_index = i - 1;
				ui.textEdit_msg->append(CC("[+] 文件2 合并偏移点 : ") + QString::number(f2vec_zp_index));
			}
		}
	}

	if (f2vec_zp_index == -1) {
		ui.textEdit_msg->append(CC("[-] 文件2 合并点查找失败!"));
		return;
	}

	//----------------------------------- File2 complete

	ui.textEdit_msg->append(CC("[+] 开始合并..."));


	int f1_merge_count = f1_pack_n - f1vec_zp_index - 1;
	int f2_merge_count = f2_pack_n - f2vec_zp_index - 1;
	int merge_count = std::min(f1_merge_count, f2_merge_count);

	QVector<stuWavePoint> f3pack_vec;

	for (int i = 0; i < merge_count; i++) {
		stuWavePoint s1 = f1pack_vec[i + f1vec_zp_index];
		stuWavePoint s2 = f2pack_vec[i + f2vec_zp_index];
		stuWavePoint s3;
		s3.time = i + 1;
		s3.idx = 380 + (i * 625);
		memset(s3.data, 0, sizeof(s3.data));
		memset(s3.sw, 0, sizeof(s3.sw));


		s3.data[48] = s1.data[48] + s2.data[48];
		f3pack_vec.push_back(s3);
	}

	ui.textEdit_msg->append(CC("[+] 合并完成 !"));

	QString dateStr, timeStr;
	QString output_filename_dat;
	QString output_filename_cfg;


	QString year_str = QString::number(QDate::currentDate().year());
	QString month_str = QString::number(QDate::currentDate().month());
	QString day_str = QString::number(QDate::currentDate().day());
	dateStr = year_str + month_str + day_str;

	if (month_str.size() == 1) month_str = "0" + month_str;
	if (day_str.size() == 1) day_str = "0" + day_str;

	timeStr = QTime::currentTime().toString();

	output_filename_dat = dateStr.replace('/', '-') + "_" + timeStr.replace(':', '-') + ".dat";
	output_filename_cfg = dateStr.replace('/', '-') + "_" + timeStr.replace(':', '-') + ".cfg";

	int pmid = f1path.lastIndexOf('.');
	QString cfg_source_path = f1path.mid(0, pmid) + ".cfg";

	QFile cfgFile(cfg_source_path);
	QFile outputCfgFile(merge_output_path + output_filename_cfg);

	if (outputCfgFile.open(QIODevice::WriteOnly)) {

	}
	else {
		ui.textEdit_msg->append(CC("[-] cfg文件写入失败!"));
		QFile::remove(merge_output_path + output_filename_cfg);
		return;
	}

	if (cfgFile.open(QIODevice::ReadOnly)) {
		char buf[1024];
		int readlen;
		int curlen = 0;
		int dataLen = -1;
		int suffixLen = -1;
		while (1) {
			curlen++;
			readlen = cfgFile.readLine(buf, sizeof(buf));
			if (readlen < 0) break;
			if (curlen == 2) {
				QString num_str;
				for (int i = 0; i < readlen - 2 && buf[i] != ','; i++) {
					num_str.push_back(buf[i]);
				}
				dataLen = num_str.toInt();

			}
			if (dataLen != -1 && curlen == dataLen + 4) {
				QString num_str;
				for (int i = 0; i < readlen - 2; i++) {
					num_str.push_back(buf[i]);
				}
				suffixLen = num_str.toInt();

			}
			if (dataLen != -1 && suffixLen != -1 && curlen == dataLen + suffixLen + 4) {
				int pi;
				for (pi = 0; pi < readlen - 2 && buf[pi] != ','; pi++) {}
				pi++;

				QString mc_str = QString::number(merge_count);
				for (int i = 0; i < mc_str.size(); i++, pi++) {
					buf[pi] = mc_str[i].toLatin1();
				}
				buf[pi++] = '\r';
				buf[pi++] = '\n';
				dataLen = pi;
			}
			outputCfgFile.write(buf, readlen);
		}
	}
	else {
		ui.textEdit_msg->append(CC("[-] cfg文件写入失败!"));
		return;
	}

	QFile file3(merge_output_path + output_filename_dat);
	if (file3.open(QIODevice::WriteOnly)) {
		for (int i = 0; i < f3pack_vec.size(); i++) {
			file3.write((char*)&f3pack_vec[i], sizeof(stuWavePoint));
		}
	}
	else {
		ui.textEdit_msg->append(CC("[-] dat文件写入失败!"));
		return;
	}
	ui.textEdit_msg->append(CC("[+] 文件写入成功! : ") + merge_output_path + output_filename_dat);
}

void FileMerge::on_pushButton_outputPath_clicked()
{
	QFileDialog *fileDialog = new QFileDialog(this);
	fileDialog->setWindowTitle(CC("选择输出文件夹"));
	fileDialog->setDirectory("c:/");
	fileDialog->setFileMode(QFileDialog::Directory);
	fileDialog->setViewMode(QFileDialog::Detail);
	QStringList directoryPathList;
	if (fileDialog->exec()) {
		directoryPathList = fileDialog->selectedFiles();
	}
	if (directoryPathList.size()) {
		QString directoryPath = directoryPathList.first();
		if (directoryPath[directoryPath.size() - 1] != "/") {
			directoryPath.push_back("/");
		}
		ui.label_outputPath->setText(directoryPath);
		if (currentFunction == 0) {
			merge_output_path = directoryPath;
		}
		else {
			process_output_path = directoryPath;
		}
	}
}

void FileMerge::on_setZeroPoint() {
	if (zeroPintSetFlag == false) {
		zeroPintSetFlag = true;
		ShowZeroPointSet();
	}
	else {
		zeroPintSetFlag = false;
		HideZeroPointSet();
	}
}

void FileMerge::HideZeroPointSet() {
	action_configure->setText(CC("零点偏移设置(OFF)"));
	for (int i = 0; i < ui.horizontalLayout_setZeroPoint->count() - 1; i++) {
		((QWidget*)(ui.horizontalLayout_setZeroPoint->itemAt(i)->widget()))->hide();
	}

	ui.verticalLayout->removeItem(ui.horizontalLayout_setZeroPoint);

	for (int i = 0; i < ui.horizontalLayout_splitLine->count(); i++) {
		((QWidget*)(ui.horizontalLayout_splitLine->itemAt(i)->widget()))->hide();
	}
	ui.verticalLayout->removeItem(ui.horizontalLayout_splitLine);

}

void FileMerge::ShowZeroPointSet() {
	action_configure->setText(CC("零点偏移设置(ON)"));
	for (int i = 0; i < ui.horizontalLayout_setZeroPoint->count() - 1; i++) {
		((QWidget*)(ui.horizontalLayout_setZeroPoint->itemAt(i)->widget()))->show();
	}

	ui.verticalLayout->insertItem(0, ui.horizontalLayout_setZeroPoint);

	for (int i = 0; i < ui.horizontalLayout_splitLine->count(); i++) {
		((QWidget*)(ui.horizontalLayout_splitLine->itemAt(i)->widget()))->show();
	}
	ui.verticalLayout->insertItem(1, ui.horizontalLayout_splitLine);
}

void FileMerge::on_switchFunction() {
	if (currentFunction == 0) {
		currentFunction = 1;
		action_configure->setVisible(false);
		action_switch->setText(CC("功能切换(文件合并)"));
		ui.pushButton_merge->hide();
		ui.pushButton_process->show();
		if (zeroPintSetFlag) {
			zeroPintSetFlag = false;
			HideZeroPointSet();
		}
		for (int i = 0; i < ui.horizontalLayout_f2->count(); i++) {
			((QWidget*)(ui.horizontalLayout_f2->itemAt(i)->widget()))->hide();
		}  
		layout_spacing = ui.horizontalLayout_f2->spacing(); 
		ui.verticalLayout->removeItem(ui.horizontalLayout_f2);
		ui.label_outputPath->setText(process_output_path);
	}
	else {
		currentFunction = 0;
		action_configure->setVisible(true);
		action_switch->setText(CC("功能切换(波形处理)"));
		ui.pushButton_merge->show();
		ui.pushButton_process->hide();
		for (int i = 0; i < ui.horizontalLayout_f2->count(); i++) {
			((QWidget*)(ui.horizontalLayout_f2->itemAt(i)->widget()))->show();
		}
		ui.horizontalLayout_f2->setSpacing(layout_spacing);
		ui.verticalLayout->insertItem(1, ui.horizontalLayout_f2);
		ui.label_outputPath->setText(merge_output_path);
	}

}

void FileMerge::on_pushButton_process_clicked()
{
	fpack_vec.clear();
	QString fpath = ui.label_f1path->text();
	ui.textEdit_msg->append(" -------- " + QDateTime::currentDateTime().time().toString() + " Wave Process" + " -------- ");

	if (fpath == "/") {
		ui.textEdit_msg->append(CC("[-] 文件未选择!"));
		return;
	}

	QFile file(fpath);
	if (!file.open(QIODevice::ReadOnly)) {
		ui.textEdit_msg->append(CC("[-] 文件打开失败!"));
		return;
	}

	int packSize = (int)sizeof(stuWavePoint_B);
	int file_pack_n;
	if (file.size() % packSize == 0) {
		file_pack_n = file.size() / packSize;
		ui.textEdit_msg->append(CC("[+] 文件数据集大小 : ") + QString::number(file_pack_n));
	}
	else {
		ui.textEdit_msg->append(CC("[-] 文件数据集解析错误 : ") + QString::number(file.size()));
		return;
	}


	for (int i = 0; i < file_pack_n; i++) {
		stuWavePoint_B tmp_wp;
		file.read((char*)&tmp_wp, packSize);
		fpack_vec.push_back(tmp_wp);
	}
	for (int i = 0, l = -1; i < file_pack_n; i++) {
		if (i == 0) {
			if (fpack_vec[0].data[8] == 0 && fpack_vec[1].data[8] < 0)
				l = 0;
		}
		else {
			if (i != file_pack_n - 1 && fpack_vec[i - 1].data[8] > 0 && fpack_vec[i].data[8] == 0 && fpack_vec[i + 1].data[8] < 0) {
				if (l != -1) range_vec.push_back({ l,i });
				l = i;
			}
			if (fpack_vec[i - 1].data[8] > 0 && fpack_vec[i].data[8] < 0) {
				if (l != -1) range_vec.push_back({ l,i });
				l = i;
			}
		}
	}

	double maxLen = INT_MIN;
	double minLen = INT_MAX;


	for (int i = 0; i < range_vec.size(); i++) {
		int maxV = INT_MIN;
		int minV = INT_MAX;
		int len = range_vec[i].second - range_vec[i].first;
		if (len > maxLen) maxLen = len;
		if (len < minLen) minLen = len;
		if (len < 250) qDebug() << range_vec[i].first << " " << range_vec[i].second;
		for (int j = range_vec[i].first; j <= range_vec[i].second; j++) {
			if (fpack_vec[j].data[8] > maxV) maxV = fpack_vec[j].data[8];
			if (fpack_vec[j].data[8] < minV) minV = fpack_vec[j].data[8];
		}

		maxV /= LIMIT_GRANULARIRY;
		minV /= LIMIT_GRANULARIRY;

		bool insertOK = false;

		for (IIVP &iiv : wave_vec) {
			IIP &iip = iiv.first;
			QVector<int> &v = iiv.second;
			if (iip.first == maxV && iip.second == minV) {
				insertOK = true;
				v.push_back(i);
				break;
			}
		}

		if (!insertOK) {
			IIP iivp;
			iivp.first = maxV;
			iivp.second = minV;
			IIVP iiv;
			iiv.first = iivp;
			iiv.second.push_back(i);
			wave_vec.push_back(iiv);
		}
	}

	qDebug() << "# range        :" << minLen << " " << maxLen;
	qDebug() << "# wave vec size:" << wave_vec.size();

	for (int i = 0; i < wave_vec.size(); i++) {
		QMap<int, int> len_map;
		QVector<int> &range_index_vec = wave_vec[i].second;
		typedef QPair<int, QVector<int>> IVP;
		QVector<IVP> len_range_vec;

		for (int j = 0; j < range_index_vec.size(); j++) {
			int idx = range_index_vec[j];
			int len = range_vec[idx].second - range_vec[idx].first;
			if (len_map.find(len) == len_map.end()) {
				len_map[len] = len_range_vec.size();
				QVector<int> tv;
				tv.push_back(idx);
				len_range_vec.push_back({ len,tv });
			}
			else {
				qDebug() << "Add Entrey Len:" << len;
				int target = len_map[len];
				len_range_vec[target].second.push_back(idx);
			}
		}

		for (int j = 0; j < len_range_vec.size(); j++) {
			int len = len_range_vec[j].first;

			for (int k = 0; k < len; k++) {
				QVector<short> value_vec;
				for (int l = 0; l < len_range_vec[j].second.size(); l++) {
					int ridx = len_range_vec[j].second[l];
					int fidx = range_vec[ridx].first + k;
					value_vec.push_back(fpack_vec[fidx].data[8]);
				}
				int res = GetLongVecAvg(value_vec);
				for (int l = 0; l < len_range_vec[j].second.size(); l++) {
					int ridx = len_range_vec[j].second[l];
					int fidx = range_vec[ridx].first + k;
					fpack_vec[fidx].data[8] = res;
				}

			}
		}
	}


	int avlLen = (minLen + maxLen) / 2 * 5;

	QVector<stuWavePoint_B> output_vec;
	for (int i = 0; i < fpack_vec.size() - avlLen; i++) {
		stuWavePoint_B tmp_wp;
		tmp_wp.time = fpack_vec[i].time;
		tmp_wp.idx = fpack_vec[i].idx;
		memcpy(tmp_wp.data, fpack_vec[i].data, sizeof(fpack_vec[i].data));
		if (i >= avlLen - 1) {
			tmp_wp.data[12] = fpack_vec[i].data[8] - fpack_vec[i - avlLen].data[8];
			if (i - avlLen > 0) {
				double d = fpack_vec[i].data[8] - CalcuCurveAvg(i - avlLen - 1, fpack_vec[i - avlLen - 1].data[8], \
					i - avlLen, fpack_vec[i - avlLen].data[8], \
					i - avlLen + 1, fpack_vec[i - avlLen + 1].data[8]);
				//                tmp_wp.data[13] = d + 0.5;
			}
			if (i % 100 == 0) {
				//                qDebug() << tmp_wp.data[12] << " " << tmp_wp.data[13];
			}
		}
		output_vec.push_back(tmp_wp);
	}

	QString output_path = ui.label_outputPath->text();

	QString dateStr, timeStr;
	QString output_filename_dat;
	QString output_filename_cfg;

	QString year_str = QString::number(QDate::currentDate().year());
	QString month_str = QString::number(QDate::currentDate().month());
	QString day_str = QString::number(QDate::currentDate().day());
	dateStr = year_str + month_str + day_str;

	if (month_str.size() == 1) month_str = "0" + month_str;
	if (day_str.size() == 1) day_str = "0" + day_str;

	timeStr = QTime::currentTime().toString();

	output_filename_dat = dateStr.replace('/', '-') + "_" + timeStr.replace(':', '-') + ".dat";
	output_filename_cfg = dateStr.replace('/', '-') + "_" + timeStr.replace(':', '-') + ".cfg";


	int pmid = fpath.lastIndexOf('.');
	QString cfg_source_path = fpath.mid(0, pmid) + ".cfg";

	QFile cfgFile(cfg_source_path);
	QFile outputCfgFile(process_output_path + output_filename_cfg);
	qDebug() << process_output_path + output_filename_cfg;

	if (outputCfgFile.open(QIODevice::WriteOnly)) {

	}
	else {
		ui.textEdit_msg->append(CC("[-] cfg文件写入失败!"));
		QFile::remove(process_output_path + output_filename_cfg);
		return;
	}

	if (cfgFile.open(QIODevice::ReadOnly)) {
		char backupbuf_a[1024];
		int backuplen_a;
		char backupbuf_b[1024];
		int backuplen_b;

		char buf[1024];
		int readlen;
		int curlen = 0;
		int dataLen = -1;
		int suffixLen = -1;
		while (1) {
			curlen++;
			readlen = cfgFile.readLine(buf, sizeof(buf));
			if (readlen < 0) break;
			if (curlen == 2) {
				QString num_str;
				for (int i = 0; i < readlen - 2 && buf[i] != ','; i++) {
					num_str.push_back(buf[i]);
				}
				dataLen = num_str.toInt();
			}
			if (dataLen != -1 && curlen == dataLen + 4) {
				QString num_str;
				for (int i = 0; i < readlen - 2; i++) {
					num_str.push_back(buf[i]);
				}
				suffixLen = num_str.toInt();
			}
			if (curlen == 11) {
				QString tmpstr_a = GeneCfgStr(buf, 13, "output_A");
				backuplen_a = tmpstr_a.toLatin1().size();
				memcpy(backupbuf_a, tmpstr_a.toLatin1().data(), backuplen_a);

				QString tmpstr_b = GeneCfgStr(buf, 14, "output_B");
				backuplen_b = tmpstr_b.toLatin1().size();
				memcpy(backupbuf_b, tmpstr_b.toLatin1().data(), backuplen_b);

			}

			if (curlen == 15) {
				memcpy(buf, backupbuf_a, backuplen_a);
				readlen = backuplen_a;
			}
			/*if(curlen == 16) {
				memcpy(buf,backupbuf_b,backuplen_b);
				readlen = backuplen_b;
			}*/

			if (dataLen != -1 && suffixLen != -1 && curlen == dataLen + suffixLen + 4) {
				int pi;
				for (pi = 0; pi < readlen - 2 && buf[pi] != ','; pi++) {}
				pi++;

				QString mc_str = QString::number(output_vec.size());
				for (int i = 0; i < mc_str.size(); i++, pi++) {
					buf[pi] = mc_str[i].toLatin1();
				}
				buf[pi++] = '\r';
				buf[pi++] = '\n';
				dataLen = pi;
			}
			outputCfgFile.write(buf, readlen);
		}
	}
	else {
		ui.textEdit_msg->append(CC("[-] cfg文件写入失败!"));
		return;
	}

	QFile datFile(process_output_path + output_filename_dat);
	if (datFile.open(QIODevice::WriteOnly)) {
		for (int i = 0; i < output_vec.size(); i++) {
			datFile.write((char*)&output_vec[i], sizeof(stuWavePoint_B));
		}
	}
	else {
		ui.textEdit_msg->append(CC("[-] dat文件写入失败!"));
		return;
	}
	ui.textEdit_msg->append(CC("[+] 文件写入成功! : ") + merge_output_path + output_filename_dat);
}

double FileMerge::CalcuCurveAvg(double x0, double y0, double x1, double y1, double x2, double y2, int granularity) {

	return 0;

}

QString FileMerge::GeneCfgStr(char *buf, int num, QString str) {
	QString tmpstr(buf);
	QStringList sp_str = tmpstr.split(',');
	sp_str[0] = QString::number(num);
	QString num_b = QString::number(num);
	QString num_char = num_b + "#" + str;
	sp_str[1] = num_char;
	tmpstr.clear();
	for (int i = 0; i < sp_str.size(); i++) {
		tmpstr += sp_str[i];
		if (i != sp_str.size() - 1) {
			tmpstr += ",";
		}
	}
	return tmpstr;
}

double FileMerge::GetLongVecAvg(QVector<short> &vec) {

	if (vec.size() == 0) return 0;
	int bucket_count = vec.size() / BUCKET_SIZE;
	double buckets_avg = 0;;

	if (bucket_count) {
		QVector<double> buckets(bucket_count, 0);
		for (int i = 0; i < bucket_count * BUCKET_SIZE; i++) {
			buckets[i / BUCKET_SIZE] += vec[i];
		}
		double bucket_total = 0;
		for (int i = 0; i < bucket_count; i++) {
			bucket_total += buckets[i] / 256;
		}
		buckets_avg = bucket_total / bucket_count;
	}
	else {
		double total_value = 0;
		for (int i = 0; i < vec.size(); i++) {
			total_value += vec[i];
		}
		return total_value / vec.size();
	}

	if (bucket_count * BUCKET_SIZE == vec.size()) {
		return buckets_avg;
	}

	int surplus = vec.size() - bucket_count * BUCKET_SIZE;
	double surplus_total = 0;
	double surplus_avg;
	for (int i = bucket_count * BUCKET_SIZE; i < vec.size(); i++) {
		surplus_total += vec[i];
	}

	surplus_avg = surplus_total / surplus;
	double res = buckets_avg + (surplus * (surplus_avg - buckets_avg)) / vec.size();

	return res;
}