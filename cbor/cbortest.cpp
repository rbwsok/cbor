#include <gtest/gtest.h>

#include "stdafx.h"

#include "cbor.h"

TRCBORWriter writer;
TRCBORReader reader;

int main()
{
	::testing::InitGoogleTest();
	RUN_ALL_TESTS();

	system("pause");

	return 0;
}

//////////////////////////////////////////////////////////////////////////////
// Test TRCBORWriter

TEST(TRCBORWriter, Size0)
{
	ASSERT_EQ(writer.Size(), 0);
}

TEST(TRCBORWriter, Write8U)
{
	void *p = writer.GetCurrentPointer();
	uint32_t s = writer.Size();

	writer.Write8U(0x88);
	uint8_t value = *(uint8_t*)p;

	ASSERT_EQ((uint8_t*)writer.GetCurrentPointer() - (uint8_t*)p, 1);
	ASSERT_EQ(writer.Size() - s, 1);
	ASSERT_EQ(value, 0x88);
}

TEST(TRCBORWriter, Write16U)
{
	void *p = writer.GetCurrentPointer();
	uint32_t s = writer.Size();

	writer.Write16U(0xFA73);

	uint16_t value = *(uint16_t*)p;

	ASSERT_EQ((uint8_t*)writer.GetCurrentPointer() - (uint8_t*)p, 2);
	ASSERT_EQ(writer.Size() - s, 2);
	ASSERT_EQ(value, 0xFA73);
}

TEST(TRCBORWriter, Write32U)
{
	void *p = writer.GetCurrentPointer();
	uint32_t s = writer.Size();

	writer.Write32U(0xE4589A73);

	uint32_t value = *(uint32_t*)p;

	ASSERT_EQ((uint8_t*)writer.GetCurrentPointer() - (uint8_t*)p, 4);
	ASSERT_EQ(writer.Size() - s, 4);
	ASSERT_EQ(value, 0xE4589A73);
}

TEST(TRCBORWriter, Write64U)
{
	void *p = writer.GetCurrentPointer();
	uint32_t s = writer.Size();

	writer.Write64U(0xE4589A73A9E4E721);

	uint64_t value = *(uint64_t*)p;

	ASSERT_EQ((uint8_t*)writer.GetCurrentPointer() - (uint8_t*)p, 8);
	ASSERT_EQ(writer.Size() - s, 8);
	ASSERT_EQ(value, 0xE4589A73A9E4E721);
}

TEST(TRCBORWriter, Size15)
{
	ASSERT_EQ(writer.Size(), 15);
}

TEST(TRCBORWriter, Write8I)
{
	void *p = writer.GetCurrentPointer();
	uint32_t s = writer.Size();

	writer.Write8U(-76);

	int8_t value = *(int8_t*)p;

	ASSERT_EQ((uint8_t*)writer.GetCurrentPointer() - (uint8_t*)p, 1);
	ASSERT_EQ(writer.Size() - s, 1);
	ASSERT_EQ(value, -76);
}

TEST(TRCBORWriter, Write16I)
{
	void *p = writer.GetCurrentPointer();
	uint32_t s = writer.Size();

	writer.Write16I(-31789);

	int16_t value = *(int16_t*)p;

	ASSERT_EQ((uint8_t*)writer.GetCurrentPointer() - (uint8_t*)p, 2);
	ASSERT_EQ(writer.Size() - s, 2);
	ASSERT_EQ(value, -31789);
}

TEST(TRCBORWriter, Write32I)
{
	void *p = writer.GetCurrentPointer();
	uint32_t s = writer.Size();

	writer.Write32I(-1948179606);

	uint32_t value = *(uint32_t*)p;

	ASSERT_EQ((uint8_t*)writer.GetCurrentPointer() - (uint8_t*)p, 4);
	ASSERT_EQ(writer.Size() - s, 4);
	ASSERT_EQ(value, -1948179606);
}

TEST(TRCBORWriter, Write64I)
{
	void *p = writer.GetCurrentPointer();
	uint32_t s = writer.Size();

	writer.Write64I(-8750173327307698552);

	uint64_t value = *(uint64_t*)p;

	ASSERT_EQ((uint8_t*)writer.GetCurrentPointer() - (uint8_t*)p, 8);
	ASSERT_EQ(writer.Size() - s, 8);
	ASSERT_EQ(value, -8750173327307698552);
}

