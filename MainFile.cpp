#define CATCH_CONFIG_MAIN
#include "catch.hpp"

#include "FeatherMock.h"
#include "Dummy_AppLogger.h"



struct Student {

    Student() {}

    Student const & operator = (Student const& _stu) {
        name = _stu.name;
        age = _stu.age;
        return *this;
    }
    bool operator == (Student const & _stu) const {
        std::cout << "My name is " << name << " and Age is : " << age << std::endl;
        std::cout << "His name is " << _stu.name << " and Age is : " << _stu.age << std::endl;
        return (name == _stu.name) && (age == _stu.age);
    }

  


    std::string name;
    uint16_t     age;
};

std::ostream& operator << (std::ostream& os, const Student& _stu) {
    return os << _stu.name << _stu.age << "\n";
}





MockSystem::MockSystem()
{
    ADD_NEW_MOCK_OBJ("Api_from_other_module_1", int(int, int));
    ADD_NEW_MOCK_OBJ("Api_from_other_module_2", void(int, int));
    ADD_NEW_MOCK_OBJ("Api_from_other_module_3", void(void));
    ADD_NEW_MOCK_OBJ("Api_from_other_module_4", int(void));
    ADD_NEW_MOCK_OBJ("Api_from_other_module_5", void(Student const& _stu, int));
    ADD_NEW_MOCK_OBJ("Api_from_other_module_6", void(Student & _stu, int));
    ADD_NEW_MOCK_OBJ("Api_from_other_module_7", void(Student _stu, int));
    ADD_NEW_MOCK_OBJ("Foo::SetName", void(std::string const &));
}

#define MockOf_Api_from_other_module_1 MockFun("Api_from_other_module_1", int(int, int))
#define MockOf_Api_from_other_module_2 MockFun("Api_from_other_module_2", void(int, int))
#define MockOf_Api_from_other_module_3 MockFun("Api_from_other_module_3", void())
#define MockOf_Api_from_other_module_4 MockFun("Api_from_other_module_4", int())
#define MockOf_Api_from_other_module_5 MockFun("Api_from_other_module_5", void(Student const & _stu, int))
#define MockOf_Api_from_other_module_6 MockFun("Api_from_other_module_6", void(Student & _stu, int))
#define MockOf_Api_from_other_module_7 MockFun("Api_from_other_module_7", void(Student _stu, int))
#define MockOf_Foo_SetName             MockFun("Foo::SetName",            void(std::string const &))

#define NEW_FUNC_CALL_RECODER(...) MockFun(__FUNCTION__, int(int, int)).Call(__VA_ARGS__);

//#define A_Mock(name) MockFun(#name, decltype(name)).Call
#define MockOf(name) MockOf_##name

#define FUNC_ENTRY(name) return MockOf(name).Call
// or
// #define FUNC_ENTRY(name) FUNC_CALL_RECODER(name)


class Foo {
public:
    void SetName(std::string const& _name) {
        FUNC_ENTRY(Foo_SetName)(_name);
    }

    std::string name;
};

int Api_from_other_module_1(int _p1, int _p2) {

    int z = 7;
    ENSURE(_p1 != 2)(_p1)(_p2)(z).Msg("NONONONONO");
    ENSURE(false).Msg("NONONONONO");

   
    FUNC_ENTRY(Api_from_other_module_1)(_p1, _p2);

    


}

void Api_from_other_module_2(int _p1, int _p2) {
    FUNC_ENTRY(Api_from_other_module_2)(_p1, _p2);
}

void Api_from_other_module_3() {
    FUNC_ENTRY(Api_from_other_module_3)();
}

int Api_from_other_module_4() {
    FUNC_ENTRY(Api_from_other_module_4)();
}

void Api_from_other_module_5(Student const & _stu, int _i) {
    FUNC_ENTRY(Api_from_other_module_5)(_stu, _i);
    // ....
    // ....
}

void Api_from_other_module_6(Student & _stu, int _i) {
    FUNC_ENTRY(Api_from_other_module_6)(_stu, _i);

    // ....Api_from_other_module_5(xx, yy)
    // ....
}

void Api_from_other_module_7(Student _stu, int _i) {
    FUNC_ENTRY(Api_from_other_module_7)(_stu, _i);
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
    stu.age = 3;
    stu.name = "King";
    Api_from_other_module_7(stu, 4);

    return 5;  
}


TEST_CASE("Dialing", "[Normal][Current]") {

    WHEN("Case 1") {
        Msg msg;
        msg.id = 3;
        //ActionOf(CHECK(TestTarget(msg) == 4));

        MockOf_Api_from_other_module_1.WillActLike([](auto _p1, auto _p2) {
            REQUIRE(_p1 == 2);
            return 4;
        });

        MockOf_Api_from_other_module_4.WillActLike([]() {
            return 4; 
        });

        REQUIRE(TestTarget(msg) == 3);


    }
}
