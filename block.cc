#include "block.h"

#include <string.h>

#include <memory>
#include <unordered_set>

#include "common.h"
#include "log.h"

namespace {
//        0, 0, 0, 0,
//        0, 1, 1, 0,
//        0, 1, 1, 0,
//        0, 0, 0, 0,
constexpr BlockPoints kBlockO = {{0, 0, 1, 1}, {0, 1, 1, 0}};
//        0, 1, 0, 0,
//        0, 1, 0, 0,
//        0, 1, 0, 0,
//        0, 1, 0, 0,
constexpr BlockPoints kBlockI = {{0, 0, 0, 0}, {-1, 0, 1, 2}};
//        0, 0, 0, 0,
//        0, 1, 1, 0,
//        1, 1, 0, 0,
//        0, 0, 0, 0,
constexpr BlockPoints kBlockS = {{0, 0, -1, 1}, {0, 1, 1, 0}};
//        0, 1, 1, 0,
//        1, 1, 0, 0,
//        0, 0, 0, 0,
//        0, 0, 0, 0,
constexpr BlockPoints kBlockZ = {{0, -1, 0, 1}, {0, 0, 1, 1}};
//        1, 1, 0, 0,
//        1, 0, 0, 0,
//        1, 0, 0, 0,
//        0, 0, 0, 0,
constexpr BlockPoints kBlockJ = {{-1, -1, -1, 0}, {-1, 0, 1, -1}};

//        1, 0, 0, 0,
//        1, 0, 0, 0,
//        1, 1, 0, 0,
//        0, 0, 0, 0,
constexpr BlockPoints kBlockL = {{-1, -1, -1, 0}, {-1, 0, 1, 1}};
//        0, 1, 0, 0,
//        1, 1, 1, 0,
//        0, 0, 0, 0,
//        0, 0, 0, 0,
constexpr BlockPoints kBlockT = {{-1, 0, 0, 1}, {0, 0, -1, 0}};
}  // namespace

BlockCreator* BlockCreator::GetInstance() {
    static BlockCreator s_instance;
    return &s_instance;
}

BlockCreator::BlockCreator() {
    cur_block_ = CreateRandom();
    for (int i = 0; i < kNextBlockNumber; ++i) {
        next_blocks_.push_back(CreateRandom());
    }
}

std::shared_ptr<Block> BlockCreator::GetCurrentBlock() const {
    return cur_block_;
}

std::list<std::shared_ptr<Block>> BlockCreator::GetNextBlocks() {
    return next_blocks_;
}

void BlockCreator::CreateNext() {
    cur_block_ = next_blocks_.front();
    next_blocks_.pop_front();
    next_blocks_.push_back(CreateRandom());
}

std::shared_ptr<Block> BlockCreator::Create(Type type) {
#define CREATE_BLOCK_ON_CASE(t, cnt, clr) \
    case Type::t:                         \
        return CreateImpl(Type::t, clr, cnt, kBlock##t, #t);

    switch (type) {
        CREATE_BLOCK_ON_CASE(O, 1, 1)
        CREATE_BLOCK_ON_CASE(I, 2, 2)
        CREATE_BLOCK_ON_CASE(Z, 2, 3)
        CREATE_BLOCK_ON_CASE(S, 2, 4)
        CREATE_BLOCK_ON_CASE(L, 4, 5)
        CREATE_BLOCK_ON_CASE(J, 4, 6)
        CREATE_BLOCK_ON_CASE(T, 4, 7)
        default:
            break;
    }

    assert(0);
    return nullptr;
}

std::shared_ptr<Block> BlockCreator::CreateRandom() {
    Type type = static_cast<Type>(rand() % 7);
    return Create(type);
}

std::shared_ptr<Block> BlockCreator::CreateImpl(Type type, Color color,
                                                int rotate_count,
                                                const BlockPoints& ps,
                                                const char* name) {
    //    LOG("create block [%s] type [%d]", name, type);
    std::shared_ptr<Block> block =
        std::make_shared<Block>(name, type, color, rotate_count, ps);
    BlockPoints cur_ps = block->GetPoints();
    for (int i = 1; i < rotate_count; ++i) {
        BlockPoints next_ps;
        // LOG("rotate block name %s type %d  idx %d", name, type, i);
        RotateBlock(cur_ps, &next_ps);
        block->AddPoint(next_ps);
        cur_ps = next_ps;
    }

    return block;
}

Block::Block(const char* name, Type type, Color color, int max_rotate_cnt,
             const BlockPoints& ps) {
    name_ = name;
    color_ = color;
    type_ = type;
    x_ = 0;
    y_ = 0;
    cur_rotate_idx_ = 0;
    max_rotate_cnt_ = max_rotate_cnt;
    AddPoint(ps);
}

BlockPoints Block::GetPoints() const {
    assert(points_.size() > cur_rotate_idx_);
    return points_[cur_rotate_idx_];
}

void Block::AddPoint(const BlockPoints& ps) { points_.emplace_back(ps); }

void Block::Rotate(bool clockwise) {
    // LOG("cur_rotate_idx_ %d", cur_rotate_idx_);
    cur_rotate_idx_ =
        (max_rotate_cnt_ + cur_rotate_idx_ + (clockwise ? 1 : -1)) %
        max_rotate_cnt_;
}

void Block::Move(Action action) {
    switch (action) {
        case Action::DOWN:
            SetY(GetY() + 1);
            break;
        case Action::LEFT:
            SetX(GetX() - 1);
            break;
        case Action::RIGHT:
            SetX(GetX() + 1);
            break;
        default:
            break;
    }
}