TEST(TRCBORWriter, Write32F)
{
	void *p = writer.GetCurrentPointer();
	uint32_t s = writer.Size();

	writer.Write32F(-0.44567342f);

	float value = *(float*)p;

	ASSERT_EQ((uint8_t*)writer.GetCurrentPointer() - (uint8_t*)p, 4);
	ASSERT_EQ(writer.Size() - s, 4);
	ASSERT_EQ(value, -0.44567342f);
}

TEST(TRCBORWriter, Write64F)
{
	void *p = writer.GetCurrentPointer();
	uint32_t s = writer.Size();

	writer.Write64F(-0.44567342);

	double value = *(double*)p;

	ASSERT_EQ((uint8_t*)writer.GetCurrentPointer() - (uint8_t*)p, 8);
	ASSERT_EQ(writer.Size() - s, 8);
	ASSERT_EQ(value, -0.44567342);
}

TEST(TRCBORWriter, Size42)
{
	ASSERT_EQ(writer.Size(), 42);
}

TEST(TRCBORWriter, WriteString)
{
	void *p = writer.GetCurrentPointer();
	uint32_t s = writer.Size();

	std::string test = "I was here";
	writer.WriteString(test);

	uint8_t* value = (uint8_t*)p;

	ASSERT_EQ((uint8_t*)writer.GetCurrentPointer() - (uint8_t*)p, test.size());
	ASSERT_EQ(writer.Size() - s, test.size());
	ASSERT_TRUE(0 == std::memcmp(value, test.c_str(), test.size()));
}

TEST(TRCBORWriter, WriteBuffer)
{
	void *p = writer.GetCurrentPointer();
	uint32_t s = writer.Size();

	uint8_t buf[] = { 0, 1, 2, 3, 4, 5, 6 };
	
	writer.WriteBuffer(buf, sizeof(buf));

	uint8_t* value = (uint8_t*)p;

	ASSERT_EQ((uint8_t*)writer.GetCurrentPointer() - (uint8_t*)p, sizeof(buf));
	ASSERT_EQ(writer.Size() - s, sizeof(buf));
	ASSERT_TRUE(0 == std::memcmp(value, buf, sizeof(buf)));
}

TEST(TRCBORWriter, Clear)
{
	writer.Clear();
	ASSERT_EQ(writer.GetCurrentPointer(), writer.Pointer());
	ASSERT_EQ(writer.Size(), 0);
}

TEST(TRCBORWriter, WriteCBORValue32)
{
	void *p = writer.GetCurrentPointer();
	uint32_t s = writer.Size();

	writer.WriteCBORValue(5);

	uint8_t value = *(uint8_t*)p;

	ASSERT_EQ((uint8_t*)writer.GetCurrentPointer() - (uint8_t*)p, 1);
	ASSERT_EQ(writer.Size() - s, 1);
	ASSERT_EQ(value, 5);

	p = writer.GetCurrentPointer();
	s = writer.Size();

	writer.WriteCBORValue(0x70);

	uint16_t value16 = *(uint16_t*)p;

	ASSERT_EQ((uint8_t*)writer.GetCurrentPointer() - (uint8_t*)p, 2);
	ASSERT_EQ(writer.Size() - s, 2);
	ASSERT_EQ(value16, 0x7018); // 0x70 - ÷èñëî 0x18 - òèï uint8_t

	p = writer.GetCurrentPointer();
	s = writer.Size();

	writer.WriteCBORValue(0x7090);

	uint32_t value32 = ((uint8_t*)p)[0] | ((uint8_t*)p)[1] << 8 | ((uint8_t*)p)[2] << 16;

	ASSERT_EQ((uint8_t*)writer.GetCurrentPointer() - (uint8_t*)p, 3);
	ASSERT_EQ(writer.Size() - s, 3);
	ASSERT_EQ(value32, 0x709019); // 0x9070 - ÷èñëî 0x19 - òèï uint16_t

	p = writer.GetCurrentPointer();
	s = writer.Size();

	writer.WriteCBORValue(-4577834);

	uint64_t value64 = *(uint32_t*)p | (uint64_t)(((uint8_t*)p)[4]) << 32;

	ASSERT_EQ((uint8_t*)writer.GetCurrentPointer() - (uint8_t*)p, 5);
	ASSERT_EQ(writer.Size() - s, 5);
	ASSERT_EQ(value64, 0x45DA293a); // 0x45DA29 - ÷èñëî 0x3a - òèï uint16_t È îòðèöàòåëüíûé çíàê
}

