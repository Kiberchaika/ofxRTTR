/************************************************************************************
*                                                                                   *
*   Copyright (c) 2014 - 2018 Axel Menzel <info@rttr.org>                           *
*                                                                                   *
*   This file is part of RTTR (Run Time Type Reflection)                            *
*   License: MIT License                                                            *
*                                                                                   *
*   Permission is hereby granted, free of charge, to any person obtaining           *
*   a copy of this software and associated documentation files (the "Software"),    *
*   to deal in the Software without restriction, including without limitation       *
*   the rights to use, copy, modify, merge, publish, distribute, sublicense,        *
*   and/or sell copies of the Software, and to permit persons to whom the           *
*   Software is furnished to do so, subject to the following conditions:            *
*                                                                                   *
*   The above copyright notice and this permission notice shall be included in      *
*   all copies or substantial portions of the Software.                             *
*                                                                                   *
*   THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR      *
*   IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,        *
*   FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE     *
*   AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER          *
*   LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,   *
*   OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE   *
*   SOFTWARE.                                                                       *
*                                                                                   *
*************************************************************************************/

#include "from_json.h"

#include <cstdio>
#include <string>
#include <vector>
#include <array>

#include <iostream>

#define RAPIDJSON_HAS_STDSTRING 1
#include <rapidjson/prettywriter.h> // for stringify JSON
#include <rapidjson/document.h>     // rapidjson's DOM-style API
#include <rttr/type>

using namespace rapidjson;
using namespace rttr;

#include <rttr/registration>

RTTR_REGISTRATION
{
	registration::class_<Variant>("Variant")
	.constructor<>()
	.property("v", &Variant::v)
	.property("type", &Variant::type)
	;
}

