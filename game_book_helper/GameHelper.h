#pragma once

#include <string>
#include <unordered_map>
#include <optional>
#include <chrono>
#include <set>

struct node {
    int id{};
    int parent{};
    bool needCheck{};
    std::set<int> childs;
    std::string desc;
};

class GameHelper
{
public:
    GameHelper();
    ~GameHelper();

    int play();

private:
    void err(std::string_view msg);
    void init(int n);
    void go(int to);
    void add(int from, int to);
    void setNeedCheck(int id, bool check);
    void backlog(const std::string& clue);
    void addBacklog(int from, const std::string& to);
    void clue();
    void setClue(const std::string& clue, int x);
    void memo(int id);
    void addMemo(int id, const std::string& desc);
    void show(int id, const std::string& prefix = {}, bool isLast = true);
    void showAll();
    int findRoot(int id);
    void load();

    const std::chrono::system_clock::time_point startedAt;
    std::string saveFileName;
    int cursor;
    std::vector<node> nodes;
    std::unordered_map<std::string, std::optional<int>> clues;
    std::unordered_map<std::string, std::vector<std::pair<int, std::string>>> backlogs;
};
