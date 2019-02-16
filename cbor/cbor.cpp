#include "cbor.h"
#include "utf8.h"

///////////////////////////
// RFC 7049 (CBOR)

//////////////////////////////////////////////////////////////
// CBOR Writer

TRCBORWriter::TRCBORWriter() : 
	blockmemsize(512),
	fullsize(512),
	usesize(0)
{
	pointer = malloc(fullsize);
}

TRCBORWriter::~TRCBORWriter()
{
	free(pointer);
	pointer = nullptr;
}

void TRCBORWriter::needmemory(size_t needsize)
{
	if (usesize + needsize > fullsize)
	{
		fullsize = usesize + needsize + blockmemsize;
		pointer = realloc(pointer, fullsize);
	}
}

void TRCBORWriter::Clear(void)
{
	usesize = 0;
	if (fullsize > 1024 * 10)
	{
		fullsize = 1024 * 10;
		pointer = realloc(pointer, fullsize);
	}
}

size_t TRCBORWriter::Size(void)  const
{
	return usesize;
}

void TRCBORWriter::SetSize(size_t size)
{
	usesize = size;
}

void* TRCBORWriter::GetCurrentPointer(void) const
{
	return (uint8_t*)pointer + usesize;
}

void* TRCBORWriter::Pointer(void) const
{
	return pointer;
}

// запись значения (или размера строки, буфера и т.д.)
void TRCBORWriter::writeCBORSizeValue32(uint8_t majortype, uint32_t value)
{
	uint8_t additionaltype;

	if (value == (value & 0xffff))
	{
		if (value == (value & 0xff))
		{
			additionaltype = 24; // uint8_t
			Write8U((majortype << 5) | additionaltype);
			Write8U(value);
		}
		else
		{
			additionaltype = 25; // uint16_t
			Write8U((majortype << 5) | additionaltype);
			Write16U(value);
		}
	}
	else
	{
		additionaltype = 26; // uint32_t
		Write8U((majortype << 5) | additionaltype);
		Write32U(value);
	}
}

void TRCBORWriter::WriteCBORValue(int32_t value)
{
	uint8_t majortype(HCBOR_POSITIVEINTEGER);
	uint32_t resvalue;

	if (value < 0)
	{
		majortype = HCBOR_NEGATIVEINTEGER;
		resvalue = -value - 1;
	}
	else
		resvalue = value;

	if (resvalue < 24)
		Write8U((majortype << 5) | resvalue);
	else	
		writeCBORSizeValue32(majortype, resvalue);
}

void TRCBORWriter::WriteCBORValue(int64_t value)
{
	int64_t resvalue;
	uint8_t majortype(HCBOR_POSITIVEINTEGER);
	uint8_t additionaltype;

	if (value < 0)
	{
		majortype = HCBOR_NEGATIVEINTEGER;
		resvalue = -value - 1;
	}
	else
		resvalue = value;

	if (resvalue < 24)
	{
		Write8U((majortype << 5) | (uint8_t)resvalue);
	}
	else
	{
		if (resvalue == (resvalue & 0xffffffff))
		{
			writeCBORSizeValue32(majortype, (uint32_t)resvalue);
		}
		else
		{
			additionaltype = 27; // uint64_t
			Write8U((majortype << 5) | additionaltype);
			Write64I(resvalue);
		}
	}
}

void TRCBORWriter::WriteCBORByteArray(void* buffer, size_t sizebuffer)
{
	uint8_t majortype(HCBOR_BYTEARRAY);

	if (sizebuffer < 24)
		Write8U((majortype << 5) | (uint8_t)sizebuffer);
	else
		writeCBORSizeValue32(majortype, sizebuffer);
	
	WriteBuffer(buffer, sizebuffer);
}

void TRCBORWriter::WriteCBORString(const std::string& str)
{
	uint8_t majortype(HCBOR_STRING_UTF8);

	if (str.size() < 24)
		Write8U((majortype << 5) | (uint8_t)str.size());
	else
		writeCBORSizeValue32(majortype, (uint32_t)str.size());

	WriteString(str);
}

void TRCBORWriter::WriteCBORString(const std::wstring& str)
{
	std::string utf8str;
	utf8str = wstrTOutf8(str);
	WriteCBORString(utf8str);
}

