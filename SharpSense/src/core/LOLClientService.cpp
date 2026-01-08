#include "core/LolClientService.h"
#include "core/StructType.h"   // 确保能看到 QueueType / MatchSummary

#include <QFile>
#include <QDir>
#include <QNetworkRequest>
#include <QNetworkReply>
#include <QEventLoop>
#include <QSslConfiguration>
#include <QSslSocket>
#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonArray>
#include <QDebug>

// ====== 新增：工具函数，做一些映射 ======
namespace {

// queueId → QueueType + 显示文字
struct QueueMapping {
    QueueType   type;
    const char* label;
};

QueueMapping mapQueueFromId(int queueId)
{
    switch (queueId) {
    case 420:  // S13 之后：Ranked Solo/Duo
        return { QueueType::RankedSolo, "Ranked Solo/Duo" };
    case 440:  // Ranked Flex
        return { QueueType::RankedFlex, "Ranked Flex" };
    case 400:  // Normal Draft
    case 430:  // Normal Blind
        return { QueueType::Normal, "Normal" };
    case 450:  // ARAM
        return { QueueType::Aram, "ARAM" };
    case 1700: // Arena / 海克斯大乱斗一类
        return { QueueType::Arena, "Arena" };
    default:
        return { QueueType::Other, "Other" };
    }
}

MatchResult mapResultFromWinBool(bool win)
{
    return win ? MatchResult::Win : MatchResult::Loss;
}

} // namespace

LolClientService::LolClientService(QObject *parent)
    : QObject(parent)
{
}

// 读取 lockfile，解析出协议 / 端口 / 密码，拼出 baseUrl + 授权头
bool LolClientService::initFromLockfile(const std::string &customLockfilePath)
{
    std::string err;
    bool ok = loadLockfile(customLockfilePath, &err);
    if (!ok) {
        qWarning() << "initFromLockfile failed:" << QString::fromStdString(err);
    }
    return ok;
}

bool LolClientService::loadLockfile(const std::string &customPath,
                                    std::string *errorOut)
{
    QString lfPath;

    if (!customPath.empty()) {
        lfPath = QString::fromStdString(customPath);
    } else {
#ifdef Q_OS_MAC
        // ⚠️ 这里是示例路径，按你自己的客户端安装位置修改
        lfPath = "/Applications/League of Legends.app/Contents/LoL/lockfile";
#else
        // Windows 示例路径，同样请根据实际情况改
        lfPath = "C:/Riot Games/League of Legends/lockfile";
#endif
    }

    QFile f(lfPath);
    if (!f.open(QIODevice::ReadOnly | QIODevice::Text)) {
        if (errorOut) {
            *errorOut = "Failed to open lockfile: " + lfPath.toStdString();
        }
        return false;
    }

    QString line = QString::fromUtf8(f.readAll()).trimmed();
    f.close();

    // lockfile 一般格式: LeagueClientUx:PID:PORT:PASSWORD:PROTOCOL
    const QStringList parts = line.split(':');
    if (parts.size() < 5) {
        if (errorOut) {
            *errorOut = "Invalid lockfile content: " + line.toStdString();
        }
        return false;
    }

    const QString port     = parts[2];
    const QString password = parts[3];
    const QString protocol = parts[4]; // 一般是 https

    const QString baseUrl = QString("%1://127.0.0.1:%2")
                                .arg(protocol, port);

    mBaseUrl = baseUrl.toStdString();

    const QByteArray authPlain = "riot:" + password.toUtf8();
    mAuthHeader = "Basic " + authPlain.toBase64();

    mReady = true;
    return true;
}

