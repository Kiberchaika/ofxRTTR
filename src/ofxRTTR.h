#pragma once

#include <cstdio>
#include <string>
#include <vector>
#include <array>
#include <map>
#include <iostream>

#include <rttr/registration>

#include "to_json.h"
#include "from_json.h"

using namespace rttr;

namespace io
{
	size_t get_obj_properties_count(const instance& obj2)
	{
		instance obj = obj2.get_type().get_raw_type().is_wrapper() ? obj2.get_wrapped_instance() : obj2;
		return obj.get_derived_type().get_properties().size();
	}
}