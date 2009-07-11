#include <cassert>
#include <cmath>
#include <cstddef>
#include <cstdio>
#include <vector>

#include "fuzzy.h"

namespace {

bool close(double a, double b, double tolerance = 1e-9)
{
    return std::fabs(a - b) < tolerance;
}

void testMembershipShape()
{
    Triangle triangle(10.0, 20.0, 30.0);

    assert(close(triangle.membership(20.0), 1.0));
    assert(close(triangle.membership(15.0), 0.5));
    assert(close(triangle.membership(25.0), 0.5));
    assert(close(triangle.membership(10.0), 0.0));
    assert(close(triangle.membership(30.0), 0.0));
    assert(close(triangle.membership(5.0), 0.0));
    assert(close(triangle.membership(100.0), 0.0));
}

void testMembershipStaysInRange()
{
    Triangle triangle(0.0, 5.0, 10.0);

    for (double value = -5.0; value <= 15.0; value += 0.25)
    {
        double m = triangle.membership(value);
        assert(m >= 0.0 && m <= 1.0);
    }
}

void testOverlappingSets()
{
    Controller controller = fanController();
    std::vector<double> strengths = controller.strengths(24.0);

    /* at 24 degrees the mild and warm rules both have something to say */
    assert(strengths[1] > 0.0);
    assert(strengths[2] > 0.0);
    assert(strengths[0] == 0.0);
    assert(strengths[3] == 0.0);
}

void testColdRoomStopsTheFan()
{
    Controller controller = fanController();
    assert(close(controller.evaluate(5.0), 0.0));
}

void testHotRoomRunsItFlatOut()
{
    Controller controller = fanController();
    assert(close(controller.evaluate(45.0), 100.0));
}

void testAnswerRisesWithTemperature()
{
    Controller controller = fanController();

    double previous = -1.0;
    for (double degrees = 0.0; degrees <= 45.0; degrees += 1.0)
    {
        double speed = controller.evaluate(degrees);
        assert(speed >= previous - 1e-9);
        assert(speed >= 0.0 && speed <= 100.0);
        previous = speed;
    }
}

void testMiddleOfASetGivesItsOutput()
{
    Controller controller = fanController();

    /* 21 is the peak of "mild" and nothing else reaches it */
    assert(close(controller.evaluate(21.0), 35.0));
}

void testEmptyControllerIsQuiet()
{
    Controller controller;
    assert(close(controller.evaluate(20.0), 0.0));
}

}

int main()
{
    testMembershipShape();
    testMembershipStaysInRange();
    testOverlappingSets();
    testColdRoomStopsTheFan();
    testHotRoomRunsItFlatOut();
    testAnswerRisesWithTemperature();
    testMiddleOfASetGivesItsOutput();
    testEmptyControllerIsQuiet();

    Controller controller = fanController();
    for (double degrees = 8.0; degrees <= 40.0; degrees += 4.0)
        printf("%4.0f degrees -> fan at %5.1f%%\n", degrees,
               controller.evaluate(degrees));
    printf("all tests passed\n");
    return 0;
}
