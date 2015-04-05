﻿
/*
* breeze License
* Copyright (C) 2015 YaweiZhang <yawei_zhang@foxmail.com>.
*
* Licensed under the Apache License, Version 2.0 (the "License");
* you may not use this file except in compliance with the License.
* You may obtain a copy of the License at
*
* http://www.apache.org/licenses/LICENSE-2.0
*
* Unless required by applicable law or agreed to in writing, software
* distributed under the License is distributed on an "AS IS" BASIS,
* WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
* See the License for the specific language governing permissions and
* limitations under the License.
*/


/*
*  file desc 
*  event trigger
*/




#ifndef _EVENT_TRIGGER_H_
#define _EVENT_TRIGGER_H_
#include <common.h>




//事件触发器只为其他系统模块提供事件出发服务. 这些系统模块指的是 活动,任务,成就 等.
//参数只允许
enum EventTriggerEnum : unsigned short
{
	ETRIGGER_USER_LOGIN = 0, // 玩家登录事件 triggerID, UserID
};

class  EventTrigger :public Singleton<EventTrigger>
{
public:
	EventTrigger();
	~EventTrigger();
	typedef std::function<void(UserID uID, unsigned long long param1, unsigned long long param2, unsigned long long param3)> TriggerHandler;
	bool init();

	template<class T>
	inline void pushvalue(lua_State *L, const T & val)
	{
		lua_pushinteger(L, (lua_Integer)val);
	}
	template<class T>
	inline void pushvalue(lua_State *L, const T * str)
	{
		lua_pushlstring(L, str, strlen(str));
	}
	template<class T>
	inline void pushvalue(lua_State *L, T * str)
	{
		lua_pushlstring(L, str, strlen(str));
	}
	template<>
	inline void pushvalue(lua_State *L, const std::string & str)
	{
		lua_pushlstring(L, str.c_str(), str.length());
	}

	template<>
	inline void pushvalue(lua_State *L, const unsigned long long & id)
	{
		//小端序
		lua_pushlstring(L, (const char*)&id, sizeof(id));
	}
	template<>
	inline void pushvalue(lua_State *L, const long long & id)
	{
		//小端序
		lua_pushlstring(L, (const char*)&id, sizeof(id));
	}

	
	template<typename T, typename ... Args>
	inline void trigger(EventTriggerEnum triggerID, const T & val, Args ... args)
	{
		if (_firstTrigger)
		{
			lua_settop(_luaState, 0);
			_firstTrigger = false;
			lua_pushinteger(_luaState, triggerID);
		}
		pushvalue(_luaState, val);
		trigger(triggerID, args ...);
	}
	template<typename T>
	inline void trigger(EventTriggerEnum triggerID, const T & val)
	{
		if (_firstTrigger)
		{
			lua_settop(_luaState, 0);
			_firstTrigger = false;
			lua_pushinteger(_luaState, triggerID);
		}
		pushvalue(_luaState, val);
		trigger(triggerID);
		_firstTrigger = true;
	}

	//lua callback
	void watching(EventTriggerEnum triggerID, int ref){ _watchings[triggerID].insert(ref); }
	void unwatching(EventTriggerEnum triggerID, int ref){ _watchings[triggerID].erase(ref); }
private:
	void trigger(EventTriggerEnum triggerID);
private:
	bool _firstTrigger = true;
	lua_State * _luaState = NULL;
	std::map<EventTriggerEnum, std::set<int>> _watchings;
};




































#endif