TEST(TRCBORWriter, WriteCBORValue64)
{
	void *p = writer.GetCurrentPointer();
	uint32_t s = writer.Size();

	writer.WriteCBORValue(7);

	uint8_t value = *(uint8_t*)p;

	ASSERT_EQ((uint8_t*)writer.GetCurrentPointer() - (uint8_t*)p, 1);
	ASSERT_EQ(writer.Size() - s, 1);
	ASSERT_EQ(value, 7);

	p = writer.GetCurrentPointer();
	s = writer.Size();

	writer.WriteCBORValue(0x55);

	uint16_t value16 = *(uint16_t*)p;

	ASSERT_EQ((uint8_t*)writer.GetCurrentPointer() - (uint8_t*)p, 2);
	ASSERT_EQ(writer.Size() - s, 2);
	ASSERT_EQ(value16, 0x5518); // 0x55 - ÷èñëî 0x18 - òèï uint8_t

	p = writer.GetCurrentPointer();
	s = writer.Size();

	writer.WriteCBORValue(0x7495);

	uint32_t value32 = ((uint8_t*)p)[0] | ((uint8_t*)p)[1] << 8 | ((uint8_t*)p)[2] << 16;

	ASSERT_EQ((uint8_t*)writer.GetCurrentPointer() - (uint8_t*)p, 3);
	ASSERT_EQ(writer.Size() - s, 3);
	ASSERT_EQ(value32, 0x749519); // 0x9574 - ÷èñëî 0x19 - òèï uint16_t

	p = writer.GetCurrentPointer();
	s = writer.Size();

	writer.WriteCBORValue(-4577834);

	uint64_t value64 = *(uint32_t*)p | (uint64_t)(((uint8_t*)p)[4]) << 32;

	ASSERT_EQ((uint8_t*)writer.GetCurrentPointer() - (uint8_t*)p, 5);
	ASSERT_EQ(writer.Size() - s, 5);
	ASSERT_EQ(value64, 0x45DA293a); // 0x45DA29 - ÷èñëî 0x3a - òèï uint16_t È îòðèöàòåëüíûé çíàê

	p = writer.GetCurrentPointer();
	s = writer.Size();

	writer.WriteCBORValue(0x1122334455667788);

	uint8_t valuesign = *(uint8_t*)p;
	value64 = *(uint64_t*)((uint8_t*)p + 1);

	ASSERT_EQ((uint8_t*)writer.GetCurrentPointer() - (uint8_t*)p, 9);
	ASSERT_EQ(writer.Size() - s, 9);
	ASSERT_EQ(valuesign, 0x1b);
	ASSERT_EQ(value64, 0x1122334455667788);
}

TEST(TRCBORWriter, WriteCBORByteArray)
{
	void *p = writer.GetCurrentPointer();
	uint32_t s = writer.Size();

	uint8_t buf[] = { 0, 1, 2, 3, 4, 5, 6 };

	writer.WriteCBORByteArray(buf, sizeof(buf));

	uint8_t valuesign = *(uint8_t*)p;
	uint8_t* value = (uint8_t*)p + 1;

	ASSERT_EQ((uint8_t*)writer.GetCurrentPointer() - (uint8_t*)p, sizeof(buf) + 1);
	ASSERT_EQ(writer.Size() - s, sizeof(buf) + 1);
	ASSERT_EQ(valuesign, 0x47);
	ASSERT_TRUE(0 == std::memcmp(value, buf, sizeof(buf)));

	p = writer.GetCurrentPointer();
	s = writer.Size();

	uint8_t buf2[] = { 0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16, 17, 18, 19, 20,
					  19, 18, 17, 16, 15, 14, 13, 12, 11, 10, 9, 8, 7, 6, 5, 4, 3, 2, 1, 0 };

	writer.WriteCBORByteArray(buf2, sizeof(buf2));

	uint16_t valuesign16 = *(uint16_t*)p;
	value = (uint8_t*)p + 2;

	ASSERT_EQ((uint8_t*)writer.GetCurrentPointer() - (uint8_t*)p, sizeof(buf2) + 2);
	ASSERT_EQ(writer.Size() - s, sizeof(buf2) + 2);
	ASSERT_EQ(valuesign16, 0x2958);
	ASSERT_TRUE(0 == std::memcmp(value, buf2, sizeof(buf2)));

}

