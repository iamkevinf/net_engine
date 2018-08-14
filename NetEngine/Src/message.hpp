#ifndef __MESSAGE_HPP__
#define __MESSAGE_HPP__

namespace knet
{
	struct MessageBody
	{
		static const int32_t DATA_SIZE = 1024 * 128;

		static const int32_t HEADER_TYPE_BYTES = sizeof(uint16_t);
		static const int32_t HEADER_LEN_BYTES = sizeof(uint32_t);
		static const int32_t HEADER_SIZE = HEADER_LEN_BYTES + HEADER_TYPE_BYTES;;

		uint32_t size;
		uint16_t type;
		uint8_t data[DATA_SIZE];

		MessageBody()
		{
			size = 0;
			type = 0;
			::memset(data, 0, DATA_SIZE);
		}

		int32_t GetSize()
		{
			return size + HEADER_LEN_BYTES;
		}
	};

}; // end of namespace knet

#endif // __MESSAGE_HPP__