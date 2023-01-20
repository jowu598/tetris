#pragma once

#include <assert.h>

#include <list>
#include <memory>
#include <mutex>
#include <vector>

typedef int Color;
#define WINDOW_WIDTH (12)
#define WINDOW_HEIGHT (20)

struct Dot {
    Color color;
    bool is_filled;
};

struct BlockPoints {
    int x[4];
    int y[4];
};

enum class Type : int { O = 0, S, Z, L, T, J, I };
enum class Action : int { DOWN = 1, LEFT, RIGHT };

class Block {
public:
    Block(const char *name, Type type, Color color, int max_rotate_cnt,
          const BlockPoints &ps);

    void Rotate(bool clockwise);
    void Move(Action action);

    const char *GetName() const { return name_; }
    int GetX() const { return x_; }
    int GetY() const { return y_; }
    Color GetColor() const { return color_; }
    Type GetType() const { return type_; }
    BlockPoints GetPoints() const;
    void AddPoint(const BlockPoints &ps);

    void SetX(int x) { x_ = x; }
    void SetY(int y) { y_ = y; }

protected:
    const char *name_;
    int x_;
    int y_;
    Color color_;
    Type type_;
    std::vector<BlockPoints> points_;
    int max_rotate_cnt_;
    int cur_rotate_idx_;
};

class BlockCreator {
public:
    static BlockCreator *GetInstance();
    std::shared_ptr<Block> Create(Type type);
    std::shared_ptr<Block> CreateRandom();

    std::shared_ptr<Block> GetCurrentBlock() const;
    std::list<std::shared_ptr<Block>> GetNextBlocks();
    void CreateNext();
    void ResumeBack(Type type);

private:
    BlockCreator();
    std::shared_ptr<Block> CreateImpl(Type type, Color clr, int rotate_count,
                                      const BlockPoints &ps, const char *name);

private:
    static constexpr int kNextBlockNumber = 4;
    std::shared_ptr<Block> cur_block_;
    std::list<std::shared_ptr<Block>> next_blocks_;
    std::mutex mu_;
};