TEST(TRCBORWriter, WriteCBORString)
{
	void *p = writer.GetCurrentPointer();
	uint32_t s = writer.Size();

	std::string test = "I was here";
	writer.WriteCBORString(test);

	uint8_t valuesign = *(uint8_t*)p;
	uint8_t* value = (uint8_t*)p + 1;

	ASSERT_EQ((uint8_t*)writer.GetCurrentPointer() - (uint8_t*)p, test.size() + 1);
	ASSERT_EQ(writer.Size() - s, test.size() + 1);
	ASSERT_EQ(valuesign, 0x6a);
	ASSERT_TRUE(0 == std::memcmp(value, test.c_str(), test.size()));
}

TEST(TRCBORWriter, WriteCBORWString)
{
	void *p = writer.GetCurrentPointer();
	uint32_t s = writer.Size();

	std::wstring test = L"Òåñòîâàÿ ñòðîêà";
	writer.WriteCBORString(test);

	uint16_t valuesign = *(uint16_t*)p;
	uint8_t* value = (uint8_t*)p + 2;
	uint8_t eqsample[] = { 0xD0, 0xA2, 0xD0, 0xB5, 0xD1, 0x81, 0xD1, 0x82,
						   0xD0, 0xBE, 0xD0, 0xB2, 0xD0, 0xB0, 0xD1, 0x8F,
						   0x20, 0xD1, 0x81, 0xD1, 0x82, 0xD1, 0x80, 0xD0,
						   0xBE, 0xD0, 0xBA, 0xD0, 0xB0 };

	ASSERT_EQ((uint8_t*)writer.GetCurrentPointer() - (uint8_t*)p, sizeof(eqsample) + 2);
	ASSERT_EQ(writer.Size() - s, sizeof(eqsample) + 2);
	ASSERT_EQ(valuesign, 0x1D78);
	ASSERT_TRUE(0 == std::memcmp(value, eqsample, sizeof(eqsample)));
}

TEST(TRCBORWriter, WriteCBORFloat32)
{
	void *p = writer.GetCurrentPointer();
	uint32_t s = writer.Size();

	writer.WriteCBORFloat(104.5623344f);

	uint8_t valuesign = *(uint8_t*)p;
	float value32 = *(float*)((uint8_t*)p + 1);

	ASSERT_EQ((uint8_t*)writer.GetCurrentPointer() - (uint8_t*)p, 5);
	ASSERT_EQ(writer.Size() - s, 5);
	ASSERT_EQ(valuesign, 0xFA);
	ASSERT_EQ(value32, 104.5623344f);
}

TEST(TRCBORWriter, WriteCBORFloat64)
{
	void *p = writer.GetCurrentPointer();
	uint32_t s = writer.Size();

	writer.WriteCBORFloat(10445645624.56423423423344);

	uint8_t valuesign = *(uint8_t*)p;
	double value64 = *(double*)((uint8_t*)p + 1);

	ASSERT_EQ((uint8_t*)writer.GetCurrentPointer() - (uint8_t*)p, 9);
	ASSERT_EQ(writer.Size() - s, 9);
	ASSERT_EQ(valuesign, 0xFB);
	ASSERT_EQ(value64, 10445645624.56423423423344);
}

