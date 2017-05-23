/**
 *    > Author:            UncP
 *    > Mail:         770778010@qq.com
 *    > Github:    https://www.github.com/UncP/Mushroom
 *    > Created Time:  2017-05-19 10:05:56
**/

#ifndef _UNIT_TEST_HPP_
#define _UNIT_TEST_HPP_

#include <vector>
#include <typeinfo>
#include <cstring>
#include <iostream>
#include <cmath>

#define TEST_NAME(test_name) test_name##_TEST

#define TEST(test_name) \
class TEST_NAME(test_name) : public TestCase \
{ \
	public: \
		TEST_NAME(test_name)(const char *name):TestCase(name) { } \
		virtual void Run(); \
	private: \
		static TestCase* const test_case_; \
}; \
TestCase* const TEST_NAME(test_name)::test_case_ = UnitTest::GetInstance()->RegisterTestCase( \
new TEST_NAME(test_name)(#test_name)); \
void TEST_NAME(test_name)::Run()

#define RUN_ALL_TESTS() UnitTest::GetInstance()->Run();

class TestCase
{
	public:
		TestCase(const char *case_name) : case_name_(case_name) { };
		virtual void Run() = 0;
		virtual ~TestCase() { }

		const char *case_name_;
		bool        test_result_;
};

class UnitTest
{
	public:
		static UnitTest* GetInstance() {
			static UnitTest unit_test;
			return &unit_test;
		}

		TestCase* RegisterTestCase(TestCase *testcase) {
			test_cases_.push_back(testcase);
			return testcase;
		}

		bool Run() {
			test_result_ = true;

			std::cout << "\033[33m[ Start ] \033[33;1m" << test_cases_.size() << "\033[0m Unit Tests\n\n";

			for (auto it = test_cases_.begin(); it != test_cases_.end(); ++it) {
				TestCase *test_case = *it;
				current_test_case_ = test_case;
				current_test_case_->test_result_ = true;

				std::cout << "\033[34m[ Run ] \033[0m" << test_case->case_name_ << std::endl;

				test_case->Run();

				if (test_case->test_result_)
					std::cout << "\033[32m" << "[ Pass ] \033[0m" << test_case->case_name_ << std::endl;
				else
					std::cout << "\033[31m" << "[ Fail ] \033[0m" << test_case->case_name_ << std::endl;

				if (test_case->test_result_) {
					++passed_num_;
				} else {
					++failed_num_;
					test_result_ = false;
				}
			}

			std::cout << "\n\033[33m[ ALL  ] \033[33;1m" << test_cases_.size() << "\033[0m\n";
			std::cout << "\033[32m" << "[ PASS ] \033[32;1m" << passed_num_ << "\033[0m\n";
			std::cout << "\033[31m" << "[ FAIL ] \033[31;1m" << failed_num_ << "\033[0m\n";
			return !test_result_;
		}

		~UnitTest() {
			for (auto i = test_cases_.begin(); i != test_cases_.end(); ++i)
				delete *i;
		}

		TestCase              *current_test_case_;
		bool                   test_result_;
		int                    passed_num_;
		int                    failed_num_;
		std::vector<TestCase*> test_cases_;
};

enum OperatorType
{
	OPERATOR_TYPE_EQ,
	OPERATOR_TYPE_NE,
	OPERATOR_TYPE_GT,
	OPERATOR_TYPE_LT,
	OPERATOR_TYPE_GE,
	OPERATOR_TYPE_LE
};

template <class ElemType>
bool CheckNumericalData(ElemType left_value, ElemType right_value,
	const char *str_left_value, const char *str_right_value,
	const char *file_name, const unsigned long line_num, OperatorType operator_type)
{
	bool condition = false;
	char str_operator[5] = {0};

	switch (operator_type) {
		case OPERATOR_TYPE_EQ:
			if (typeid(ElemType) == typeid(double))
				condition = !(std::fabs(left_value - right_value) < 1e-8);
			else if (typeid(ElemType) == typeid(float))
				condition = !(std::fabs(left_value - right_value) < 1e-6);
			else
				condition = !(left_value == right_value);
			strcpy(str_operator, " == ");
			break;

		case OPERATOR_TYPE_NE:
			if (typeid(ElemType) == typeid(double))
				condition = !(std::fabs(left_value - right_value) > 1e-8);
			else if (typeid(ElemType) == typeid(float))
				condition = !(std::fabs(left_value - right_value) > 1e-6);
			else
				condition = !(left_value != right_value);
			strcpy(str_operator, " != ");
			break;

		case OPERATOR_TYPE_GT:
			condition = !(left_value > right_value);
			strcpy(str_operator, " > ");
			break;

		case OPERATOR_TYPE_LT:
			condition = !(left_value < right_value);
			strcpy(str_operator, " < ");
			break;

		case OPERATOR_TYPE_LE:
			condition = !(left_value <= right_value);
			strcpy(str_operator, " <= ");
			break;

		case OPERATOR_TYPE_GE:
			condition = !(left_value >= right_value);
			strcpy(str_operator, " >= ");
			break;
	}

	if (condition) {
		UnitTest::GetInstance()->current_test_case_->test_result_ = false;
		std::cout << "\033[36;1m" << file_name << "\033[0m: \033[31;1m" << line_num << "\033[0m: ";
		std::cout << "\033[33;1mExpect: \033[0m" << str_left_value << str_operator << str_right_value << std::endl;
	}
	return !condition;
}

bool CheckStrData(const char *left_value, const char *right_value,
	const char *str_left_value, const char *str_right_value,
	const char *file_name, const unsigned long line_num, OperatorType operator_type)
{
	bool condition = false;
	char str_operator[5] = {0};

	if (operator_type == OPERATOR_TYPE_EQ) {
		condition = !((strcmp(left_value, right_value) == 0));
		strcpy(str_operator, " == ");
	}
	else if (operator_type == OPERATOR_TYPE_NE) {
		condition = !((strcmp(left_value, right_value) != 0));
		strcpy(str_operator, " != ");
	}

	if (condition) {
		UnitTest::GetInstance()->current_test_case_->test_result_ = false;
		std::cout << "\033[36;1m" << file_name << "\033[0m: \033[31;1m" << line_num << "\033[0m: ";
		std::cout << "\033[33;1mExpect: \033[0m" << str_left_value << str_operator << str_right_value << std::endl;
	}

	return !condition;
}

#define CHECK_NUMERICAL_DATA(left_value, right_value, operator_type) \
CheckNumericalData(left_value, right_value, #left_value, #right_value, __FILE__, __LINE__, operator_type)
#define CHECK_STR_DATA(left_value, right_value, operator_type) \
CheckStrData(left_value, right_value, #left_value, #right_value, __FILE__, __LINE__, operator_type)

#define EXPECT_EQ(left_value, right_value) CHECK_NUMERICAL_DATA(left_value, right_value, OPERATOR_TYPE_EQ)
#define EXPECT_NE(left_value, right_value) CHECK_NUMERICAL_DATA(left_value, right_value, OPERATOR_TYPE_NE)
#define EXPECT_GT(left_value, right_value) CHECK_NUMERICAL_DATA(left_value, right_value, OPERATOR_TYPE_GT)
#define EXPECT_LT(left_value, right_value) CHECK_NUMERICAL_DATA(left_value, right_value, OPERATOR_TYPE_LT)
#define EXPECT_GE(left_value, right_value) CHECK_NUMERICAL_DATA(left_value, right_value, OPERATOR_TYPE_GE)
#define EXPECT_LE(left_value, right_value) CHECK_NUMERICAL_DATA(left_value, right_value, OPERATOR_TYPE_LE)

#define ASSERT_EQ(left_value, right_value) if (!CHECK_NUMERICAL_DATA(left_value, right_value, OPERATOR_TYPE_EQ)) return;
#define ASSERT_NE(left_value, right_value) if (!CHECK_NUMERICAL_DATA(left_value, right_value, OPERATOR_TYPE_NE)) return;
#define ASSERT_GT(left_value, right_value) if (!CHECK_NUMERICAL_DATA(left_value, right_value, OPERATOR_TYPE_GT)) return;
#define ASSERT_LT(left_value, right_value) if (!CHECK_NUMERICAL_DATA(left_value, right_value, OPERATOR_TYPE_LT)) return;
#define ASSERT_GE(left_value, right_value) if (!CHECK_NUMERICAL_DATA(left_value, right_value, OPERATOR_TYPE_GE)) return;
#define ASSERT_LE(left_value, right_value) if (!CHECK_NUMERICAL_DATA(left_value, right_value, OPERATOR_TYPE_LE)) return;

#define EXPECT_TRUE(condition) CHECK_NUMERICAL_DATA(static_cast<bool>(condition), true, OPERATOR_TYPE_EQ)
#define EXPECT_FALSE(condition) CHECK_NUMERICAL_DATA(static_cast<bool>(condition), false, OPERATOR_TYPE_EQ)
#define ASSERT_TRUE(condition) if (!CHECK_NUMERICAL_DATA(static_cast<bool>(condition), true, OPERATOR_TYPE_EQ)) return;
#define ASSERT_FALSE(condition) if (!CHECK_NUMERICAL_DATA(static_cast<bool>(condition), false, OPERATOR_TYPE_EQ)) return;
#define EXPECT_STREQ(left_value, right_value) CHECK_STR_DATA(left_value, right_value, OPERATOR_TYPE_EQ)
#define EXPECT_STRNE(left_value, right_value) CHECK_STR_DATA(left_value, right_value, OPERATOR_TYPE_NE)
#define ASSERT_STREQ(left_value, right_value) if (!CHECK_STR_DATA(left_value, right_value, OPERATOR_TYPE_EQ)) return;
#define ASSERT_STRNE(left_value, right_value) if (!CHECK_STR_DATA(left_value, right_value, OPERATOR_TYPE_NE)) return;

#endif /* _UNIT_TEST_HPP_ */