namespace
{

/////////////////////////////////////////////////////////////////////////////////////////

void fromjson_recursively(instance obj, Value& json_object);

/////////////////////////////////////////////////////////////////////////////////////////

variant extract_basic_types(Value& json_value)
{
    switch(json_value.GetType())
    {
        case kStringType:
        {
            return std::string(json_value.GetString());
            break;
        }
        case kNullType:     break;
        case kFalseType:
        case kTrueType:
        {
            return json_value.GetBool();
            break;
        }
        case kNumberType:
        {
            if (json_value.IsInt())
                return json_value.GetInt();
            else if (json_value.IsDouble())
                return json_value.GetDouble();
            else if (json_value.IsUint())
                return json_value.GetUint();
            else if (json_value.IsInt64())
                return json_value.GetInt64();
            else if (json_value.IsUint64())
                return json_value.GetUint64();
            break;
        }
        // we handle only the basic types here
        case kObjectType:
        case kArrayType: return variant();
    }

    return variant();
}


/////////////////////////////////////////////////////////////////////////////////////////

static void write_array_recursively(variant_sequential_view& view, Value& json_array_value)
{
    view.set_size(json_array_value.Size());
    const type array_value_type = view.get_rank_type(1);

    for (SizeType i = 0; i < json_array_value.Size(); ++i)
    {
        auto& json_index_value = json_array_value[i];
        if (json_index_value.IsArray())
        {
            auto sub_array_view = view.get_value(i).create_sequential_view();
            write_array_recursively(sub_array_view, json_index_value);
        }
        else if (json_index_value.IsObject())
        {
            variant var_tmp = view.get_value(i);
            variant wrapped_var = var_tmp.extract_wrapped_value();
            fromjson_recursively(wrapped_var, json_index_value);
            view.set_value(i, wrapped_var);
        }
        else
        {
            variant extracted_value = extract_basic_types(json_index_value);
            if (extracted_value.convert(array_value_type))
                view.set_value(i, extracted_value);
        }
    }
}

variant extract_value(Value::MemberIterator& itr, const type& t)
{
    auto& json_value = itr->value;
    variant extracted_value = extract_basic_types(json_value);
    const bool could_convert = extracted_value.convert(t);
    if (!could_convert)
    {
        if (json_value.IsObject())
        {
            constructor ctor = t.get_constructor();
            for (auto& item : t.get_constructors())
            {
                if (item.get_instantiated_type() == t)
                    ctor = item;
            }
            extracted_value = ctor.invoke();
            fromjson_recursively(extracted_value, json_value);
        }
    }

    return extracted_value;
}

static void write_associative_view_recursively(variant_associative_view& view, Value& json_array_value)
{
    for (SizeType i = 0; i < json_array_value.Size(); ++i)
    {
        auto& json_index_value = json_array_value[i];
        if (json_index_value.IsObject()) // a key-value associative view
        {
            Value::MemberIterator key_itr = json_index_value.FindMember("key");
            Value::MemberIterator value_itr = json_index_value.FindMember("value");

			std::string k  = key_itr->value.GetString();

            if (key_itr != json_index_value.MemberEnd() &&
                value_itr != json_index_value.MemberEnd())
            {
				if (view.get_value_type() == type::get<variant>()) {
					auto key_var = extract_value(key_itr, view.get_key_type());
					auto value_var = extract_basic_types(value_itr->value);
					if (key_var && value_var)
					{
						view.insert(key_var, value_var);
					}
				}
				else {
					auto key_var = extract_value(key_itr, view.get_key_type());
					auto value_var = extract_value(value_itr, view.get_value_type());
					if (key_var && value_var)
					{
						view.insert(key_var, value_var);
					}
				}
            }
        }
        else // a key-only associative view
        {
            variant extracted_value = extract_basic_types(json_index_value);
            if (extracted_value && extracted_value.convert(view.get_key_type()))
                view.insert(extracted_value);
        }
    }
}

/////////////////////////////////////////////////////////////////////////////////////////

void fromjson_recursively(instance obj2, Value& json_object)
{
    instance obj = obj2.get_type().get_raw_type().is_wrapper() ? obj2.get_wrapped_instance() : obj2;
    const auto prop_list = obj.get_derived_type().get_properties();

	auto t = obj.get_derived_type();
	auto ttt3= t.get_raw_type();
	auto  ttt = obj.get_type();
	auto  ttt1 = obj.get_wrapped_instance();
	bool w = t.is_wrapper();
	auto f = t.get_id();
	bool dd = t.is_valid();

    for (auto prop : prop_list)
    {
        Value::MemberIterator ret = json_object.FindMember(prop.get_name().data());

		std::string nameX = prop.get_name().data();

		if (nameX == "var") {
			int c = 1;
		}

        if (ret == json_object.MemberEnd())
            continue;
        const type value_t = prop.get_type();

        auto& json_value = ret->value;
        switch(json_value.GetType())
        {
            case kArrayType:
            {
                variant var;
                if (value_t.is_sequential_container())
                {
                    var = prop.get_value(obj);
                    auto view = var.create_sequential_view();
                    write_array_recursively(view, json_value);
                }
                else if (value_t.is_associative_container())
                {
                    var = prop.get_value(obj);
                    auto associative_view = var.create_associative_view();
                    write_associative_view_recursively(associative_view, json_value);
                }

				if (value_t == type::get<variant>()) {
					var = prop.get_value(obj);
					if (var.is_sequential_container())
					{
						auto view = var.create_sequential_view();
						write_array_recursively(view, json_value);
					}
					else if (var.is_associative_container())
					{
						auto associative_view = var.create_associative_view();
						write_associative_view_recursively(associative_view, json_value);
					}
					
					/*
					std::map<int, Variant> my_map = { { 1, Variant("one") }, { 2,  Variant("one") }, { 3,  Variant("one") } };
					variant v2 = my_map;
					variant_associative_view view = v2.create_associative_view(); // creates a view of the hold container in the variant (data is not copied!)
					
					
					view.insert(variant(4), variant(Variant("two")));
					view.insert(variant(5), Variant("three"));

					
					//auto t= view.get_value_type();

					//view.begin() = 1;
					var = v2;
					*/
				}

                prop.set_value(obj, var);
                break;
            }
            case kObjectType:
            {
                variant var = prop.get_value(obj);
                fromjson_recursively(var, json_value);
                prop.set_value(obj, var);
                break;
            }
            default:
            {
                variant extracted_value = extract_basic_types(json_value);
				
				if (value_t == type::get<variant>()) {
					prop.set_value(obj, extracted_value);
				}
				else if (extracted_value.convert(value_t)) {
					// REMARK: CONVERSION WORKS ONLY WITH "const type", check whether this is correct or not!
					prop.set_value(obj, extracted_value);
				}
			}
        }
    }
}

} // end namespace anonymous

/////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////

namespace io
{

bool from_json(const std::string& json, rttr::instance obj)
{
    Document document;  // Default template parameter uses UTF8 and MemoryPoolAllocator.

    // "normal" parsing, decode strings to new buffers. Can use other input stream via ParseStream().
    if (document.Parse(json.c_str()).HasParseError())
        return false;

    fromjson_recursively(obj, document);

    return true;
}

/////////////////////////////////////////////////////////////////////////////////////////

} // end namespace io