TEST(TRCBORWriter, WriteCBORBool)
{
	void *p = writer.GetCurrentPointer();
	uint32_t s = writer.Size();

	writer.WriteCBORBool(false);

	uint8_t value = *(uint8_t*)p;

	ASSERT_EQ((uint8_t*)writer.GetCurrentPointer() - (uint8_t*)p, 1);
	ASSERT_EQ(writer.Size() - s, 1);
	ASSERT_EQ(value, 0xf4);

	p = writer.GetCurrentPointer();
	s = writer.Size();

	writer.WriteCBORBool(true);

	value = *(uint8_t*)p;

	ASSERT_EQ((uint8_t*)writer.GetCurrentPointer() - (uint8_t*)p, 1);
	ASSERT_EQ(writer.Size() - s, 1);
	ASSERT_EQ(value, 0xf5);

}

TEST(TRCBORWriter, WriteCBORNull)
{
	void *p = writer.GetCurrentPointer();
	uint32_t s = writer.Size();

	writer.WriteCBORNull();

	uint8_t value = *(uint8_t*)p;

	ASSERT_EQ((uint8_t*)writer.GetCurrentPointer() - (uint8_t*)p, 1);
	ASSERT_EQ(writer.Size() - s, 1);
	ASSERT_EQ(value, 0xf6);
}

TEST(TRCBORWriter, WriteCBORUndefined)
{
	void *p = writer.GetCurrentPointer();
	uint32_t s = writer.Size();

	writer.WriteCBORUndefined();

	uint8_t value = *(uint8_t*)p;

	ASSERT_EQ((uint8_t*)writer.GetCurrentPointer() - (uint8_t*)p, 1);
	ASSERT_EQ(writer.Size() - s, 1);
	ASSERT_EQ(value, 0xf7);
}

TEST(TRCBORWriter, WriteCBORItemsArrayMarker)
{
	void *p = writer.GetCurrentPointer();
	uint32_t s = writer.Size();

	writer.WriteCBORItemsArrayMarker(10);

	uint8_t value = *(uint8_t*)p;

	ASSERT_EQ((uint8_t*)writer.GetCurrentPointer() - (uint8_t*)p, 1);
	ASSERT_EQ(writer.Size() - s, 1);
	ASSERT_EQ(value, 0x8a);
}

TEST(TRCBORWriter, WriteCBORPairsArrayMarker)
{
	void *p = writer.GetCurrentPointer();
	uint32_t s = writer.Size();

	writer.WriteCBORPairsArrayMarker(4);

	uint8_t value = *(uint8_t*)p;

	ASSERT_EQ((uint8_t*)writer.GetCurrentPointer() - (uint8_t*)p, 1);
	ASSERT_EQ(writer.Size() - s, 1);
	ASSERT_EQ(value, 0xA4);
}

TEST(TRCBORWriter, WriteCBORStopArrayMarker)
{
	void *p = writer.GetCurrentPointer();
	uint32_t s = writer.Size();

	writer.WriteCBORStopArrayMarker();

	uint8_t value = *(uint8_t*)p;

	ASSERT_EQ((uint8_t*)writer.GetCurrentPointer() - (uint8_t*)p, 1);
	ASSERT_EQ(writer.Size() - s, 1);
	ASSERT_EQ(value, 0xff);
}

//////////////////////////////////////////////////////////////////////////////
// Test TRCBORReader

