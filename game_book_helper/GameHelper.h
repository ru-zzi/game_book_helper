#pragma once

#include <string>
#include <unordered_map>
#include <optional>
#include <chrono>
#include <set>
#include <Windows.h>

struct node {
    int id{};
    int parent{};
    bool needCheck{};
    std::vector<std::string> childs;
    std::vector<std::string> memo;
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
    void setRoot(int id, const std::string& memo);
    void go(int id);
    void add(int from, const std::string& to);
    void removeLastBranch();
    void setNeedCheck(int id, bool check);
    void setClue(const std::string& clue, int x);
    void addMemo(int id, const std::string& desc);
    void search(const std::string& token);
    void show(int id, const std::string& prefix = {}, bool isLast = true, const std::string& log = {});
    void showAll();
    void showClue();
    int findRoot(int id);
    std::optional<int> trySum(const std::string& log);
    void load();

    const std::chrono::system_clock::time_point startedAt;
    const HANDLE consoleHandle;
    std::chrono::seconds totalPlayTime;
    std::string saveFileName;
    int max_id{};
    int cursor{};
    std::vector<node> nodes;
    std::unordered_map<std::string, std::optional<int>> clues;
};
