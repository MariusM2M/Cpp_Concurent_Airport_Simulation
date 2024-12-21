#include <exception>
#include <string>

class Exception : public std::exception {
  
    private:
        std::string _message;

    public:
        Exception(const std::string& message) : _message(message) {}
        const char* what() const throw() { return _message.c_str(); }

};