TEST(TRCBORReader, ReadStream)
{
/*
	create JSON stream
	{
		“Name”: “Adolf”
		“Age” : 44
		“Data” :
		{
			“Subdata1”: “kikoz1”,
			“Subdata2” : “kikoz2”,
			“Subdata3” : 88
		}
		“Array”: [10, 20, 30, false, true]
	}
*/
	writer.Clear();

	writer.WriteCBORPairsArrayMarker(4); // 4 - pairs count
		// first pair
		writer.WriteCBORString("Name");
		writer.WriteCBORString("Adolf");
		// second pair
		writer.WriteCBORString("Age");
		writer.WriteCBORValue(44);
		// third pair
		writer.WriteCBORString("Data");
		writer.WriteCBORPairsArrayMarker(3); // 3 - pairs count
			// first pair
			writer.WriteCBORString("Subdata1");
			writer.WriteCBORString("kikoz1");
			// second pair
			writer.WriteCBORString("Subdata2");
			writer.WriteCBORString("kikoz2");
			// third pair
			writer.WriteCBORString("Subdata3");
			writer.WriteCBORValue(88);
		
		writer.WriteCBORString("Array");
		writer.WriteCBORItemsArrayMarker(); // item size - undefined
			writer.WriteCBORValue(10);
			writer.WriteCBORValue(20);
			writer.WriteCBORValue(30);
			writer.WriteCBORBool(false);
			writer.WriteCBORBool(true);
		writer.WriteCBORStopArrayMarker();

	// read stream
	TRHCBOROutType valuetype;
	uint64_t outvalue;
	uint32_t valuesize;

	int nestinglevel = 0;
	int pairnumber = 0, subpairnumber = 0;
	int elementnumber = 0;

	std::string utf8str;

	reader.SetBuffer(writer.Pointer(), writer.Size());
	while (reader.ParseCBOR(valuetype, &outvalue, valuesize) == true)
	{
		switch (valuetype)
		{
		case HCBOROUT_INT:
			if (nestinglevel == 1)
			{
				if (pairnumber == 1)
				{
					if (elementnumber == 1)
					{
						ASSERT_EQ(outvalue & 0xffffffff, 44);
						elementnumber = 0;
						pairnumber++;
					}
				}
			}
			else
			if (nestinglevel == 2)
			{
				if (pairnumber == 2 && subpairnumber == 3)
				{
					if (elementnumber == 1)
					{
						ASSERT_EQ(outvalue & 0xffffffff, 88);
						elementnumber = 0;
						subpairnumber++;
						pairnumber++;
					}
				}
			}
			break;
		case HCBOROUT_INT64:
			break;
		case HCBOROUT_FLOAT32:
			break;
		case HCBOROUT_FLOAT64:
			break;
		case HCBOROUT_TRUE:
			break;
		case HCBOROUT_FALSE:
			break;
		case HCBOROUT_NULL:
			break;
		case HCBOROUT_UNDEFINED:
			break;
		case HCBOROUT_BYTEARRAY:
			break;
		case HCBOROUT_STRING_UTF8:
			utf8str = std::string((char*)(outvalue & 0xffffffff), valuesize);
			if (nestinglevel == 1)
			{
				if (pairnumber == 0)
				{
					if (elementnumber == 0)
					{
						ASSERT_STREQ(utf8str.c_str(), "Name");
						elementnumber++;
					}
					else
					if (elementnumber == 1)
					{
						ASSERT_STREQ(utf8str.c_str(), "Adolf");
						elementnumber = 0;
						pairnumber++;
					}
				}
				else
				if (pairnumber == 1)
				{
					if (elementnumber == 0)
					{
						ASSERT_STREQ(utf8str.c_str(), "Age");
						elementnumber++;
					}
				}
				else
				if (pairnumber == 2)
				{
					if (elementnumber == 0)
					{
						ASSERT_STREQ(utf8str.c_str(), "Data");
						elementnumber++;
					}
				}
				if (pairnumber == 3)
				{
					if (elementnumber == 0)
					{
						ASSERT_STREQ(utf8str.c_str(), "Array");
						elementnumber++;
					}
				}
			}

			if (nestinglevel == 2)
			{
				if (pairnumber == 2 && subpairnumber == 0)
				{
					if (elementnumber == 0)
					{
						ASSERT_STREQ(utf8str.c_str(), "Subdata1");
						elementnumber++;
					}
					else
					if (elementnumber == 1)
					{
						ASSERT_STREQ(utf8str.c_str(), "kikoz1");
						elementnumber = 0;
						subpairnumber++;
					}
				}
				else
				if (pairnumber == 2 && subpairnumber == 1)
				{
					if (elementnumber == 0)
					{
						ASSERT_STREQ(utf8str.c_str(), "Subdata2");
						elementnumber++;
					}
					else
					if (elementnumber == 1)
					{
						ASSERT_STREQ(utf8str.c_str(), "kikoz2");
						elementnumber = 0;
						subpairnumber++;
					}
				}
				else
				if (pairnumber == 2 && subpairnumber == 2)
				{
					if (elementnumber == 0)
					{
						ASSERT_STREQ(utf8str.c_str(), "Subdata3");
						elementnumber++;
					}
				}
			}
			break;
		case HCBOROUT_ITEMSARRAY_MARKER:
			nestinglevel++;
			break;
		case HCBOROUT_PAIRSARRAY_MARKER:
			if (pairnumber == 0)
				nestinglevel++;
			else
			if (pairnumber == 2 && elementnumber == 1)
			{
				nestinglevel++;
				elementnumber = 0;
			}
			break;
		case HCBOROUT_ENDARRAY_MARKER:
			nestinglevel--;
			break;
		}
	}
}


