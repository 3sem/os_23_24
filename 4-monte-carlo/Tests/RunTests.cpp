#include "Driver.h"
#include "gtest/gtest.h"

const double DefaultFitTolerance = 0.001;
const double DefaultTimeTolerance = 0.5;
const size_t DefaultNWorkers = 8;
const size_t DefaultNPoints = 10000000;

struct TestArgs {
  double FitTolerance = DefaultFitTolerance;
  double TimeTolerance = DefaultTimeTolerance;
  size_t NWorkers = DefaultNWorkers;
  size_t NPoints = DefaultNPoints;
};

#define EXPECT_FIT(a, b, tolerance) EXPECT_LE(std::abs(a - b), tolerance)

void TestFunction(FunctionT function, FunctionT primitive, TestArgs args = {}) {
  DriverResult res1Worker = Driver(function, 1, args.NPoints);
  DriverResult resNWorkers = Driver(function, args.NWorkers, args.NPoints);
  double expected = primitive(1) - primitive(0);

  EXPECT_FIT(res1Worker.Result, expected, args.FitTolerance);
  EXPECT_FIT(resNWorkers.Result, expected, args.FitTolerance);

  double timeRel = res1Worker.TimeS / resNWorkers.TimeS;
  double timeExp = args.NWorkers * args.TimeTolerance;

  EXPECT_FIT(timeRel, args.NWorkers, timeExp);
}

#define FUNC(name) double name(double x)

FUNC(f1) { return 1; }
FUNC(f1_p) { return x; }

FUNC(fx) { return x; }
FUNC(fx_p) { return x * x / 2; }

FUNC(fx2) { return x * x; }
FUNC(fx2_p) { return x * x * x / 3; }

FUNC(sinx) { return cos(x); }
FUNC(sinx_p) { return sin(x); }

FUNC(ex) { return (exp(x) - 1) / exp(1); }
FUNC(ex_p) { return (exp(x) - x) / exp(1); }

TEST(MonteCarlo, f1) { TestFunction(f1, f1_p); }
TEST(MonteCarlo, fx) { TestFunction(fx, fx_p); }
TEST(MonteCarlo, fx2) { TestFunction(fx2, fx2_p); }
TEST(MonteCarlo, sinx) { TestFunction(sinx, sinx_p); }
TEST(MonteCarlo, ex) { TestFunction(ex, ex_p); }
