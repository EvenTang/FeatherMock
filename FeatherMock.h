#pragma once

// V1.0
// CopyRight : EvenTang MIT License
//

#include <functional>
#include <string>
#include <map>
#include <memory>
#include <algorithm>

class MockObj {
public:
    MockObj(std::string const& _name) : m_name(_name), m_is_called(false) {}

    std::string const& Name(void) const {
        return m_name;
    }

    bool IsCalled() const {
        bool temp = m_is_called;
        return temp;
    }

    virtual void ClearMockStatus(void) {
        m_is_called = false;
    }

    virtual void GetParameters(void* _p) const {}


protected:
    bool m_is_called;
private:
    std::string m_name;
};

// default template without implementation
template <typename T>
class FunctionMock;

template <typename R, typename ...Args>
class FunctionMock<R(Args...)> : public MockObj {

public:
    //  using ParameterPack = std::tuple<Args&&...>;
    using ParaPack = std::tuple<typename std::remove_const<typename std::remove_reference<Args>::type>::type...>;
public:
    FunctionMock(std::string const& _name) 
        : MockObj(_name) { 

        // Check this!
        m_default_behavior = [&](Args... _args) { return R(); };
    }

    FunctionMock(std::string const& _name, std::function<R(Args...)> _default_behavior) // , R&& _default_return = R())
        : MockObj(_name) , m_default_behavior(_default_behavior) {
    }

    R Call(Args ... _args) {
        m_is_called = true;
        // store parameters
        m_parameters = std::forward_as_tuple(std::forward<Args>(_args)...);

        if (m_fun) {
            return m_fun(_args...);
        }
        
        return m_default_behavior(_args...);
    }

    std::tuple<Args...> GetAllParameters() {
        return m_parameters;
    }

    FunctionMock<R(Args...)>& SetBehavior(std::function<R(Args...)> _checker) {
        m_fun = _checker;
        return *this;
    }

    FunctionMock<R(Args...)>& SetDefaultBehavior(std::function<R(Args...)> _checker) {
        m_default_behavior = _checker;
        return *this;
    }

    bool IsCallWithParameters(Args... _args) {
        auto check_items = std::make_tuple(_args...);
        return check_items == m_parameters;
    }

    template <uint8_t Index, typename Arg>
    bool IsCallWithParametersOf(Arg const& _arg) {
        return _arg == std::get<Index - 1>(m_parameters);
    }

    //template <uint8_t Index, typename Arg>
    //bool IsParameterMeets(std::function<bool(Arg const &)> _checker) {
    //    return _checker(std::get<Index - 1>(m_parameters));
    //}

    template <uint8_t Index>
    bool IsParameterMeets(std::function<bool(std::tuple_element_t<Index -1, ParaPack> const &)> _checker) {
        return _checker(std::get<Index - 1>(m_parameters));
    }

    virtual void ClearMockStatus(void) {
        MockObj::ClearMockStatus();
        m_fun = nullptr;
    }

private:
    std::function<R(Args...)> m_fun;
    std::function<R(Args...)> m_default_behavior;

    ParaPack m_parameters;
};

#define WillReturn(rslt) SetBehavior([&](auto ...args) { return rslt; })
#define WillDefaultReturn(rslt) SetDefaultBehavior([&](auto ...args) { return rslt; })
#define IsParameterOneMeets(cond)   IsParameterMeets<1>([&](auto const& _p1) { return (cond); })
#define IsParameterTwoMeets(cond)   IsParameterMeets<2>([&](auto const& _p2) { return (cond); })
#define IsParameterThreeMeets(cond) IsParameterMeets<3>([&](auto const& _p3) { return (cond); })
#define IsParameterFourMeets(cond)  IsParameterMeets<4>([&](auto const& _p4) { return (cond); })
#define IsParameterFiveMeets(cond)  IsParameterMeets<5>([&](auto const& _p5) { return (cond); })

class MockSystem {
public:
    using MockObjHandle = std::shared_ptr<MockObj>;

public:
    MockSystem();

public:

    static MockSystem& GetInstance(void) {
        static MockSystem mock_sys;
        return mock_sys;
    }

    void AddNewMockObj(std::string const& _name, MockObjHandle _mock_obj) {
        m_mock_objs[_name] = _mock_obj;
    }

    MockObjHandle FindMockObjByName(std::string const& _name) {
        return m_mock_objs[_name];
    }

    void RefreshMockSystem() {
        std::for_each(m_mock_objs.begin(), m_mock_objs.end(), [](auto & _mock_obj) {
            _mock_obj.second->ClearMockStatus();
        });
    }

private:
    std::map<std::string, MockObjHandle> m_mock_objs;
};

template <typename T>
MockSystem::MockObjHandle CreateMock(std::string const _name) {
    return std::make_shared<FunctionMock<T>>(_name);
}

#define ADD_NEW_MOCK_OBJ(fun_name, signature)  m_mock_objs[#fun_name] = CreateMock<signature>(#fun_name)

#define MockFun(fun_name, signature) dynamic_cast<FunctionMock<signature>&>( \
        *MockSystem::GetInstance().FindMockObjByName(#fun_name))



// sample
//#define MockOf_Api_from_other_module_1 MockFun("Api_from_other_module_1", int(int, int))
//#define MockOf_Api_from_other_module_2 MockFun("Api_from_other_module_2", void(int, int))
//#define MockOf_Api_from_other_module_3 MockFun("Api_from_other_module_3", void())
//#define MockOf_Api_from_other_module_4 MockFun("Api_from_other_module_4", int())