// 同步阻塞调用：从 LCU 拿当前召唤师最近几场战绩
MatchSummaryList LolClientService::fetchCurrentSummonerMatches(int count,
                                                               std::string *errorOut)
{
    MatchSummaryList result;

    if (!mReady) {
        if (!initFromLockfile()) {
            if (errorOut) {
                *errorOut = "LolClientService not ready (initFromLockfile failed)";
            }
            return result;
        }
    }

    if (count <= 0) {
        return result;
    }

    // 构造 URL:
    // /lol-match-history/v1/products/lol/current-summoner/matches?begIndex=0&endIndex=count-1
    const std::string rel =
        "/lol-match-history/v1/products/lol/current-summoner/matches?begIndex=0&endIndex="
        + std::to_string(count - 1);

    const QUrl url(QString::fromStdString(mBaseUrl + rel));
    QNetworkRequest req(url);

    // 自签名证书，关掉校验
    QSslConfiguration sslConfig = QSslConfiguration::defaultConfiguration();
    sslConfig.setPeerVerifyMode(QSslSocket::VerifyNone);
    sslConfig.setProtocol(QSsl::TlsV1_2OrLater);
    req.setSslConfiguration(sslConfig);

    req.setRawHeader("Authorization", mAuthHeader);
    req.setRawHeader("Accept", "application/json");

    QNetworkReply *reply = mNetwork.get(req);

    QEventLoop loop;
    QObject::connect(reply, &QNetworkReply::finished, &loop, &QEventLoop::quit);
    loop.exec();

    if (reply->error() != QNetworkReply::NoError) {
        if (errorOut) {
            *errorOut = "Network error: " +
                        reply->errorString().toStdString();
        }
        reply->deleteLater();
        return result;
    }

    const QByteArray data = reply->readAll();
    reply->deleteLater();

    return parseMatchHistoryJson(data, errorOut);
}

// 解析 LCU 的 match-history JSON
// 参考结构：history["games"]["games"][i]["participants"][0]["stats"][...]
MatchSummaryList LolClientService::parseMatchHistoryJson(const QByteArray &data,
                                                         std::string *errorOut)
{
    MatchSummaryList list;

    QJsonParseError parseErr{};
    QJsonDocument doc = QJsonDocument::fromJson(data, &parseErr);
    if (parseErr.error != QJsonParseError::NoError || !doc.isObject()) {
        if (errorOut) {
            *errorOut = "JSON parse error: " +
                        parseErr.errorString().toStdString();
        }
        return list;
    }

    const QJsonObject root      = doc.object();
    const QJsonObject gamesRoot = root.value(QStringLiteral("games")).toObject();
    const QJsonArray  gamesArr  = gamesRoot.value(QStringLiteral("games")).toArray();

    for (const QJsonValue &v : gamesArr) {
        if (!v.isObject()) continue;
        const QJsonObject gameObj = v.toObject();

        MatchSummary m;

        // ---------- 基本信息 ----------
        // gameId
        if (gameObj.contains(QStringLiteral("gameId"))) {
            const auto gameIdVar = gameObj.value("gameId").toVariant();
            m.id.id = gameIdVar.toString().toStdString();
        }

        // 起始时间
        if (gameObj.contains(QStringLiteral("gameCreationDate"))) {
            m.gameStartMillis = gameObj.value("gameCreationDate").toVariant().toLongLong();
        }

        // 时长
        if (gameObj.contains(QStringLiteral("gameDuration"))) {
            m.durationSeconds = gameObj.value("gameDuration").toInt();
        }

        // ---------- 队列 / 模式 ----------
        int queueId = -1;
        if (gameObj.contains(QStringLiteral("queueId"))) {
            queueId = gameObj.value("queueId").toInt();
        }

        QueueMapping qm = mapQueueFromId(queueId);
        m.queueType  = qm.type;          // ✅ 不再是 string，而是枚举
        m.queueLabel = qm.label;         // ✅ 展示在 UI 的文字
        // 如果你还保留了 rawQueueId 字段，可以在 StructType 里加上再赋值：
        // m.rawQueueId = std::to_string(queueId);

        // ---------- 参与者（当前召唤师） ----------
        const QJsonArray participants = gameObj.value("participants").toArray();
        if (!participants.isEmpty() && participants.at(0).isObject()) {
            const QJsonObject p     = participants.at(0).toObject();
            const QJsonObject stats = p.value("stats").toObject();

            // 英雄信息
            if (p.contains(QStringLiteral("championId"))) {
                const int champId = p.value("championId").toInt();
                // 简单先把 championId 当成字符串名存进去；后面你可以做 id→名字映射
                m.champion.internalKey = std::to_string(champId);
                m.champion.name        = std::to_string(champId);
            }

            // KDA
            m.kills   = stats.value("kills").toInt();
            m.deaths  = stats.value("deaths").toInt();
            m.assists = stats.value("assists").toInt();

            // 胜负
            if (stats.contains(QStringLiteral("win"))) {
                bool winFlag = stats.value("win").toBool(false);
                m.result = mapResultFromWinBool(winFlag);
            } else {
                m.result = MatchResult::Unknown;
            }
        }

        list.push_back(std::move(m));
    }

    return list;
}
