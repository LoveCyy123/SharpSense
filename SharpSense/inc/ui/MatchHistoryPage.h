#include <qwidget.h>
#include <QTableView>
#include "core/StructType.h"

class MatchHistoryPage : public QWidget {
    Q_OBJECT
public:
    explicit MatchHistoryPage(QWidget *parent = nullptr);

    void setMatches(const QVector<MatchSummary> &matches);

signals:
    void matchSelected(const MatchId &id);

private:
    QTableView *mTable;
    QWidget    *mDetailPanel;
};
