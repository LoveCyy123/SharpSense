#pragma once

#include <QObject>
#include <QNetworkAccessManager>
#include <QByteArray>
#include <string>
#include <vector>

#include "core/StructType.h"

// 通过 LCU（本地 League Client）获取当前登录账号的战绩
class LolClientService : public QObject
{
    Q_OBJECT
public:
    explicit LolClientService(QObject *parent = nullptr);

    // 读取 lockfile，初始化 LCU 连接信息
    // customLockfilePath 为空时会用一个默认路径（你可按自己环境改）
    bool initFromLockfile(const std::string &customLockfilePath = "");

    // 同步阻塞版本：获取当前召唤师最近几场比赛
    // count: 想要的场数；errorOut 非空时写入错误信息
    MatchSummaryList fetchCurrentSummonerMatches(int count = 10,
                                                 std::string *errorOut = nullptr);

private:
    bool loadLockfile(const std::string &customPath, std::string *errorOut);
    MatchSummaryList parseMatchHistoryJson(const QByteArray &data,
                                           std::string *errorOut);

private:
    QNetworkAccessManager mNetwork;
    std::string           mBaseUrl;    // 例如: https://127.0.0.1:61123
    QByteArray            mAuthHeader; // "Basic xxxxxxx"
    bool                  mReady = false;
};
