/**
 * Copyright (c) 2014, Ford Motor Company
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 *
 * Redistributions of source code must retain the above copyright notice, this
 * list of conditions and the following disclaimer.
 *
 * Redistributions in binary form must reproduce the above copyright notice,
 * this list of conditions and the following
 * disclaimer in the documentation and/or other materials provided with the
 * distribution.
 *
 * Neither the name of the Ford Motor Company nor the names of its contributors
 * may be used to endorse or promote products derived from this software
 * without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
 * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE
 * LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
 * CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
 * SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
 * INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
 * CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
 * ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
 * POSSIBILITY OF SUCH DAMAGE.
 */

#include "gtest/gtest.h"
#include "json/value.h"
#include "rpc_base/rpc_base.h"
#include "rpc_base/rpc_base_json_inl.h"

namespace test {
using namespace rpc;
using Json::Value;

namespace {
enum TestEnum {
  kValue0,
  kValue1,
  kInvalidValue
};

bool IsValidEnum(TestEnum val) {
  return val == kValue0 || val == kValue1;
}

bool EnumFromJsonString(const std::string& value, TestEnum* enm) {
  if (value == "kValue0") {
    *enm = kValue0;
    return true;
  } else if (value == "kValue1") {
    *enm = kValue1;
    return true;
  } else {
    return false;
  }
}

const char* EnumToJsonString(TestEnum enm) {
  switch(enm) {
    case kValue0: return "kValue0";
    case kValue1: return "kValue1";
    default: return "UNKNOWN";
  }
}

}  // namespace

TEST(ValidatedTypesJson, BooleanFromJsonTest) {
  Value val(true);
  Boolean boolean(val);
  ASSERT_TRUE(boolean.is_initialized());
  ASSERT_TRUE(boolean.is_valid());
  ASSERT_EQ(boolean, true);
  Value readback = boolean.ToJsonValue();
  ASSERT_TRUE(readback.isBool());
  ASSERT_EQ(readback.asBool(), true);
}

TEST(ValidatedTypesJson, BooleanFromInvalidJsonTest) {
  Value inv(7);
  Boolean boolean(inv);
  ASSERT_TRUE(boolean.is_initialized());
  ASSERT_FALSE(boolean.is_valid());
}

TEST(ValidatedTypesJson, IntegerFromJsonTest) {
  Value int_val(42);
  Integer<int32_t, -5, 192> integer(int_val);
  ASSERT_TRUE(integer.is_initialized());
  ASSERT_TRUE(integer.is_valid());
  Value readback = integer.ToJsonValue();
  ASSERT_TRUE(readback.isInt());
  ASSERT_EQ(readback.asInt(), 42);
}

TEST(ValidatedTypesJson, IntegerFromOverflowingJsonTest) {
  Value int_val(0xFFFFFFFFFFll);
  Integer<int32_t, -5, 192> integer(int_val);
  ASSERT_TRUE(integer.is_initialized());
  ASSERT_FALSE(integer.is_valid());
}

TEST(ValidatedTypesJson, IntegerFromInvalidJsonTest) {
  Value str_val("Hello");
  Integer<int8_t, -3, 15> integer(str_val);
  ASSERT_TRUE(integer.is_initialized());
  ASSERT_FALSE(integer.is_valid());
}

TEST(ValidatedTypesJson, IntegerFromOutOfRangeValueTest) {
  Value big_int_val(500);
  Integer<int8_t, 0, 100> integer(big_int_val);
  ASSERT_TRUE(integer.is_initialized());
  ASSERT_FALSE(integer.is_valid());
}

TEST(ValidatedTypesJson, FloatFromJsonTest) {
  Value float_value(4.2);
  Float<1, 7> flt(float_value);
  ASSERT_TRUE(flt.is_initialized());
  ASSERT_TRUE(flt.is_valid());
  Value readback = flt.ToJsonValue();
  ASSERT_TRUE(readback.isDouble());
  ASSERT_EQ(readback.asDouble(), 4.2);
}

TEST(ValidatedTypesJson, FloatFromInvalidJsonTest) {
  Value str_val("Hello");
  Float<-5, 3> flt(str_val);
  ASSERT_TRUE(flt.is_initialized());
  ASSERT_FALSE(flt.is_valid());
}

TEST(ValidatedTypesJson, StringFromJsonTest) {
  Value str_val("Hello");
  String<42> str(str_val);
  ASSERT_TRUE(str.is_initialized());
  ASSERT_TRUE(str.is_valid());
  Value readback = str.ToJsonValue();
  ASSERT_TRUE(readback.isString());
  ASSERT_STREQ(readback.asCString(), "Hello");
}

TEST(ValidatedTypesJson, StringFromInvalidJsonTest) {
  Value int_val(42);
  String<500> str(int_val);
  ASSERT_TRUE(str.is_initialized());
  ASSERT_FALSE(str.is_valid());
}

TEST(ValidatedTypesJson, StringFromToLongJsonString) {
  Value str_val("Too long string");
  String<5> str(str_val);
  ASSERT_TRUE(str.is_initialized());
  ASSERT_FALSE(str.is_valid());
}

TEST(ValidatedTypesJson, EnumFromJsonTest) {
  Value str_enum("kValue1");
  Enum<TestEnum> enm(str_enum);
  ASSERT_TRUE(enm.is_initialized());
  ASSERT_TRUE(enm.is_valid());
  Value readback = enm.ToJsonValue();
  ASSERT_TRUE(readback.isString());
  ASSERT_STREQ(readback.asCString(), "kValue1");
}

TEST(ValidatedTypesJson, EnumFromInvalidJsonTest) {
  Value str_value("Random string");
  Enum<TestEnum> enm(str_value);
  ASSERT_TRUE(enm.is_initialized());
  ASSERT_FALSE(enm.is_valid());
}

TEST(ValidatedTypesJson, ArrayFromJsonTest) {
  Value array_value;
  array_value.append(Value("haha"));
  array_value.append(Value("hoho"));
  Array<String<32>, 2, 5> arr(array_value);
  ASSERT_TRUE(arr.is_initialized());
  ASSERT_TRUE(arr.is_valid());
  Value readback = arr.ToJsonValue();
  ASSERT_TRUE(readback.isArray());
  ASSERT_EQ(readback.size(), array_value.size());
}

TEST(ValidatedTypesJson, ArrayFromInvalidJsonTest) {
  Value array_value;
  array_value.append(Value("Hello"));
  array_value.append(Value("World"));
  Array<Integer<int8_t, 0, 32>, 2, 4> int_array(array_value);
  ASSERT_TRUE(int_array.is_initialized());
  ASSERT_FALSE(int_array.is_valid());
  ASSERT_EQ(int_array.size(), array_value.size());
}

TEST(ValidatedTypesJson, OptionalBoolFromJsonTest) {
  Value bool_value(true);
  Optional< Boolean > optional_bool;
  *optional_bool = Boolean(bool_value);
  ASSERT_TRUE(optional_bool.is_initialized());
  ASSERT_TRUE(optional_bool.is_valid());
  Value readback = optional_bool->ToJsonValue();
  ASSERT_TRUE(readback.isBool());
  ASSERT_EQ(readback.asBool(), true);
}

TEST(ValidatedTypesJson, OptionalIntFromJsonTest) {
  Value int_value(42);
  Optional< Integer<int64_t, 42, 43> > optional_int;
  *optional_int = Integer<int64_t, 42, 43> (int_value);
  ASSERT_TRUE(optional_int.is_initialized());
  ASSERT_TRUE(optional_int.is_valid());
  Value readback = optional_int->ToJsonValue();
  ASSERT_TRUE(readback.isInt());
  ASSERT_EQ(readback.asInt(), 42);
}


}  // namespace test