//////////////////////////////////////////////////////////////////////////////
// Test TRCBORObjectModel

TEST(TRCBORObjectModel, Strings)
{
	writer.Clear();

	char* strarr[] = { "Name", "Adolf", "Age", "Data", "Subdata1", "kikoz1", "Subdata2", "kikoz2", "Subdata3", "Array" };
	
	for (int i = 0; i < sizeof(strarr)/sizeof(char*); ++i)	
	{ 
		writer.WriteCBORString(strarr[i]);
	}
	
	TRCBORObjectModel CBOR;
	CBOR.SetBuffer(writer.Pointer(), writer.Size());
	CBOR.Parse();

	TRCBORObject* Object;

	size_t count = CBOR.GetChildsCount();
	ASSERT_EQ(count, sizeof(strarr)/sizeof(char*));

	for (size_t i = 0; i < count; ++i)
	{
		Object = CBOR.GetChild(i);

		ASSERT_EQ(strarr[i], Object->AsString());
	}
	
}

TEST(TRCBORObjectModel, Integers)
{
	writer.Clear();

	int values[] = { -10, -20, -30, 10, 20, 30 };

	for (int i = 0; i < sizeof(values)/sizeof(int); ++i)
	{
		writer.WriteCBORValue(values[i]);
	}

	TRCBORObjectModel CBOR;
	CBOR.SetBuffer(writer.Pointer(), writer.Size());
	CBOR.Parse();

	TRCBORObject* Object;

	size_t count = CBOR.GetChildsCount();
	ASSERT_EQ(count, sizeof(values) / sizeof(int));

	for (size_t i = 0; i < count; ++i)
	{
		Object = CBOR.GetChild(i);

		int32_t v = Object->AsInt32();

		ASSERT_EQ(values[i], v);
	}

}

TEST(TRCBORObjectModel, Integers64)
{
	writer.Clear();

	int64_t values[] = { -1088, -2560, -343630, 13540, 435620, 45630 };

	for (int i = 0; i < sizeof(values) / sizeof(int64_t); ++i)
	{
		writer.WriteCBORValue(values[i]);
	}

	TRCBORObjectModel CBOR;
	CBOR.SetBuffer(writer.Pointer(), writer.Size());
	CBOR.Parse();

	TRCBORObject* Object;

	size_t count = CBOR.GetChildsCount();
	ASSERT_EQ(count, sizeof(values) / sizeof(int64_t));

	for (size_t i = 0; i < count; ++i)
	{
		Object = CBOR.GetChild(i);

		int64_t v = Object->AsInt64();

		ASSERT_EQ(values[i], v);
	}

}

TEST(TRCBORObjectModel, Floats)
{
	writer.Clear();

	float values[] = { -10.6f, -20.5f, 0.000345f, 10.444556f, 20.243345f, 30.6677f };

	for (int i = 0; i < sizeof(values) / sizeof(int); ++i)
	{
		writer.WriteCBORFloat(values[i]);
	}

	TRCBORObjectModel CBOR;
	CBOR.SetBuffer(writer.Pointer(), writer.Size());
	CBOR.Parse();

	TRCBORObject* Object;

	size_t count = CBOR.GetChildsCount();
	ASSERT_EQ(count, sizeof(values) / sizeof(int));

	for (size_t i = 0; i < count; ++i)
	{
		Object = CBOR.GetChild(i);

		float v = Object->AsFloat();

		ASSERT_EQ(values[i], v);
	}

}

