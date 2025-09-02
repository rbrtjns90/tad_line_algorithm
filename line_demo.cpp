#include <vector>
#include <string>
#include <functional>
#include <chrono>
#include <iostream>
#include <iomanip>
#include <cstdint>
#include <cstdlib>
#include <algorithm>
using namespace std;

// ---------- Canvas ----------
struct Canvas {
    int W, H;
    vector<char> pix;
    Canvas(int w, int h, char bg='.') : W(w), H(h), pix(w*h, bg) {}
    void clear(char bg='.') { fill(pix.begin(), pix.end(), bg); }
    inline void plot(int x, int y, char c='#') {
        if ((unsigned)x < (unsigned)W && (unsigned)y < (unsigned)H) pix[y*W + x] = c;
    }
    string str() const {
        string s; s.reserve((W+1)*H);
        for (int y=0; y<H; ++y) {
            for (int x=0; x<W; ++x) s.push_back(pix[y*W + x]);
            s.push_back('\n');
        }
        return s;
    }
    bool equals(const Canvas& o) const { return W==o.W && H==o.H && pix==o.pix; }
};

// ---------- Helpers ----------
static inline void swap_int(int &a, int &b){ int t=a; a=b; b=t; }

using PlotFn = function<void(int,int, char)>;

// ---------- Terry A. Davis–style no-branch add sloping (8.8 fixed-point) ----------
void draw_line_tad_no_branch(int x0, int y0, int x1, int y1, const PlotFn& plot, char ink='#'){
    int dx = x1 - x0;
    int dy = y1 - y0;

    if (dx==0 && dy==0) { plot(x0, y0, ink); return; }

    if (abs(dx) >= abs(dy)) {
        // Walk X, accumulate Y in 8.8
        if (x0 > x1) { swap_int(x0,x1); swap_int(y0,y1); dx = x1-x0; dy = y1-y0; }
        int32_t ady = abs(dy);
        int32_t slope88 = (dx==0) ? 0 : ((int32_t)ady << 8) / dx;
        if (dy < 0) slope88 = -slope88;
        int32_t y88 = ((int32_t)y0 << 8);
        for (int x = x0; x <= x1; ++x) {
            plot(x, (int)(y88 >> 8), ink);
            y88 += slope88; // branchless add
        }
    } else {
        // Walk Y, accumulate X in 8.8
        if (y0 > y1) { swap_int(x0,x1); swap_int(y0,y1); dx = x1-x0; dy = y1-y0; }
        int32_t adx = abs(dx);
        int32_t slope88 = (dy==0) ? 0 : ((int32_t)adx << 8) / abs(dy);
        if (dx < 0) slope88 = -slope88;
        int32_t x88 = ((int32_t)x0 << 8);
        for (int y = y0; y <= y1; ++y) {
            plot((int)(x88 >> 8), y, ink);
            x88 += slope88; // branchless add
        }
    }
}

// ---------- Standard Bresenham ----------
void draw_line_bresenham(int x0, int y0, int x1, int y1, const PlotFn& plot, char ink='#'){
    int dx = abs(x1 - x0);
    int dy = abs(y1 - y0);
    int sx = (x0 < x1) ? 1 : -1;
    int sy = (y0 < y1) ? 1 : -1;
    int err = dx - dy;

    while (true) {
        plot(x0, y0, ink);
        if (x0 == x1 && y0 == y1) break;
        int e2 = err << 1;
        if (e2 > -dy) { err -= dy; x0 += sx; }
        if (e2 <  dx) { err += dx; y0 += sy; }
    }
}

// ---------- Timing helpers ----------
using clk = std::chrono::high_resolution_clock;
using usec = std::chrono::microseconds;

struct Line { int x0,y0,x1,y1; const char* name; };

