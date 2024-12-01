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
    std::set<std::string> backlogs;
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
    void add(int from, const std::string& to);
    void setNeedCheck(int id, bool check);
    void clue();
    void setClue(const std::string& clue, int x);
    void memo(int id);
    void addMemo(int id, const std::string& desc);
    void show(int id, const std::string& prefix = {}, bool isLast = true, const std::string& log = {});
    void showAll();
    int findRoot(int id);
    std::optional<int> trySum(const std::string& log);
    void load();

    const std::chrono::system_clock::time_point startedAt;
    std::string saveFileName;
    int cursor;
    std::vector<node> nodes;
    std::unordered_map<std::string, std::optional<int>> clues;
};
