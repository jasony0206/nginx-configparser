#include "gtest/gtest.h"
#include "config_parser.h"

TEST(NginxConfigParserTest, SimpleConfig) {
  NginxConfigParser parser;
  NginxConfig out_config;

  bool success = parser.Parse("example_config", &out_config);

  EXPECT_TRUE(success);
}

class NginxStringConfigParserTest : public ::testing::Test {
protected:
    bool ParseString(const std::string config_string) {
      std::stringstream config_stream(config_string);
      return parser_.Parse(&config_stream, &out_config_);
    }

    NginxConfigParser parser_;
    NginxConfig out_config_;
};

TEST_F(NginxStringConfigParserTest, UsingFixture) {
  std::string config_string = "foo bar;";
  EXPECT_TRUE(ParseString(config_string));
  EXPECT_EQ("foo", out_config_.statements_[0]->tokens_[0]);
  EXPECT_EQ("bar", out_config_.statements_[0]->tokens_[1]);
}

TEST_F(NginxStringConfigParserTest, NestedConfig) {
  std::string config_string = "foo { foo bar; }";
  EXPECT_TRUE(ParseString(config_string));
  EXPECT_EQ("foo", out_config_.statements_[0]->tokens_[0]);
}

TEST_F(NginxStringConfigParserTest, InvalidConfig) {
  std::string config_string = "foo { foo bar;";
  EXPECT_FALSE(ParseString(config_string));
}

TEST_F(NginxStringConfigParserTest, TwoEndBlocksInARow) {
  std::string config_string = "foo { foo { bar; } }";
  EXPECT_TRUE(ParseString(config_string));
}

TEST_F(NginxStringConfigParserTest, NestedConfigWithStatements) {
  EXPECT_TRUE(ParseString("server { foo { bar; } listen 80; }"));
}

TEST_F(NginxStringConfigParserTest, StatementFollowingComment) {
  EXPECT_TRUE(ParseString("# This is a comment \n foo;"));
}

TEST_F(NginxStringConfigParserTest, CommentFollowingStatement) {
  EXPECT_TRUE(ParseString("foo; # This is a comment"));
}