int main(){
    const int W = 48, H = 20;
    const int BENCH_ITERS = 10000;   // adjust for your machine
    vector<Line> tests = {
        { 2,  2, 45,  5, "shallow +slope"},
        { 2,  2,  8, 18, "steep +slope"  },
        { 45,  2,  2,  5, "shallow -slope (reversed X)"},
        {  8, 18,  2,  2, "steep -slope (reversed both)"},
        {  0, 10, 47, 10, "horizontal"},
        { 23,  0, 23, 19, "vertical"  },
        {  4,  4,  4,  4, "single point"},
        { 40, 18,  4,  1, "long descending"},
        {  0,  0, 47, 19, "diag (down-right)"},
        { 47,  0,  0, 19, "diag (down-left)"},
    };

    for (const auto& t : tests) {
        Canvas c1(W,H), c2(W,H);

        // Plotters
        size_t plotsum = 0; // anti-optimization guard
        PlotFn plot1 = [&](int x,int y,char ink){ c1.plot(x,y,ink); plotsum += (size_t)(x*1315423911u + y*2654435761u); };
        PlotFn plot2 = [&](int x,int y,char ink){ c2.plot(x,y,ink); plotsum += (size_t)(x*2166136261u + y*16777619u); };

        // --- Single-run timing (includes plotting into canvas) ---
        auto t0 = clk::now();
        draw_line_tad_no_branch(t.x0, t.y0, t.x1, t.y1, plot1);
        auto t1 = clk::now();
        draw_line_bresenham (t.x0, t.y0, t.x1, t.y1, plot2);
        auto t2 = clk::now();

        auto tad_us  = std::chrono::duration_cast<usec>(t1 - t0).count();
        auto bres_us = std::chrono::duration_cast<usec>(t2 - t1).count();

        bool same = c1.equals(c2);

        cout << "=== Test: " << t.name
             << "  (" << t.x0 << "," << t.y0 << ")->(" << t.x1 << "," << t.y1 << ") ===\n";
        cout << "Single-run:  TAD 8.8 = " << tad_us  << " us,   Bresenham = " << bres_us << " us\n";
        cout << "Match: " << (same ? "YES ✅" : "NO ❌") << "   (plotsum=" << plotsum << ")\n\n";

        // Show the drawings side by side
        cout << "[TAD 8.8 Fixed-Point]\n" << c1.str() << "\n";
        cout << "[Bresenham Algorithm]\n" << c2.str() << "\n";

        // --- Batch benchmark: run many iterations into a throwaway canvas (no printing) ---
        Canvas bench(W,H);
        PlotFn benchPlot = [&](int x,int y,char ink){ bench.plot(x,y,ink); };

        // Warm-up
        for (int i=0;i<100;++i) draw_line_tad_no_branch(t.x0, t.y0, t.x1, t.y1, benchPlot, '#');
        for (int i=0;i<100;++i) draw_line_bresenham (t.x0, t.y0, t.x1, t.y1, benchPlot, '#');

        bench.clear('.');
        auto b0 = clk::now();
        for (int i=0;i<BENCH_ITERS;++i) {
            // vary the ink a tad to avoid trivial dedup
            char ink = (char)('#' + (i & 1));
            draw_line_tad_no_branch(t.x0, t.y0, t.x1, t.y1, benchPlot, ink);
        }
        auto b1 = clk::now();

        bench.clear('.');
        auto b2 = clk::now();
        for (int i=0;i<BENCH_ITERS;++i) {
            char ink = (char)('#' + (i & 1));
            draw_line_bresenham(t.x0, t.y0, t.x1, t.y1, benchPlot, ink);
        }
        auto b3 = clk::now();

        auto tad_batch_us  = std::chrono::duration_cast<usec>(b1 - b0).count();
        auto bres_batch_us = std::chrono::duration_cast<usec>(b3 - b2).count();

        // Per-iteration averages in nanoseconds (nice to compare)
        double tad_avg_ns  = (1e3 * (double)tad_batch_us)  / BENCH_ITERS;
        double bres_avg_ns = (1e3 * (double)bres_batch_us) / BENCH_ITERS;

        cout << "Batch x" << BENCH_ITERS << ":\n";
        cout << "  TAD 8.8 total = " << tad_batch_us  << " us  (avg " << fixed << setprecision(1) << tad_avg_ns  << " ns/draw)\n";
        cout << "  Bresenham total = " << bres_batch_us << " us  (avg " << fixed << setprecision(1) << bres_avg_ns << " ns/draw)\n";
        cout << "\n";
    }

    return 0;
}
