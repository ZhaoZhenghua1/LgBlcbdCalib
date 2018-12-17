#pragma once


#include ".\include\rapidjson\document.h"
#include ".\include\rapidjson\error\en.h"
#include ".\include\rapidjson\stringbuffer.h"
#include ".\include\rapidjson\prettywriter.h"

typedef rapidjson::GenericDocument<rapidjson::UTF16<>> WDocument;
typedef rapidjson::GenericValue<rapidjson::UTF16<> > WValue;
typedef rapidjson::GenericStringBuffer<rapidjson::UTF16<> > WStringBuffer;

std::wstring WValueToString(const WValue& value);

typedef unsigned char byte;
