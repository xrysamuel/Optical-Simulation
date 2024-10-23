#ifndef LUAAPI_H
#define LUAAPI_H

extern "C"
{
#include "lua/lua.h"
#include "lua/lauxlib.h"
#include "lua/lualib.h"
}

#include <string>
#include <vector>
#include <type_traits>

class LuaExecutionException : public std::exception
{
};

// Abstraction of a Lua interpreter
class LuaInterpreter
{
public:
    using CallbackFunctionWrapper = int (*)(lua_State *L);

    LuaInterpreter()
    {
        L = luaL_newstate();
        luaL_openlibs(L);
    }

    void Run(const std::string &script_file)
    {
        if (luaL_loadfile(L, script_file.c_str()) || lua_pcall(L, 0, 0, 0))
            throw LuaExecutionException();
    }

    // Register Lua functions as functors, requiring the return type to be void and the parameter to be std::vector<double>
    template <class CallbackFunctor>
    void RegisterLuaFunction(const std::string &lua_function_name)
    {
        static_assert(FunctorTraits<CallbackFunctor, void, std::vector<double>>::valid, "Function Type Error");
        lua_register(L, lua_function_name.c_str(), GetWrapper<CallbackFunctor>());
    }

    ~LuaInterpreter() { lua_close(L); }

private:
    lua_State *L;

    // Class template used to check the functor type
    template <typename Functor, typename ReturnType, typename... Args>
    struct FunctorTraits
    {
        static constexpr bool valid = std::is_invocable_r_v<ReturnType, Functor, Args...>;
    };

    template <class CallbackFunctor>
    CallbackFunctionWrapper GetWrapper()
    {
        return [](lua_State *L) -> int
        {
            // Get the number of parameters
            int num_args = lua_gettop(L);

            // Construct the parameter list
            std::vector<double> args;
            for (int i = 1; i <= num_args; ++i)
            {
                if (!lua_isnumber(L, i))
                {
                    throw LuaExecutionException();
                }
                double arg = lua_tonumber(L, i);
                args.push_back(arg);
            }

            // Invoke the callback function
            CallbackFunctor()(args);

            return 0;
        };
    }
};

#endif