TEST(TRCBORObjectModel, ItemsArray)
{
	writer.Clear();

	writer.WriteCBORValue(1);
	writer.WriteCBORValue(2);
	writer.WriteCBORItemsArrayMarker(4);
		writer.WriteCBORValue(31);
		writer.WriteCBORValue(32);
		writer.WriteCBORValue(33);
		writer.WriteCBORValue(34);
	writer.WriteCBORValue(4);

	TRCBORObjectModel CBOR;
	CBOR.SetBuffer(writer.Pointer(), writer.Size());
	CBOR.Parse();

	TRCBORObject *Object, *EmbObject;

	size_t count = CBOR.GetChildsCount();
	ASSERT_EQ(count, 4);

	for (size_t i = 0; i < count; ++i)
	{
		Object = CBOR.GetChild(i);
		if (Object->GetType() == HOBJTYPE_INT)
		{
			int32_t v = Object->AsInt32();
			switch (i)
			{
			case 0:
				ASSERT_EQ(v, 1);
				break;
			case 1:
				ASSERT_EQ(v, 2);
				break;
			case 3:
				ASSERT_EQ(v, 4);
				break;
			}
			
		}
		else
		if (Object->GetType() == HOBJTYPE_ITEMSARRAY)
		{
			for (size_t j = 0; j < Object->GetChildsCount(); ++j)
			{
				EmbObject = Object->GetChild(j);
				int32_t v = EmbObject->AsInt32();
				ASSERT_EQ(v, 31 + j);
			}
		}
	}
}

TEST(TRCBORObjectModel, PairsArray)
{
	writer.Clear();

	writer.WriteCBORValue(-5);
	writer.WriteCBORValue(26);
	writer.WriteCBORPairsArrayMarker(4);
		writer.WriteCBORString("item");
		writer.WriteCBORValue(31);
		writer.WriteCBORString("item");
		writer.WriteCBORValue(32);
		writer.WriteCBORString("item");
		writer.WriteCBORValue(33);
		writer.WriteCBORString("item");
		writer.WriteCBORValue(34);
	writer.WriteCBORValue(477);

	TRCBORObjectModel CBOR;
	CBOR.SetBuffer(writer.Pointer(), writer.Size());
	CBOR.Parse();

	TRCBORObject *Object, *EmbObject;

	size_t count = CBOR.GetChildsCount();
	ASSERT_EQ(count, 4);

	for (size_t i = 0; i < count; ++i)
	{
		Object = CBOR.GetChild(i);
		if (Object->GetType() == HOBJTYPE_INT)
		{
			int32_t v = Object->AsInt32();
			switch (i)
			{
			case 0:
				ASSERT_EQ(v, -5);
				break;
			case 1:
				ASSERT_EQ(v, 26);
				break;
			case 3:
				ASSERT_EQ(v, 477);
				break;
			}

		}
		else
		if (Object->GetType() == HOBJTYPE_PAIRSARRAY)
		{
			int a = 0;
			for (size_t j = 0; j < Object->GetChildsCount(); ++j)
			{
				EmbObject = Object->GetChild(j);
				if (EmbObject->GetType() == HOBJTYPE_INT)
				{
					int32_t v = EmbObject->AsInt32();
					ASSERT_EQ(v, 31 + a);
					a++;
				}
				else
				if (EmbObject->GetType() == HOBJTYPE_STRING_UTF8)
				{
					std::string v = EmbObject->AsString();
					ASSERT_EQ(v, "item");
				}
			}
		}
	}
}

TEST(TRCBORObjectModel, ByteArray)
{
	writer.Clear();

	char buff[] = { 00, 11, 22, 33, 44, 55, 66, 77 };

	writer.WriteCBORByteArray(buff, sizeof(buff));

	TRCBORObjectModel CBOR;
	CBOR.SetBuffer(writer.Pointer(), writer.Size());
	CBOR.Parse();

	TRCBORObject* Object;

	size_t count = CBOR.GetChildsCount();

	Object = CBOR.GetChild(0);
	if (Object->GetType() == HOBJTYPE_BYTEARRAY)
	{
		void* bytearrayptr;
		size_t bytearraysize;
		Object->GetByteArray(&bytearrayptr, bytearraysize);

		ASSERT_TRUE(0 == std::memcmp(bytearrayptr, buff, bytearraysize));
	}

}