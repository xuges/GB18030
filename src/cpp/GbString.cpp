#include "GbString.hpp"

namespace gb18030
{
	namespace v2022
	{
		namespace internal
		{
			extern uint32_t ucsToGbBmp[65535];
			extern char32_t gbToUcsBmp[65535];
			extern char32_t gbToUcsBmpExt[4][10][126][11];

			using CharBytes = std::array<uint8_t, 4>;

			//��ȡ�ֽ�����
			inline CharBytes getCharBytes(uint32_t value) noexcept
			{
				//TODO: Ĭ��С�� ���Ǵ�ˣ�
				return *reinterpret_cast<CharBytes*>(&value);
			}

			//�ֽ�����ת�ַ�
			inline uint32_t fromCharBytes(const uint8_t* data, size_t len) noexcept
			{
				//TODO: Ĭ��С�� ���Ǵ�ˣ�
				if (len != 0)
				{
					if (len == 4)
						return *reinterpret_cast<const uint32_t*>(data);

					CharBytes bytes = getCharBytes(0);
					bytes[0] = data[0];
					
					if (len > 1)
						bytes[1] = data[1];
					if (len > 2)
						bytes[2] = data[2];
					
					return *reinterpret_cast<uint32_t*>(&bytes);
				}
				return 0;
			}

			//��ȡGB18030�ַ�ռ���ֽ���
			inline size_t getGbCharSize(uint32_t c) noexcept
			{
				if (c <= 0x7F)
					return 1;
				else if (c <= 0xFFFF)
					return 2;
				return 4;
			}

			//��ȡGB18030�ַ��߽�
			inline size_t getGbCharBound(const uint8_t* data, size_t len) noexcept
			{
				if (len != 0)
				{
					if (data[0] <= 0x7F)
					{
						return 1;
					}
					else if (0x7F < data[1] || 0x40 <= data[1] && data[1] <= 0x7E)
					{
						return 2;
					}
					else if (0x30 <= data[1] && data[1] <= 0x39 && 0x7F < data[2] && 0x30 <= data[3] && data[3] <= 0x39)
					{
						return 4;
					}
				}
				return 0;
			}

			inline uint32_t unicodeToGb(char32_t c) noexcept
			{
				if (c <= 0x7F)  //���ֽ��� ��ͬ
					return (uint32_t)c;
				else if (c <= 0xFFFF)  //˫�ֽ��� BMP
					return ucsToGbBmp[c];
				else if (c <= 0x10FFFF)  //���ֽ��� SMP
					return c;  //TODO: implement

				return 0;  //�Ƿ��ַ�
			}

			inline char32_t gbToUnicode(uint32_t c) noexcept
			{
				auto size = getGbCharSize(c);
				if (size == 1)
					return (char32_t)c;

				if (size == 2)
					return gbToUcsBmp[c];

				if (size == 4)
				{
					//TODO: implement
				}

				return 0;  //�Ƿ��ַ�
			}
		}

		size_t GbChar::size() const noexcept
		{
			return internal::getGbCharSize(char_);
		}

		GbChar GbChar::fromUnicode(char32_t u32) noexcept
		{
			return GbChar{ internal::unicodeToGb(u32) };
		}

		char32_t GbChar::toUnicode() const noexcept
		{
			return internal::gbToUnicode(char_);
		}

		GbChar GbChar::fromBytes(const char* data, size_t len) noexcept
		{
			auto bound = internal::getGbCharBound((const uint8_t*)data, len);
			return GbChar{ internal::fromCharBytes((const uint8_t*)data, bound) };
		}

		std::array<uint8_t, 4> GbChar::toBytes() const noexcept
		{
			return internal::getCharBytes(char_);
		}
	}
}