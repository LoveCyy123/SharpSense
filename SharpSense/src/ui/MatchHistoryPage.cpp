#include "ui/MatchHistoryPage.h"

MatchHistoryPage::MatchHistoryPage(QWidget *parent)
    : QWidget(parent),
    mTable(new QTableView(this)),
    mDetailPanel(new QWidget(this))//,
    //mModel(new QStandardItemModel(this))
{

}
