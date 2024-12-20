#include "GameHelper.h"

#include <ranges>
#include <fstream>
#include <print>
#include <iostream>
#include <functional>
#include <algorithm>

constexpr std::string_view gameNames[] = { "늑대인간_마을에서_탈출", "쌍둥이_섬에서_탈출", "10인의_우울한_용의자" };

bool isRoot(const node& x) {
    return x.id == x.parent;
}

std::vector<std::string> parse(std::string_view sv)
{
    return std::views::split(sv, '+') | std::ranges::to<std::vector<std::string>>();
}

GameHelper::GameHelper()
    : startedAt(std::chrono::system_clock::now())
{
    for (int i = 0; i < 3; ++i)
    {
        std::print("{}. {}\n", i, gameNames[i]);
    }
    std::print("게임 번호를 선택하세요: ");
    int n;
    std::cin >> n;

    saveFileName = std::format("saveFiles/{}.txt", gameNames[n]);

    load();
}

GameHelper::~GameHelper()
{
}

int GameHelper::play()
{
    std::fstream file;
    file.open(saveFileName, std::ios::app);

    std::string line;

    while (std::print("\n===================================\n\n"), std::getline(std::cin, line))
    {
        std::stringstream ss(line);
        std::string cmd;
        ss >> cmd;

        std::print("\n");

        if (cmd == "init")
        {
            int n;
            if (!(ss >> n))
            {
                err("input like: init n");
                continue;
            }
            init(n);
        }
        else if (cmd == "go")
        {
            int id;
            if (!(ss >> id))
            {
                err("input like: go id");
                continue;
            }
            go(id);
            show(findRoot(cursor));
        }
        else if (cmd == "add")
        {
            std::string id;
            if (!(ss >> id))
            {
                err("input like: add id|backlog");
                continue;
            }
            add(cursor, id);
            show(findRoot(cursor));
        }
        else if (cmd == "needcheck")
        {
            setNeedCheck(cursor, true);
        }
        else if (cmd == "memo")
        {
            std::string memo;
            ss.ignore();
            if (!std::getline(ss, memo))
            {
                showMemo(cursor);
                continue;
            }
            addMemo(cursor, memo);
            showMemo(cursor);
        }
        else if (cmd == "clue")
        {
            showClue();
            continue;
        }
        else if (cmd == "setclue")
        {
            std::string c;
            int x;
            if (!(ss >> c) || !(ss >> x))
            {
                err("input like: setclue clue x");
                continue;
            }
            setClue(c, x);
            showClue();
        }
        else if (cmd == "show")
        {
            show(findRoot(cursor));
            continue;
        }
        else if (cmd == "showall")
        {
            showAll();
            continue;
        }
        else if (cmd == "help")
        {
            std::print("init n           : 최대 n번까지 있는 게임 초기화.\n"
                "go id            : n번단락으로 이동. n번이 이전단락이 없다면 root단락.\n"
                "add id|backlog   : 현재 단락에서 n단락으로 이동하는 간선 추가.\n"
                "clue             : 등록된 모든 단서와 값 출력, 아직 발견되지 않았으면 -1.\n"
                "setclue clue x   : str단서는 n. 연관된 백로그 중 알아낸 간선을 모두 추가.\n"
                "memo [memo]      : 현재 단락에 등록된 메모를 출력, 현재 단락에 str 메모를 추가.\n"
                "show             : 현재 단락의 연결상태를 출력.\n"
                "needcheck        : 현재 단락을 확인 필요한 상태로 변경\n"
                "showall          : 모든 단락 연결상태를 출력.\n"
                "quit             : 게임 종료\n");
            continue;
        }
        else if (cmd == "quit")
        {
            break;
        }
        else
        {
            err("invalid command.");
            continue;
        }

        file << line << std::endl;
    }

    const auto duration = std::chrono::duration_cast<std::chrono::seconds>(std::chrono::system_clock::now() - startedAt);
    totalPlayTime += duration;
    file << std::format("playtime {}", duration.count()) << std::endl;

    file.close();

    std::print("GameHelper::오늘의 게임끝~\n오늘 게임시간 : {}\n누적 게임시간 : {}\n",
        std::chrono::duration_cast<std::chrono::minutes>(duration),
        std::chrono::duration_cast<std::chrono::minutes>(totalPlayTime));

    return 0;
}

void GameHelper::err(std::string_view msg)
{
    std::cerr << "[error] " << msg << std::endl;
}

void GameHelper::init(int n)
{
    nodes = std::vector<node>(n + 5);
    for (int i : std::views::iota(0, n + 5))
    {
        nodes[i].id = i;
    }
}

void GameHelper::go(int id)
{
    cursor = id;
    if (!nodes[id].parent)
    {
        nodes[id].parent = id;
    }
    setNeedCheck(id, false);
}