void TRCBORWriter::WriteCBORItemsArrayMarker(uint32_t itemscount)
{
	uint8_t majortype(HCBOR_ITEMSARRAY);

	if (itemscount == UINT32_MAX)
	{
		uint8_t additionaltype(31); // неопределенная длина
		Write8U((majortype << 5) | additionaltype);
	}
	else
	{
		if (itemscount < 24)
			Write8U((majortype << 5) | itemscount);
		else
			writeCBORSizeValue32(majortype, itemscount);
	}
}

void TRCBORWriter::WriteCBORPairsArrayMarker(uint32_t pairscount)
{
	uint8_t majortype(HCBOR_PAIRSARRAY);

	if (pairscount == UINT32_MAX)
	{
		uint8_t additionaltype(31); // неопределенная длина
		Write8U((majortype << 5) | additionaltype);
	}
	else
	{
		if (pairscount < 24)
			Write8U((majortype << 5) | pairscount);
		else
			writeCBORSizeValue32(majortype, pairscount);
	}
}

void TRCBORWriter::WriteCBORFloat(float value)
{
	uint8_t majortype(HCBOR_FLOATSIMPLE);
	uint8_t additionaltype(26); // float 32-bit

	Write8U((majortype << 5) | additionaltype);
	Write32F(value);
}

void TRCBORWriter::WriteCBORFloat(double value)
{
	uint8_t majortype(HCBOR_FLOATSIMPLE);
	uint8_t additionaltype(27); // float 64-bit

	Write8U((majortype << 5) | additionaltype);
	Write64F(value);
}

void TRCBORWriter::WriteCBORBool(bool value)
{
	uint8_t majortype(HCBOR_FLOATSIMPLE);
	uint8_t additionaltype;

	if (value == false)
		additionaltype = 20; // false
	else
		additionaltype = 21; // true

	Write8U((majortype << 5) | additionaltype);
}

void TRCBORWriter::WriteCBORNull(void)
{
	uint8_t majortype(HCBOR_FLOATSIMPLE);
	uint8_t additionaltype(22); // Null

	Write8U((majortype << 5) | additionaltype);
}

void TRCBORWriter::WriteCBORUndefined(void)
{
	uint8_t majortype(HCBOR_FLOATSIMPLE);
	uint8_t additionaltype(23); // Undefined

	Write8U((majortype << 5) | additionaltype);
}

void TRCBORWriter::WriteCBORStopArrayMarker(void)
{
	uint8_t majortype(HCBOR_FLOATSIMPLE);
	uint8_t additionaltype(31); // конец массива

	Write8U((majortype << 5) | additionaltype);
}

void TRCBORWriter::Write8U(uint8_t value)
{
	needmemory(1);

	*((uint8_t*)pointer + usesize) = value;

	++usesize;
}

void TRCBORWriter::Write16U(uint16_t value)
{
	needmemory(2);

	*((uint16_t*)((uint8_t*)pointer + usesize)) = value;

	usesize += 2;
}

void TRCBORWriter::Write32U(uint32_t value)
{
	needmemory(4);

	*((uint32_t*)((uint8_t*)pointer + usesize)) = value;

	usesize += 4;
}

void TRCBORWriter::Write64U(uint64_t value)
{
	needmemory(8);

	*((uint64_t*)((uint8_t*)pointer + usesize)) = value;

	usesize += 8;
}

void TRCBORWriter::Write8I(int8_t value)
{
	needmemory(1);

	*((int8_t*)pointer + usesize) = value;

	++usesize;
}

void TRCBORWriter::Write16I(int16_t value)
{
	needmemory(2);

	*((int16_t*)((uint8_t*)pointer + usesize)) = value;

	usesize += 2;
}

void TRCBORWriter::Write32I(int32_t value)
{
	needmemory(4);

	*((int32_t*)((uint8_t*)pointer + usesize)) = value;

	usesize += 4;
}

void TRCBORWriter::Write64I(int64_t value)
{
	needmemory(8);

	*((int64_t*)((uint8_t*)pointer + usesize)) = value;

	usesize += 8;
}

