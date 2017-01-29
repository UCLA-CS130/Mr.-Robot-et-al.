#include <sstream>
#include <string>

#include "lightning_server.h"
#include "gtest/gtest.h"

TEST(TestTests, FirstTest) {
  EXPECT_EQ(1, 1)
    << "This should succeed!";
}

TEST(TestTests, SecondTest) {
  EXPECT_EQ(1, 2)
    << "This should fail, seeing this message is good!";
}

