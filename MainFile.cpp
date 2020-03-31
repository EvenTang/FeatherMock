#define CATCH_CONFIG_MAIN
#include "catch.hpp"

#include "FeatherMock.h"

struct Student {

    Student() {}

    Student const & operator = (Student const& _stu) {
        return *this;
    }
    bool operator == (Student const & _stu) const {
        return true;
    }


    std::string name;
    uint8_t     age;
};

MockSystem::MockSystem()
{
    ADD_NEW_MOCK_OBJ("Api_from_other_module_1", int(int, int));
    ADD_NEW_MOCK_OBJ("Api_from_other_module_2", void(int, int));
    ADD_NEW_MOCK_OBJ("Api_from_other_module_3", void(void));
    ADD_NEW_MOCK_OBJ("Api_from_other_module_4", int(void));
    ADD_NEW_MOCK_OBJ("Api_from_other_module_5", void(Student const& _stu, int));
    ADD_NEW_MOCK_OBJ("Api_from_other_module_6", void(Student & _stu, int));
    ADD_NEW_MOCK_OBJ("Api_from_other_module_7", void(Student _stu, int));
}

#define MockOf_Api_from_other_module_1 MockFun("Api_from_other_module_1", int(int, int))
#define MockOf_Api_from_other_module_2 MockFun("Api_from_other_module_2", void(int, int))
#define MockOf_Api_from_other_module_3 MockFun("Api_from_other_module_3", void())
#define MockOf_Api_from_other_module_4 MockFun("Api_from_other_module_4", int())
#define MockOf_Api_from_other_module_5 MockFun("Api_from_other_module_5", void(Student const & _stu, int))
#define MockOf_Api_from_other_module_6 MockFun("Api_from_other_module_6", void(Student & _stu, int))
#define MockOf_Api_from_other_module_7 MockFun("Api_from_other_module_7", void(Student _stu, int))


int Api_from_other_module_1(int _p1, int _p2) {
    return MockOf_Api_from_other_module_1.Call(_p1, _p2);
}

void Api_from_other_module_2(int _p1, int _p2) {
    return MockOf_Api_from_other_module_2.Call(_p1, _p2);
}

void Api_from_other_module_3() {
    return MockOf_Api_from_other_module_3.Call();
}

int Api_from_other_module_4() {
    return MockOf_Api_from_other_module_4.Call();
}

void Api_from_other_module_5(Student const & _stu, int _i) {
    return MockOf_Api_from_other_module_5.Call(_stu, _i);
}

void Api_from_other_module_6(Student & _stu, int _i) {
    return MockOf_Api_from_other_module_6.Call(_stu, _i);
}

void Api_from_other_module_7(Student _stu, int _i) {
    return MockOf_Api_from_other_module_7.Call(_stu, _i);
}




class Msg {
public:
    int id;
};


int TestTarget(Msg& _msg) {

    if (Api_from_other_module_1(2, 1) == 4) {
        return Api_from_other_module_4();
    }
    Api_from_other_module_2(1, 2);
    Api_from_other_module_3();

    Student stu;
    stu.age = 0;
    stu.name = "King";
    Api_from_other_module_7(stu, 4);

    return 5;
}


TEST_CASE("Dialing", "[Normal][Current]") {

    int i = 5;

    MockOf_Api_from_other_module_1.WillReturn(4);
    MockOf_Api_from_other_module_4.WillDefaultReturn(i);

    Msg msg1;
    REQUIRE(TestTarget(msg1) == i);

    REQUIRE(MockOf_Api_from_other_module_2.IsCalled() == false);

    MockSystem::GetInstance().RefreshMockSystem();

    MockOf_Api_from_other_module_1.SetBehavior([](int _a, int) {
        REQUIRE(_a == 2);
        return 4;
    });

    Msg msg2;
    REQUIRE(TestTarget(msg2) == i);
    CHECK(MockOf_Api_from_other_module_2.IsCalled());

    Student stu;
    stu.age = 0;
    stu.name = "King";

    CHECK(MockOf_Api_from_other_module_1.IsCallWithParameters(2, 1));
    CHECK(MockOf_Api_from_other_module_7.IsCallWithParameters(stu, 4));

    CHECK(MockOf_Api_from_other_module_1.IsCallWithParametersOf<1>(2));
    CHECK(MockOf_Api_from_other_module_7.IsCallWithParametersOf<1>(stu));

    CHECK(MockOf_Api_from_other_module_1.IsParameterMeets<1>([](auto const& i) { return i > 0; }));
    CHECK(MockOf_Api_from_other_module_1.IsParameterOneMeets(_p1 > 0));

    CHECK(MockOf_Api_from_other_module_7.IsParameterMeets<1>([](auto const & _stu) { return _stu.name.size() > 3; }));
    CHECK(MockOf_Api_from_other_module_7.IsParameterOneMeets(_p1.name.size() > 3));

}
