#include <stdinc.hpp>
#include "value_conversion.hpp"
#include "../functions.hpp"
#include "../execution.hpp"
#include "../array.hpp"
#include "../../../component/notifies.hpp"

namespace scripting::lua
{
	namespace
	{
		bool is_istring(const sol::lua_value& value)
		{
			if (!value.is<std::string>())
			{
				return false;
			}

			const auto str = value.as<std::string>();

			return str[0] == '&';
		}

		script_value string_to_istring(const sol::lua_value& value)
		{
			const auto str = value.as<std::string>().erase(0, 1);
			const auto string_value = game::SL_GetString(str.data(), 0);

			game::VariableValue variable{};
			variable.type = game::SCRIPT_ISTRING;
			variable.u.uintValue = string_value;

			const auto _ = gsl::finally([&variable]()
			{
				game::RemoveRefToValue(variable.type, variable.u);
			});

			return script_value(variable);
		}

		game::VariableValue convert_function(sol::lua_value value)
		{
			const auto function = value.as<sol::protected_function>();
			const auto index = ++notifies::function_count;

			notifies::vm_execute_hooks[index] = function;

			game::VariableValue func;
			func.type = game::SCRIPT_FUNCTION;
			func.u.uintValue = index;

			return func;
		}

		sol::lua_value convert_function(lua_State* state, unsigned int pos)
		{
			if (pos < notifies::vm_execute_hooks.size())
			{
				return notifies::vm_execute_hooks[pos];
			}

			return [pos](const entity& entity, const sol::this_state s, sol::variadic_args va)
			{
				std::vector<script_value> arguments{};

				for (auto arg : va)
				{
					arguments.push_back(convert({s, arg}));
				}

				return convert(s, scripting::exec_ent_thread(entity, pos, arguments));
			};
		}
	}

	sol::lua_value entity_to_struct(lua_State* state, unsigned int parent_id)
	{
		auto table = sol::table::create(state);
		auto metatable = sol::table::create(state);

		const auto offset = 51200 * (parent_id & 1);

		metatable[sol::meta_function::new_index] = [offset, parent_id](const sol::table t, const sol::this_state s,
			const sol::lua_value& field, const sol::lua_value& value)
		{
			const auto id = field.is<std::string>()
				? scripting::find_token_id(field.as<std::string>())
				: field.as<int>();

			if (!id)
			{
				return;
			}

			const auto variable_id = game::GetVariable(parent_id, id);
			const auto variable = &game::scr_VarGlob->childVariableValue[variable_id + offset];
			const auto new_variable = convert({s, value}).get_raw();

			game::AddRefToValue(new_variable.type, new_variable.u);
			game::RemoveRefToValue(variable->type, variable->u.u);

			variable->type = (char)new_variable.type;
			variable->u.u = new_variable.u;
		};

		metatable[sol::meta_function::index] = [offset, parent_id](const sol::table t, const sol::this_state s,
			const sol::lua_value& field)
		{
			const auto id = field.is<std::string>()
				? scripting::find_token_id(field.as<std::string>())
				: field.as<int>();

			if (!id)
			{
				return sol::lua_value{s, sol::lua_nil};
			}

			const auto variable_id = game::FindVariable(parent_id, id);
			if (!variable_id)
			{
				return sol::lua_value{s, sol::lua_nil};
			}

			const auto variable = game::scr_VarGlob->childVariableValue[variable_id + offset];

			game::VariableValue result{};
			result.u = variable.u.u;
			result.type = (game::scriptType_e)variable.type;

			return convert(s, result);
		};

		metatable[sol::meta_function::to_string] = [parent_id]()
		{
			return utils::string::va("script struct => id: %i", parent_id);
		};

		table["getraw"] = [parent_id]()
		{
			return entity(parent_id);
		};

		table[sol::metatable_key] = metatable;

		return {state, table};
	}

	script_value convert(const sol::lua_value& value)
	{
		if (value.is<int>())
		{
			return {value.as<int>()};
		}

		if (value.is<unsigned int>())
		{
			return {value.as<unsigned int>()};
		}

		if (value.is<bool>())
		{
			return {value.as<bool>()};
		}

		if (value.is<double>())
		{
			return {value.as<double>()};
		}

		if (value.is<float>())
		{
			return {value.as<float>()};
		}

		if (is_istring(value))
		{
			return string_to_istring(value);
		}

		if (value.is<std::string>())
		{
			return {value.as<std::string>()};
		}

		if (value.is<array>())
		{
			return {value.as<array>()};
		}

		if (value.is<entity>())
		{
			return {value.as<entity>()};
		}

		if (value.is<vector>())
		{
			return {value.as<vector>()};
		}

		if (value.is<sol::protected_function>())
		{
			return convert_function(value);
		}

		return {};
	}

	sol::lua_value convert(lua_State* state, const script_value& value)
	{
		if (value.is<int>())
		{
			return {state, value.as<int>()};
		}

		if (value.is<float>())
		{
			return {state, value.as<float>()};
		}

		if (value.is<std::string>())
		{
			return {state, value.as<std::string>()};
		}
		
		if (value.is<std::map<std::string, script_value>>())
		{
			return entity_to_struct(state, value.get_raw().u.uintValue);
		}

		if (value.is<array>())
		{
			return {state, value.as<array>()};
		}

		if (value.is<std::function<void()>>())
		{
			return convert_function(state, value.get_raw().u.uintValue);
		}

		if (value.is<entity>())
		{
			return {state, value.as<entity>()};
		}

		if (value.is<vector>())
		{
			return {state, value.as<vector>()};
		}

		return {state, sol::lua_nil};
	}
}