void TRCBORWriter::Write32F(float value)
{
	needmemory(4);

	*((float*)((uint8_t*)pointer + usesize)) = value;

	usesize += 4;
}

void TRCBORWriter::Write64F(double value)
{
	needmemory(8);

	*((double*)((uint8_t*)pointer + usesize)) = value;

	usesize += 8;
}

void TRCBORWriter::WriteString(const std::string& value)
{
	needmemory(value.size());

	memcpy(((uint8_t*)pointer + usesize), value.c_str(), value.size());

	usesize += value.size();
}

void TRCBORWriter::WriteBuffer(void* buffer, size_t sizebuffer)
{
	needmemory(sizebuffer);

	memcpy(((uint8_t*)pointer + usesize), buffer, sizebuffer);

	usesize += sizebuffer;
}

//////////////////////////////////////////////////////////////
// CBOR Reader

TRCBORReader::TRCBORReader()
{

}

TRCBORReader::~TRCBORReader()
{

}

uint32_t TRCBORReader::readCBORSizeValue32(uint8_t additionaltype)
{
	uint32_t result;

	if (additionaltype < 24)
		result = additionaltype;
	else
	{
		result = 0;
		switch (additionaltype)
		{			// TODO для big endian надо переделать
		case 24:
			result = ReadUInt8();
			break;
		case 25:
			result = ReadUInt16();
			break;
		case 26:
			result = ReadUInt32();
			break;
		case 31:
			result = UINT32_MAX;
			break;
		}
	}

	return result;
}

uint64_t TRCBORReader::readCBORSizeValue64(uint8_t additionaltype)
{
	uint64_t result;

	if (additionaltype < 24)
		result = additionaltype;
	else
	{
		result = 0;
		switch (additionaltype)
		{			// TODO для big endian надо переделать
		case 24:
			result = ReadUInt8();
			break;
		case 25:
			result = ReadUInt16();
			break;
		case 26:
			result = ReadUInt32();
			break;
		case 27:
			result = ReadUInt64();
			break;
		case 31:
			result = UINT32_MAX;
			break;
		}
	}

	return result;
}

void TRCBORReader::SetBuffer(void* ptr, size_t sizebuffer)
{
	this->ptr = ptr;
	this->sizebuffer = sizebuffer;
	position = 0;
}

bool TRCBORReader::ParseCBOR(TRHCBOROutType& valuetype, void* outvalue, size_t& valuesize)
{
	uint8_t cbortype, majortype, additionaltype;

	if (position >= sizebuffer)
		return false;

	cbortype = ReadUInt8();

	majortype = cbortype >> 5;
	additionaltype = cbortype & 31;

	switch (majortype)
	{
	case HCBOR_POSITIVEINTEGER:
		if (additionaltype == 27)
		{
			*(uint64_t*)outvalue = readCBORSizeValue64(additionaltype);
			valuesize = 8;
			valuetype = HCBOROUT_INT64;
		}
		else
		{
			*(uint32_t*)outvalue = readCBORSizeValue32(additionaltype);
			valuesize = 4;
			valuetype = HCBOROUT_INT;
		}
		break;
	case HCBOR_NEGATIVEINTEGER:
		if (additionaltype == 27)
		{
			*(int64_t*)outvalue = -((int64_t)readCBORSizeValue64(additionaltype) + 1);
			valuesize = 8;
			valuetype = HCBOROUT_INT64;
		}
		else
		{
			*(int32_t*)outvalue = -((int32_t)readCBORSizeValue32(additionaltype) + 1);
			valuesize = 4;
			valuetype = HCBOROUT_INT;
		}
		break;
	case HCBOR_BYTEARRAY:
		valuetype = HCBOROUT_BYTEARRAY;
		valuesize = readCBORSizeValue32(additionaltype);
		*(uintptr_t*)outvalue = (uintptr_t)GetCurrentPointer();
		position = position + valuesize;
		break;
	case HCBOR_STRING_UTF8:
		valuesize = readCBORSizeValue32(additionaltype);
		valuetype = HCBOROUT_STRING_UTF8;
		*(uintptr_t*)outvalue = (uintptr_t)GetCurrentPointer();
		position = position + valuesize;
		break;
	case HCBOR_ITEMSARRAY:
		valuesize = readCBORSizeValue32(additionaltype);
		valuetype = HCBOROUT_ITEMSARRAY_MARKER;
		break;
	case HCBOR_PAIRSARRAY:
		valuesize = readCBORSizeValue32(additionaltype);
		valuetype = HCBOROUT_PAIRSARRAY_MARKER;
		break;
	case HCBOR_TAGVALUE:
		break;
	case HCBOR_FLOATSIMPLE:
		switch (additionaltype)
		{
		case 20:
			valuesize = 0;
			valuetype = HCBOROUT_FALSE;
			break;
		case 21:
			valuesize = 0;
			valuetype = HCBOROUT_TRUE;
			break;
		case 22:
			valuesize = 0;
			valuetype = HCBOROUT_NULL;
			break;
		case 23:
			valuesize = 0;
			valuetype = HCBOROUT_UNDEFINED;
			break;
		case 26:
			*(float*)outvalue = ReadFloat32();
			valuesize = 4;
			valuetype = HCBOROUT_FLOAT32;
			break;
		case 27:
			*(double*)outvalue = ReadFloat64();
			valuesize = 8;
			valuetype = HCBOROUT_FLOAT64;
			break;
		case 31:
			valuesize = 0;
			valuetype = HCBOROUT_ENDARRAY_MARKER;
			break;
		}
		break;
	}

	return true;
}

