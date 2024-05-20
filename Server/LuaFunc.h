#pragma once
#include "Lua/lua.hpp"

int callC_get_user_pos(lua_State* const L)noexcept;

int callC_get_npc_pos(lua_State* const L)noexcept;

int callC_RandomMove(lua_State* const L)noexcept;

int callC_send_chat(lua_State* const L)noexcept;

int callC_send_bye(lua_State* const L)noexcept;
