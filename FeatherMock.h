#pragma once

// V1.0
// CopyRight : EvenTang MIT License
//
// @todo : how to mock functions with variadic parameter
// @todo : how to mock template parameter


#include <functional>
#include <string>
#include <map>
#include <memory>
#include <algorithm>
#include <deque>

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

        if (!m_customize_behaviors.empty()) {
            auto temp = m_customize_behaviors.front();
            m_customize_behaviors.pop_front();

            return temp(_args...);
        }
        
        return m_default_behavior(_args...);
    }

    FunctionMock<R(Args...)>& SetBehavior(std::function<R(Args...)> _checker) {
        m_customize_behaviors.push_back(_checker);
        return *this;
    }

    FunctionMock<R(Args...)>& WillActLike(std::function<R(Args...)> _checker) {
        return SetBehavior(_checker);
    }

    FunctionMock<R(Args...)>& SetDefaultBehavior(std::function<R(Args...)> _checker) {
        m_default_behavior = _checker;
        return *this;
    }

    virtual void ClearMockStatus(void) {
        MockObj::ClearMockStatus();
        m_customize_behaviors.clear();
    }

private:
    std::deque<std::function<R(Args...)>> m_customize_behaviors;
    std::function<R(Args...)> m_default_behavior;
};

#define WillReturn(rslt) SetBehavior([&](auto ...args) { return rslt; })
#define WillDefaultReturn(rslt) SetDefaultBehavior([&](auto ...args) { return rslt; })

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



class PostAction {
public:
    PostAction(std::function<void()> _triggle, std::string _description)
        : m_behavior(_triggle), m_str_name(_description)
    {

    }


    ~PostAction() {
        m_behavior();
        MockSystem::GetInstance().RefreshMockSystem();
        // std::cout << m_str_name << std::endl;
    }

private:
    std::function<void()> m_behavior;
    std::string m_str_name;

};

#define Action_LINENAME_CAT(name, line) name##line
#define Action_LINENAME(name, line)     Action_LINENAME_CAT(name, line)
#define ActionOf(expr)                  PostAction Action_LINENAME(PostAction, __LINE__)([&]() {expr;}, #expr)


// sample
//#define MockOf_Api_from_other_module_1 MockFun("Api_from_other_module_1", int(int, int))
//#define MockOf_Api_from_other_module_2 MockFun("Api_from_other_module_2", void(int, int))
//#define MockOf_Api_from_other_module_3 MockFun("Api_from_other_module_3", void())
//#define MockOf_Api_from_other_module_4 MockFun("Api_from_other_module_4", int())

