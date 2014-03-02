
#ifndef Z3SOLUTIONCONVERTER_HPP_
# define Z3SOLUTIONCONVERTER_HPP_

# include <list>
# include <cstdlib>
# include "z3++.h"

enum eToken
    {
        E_NUMBER,
        E_OPENPAR,
        E_CLOSEPAR,
        E_MINUS,
        E_PLUS,
        E_DIV,
        E_MULT,
    };

struct token
{
    enum eToken token;
    std::string value;
};

class Z3SolutionConverter
{
public:
    static int getNumberSize(char *str)
    {
        size_t i;

        if (!str)
            return 0;
        for (i = 0; (str[i] <= '9' && str[i] >= '0') || str[i] == '.'; ++i)
            ;
        return i;
    }

    static bool fillTokenList(std::list<struct token> &list, std::string &expr)
    {
        size_t i = 0;
        size_t inc = 0;
        bool isSpace;
        
        while (i < expr.size())
            {
                struct token t;
                inc = 1;
                isSpace = false;
                switch (expr[i])
                    {
                    case '-':
                        t.token = E_MINUS;
                        break;
                    case '+':
                        t.token = E_PLUS;
                        break;
                    case '/':
                        t.token = E_DIV;
                        break;
                    case '*':
                        t.token = E_MULT;
                        break;
                    case '(':
                        t.token = E_OPENPAR;
                        break;
                    case ')':
                        t.token = E_CLOSEPAR;
                        break;
                    case ' ':
                        isSpace = true;
                        break;
                    default: // NUMBER
                        t.token = E_NUMBER;
                        inc = getNumberSize(&(expr[i]));
                    }
                if (!isSpace) {
                    t.value = expr.substr(i, inc);
                    list.push_back(t);
                }
                if (inc == 0)
                    return false;
                i += inc;
            }
        return i >= expr.size();
    }

    static float doOp(const struct token &a, const struct token &b,
                      const struct token &op)
    {
        switch (op.token)
            {
            case E_MINUS:
                return atof(a.value.c_str()) - atof(b.value.c_str());
                break;
            case E_PLUS:
                return atof(a.value.c_str()) + atof(b.value.c_str());
                break;
            case E_DIV:
                return atof(a.value.c_str()) / atof(b.value.c_str());
                break;
            case E_MULT:
                return atof(a.value.c_str()) * atof(b.value.c_str());
                break;
            default:
                return 0; // fail
            }
    }

    static float getFloatFromZ3Expr(const z3::expr &expr)
    {
        std::stringstream ss;
        std::string str;
        std::list<struct token> list;
        std::list<float> result;
        float a;
        float b;

        ss << expr;
        str = ss.str();
        if (!fillTokenList(list, str)) {
            return 0;
        }
        list.reverse();
        while (list.size() > 0)
            {
                switch (list.front().token)
                    {
                    case E_NUMBER:
                        result.push_front(atof(list.front().value.c_str()));
                        list.pop_front(); // pop number
                        break;
                    case E_PLUS:
                        a = result.front();
                        result.pop_front();
                        b = result.front();
                        result.pop_front();
                        result.push_front(a + b);
                        list.pop_front(); // pop operator
                        break;
                    case E_MINUS:
                        a = result.front();
                        result.pop_front();
                        list.pop_front();
                        if (list.front().token == E_OPENPAR)
                            {
                                result.push_front(-a);
                            }
                        else
                            {
                                b = result.front();
                                result.pop_front();
                                result.push_front(a - b);
                                list.pop_front(); // pop operator
                            }
                        break;
                    case E_DIV:
                        a = result.front();
                        result.pop_front();
                        b = result.front();
                        result.pop_front();
                        result.push_front(a / b);
                        list.pop_front(); // pop operator
                        break;
                    case E_MULT:
                        a = result.front();
                        result.pop_front();
                        b = result.front();
                        result.pop_front();
                        result.push_front(a * b);
                        list.pop_front(); // pop operator
                        break;
                    default:
                        list.pop_front();
                    }
                if (list.size() != 0 && list.front().token == E_OPENPAR)
                    list.pop_front();
                
            }
        return result.front();
    }
};

# endif
