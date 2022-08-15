#pragma once

#include <QtWidgets/QMainWindow>
#include "ui_FileMerge.h"

#pragma pack(push)
#pragma pack(1)

struct stuWavePoint {
	int time;
	int idx;
	short data[68];
	unsigned short sw[8];
};

struct stuWavePoint_B {
	int time;
	int idx;
	short data[32];
};

typedef QPair<int, int> IIP;
typedef QPair<IIP, QVector<int>> IIVP;


#pragma pack(pop)
class FileMerge : public QMainWindow
{
    Q_OBJECT

public:
    FileMerge(QWidget *parent = nullptr);
    ~FileMerge();
private:
	int layout_spacing;
	bool zeroPintSetFlag;
	int currentFunction;

	QAction* action_configure;
	QAction* action_switch;

	QVector<stuWavePoint> f1pack_vec;
	QVector<stuWavePoint> f2pack_vec;
	QVector<stuWavePoint_B> fpack_vec;

	QVector<IIP> range_vec;
	QVector<IIVP> wave_vec;

	int f1vec_zp_index;
	int f2vec_zp_index;
	int f1zp_set;
	int f2zp_set;

	QString merge_output_path;
	QString process_output_path;
private:
	void HideZeroPointSet();
	void ShowZeroPointSet();
	QString GeneCfgStr(char *buf, int num, QString str);
	double CalcuCurveAvg(double x0, double y0, double x1, double y1, double x2, double y2, int granularity = 2);
	double GetLongVecAvg(QVector<short> &vec);

private slots:
	void on_pushButton_f1choose_clicked();
	void on_pushButton_f2choose_clicked();
	void on_pushButton_merge_clicked();
	void on_pushButton_outputPath_clicked();

	void on_setZeroPoint();
	void on_switchFunction();

	void on_pushButton_process_clicked();
private:
    Ui::FileMergeClass ui;
};