void GameHelper::add(int from, const std::string& to)
{
    if (std::ranges::all_of(to, isdigit))
    {
        const auto ito = std::stoi(to);
        nodes[from].childs.insert(ito);
        nodes[ito].parent = from;
        setNeedCheck(ito, true);
    }
    else
    {
        nodes[from].backlogs.insert(to);
        if (const auto sum = trySum(to); sum)
        {
            nodes[*sum].parent = from;
            setNeedCheck(*sum, true);
        }
    }
}

void GameHelper::setNeedCheck(int id, bool check)
{
    nodes[id].needCheck = check;
}

void GameHelper::showClue()
{
    std::vector<std::pair<std::string, std::optional<int>>> v;
    v.assign_range(clues);
    std::ranges::sort(v, [](const auto& l, const auto& r) {
        if (l.first.starts_with("시간") != r.first.starts_with("시간"))
        {
            return l.first.starts_with("시간");
        }
        return l.first < r.first;
        });
    for (const auto& clue : v)
    {
        std::print("{}: {}\n", clue.first, clue.second ? *clue.second : -1);
    }
}

void GameHelper::setClue(const std::string& showClue, int x)
{
    clues[showClue] = x;
    for (const auto& node : nodes)
    {
        for (const auto& backlog : node.backlogs)
        {
            if (const auto sum = trySum(backlog); sum)
            {
                if (!nodes[*sum].parent) // 새로 발견됨
                {
                    nodes[*sum].parent = node.id;
                    setNeedCheck(*sum, true);
                }
            }
        }
    }
}

void GameHelper::showMemo(int id)
{
    std::print("memo: {}\n", nodes[id].memo);
}

void GameHelper::addMemo(int id, const std::string& adding_desc)
{
    auto& desc = nodes[id].memo;
    desc += desc.empty() ? adding_desc : ", " + adding_desc;
}

void GameHelper::show(int id, const std::string& prefix, bool isLast, const std::string& log)
{
    const auto& node = nodes[id];

    std::print("{:20}\t\t{}\n",
        std::format("{}{}{}{}{}",
            prefix,
            (isLast ? "└──" : "├──"),
            log.empty() ? "" : std::format("({})=", log),
            node.id,
            cursor == node.id ? "◀@" : node.needCheck ? "ㆍ" : ""),
        node.memo.empty() ? "" : "memo: " + node.memo);

    int isNotLast = node.childs.size() + node.backlogs.size();
    for (int id : node.childs)
    {
        show(id, prefix + (isLast ? "   " : "│  "), !--isNotLast);
    }
    for (const std::string& backlog : node.backlogs)
    {
        if (const auto sum = trySum(backlog); sum)
        {
            show(*sum, prefix + (isLast ? "   " : "│  "), !--isNotLast, backlog);
        }
        else
        {
            std::print("{:20}\n",
                std::format("{}{}{}",
                    prefix + (isLast ? "   " : "│  "),
                    (!--isNotLast ? "└──" : "├──"),
                    std::format("({})=?", backlog)));
        }
    }
}

void GameHelper::showAll()
{
    for (auto id : std::views::iota(1u, nodes.size()))
    {
        if (isRoot(nodes[id]))
        {
            show(id);
        }
    }
}

int GameHelper::findRoot(int id)
{
    while (nodes[id].parent != id)
    {
        id = nodes[id].parent;
    }
    return id;
}

std::optional<int> GameHelper::trySum(const std::string& log)
{
    int sum = 0;
    if (!std::ranges::all_of(parse(log), [&sum, this](const auto token) {
        try {
            sum += std::stoi(token);
            return true;
        }
        catch (std::invalid_argument) {
            if (clues[token].has_value())
            {
                sum += *clues[token];
                return true;
            }
        }
        return false;
        }))
    {
        return std::nullopt;
    }
    return sum;
}

void GameHelper::load()
{
    std::fstream file;
    file.open(saveFileName, std::ios::in);

    std::string line;

    while (getline(file, line))
    {
        std::stringstream ss(line);
        std::string cmd;

        ss >> cmd;

        if (cmd == "init")
        {
            int n;
            ss >> n;
            init(n);
        }
        else if (cmd == "go")
        {
            int to;
            ss >> to;
            go(to);
        }
        else if (cmd == "add")
        {
            std::string to;
            ss >> to;
            add(cursor, to);
        }
        else if (cmd == "needcheck")
        {
            setNeedCheck(cursor, true);
        }
        else if (cmd == "setclue")
        {
            std::string c;
            int x;
            ss >> c >> x;
            setClue(c, x);
        }
        else if (cmd == "memo")
        {
            std::string memo;
            ss.ignore();
            std::getline(ss, memo);
            addMemo(cursor, memo);
        }
        else if (cmd == "playtime")
        {
            int m;
            ss >> m;
            totalPlayTime += std::chrono::seconds(m);
        }
    }

    std::print("GameHelper::세이브파일 로딩 완료!\n\n{} 부터 시작~!\n", cursor);

    file.close();
}
