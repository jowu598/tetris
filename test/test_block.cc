#include <assert.h>
#include <stdio.h>

#include <vector>

struct Point {
    int x;
    int y;
};

void Rotate(const std::vector<Point>& ps, std::vector<Point>* ps_out) {
    if (!ps_out) {
        return;
    }

    for (auto p : ps) {
        ps_out->push_back({-p.y, p.x});
    }
}

void ShowPos(const std::vector<Point>& ps) {
    int a[16] = {0};
    for (auto p : ps) {
        int x = p.x + 1;
        int y = p.y + 1;
        a[x * 4 + y] = 1;
    }

    printf("---------------------\n");
    for (int i = 0; i < 16; ++i) {
        printf("%d ", a[i]);
        if ((i + 1) % 4 == 0) {
            printf("\n");
        }
    }
}

int main() {
    //        0, 1, 1, 0,
    //        1, 1, 0, 0,
    //        0, 0, 0, 0,
    //        0, 0, 0, 0,
    //
    //
    //        x, 1, 1, x,
    //        1, 1, x, x,
    //        x, x, x, x,
    //        x, x, x, x

    /* y(sin(a))
     * ^    /
     * |   /
     * |  /
     * | /
     * |/a
     * --------------> x(cos(a))
     *
     *
     *  x' = x * (1+cos(a+90))
     */
    // x= 1+cos(a+90) * r
    // y = 1+sin(a+90) * r
    // sin(a+90=cos(a) ------>  y' = x
    // cos(a+90)=-sin(a) ---->  x' = -y

    //  (0,0) ----> (0,0)
    //  (0,-1) ----> (1,0)
    //  (1,-1) ----> (1,1)
    //  (-1,0) ----> (0,-1)
    //        x, 2, x, x,
    //        1, 2, 2, x,
    //        x, x, 2, x,
    //        x, x, x, x

    {
        // Block Z
        //        0, 0, 0, 0,
        //        1, 1, 0, 0,
        //        0, 1, 1, 0,
        //        0, 0, 0, 0,
        printf("block Z\n");
        int rotate_count = 1;
        std::vector<Point> ps = {{0, 0}, {-1, 0}, {0, 1}, {1, 1}};

        ShowPos(ps);

        std::vector<Point> ps_out;
        for (int i = 1; i < rotate_count; ++i) {
            Rotate(ps, &ps_out);
            ShowPos(ps_out);
        }
    }
    {
        // Block S
        //        0, 0, 0, 0,
        //        0, 1, 1, 0,
        //        1, 1, 0, 0,
        //        0, 0, 0, 0,
        printf("block S\n");
        int rotate_count = 2;
        std::vector<Point> ps = {{0, 0}, {0, 1}, {-1, 1}, {1, 0}};

        ShowPos(ps);

        std::vector<Point> ps_out;
        for (int i = 1; i < rotate_count; ++i) {
            Rotate(ps, &ps_out);
            ShowPos(ps_out);
        }
    }
    {
        printf("block O\n");
        // Block O
        //        0, 0, 0, 0,
        //        0, 1, 1, 0,
        //        0, 1, 1, 0,
        //        0, 0, 0, 0,
        int rotate_count = 1;
        std::vector<Point> ps = {{0, 0}, {0, 1}, {1, 1}, {1, 0}};

        ShowPos(ps);

        std::vector<Point> ps_now = ps;
        for (int i = 1; i < rotate_count; ++i) {
            std::vector<Point> ps_out;
            Rotate(ps_now, &ps_out);
            ShowPos(ps_out);
            ps_now = ps_out;
        }
    }
    {
        // Block L
        //        1, 1, 0, 0,
        //        1, 0, 0, 0,
        //        1, 0, 0, 0,
        //        0, 0, 0, 0,
        printf("block L\n");
        int rotate_count = 4;
        std::vector<Point> ps = {{-1, -1}, {-1, 0}, {-1, 1}, {0, -1}};

        ShowPos(ps);

        for (int i = 1; i < rotate_count; ++i) {
            std::vector<Point> ps_out;
            Rotate(ps, &ps_out);
            ShowPos(ps_out);
            ps = ps_out;
        }
    }
}

//
