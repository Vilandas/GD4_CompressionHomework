
class GameObject;

inline uint32_t ConvertToFixed(float inNumber, float inMin, float inPrecision)
{
	return inNumber != 0
		? static_cast<int> ((inNumber - inMin) / inPrecision)
		: 0;
}

inline float ConvertFromFixed(uint32_t inNumber, float inMin, float inPrecision)
{
	return inNumber != 0
		? static_cast<float>(inNumber) * inPrecision + inMin
		: 0;
}


class OutputMemoryBitStream
{
public:

	OutputMemoryBitStream() :
		mBitHead(0),
		mBuffer(nullptr)
	{
		ReallocBuffer(1500 * 8);
	}

	~OutputMemoryBitStream() { std::free(mBuffer); }

	void		WriteBits(uint8_t inData, uint32_t inBitCount);
	void		WriteBits(const void* inData, uint32_t inBitCount);
	void Write(const Vector3& inVector);
	void Write(const char* inData);


	const 	char*	GetBufferPtr()		const { return mBuffer; }
	uint32_t		GetBitLength()		const { return mBitHead; }
	uint32_t		GetByteLength()		const { return (mBitHead + 7) >> 3; }

	void WriteBytes(const void* inData, uint32_t inByteCount) { WriteBits(inData, inByteCount << 3); }

	
	void Write( uint32_t inData, uint32_t inBitCount = 32 )	{ WriteBits( &inData, inBitCount ); }
	void Write( int inData, uint32_t inBitCount = 32 )		{ WriteBits( &inData, inBitCount ); }
	void Write( float inData )								{ WriteBits( &inData, 32 ); }

	void Write( uint16_t inData, uint32_t inBitCount = 16 )	{ WriteBits( &inData, inBitCount ); }
	void Write( int16_t inData, uint32_t inBitCount = 16 )	{ WriteBits( &inData, inBitCount ); }

	void Write( uint8_t inData, uint32_t inBitCount = 8 )	{ WriteBits( &inData, inBitCount ); }
	

	template< typename T >
	void Write(T inData, uint32_t inBitCount = sizeof(T) * 8)
	{
		static_assert(std::is_arithmetic< T >::value ||
			std::is_enum< T >::value,
			"Generic Write only supports primitive data types");
		WriteBits(&inData, inBitCount);
	}

	void 		Write(bool inData) { WriteBits(&inData, 1); }

	void WritePos(const Vector3& inVector);
	void		Write(const Quaternion& inQuat);

	template< typename T >
	void Write(const std::vector< T >& inVector)
	{
		uint32_t elementCount = inVector.size();
		Write(elementCount);
		for (const T& element : inVector)
		{
			Write(element);
		}
	}

	template< typename T >
	void Write(const std::vector<T>& inVector, uint32_t inBitCountPerElement, uint32_t maxVectorSizeInBits = 32)
	{
		uint32_t elementCount = inVector.size();
		Write(elementCount, maxVectorSizeInBits);
		Write(inBitCountPerElement, 6);	//should never be more than 6 bits of size
		for (const T& element : inVector)
		{
			Write(element, inBitCountPerElement);
		}
	}

	void Write(const std::string& inString)
	{
		uint32_t elementCount = static_cast<uint32_t>(inString.size());
		Write(elementCount);
		for (const auto& element : inString)
		{
			Write(element);
		}
	}

private:
	void		ReallocBuffer(uint32_t inNewBitCapacity);

	char*		mBuffer;
	uint32_t	mBitHead;
	uint32_t	mBitCapacity;
};

class InputMemoryBitStream
{
public:

	InputMemoryBitStream(char* inBuffer, uint32_t inBitCount) :
		mBuffer(inBuffer),
		mBitCapacity(inBitCount),
		mBitHead(0),
		mIsBufferOwner(false) {}

	InputMemoryBitStream(const InputMemoryBitStream& inOther) :
		mBitCapacity(inOther.mBitCapacity),
		mBitHead(inOther.mBitHead),
		mIsBufferOwner(true)
	{
		//allocate buffer of right size
		int byteCount = mBitCapacity / 8;
		mBuffer = static_cast<char*>(malloc(byteCount));
		//copy
		memcpy(mBuffer, inOther.mBuffer, byteCount);
	}

	~InputMemoryBitStream() { if (mIsBufferOwner) { free(mBuffer); }; }

	const 	char*	GetBufferPtr()		const { return mBuffer; }
	uint32_t	GetRemainingBitCount() 	const { return mBitCapacity - mBitHead; }

	void		ReadBits(uint8_t& outData, uint32_t inBitCount);
	void		ReadBits(void* outData, uint32_t inBitCount);

	void		ReadBytes(void* outData, uint32_t inByteCount) { ReadBits(outData, inByteCount << 3); }

	template< typename T >
	void Read(T& inData, uint32_t inBitCount = sizeof(T) * 8)
	{
		static_assert(std::is_arithmetic< T >::value ||
			std::is_enum< T >::value,
			"Generic Read only supports primitive data types");
		ReadBits(&inData, inBitCount);
	}

	template< typename T >
	void Read(std::vector< T >& outVector)
	{
		size_t elementCount;
		Read(elementCount);
		outVector.resize(elementCount);
		for (T& element : outVector)
		{
			Read(element);
		}
	}

	template< typename T >
	void Read(std::vector< T >& outVector, uint32_t inBitCountPerElement, uint32_t maxVectorSizeInBits = 32)
	{
		size_t elementCount = 0;
		Read(elementCount, maxVectorSizeInBits);
		Read(inBitCountPerElement, 6);
		outVector.resize(elementCount);
		for (T& element : outVector)
		{
			Read(element, inBitCountPerElement);
		}
	}

	void		Read(uint32_t& outData, uint32_t inBitCount = 32) { ReadBits(&outData, inBitCount); }
	void		Read(int& outData, uint32_t inBitCount = 32) { ReadBits(&outData, inBitCount); }
	void		Read(float& outData) { ReadBits(&outData, 32); }

	void		Read(uint16_t& outData, uint32_t inBitCount = 16) { ReadBits(&outData, inBitCount); }
	void		Read(int16_t& outData, uint32_t inBitCount = 16) { ReadBits(&outData, inBitCount); }

	void		Read(uint8_t& outData, uint32_t inBitCount = 8) { ReadBits(&outData, inBitCount); }
	void		Read(bool& outData) { ReadBits(&outData, 1); }

	void		Read(Quaternion& outQuat);

	void		ResetToCapacity(uint32_t inByteCapacity) { mBitCapacity = inByteCapacity << 3; mBitHead = 0; }


	void Read(std::string& inString)
	{
		uint32_t elementCount;
		Read(elementCount);
		inString.resize(elementCount);
		for (auto& element : inString)
		{
			Read(element);
		}
	}

	void Read(char* inData);
	void Read(Vector3& inVector);

	void ReadPos(Vector3& inVector);

private:
	char*		mBuffer;
	uint32_t	mBitHead;
	uint32_t	mBitCapacity;
	bool		mIsBufferOwner;

};