uint32_t TRCBORReader::ReadUInt8(void)
{
	uint8_t* ptrpos;
	ptrpos = (uint8_t*)ptr + position;
	position++;
	return *ptrpos;
}

uint32_t TRCBORReader::ReadUInt16(void)
{
	uint16_t* ptrpos;
	ptrpos = (uint16_t*)((uint8_t*)ptr + position);
	position += 2;
	return *ptrpos;
}

uint32_t TRCBORReader::ReadUInt32(void)
{
	uint32_t* ptrpos;
	ptrpos = (uint32_t*)((uint8_t*)ptr + position);
	position += 4;
	return *ptrpos;
}

uint64_t TRCBORReader::ReadUInt64(void)
{
// нормальная версия (падает на невыровненных данных)
/*	uint64_t* ptrpos;
	ptrpos = (uint64_t*)((uint8_t*)ptr + position);
	position += 8;
	return *ptrpos;*/
// кривая версия, но на ARM на невыровненных данных - только оно работает

	uint64_t p1 = ReadUInt32();
	p1 = p1 << 32;
	p1 = p1 | (uint64_t)ReadUInt32();
	return p1;
}

float TRCBORReader::ReadFloat32(void)
{
	float* ptrpos;
	ptrpos = (float*)((uint8_t*)ptr + position);
	position += 4;
	return *ptrpos;
}

double TRCBORReader::ReadFloat64(void)
{
	double* ptrpos;
	ptrpos = (double*)((uint8_t*)ptr + position);
	position += 8;
	return *ptrpos;
}

void* TRCBORReader::GetCurrentPointer(void)
{
	return (uint8_t*)ptr + position;
}

void* TRCBORReader::GetBuffer(size_t& sizebuffer)
{
	sizebuffer = this->sizebuffer;
	return this->ptr;
}


//////////////////////////////////////////////////////////////
// CBOR Object Model

TRCBORObject::TRCBORObject() : bytearray(nullptr), bytearraysize(0)
{
	memset(buffervalue, 0, sizeof(buffervalue));
}

TRCBORObject::~TRCBORObject()
{
	for (auto& it : Childs)
		delete it;
	Childs.clear();
}

size_t TRCBORObject::GetChildsCount(void)
{
	return Childs.size();
}

TRCBORObject* TRCBORObject::GetChild(size_t index)
{
	if (index >= Childs.size())
		return nullptr;

	return Childs[index];
}

bool TRCBORObject::GetByteArray(void **ptr, size_t &size)
{
	if (ObjectType != HOBJTYPE_BYTEARRAY)
		return false;

	*ptr = bytearray;
	size = bytearraysize;
	return true;
}

std::string& TRCBORObject::AsString(void)
{
	return this->Utf8Value;
}

