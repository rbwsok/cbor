#ifndef __H_CBOR_H_
#define __H_CBOR_H_

#include <stdint.h>
#include <vector>
#include <string>

// RFC7049 (CBOR)
// 3 старших бита - основной тип
enum TRHCBORMajorType
{
	HCBOR_POSITIVEINTEGER = 0,
	HCBOR_NEGATIVEINTEGER,
	HCBOR_BYTEARRAY,
	HCBOR_STRING_UTF8,
	HCBOR_ITEMSARRAY,
	HCBOR_PAIRSARRAY,
	HCBOR_TAGVALUE,
	HCBOR_FLOATSIMPLE
};

enum TRHCBOROutType
{
	HCBOROUT_INT = 0,
	HCBOROUT_INT64,
	HCBOROUT_FLOAT32,
	HCBOROUT_FLOAT64,
	HCBOROUT_TRUE,
	HCBOROUT_FALSE,
	HCBOROUT_NULL,
	HCBOROUT_UNDEFINED,
	HCBOROUT_BYTEARRAY,
	HCBOROUT_STRING_UTF8,
	HCBOROUT_ITEMSARRAY_MARKER,
	HCBOROUT_PAIRSARRAY_MARKER,
	HCBOROUT_ENDARRAY_MARKER
};

class TRCBORWriter
{
private:
	void* pointer;
	size_t fullsize; // полный размер выделенной памяти
	size_t usesize;  // размер используемой памяти

	const size_t blockmemsize; // размер блока выделяемой памяти

	void needmemory(size_t needsize);

	void writeCBORSizeValue32(uint8_t majortype, uint32_t value); // запись значения (или размера строки, буфера и т.д.)
public:
	TRCBORWriter();
	virtual ~TRCBORWriter();

	void Write8U(uint8_t value);
	void Write16U(uint16_t value);
	void Write32U(uint32_t value);
	void Write64U(uint64_t value);

	void Write8I(int8_t value);
	void Write16I(int16_t value);
	void Write32I(int32_t value);
	void Write64I(int64_t value);

	void Write32F(float value);
	void Write64F(double value);

	void WriteString(const std::string& value);

	void WriteBuffer(void* buffer, size_t sizebuffer);

	// запись информации в виде RFC7049 (CBOR)
	
	// запись 64-х битных значениях в строках, массивах и т.д. не реализована т.к. нах не нужна
	// float 16-bit тож не реализована
	// строки неопределенной длины тож не реализованы

	void WriteCBORValue(int32_t value);
	void WriteCBORValue(int64_t value);
	void WriteCBORByteArray(void* buffer, size_t sizebuffer);
	void WriteCBORString(const std::string& str);
	void WriteCBORString(const std::wstring& str);
	void WriteCBORItemsArrayMarker(uint32_t itemscount = UINT32_MAX); // иерархию массивов поддерживать руками!
	void WriteCBORPairsArrayMarker(uint32_t pairscount = UINT32_MAX); // корректное количество пар поддерживать руками!
	void WriteCBORFloat(float value);
	void WriteCBORFloat(double value);
	void WriteCBORBool(bool value);
	void WriteCBORNull(void);
	void WriteCBORUndefined(void);
	void WriteCBORStopArrayMarker(void); // признак конца массива элементов или пар

	void Clear(void);
	size_t Size(void) const;

	void* GetCurrentPointer(void) const;

	void* Pointer(void) const;

	void SetSize(size_t size);
};

class TRCBORReader
{
private:
	void* ptr;
	uint32_t sizebuffer;
	uint32_t position;

	uint32_t readCBORSizeValue32(uint8_t additionaltype);
	uint64_t readCBORSizeValue64(uint8_t additionaltype);

	uint32_t ReadUInt8(void);
	uint32_t ReadUInt16(void);
	uint32_t ReadUInt32(void);
	uint64_t ReadUInt64(void);
	float ReadFloat32(void);
	double ReadFloat64(void);

	void* GetCurrentPointer(void);
public:
	TRCBORReader();
	virtual ~TRCBORReader();

	void SetBuffer(void* ptr, size_t sizebuffer);
	void* GetBuffer(size_t& sizebuffer);
	bool ParseCBOR(TRHCBOROutType& valuetype, void* outvalue, size_t& valuesize); // все параметры выходные. буфер valueptr - 8 байт
};

enum TRHCBORObjectType
{
	HOBJTYPE_INT = 0,
	HOBJTYPE_INT64,
	HOBJTYPE_FLOAT32,
	HOBJTYPE_FLOAT64,
	HOBJTYPE_BOOL,
	HOBJTYPE_NULL,
	HOBJTYPE_UNDEFINED,
	HOBJTYPE_BYTEARRAY,
	HOBJTYPE_STRING_UTF8,
	HOBJTYPE_ITEMSARRAY,
	HOBJTYPE_PAIRSARRAY,
};

class TRCBORObject
{
	friend class TRCBORObjectModel;
private:
	std::vector<TRCBORObject*> Childs;

	TRHCBORObjectType ObjectType;

	uint8_t buffervalue[8];
	std::string Utf8Value;
	void* bytearray;
	size_t bytearraysize;
public:
	TRCBORObject();
	virtual ~TRCBORObject();	

	size_t GetChildsCount(void);
	TRCBORObject* GetChild(size_t index);

	std::string& AsString(void);
	int32_t AsInt32(void);
	int64_t AsInt64(void);
	float AsFloat(void);
	double AsDouble(void);
	bool AsBool(void);

	bool GetByteArray(void **ptr, size_t &size);

	TRHCBORObjectType GetType(void);
};

class TRCBORObjectModel
{
private:
	TRCBORReader reader;
	std::vector<TRCBORObject*> Childs;
	
	void Parse(std::vector<TRCBORObject*> &Childs, int32_t waitcount);
public:
	TRCBORObjectModel();
	virtual ~TRCBORObjectModel();

	void SetBuffer(void* ptr, size_t sizebuffer);

	void Parse(void);

	size_t GetChildsCount(void);
	TRCBORObject* GetChild(size_t index);
};


#endif
