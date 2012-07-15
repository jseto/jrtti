#ifndef jrttihelpersclassH
#define jrttihelpersclassH


#include <sstream>

namespace jrtti {
	template <typename T>
	std::string
	numToStr ( T number ) {
		std::ostringstream ss;
		ss << number;
		return ss.str();
	}

	template <typename T>
	T
	strToNum ( const std::string &str ) {
		std::istringstream ss( str );
		T result;
		return ss >> result ? result : 0;
	}


	std::string
	base64Encode( void * data, size_t size ) {
		const static char encodeLookup[] = "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789+/";
		const static char padCharacter = '=';

		std::string encodedString;
		encodedString.reserve(((size/3) + (size % 3 > 0)) * 4);
		long temp;
		char * cursor = (char *)data;
		for(size_t idx = 0; idx < size/3; idx++)
		{
				temp  = (*cursor++) << 16; //Convert to big endian
				temp += (*cursor++) << 8;
				temp += (*cursor++);
				encodedString.append(1,encodeLookup[(temp & 0x00FC0000) >> 18]);
				encodedString.append(1,encodeLookup[(temp & 0x0003F000) >> 12]);
				encodedString.append(1,encodeLookup[(temp & 0x00000FC0) >> 6 ]);
				encodedString.append(1,encodeLookup[(temp & 0x0000003F)      ]);
		}
		switch(size % 3)
		{
		case 1:
				temp  = (*cursor++) << 16; //Convert to big endian
				encodedString.append(1,encodeLookup[(temp & 0x00FC0000) >> 18]);
				encodedString.append(1,encodeLookup[(temp & 0x0003F000) >> 12]);
				encodedString.append(2,padCharacter);
				break;
		case 2:
				temp  = (*cursor++) << 16; //Convert to big endian
				temp += (*cursor++) << 8;
				encodedString.append(1,encodeLookup[(temp & 0x00FC0000) >> 18]);
				encodedString.append(1,encodeLookup[(temp & 0x0003F000) >> 12]);
				encodedString.append(1,encodeLookup[(temp & 0x00000FC0) >> 6 ]);
				encodedString.append(1,padCharacter);
				break;
		}
		return encodedString;
	}

	void * base64Decode(const std::string& input)
	{
		const static char padCharacter = '=';
		if (input.length() % 4) //Sanity check
			   error("Non-Valid base64!");
        size_t padding = 0;
        if (input.length())
        {
                if (input[input.length()-1] == padCharacter)
                        padding++;
                if (input[input.length()-2] == padCharacter)
                        padding++;
        }
        //Setup a vector to hold the result
		char * decodedBytes = new char(((input.length()/4)*3) - padding);
		long temp=0; //Holds decoded quanta
		std::string::const_iterator cursor = input.begin();
		size_t idx = 0;
        while (cursor < input.end())
        {
                for (size_t quantumPosition = 0; quantumPosition < 4; quantumPosition++)
                {
                        temp <<= 6;
                        if       (*cursor >= 0x41 && *cursor <= 0x5A) // This area will need tweaking if
                                temp |= *cursor - 0x41;                       // you are using an alternate alphabet
                        else if  (*cursor >= 0x61 && *cursor <= 0x7A)
								temp |= *cursor - 0x47;
                        else if  (*cursor >= 0x30 && *cursor <= 0x39)
                                temp |= *cursor + 0x04;
                        else if  (*cursor == 0x2B)
                                temp |= 0x3E; //change to 0x2D for URL alphabet
                        else if  (*cursor == 0x2F)
                                temp |= 0x3F; //change to 0x5F for URL alphabet
                        else if  (*cursor == padCharacter) //pad
                        {
                                switch( input.end() - cursor )
                                {
                                case 1: //One pad character
										decodedBytes[idx++] = (temp >> 16) & 0x000000FF;
										decodedBytes[idx++] = (temp >> 8 ) & 0x000000FF;
                                        return decodedBytes;
                                case 2: //Two pad characters
										decodedBytes[idx++] = (temp >> 10) & 0x000000FF;
                                        return decodedBytes;
                                default:
                                        throw std::runtime_error("Invalid Padding in Base 64!");
                                }
                        }  else
                                throw std::runtime_error("Non-Valid Character in Base 64!");
                        cursor++;
                }
				decodedBytes[idx++] = (temp >> 16) & 0x000000FF;
				decodedBytes[idx++] = (temp >> 8 ) & 0x000000FF;
				decodedBytes[idx++] = (temp      ) & 0x000000FF;
		}
		return decodedBytes;
	}


}; // namespace jrtti
#endif //jrttihelpersH
