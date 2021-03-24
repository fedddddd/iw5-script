#pragma once

namespace game
{
	typedef float vec_t;
	typedef vec_t vec2_t[2];
	typedef vec_t vec3_t[3];
	typedef vec_t vec4_t[4];

	struct cmd_function_t
	{
		cmd_function_t* next;
		const char* name;
		const char* autoCompleteDir;
		const char* autoCompleteExt;
		void(__cdecl* function)();
		int flags;
	};

	struct msg_t
	{
		int overflowed;
		int readOnly;
		char* data;
		char* splitData;
		int maxsize;
		int cursize;
		int splitSize;
		int readcount;
		int bit;
		int lastEntityRef;
	};

	struct XZoneInfo
	{
		const char* name;
		int allocFlags;
		int freeFlags;
	};

	struct scr_entref_t
	{
		unsigned __int16 entnum;
		unsigned __int16 classnum;
	};

	typedef void(__cdecl* scr_call_t)(int entref);

	enum scriptType_e
	{
		SCRIPT_NONE = 0,
		SCRIPT_OBJECT = 1,
		SCRIPT_STRING = 2,
		SCRIPT_VECTOR = 4,
		SCRIPT_FLOAT = 5,
		SCRIPT_INTEGER = 6,
		SCRIPT_END = 8,
	};

	struct VariableStackBuffer
	{
		const char* pos;
		unsigned __int16 size;
		unsigned __int16 bufLen;
		unsigned __int16 localId;
		char time;
		char buf[1];
	};

	union VariableUnion
	{
		int intValue;
		float floatValue;
		unsigned int stringValue;
		const float* vectorValue;
		const char* codePosValue;
		unsigned int pointerValue;
		VariableStackBuffer* stackValue;
		unsigned int entityId;
		unsigned int uintValue;
	};

	struct VariableValue
	{
		VariableUnion u;
		scriptType_e type;
	};

	struct function_stack_t
	{
		const char* pos;
		unsigned int localId;
		unsigned int localVarCount;
		VariableValue* top;
		VariableValue* startTop;
	};

	struct function_frame_t
	{
		function_stack_t fs;
		int topType;
	};

	struct scrVmPub_t
	{
		unsigned int* localVars;
		VariableValue* maxstack;
		int function_count;
		function_frame_t* function_frame;
		VariableValue* top;
		/*bool debugCode;
		bool abort_on_error;
		bool terminal_error;
		bool block_execution;*/
		unsigned int inparamcount;
		unsigned int outparamcount;
		unsigned int breakpointOutparamcount;
		bool showError;
		function_frame_t function_frame_start[32];
		VariableValue stack[2048];
	};

	struct scr_classStruct_t
	{
		unsigned __int16 id;
		unsigned __int16 entArrayId;
		char charId;
		const char* name;
	};

	struct ObjectVariableValue
	{
		unsigned __int16 prev;
		unsigned __int16 entnum;
		unsigned int classnum;
	};

	struct ObjectVariableChildren
	{
		unsigned int firstChild;
		unsigned int lastChild;
	};

	struct ChildVariableValue_u_f
	{
		unsigned __int16 prev;
		unsigned __int16 next;
	};

	union ChildVariableValue_u
	{
		VariableUnion u;
	};

	struct ChildBucketMatchKeys_keys
	{
		unsigned __int16 name_hi;
		unsigned __int16 parentId;
	};

	union ChildBucketMatchKeys
	{
		ChildBucketMatchKeys_keys keys;
		unsigned int match;
	};

	struct ChildVariableValue
	{
		ChildVariableValue_u u;
		unsigned int next;
		char pad[4];
		char type;
		char name_lo;
		char _pad[2];
		ChildBucketMatchKeys k;
		unsigned int nextSibling;
		unsigned int prevSibling;
	};

	struct scrVarGlob_t
	{
		ObjectVariableValue* objectVariableValue;
		__declspec(align(128)) ObjectVariableChildren* objectVariableChildren;
		__declspec(align(128)) unsigned __int16* childVariableBucket;
		__declspec(align(128)) ChildVariableValue* childVariableValue;
	};

	union DvarValue
	{
		bool enabled;
		int integer;
		unsigned int unsignedInt;
		float value;
		float vector[4];
		const char* string;
		char color[4];
	};

	struct enum_limit
	{
		int stringCount;
		const char** strings;
	};

	struct int_limit
	{
		int min;
		int max;
	};

	struct float_limit
	{
		float min;
		float max;
	};

	union DvarLimits
	{
		enum_limit enumeration;
		int_limit integer;
		float_limit value;
		float_limit vector;
	};

	struct dvar_t
	{
		const char* name;
		unsigned int flags;
		char type;
		bool modified;
		DvarValue current;
		DvarValue latched;
		DvarValue reset;
		DvarLimits domain;
		bool(__cdecl* domainFunc)(dvar_t*, DvarValue);
		dvar_t* hashNext;
	};

	struct gentity_s
	{
		int entnum;
	};
}