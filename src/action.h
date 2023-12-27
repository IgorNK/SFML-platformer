#include <string>

class Action {
    std::string m_name;
    std::string m_type;
public:
    Action(const std::string & name, const std::string & type)
    : m_name(name)
    , m_type(type) {

    };
    ~Action() {};
    const std::string & name() const {
        return m_name;
    }
    const std::string & type() const {
        return m_type;
    }
};