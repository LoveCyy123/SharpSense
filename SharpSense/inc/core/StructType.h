#pragma once

#include <string>
#include <vector>
#include <cstdint>

// =================== 基本类型 ===================

// 比赛 ID
struct MatchId {
    std::string id;   // 可以是 gameId 或 matchId，取决于来源
};

// 队列 / 模式类型
enum class QueueType {
    Unknown,
    RankedSolo,    // 单双排
    RankedFlex,    // 灵活组排
    Normal,        // 匹配
    Aram,          // 大乱斗
    Arena,         // 海克斯大乱斗 / 斗魂竞技场
    Other
};

// 胜负结果
enum class MatchResult {
    Win,
    Loss,
    Remake,
    Unknown
};

// =================== 英雄 / 出装 / 符文 ===================

// 英雄信息
struct ChampionInfo {
    std::string name;        // 展示名, 如 "Ahri"
    std::string internalKey; // 内部 key，类似 "Ahri"
    std::string iconPath;    // 图标路径（本地磁盘或资源）
};

// 出装信息
struct ItemBuild {
    std::vector<int> itemIds; // 最终出装的 itemId 列表（6神装等）
};

// 符文信息
struct RuneInfo {
    int primaryTreeId  = 0;
    int secondaryTreeId = 0;
    std::vector<int> selectedRuneIds;
};

// =================== 战绩概要 & 详情 ===================

// 战绩列表用的“概要信息”（列表展示用）
struct MatchSummary {
    MatchId     id;

    // 模式/队列
    QueueType   queueType   = QueueType::Unknown; // 结构化类型
    std::string queueLabel;                       // 展示用文字，比如 "Ranked Solo"、"ARAM" 等
    std::string rawQueueId;                       // 原始 queueId 文本（比如 "420"）

    // 英雄
    ChampionInfo champion;

    // 线位（可以先留空，后面再填 top/mid/jungle 之类）
    std::string lane;

    // 时间相关
    std::int64_t gameStartMillis = 0;             // 时间戳（毫秒）
    std::string  gameStartTime;                   // 格式化后的时间，比如 "2026-01-08 14:30"
    int          durationSeconds  = 0;            // 对局时长（秒）

    // 数据
    int kills   = 0;
    int deaths  = 0;
    int assists = 0;

    int cs       = 0;     // 补刀
    double csPerMin = 0.0;

    int gold     = 0;
    int damage   = 0;

    MatchResult result = MatchResult::Unknown;
};

// 单局详情（以后扩展阵容、伤害表之类的）
struct MatchDetail {
    MatchSummary summary;
    ItemBuild    items;
    RuneInfo     runes;
};

// 方便用的类型别名
using MatchSummaryList = std::vector<MatchSummary>;
