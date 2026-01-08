#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QListWidget>
#include <qstackedwidget.h>
#include <QTableView>

QT_BEGIN_NAMESPACE
class MainWindow : public QMainWindow {
    Q_OBJECT
public:
    explicit MainWindow(QWidget *parent = nullptr);
    ~MainWindow() override;

private:
    QWidget *mTopBar;           // 顶部搜索条
    QListWidget *mNavList;      // 左侧导航
    QStackedWidget *mPages;     // 中间不同页面（Overview/Ranked/...）

    // 战绩页面里的控件
    QTableView *mMatchTable;    // 战绩列表
    QWidget *mMatchDetailView;  // 右侧单局详情（可以再拆一个类）
};

#endif // MAINWINDOW_H