int32_t TRCBORObject::AsInt32(void)
{
	switch (this->ObjectType)
	{
	case HOBJTYPE_INT:
		return *(int32_t*)buffervalue;
		break;
	case HOBJTYPE_INT64:
		return (int32_t)(*(int64_t*)buffervalue);
		break;
	case HOBJTYPE_FLOAT32:
		return (int32_t)(*(float*)buffervalue);
		break;
	case HOBJTYPE_FLOAT64:
		return (int32_t)(*(double*)buffervalue);
		break;
	case HOBJTYPE_BOOL:
		return (int32_t)(*(uint64_t*)buffervalue);
		break;
	}

	return 0;
}

int64_t TRCBORObject::AsInt64(void)
{
	switch (this->ObjectType)
	{
	case HOBJTYPE_INT:
		return (int64_t)(*(int32_t*)buffervalue);
		break;
	case HOBJTYPE_INT64:
		return *(int64_t*)buffervalue;
		break;
	case HOBJTYPE_FLOAT32:
		return (int64_t)(*(float*)buffervalue);
		break;
	case HOBJTYPE_FLOAT64:
		return (int64_t)(*(double*)buffervalue);
		break;
	case HOBJTYPE_BOOL:
		return *(uint64_t*)buffervalue;
		break;
	}

	return 0;
}

float TRCBORObject::AsFloat(void)
{
	switch (this->ObjectType)
	{
	case HOBJTYPE_INT:
		return (float)(*(int32_t*)buffervalue);
		break;
	case HOBJTYPE_INT64:
		return (float)(*(int64_t*)buffervalue);
		break;
	case HOBJTYPE_FLOAT32:
		return *(float*)buffervalue;
		break;
	case HOBJTYPE_FLOAT64:
		return (float)(*(double*)buffervalue);
		break;
	case HOBJTYPE_BOOL:
		return (float)(*(uint64_t*)buffervalue);
		break;
	}

	return 0;
}

double TRCBORObject::AsDouble(void)
{
	switch (this->ObjectType)
	{
	case HOBJTYPE_INT:
		return (double)(*(int32_t*)buffervalue);
		break;
	case HOBJTYPE_INT64:
		return (double)(*(int64_t*)buffervalue);
		break;
	case HOBJTYPE_FLOAT32:
		return (double)(*(float*)buffervalue);
		break;
	case HOBJTYPE_FLOAT64:
		return *(double*)buffervalue;
		break;
	case HOBJTYPE_BOOL:
		return (double)(*(uint64_t*)buffervalue);
		break;
	}

	return 0;
}

bool TRCBORObject::AsBool(void)
{
	switch (this->ObjectType)
	{
	case HOBJTYPE_INT:
		return *(int32_t*)buffervalue != 0;
		break;
	case HOBJTYPE_INT64:
		return *(int64_t*)buffervalue != 0;
		break;
	case HOBJTYPE_FLOAT32:
		return *(float*)buffervalue != 0;
		break;
	case HOBJTYPE_FLOAT64:
		return *(double*)buffervalue != 0;
		break;
	case HOBJTYPE_BOOL:
		return *(uint64_t*)buffervalue != 0;
		break;
	}

	return false;
}

TRHCBORObjectType TRCBORObject::GetType(void)
{
	return ObjectType;
}

TRCBORObjectModel::TRCBORObjectModel()
{

}

TRCBORObjectModel::~TRCBORObjectModel()
{
	for (auto& it : Childs)
		delete it;
	Childs.clear();
}

void TRCBORObjectModel::SetBuffer(void* ptr, size_t sizebuffer)
{
	reader.SetBuffer(ptr, sizebuffer);
}

size_t TRCBORObjectModel::GetChildsCount(void)
{
	return Childs.size();
}

TRCBORObject* TRCBORObjectModel::GetChild(size_t index)
{
	if (index >= Childs.size())
		return nullptr;

	return Childs[index];
}

void TRCBORObjectModel::Parse(void)
{
	Childs.clear();
	Parse(Childs, 0);
}

void TRCBORObjectModel::Parse(std::vector<TRCBORObject*> &Childs, int32_t waitcount)
{
	TRHCBOROutType valuetype;
	uint8_t outvalue[8];
	size_t valuesize;

	int elementindex = 0;
	TRCBORObject* CurrentElement;

	while (reader.ParseCBOR(valuetype, outvalue, valuesize) == true)
	{
		switch (valuetype)
		{
		case HCBOROUT_INT:
			CurrentElement = new TRCBORObject;
			Childs.push_back(CurrentElement);
			CurrentElement->ObjectType = HOBJTYPE_INT;
			*(int32_t*)CurrentElement->buffervalue = *(int32_t*)outvalue;
			break;
		case HCBOROUT_INT64:
			CurrentElement = new TRCBORObject;
			Childs.push_back(CurrentElement);
			CurrentElement->ObjectType = HOBJTYPE_INT64;
			*(int64_t*)CurrentElement->buffervalue = *(int64_t*)outvalue;
			break;
		case HCBOROUT_FLOAT32:
			CurrentElement = new TRCBORObject;
			Childs.push_back(CurrentElement);
			CurrentElement->ObjectType = HOBJTYPE_FLOAT32;
			*(float*)CurrentElement->buffervalue = *(float*)outvalue;
			break;
		case HCBOROUT_FLOAT64:
			CurrentElement = new TRCBORObject;
			Childs.push_back(CurrentElement);
			CurrentElement->ObjectType = HOBJTYPE_FLOAT64;
			*(double*)CurrentElement->buffervalue = *(double*)outvalue;
			break;
		case HCBOROUT_TRUE:
			CurrentElement = new TRCBORObject;
			Childs.push_back(CurrentElement);
			CurrentElement->ObjectType = HOBJTYPE_BOOL;
			*(uint64_t*)CurrentElement->buffervalue = 1;
			break;
		case HCBOROUT_FALSE:
			CurrentElement = new TRCBORObject;
			Childs.push_back(CurrentElement);
			CurrentElement->ObjectType = HOBJTYPE_BOOL;
			*(uint64_t*)CurrentElement->buffervalue = 0;
			break;
		case HCBOROUT_NULL:
			CurrentElement = new TRCBORObject;
			Childs.push_back(CurrentElement);
			CurrentElement->ObjectType = HOBJTYPE_NULL;
			*(uint64_t*)CurrentElement->buffervalue = 0;
			break;
		case HCBOROUT_UNDEFINED:
			CurrentElement = new TRCBORObject;
			Childs.push_back(CurrentElement);
			CurrentElement->ObjectType = HOBJTYPE_UNDEFINED;
			*(uint64_t*)CurrentElement->buffervalue = 0;
			break;
		case HCBOROUT_BYTEARRAY:
			CurrentElement = new TRCBORObject;
			Childs.push_back(CurrentElement);
			CurrentElement->ObjectType = HOBJTYPE_BYTEARRAY;
			CurrentElement->bytearraysize = valuesize;
			CurrentElement->bytearray = (void*)(*(uintptr_t*)outvalue);
			break;
		case HCBOROUT_STRING_UTF8:
			CurrentElement = new TRCBORObject;
			Childs.push_back(CurrentElement);
			CurrentElement->ObjectType = HOBJTYPE_STRING_UTF8;
			CurrentElement->Utf8Value = std::string((char*)(*(uintptr_t*)outvalue), valuesize);
			break;
		case HCBOROUT_ITEMSARRAY_MARKER:
			CurrentElement = new TRCBORObject;
			Childs.push_back(CurrentElement);
			CurrentElement->ObjectType = HOBJTYPE_ITEMSARRAY;
			if (valuesize == UINT32_MAX)
				Parse(CurrentElement->Childs, -1);
			else
				Parse(CurrentElement->Childs, valuesize);
			break;
		case HCBOROUT_PAIRSARRAY_MARKER:
			CurrentElement = new TRCBORObject;
			Childs.push_back(CurrentElement);
			CurrentElement->ObjectType = HOBJTYPE_PAIRSARRAY;
			if (valuesize == UINT32_MAX)
				Parse(CurrentElement->Childs, -1);
			else
				Parse(CurrentElement->Childs, valuesize * 2);
			break;
		case HCBOROUT_ENDARRAY_MARKER:
			return;
			break;
		}

		elementindex++;
		if (waitcount > 0)
		{
			if (elementindex == waitcount)
				return;
		}
	}

}
